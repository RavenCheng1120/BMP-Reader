// 4105056005-鄭筠庭-DCSA-01.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include <iostream>
#include "stdlib.h"
#include <fstream>
#include <vector>
#include <iomanip>
#include <cmath>

using namespace std;


struct Color {
	int R;
	int G;
	int B;
};

Color sum;

bool readWidthHeight(char* fileName, uint32_t& bmpWidth, uint32_t& bmpHeight) {
	ifstream file(fileName, ios::in | ios::binary);
	if (!file)
		return false;
	uint8_t headerbytes[54] = {};
	file.read((char*)headerbytes, sizeof(headerbytes));
	//Find the width and height of the image
	bmpWidth = *(uint32_t*)(headerbytes + 18);	
	bmpHeight = *(uint32_t*)(headerbytes + 22);
	file.close();
	return true;
}

bool bmpRead(vector<vector<Color> > &imageVec,char* fileName, Color counts[256]) {
	ifstream file(fileName, ios::in | ios::binary);
	if (!file)
		return false;
	// skip header(54bytes)
	const ifstream::off_type headerSize = 54;
	file.seekg(headerSize, ios::beg);

	 // read body
	for (size_t y = 0; y != imageVec.size(); ++y) {
		for (size_t x = 0; x != imageVec[0].size(); ++x) {
			char chR, chG, chB;
			file.get(chB).get(chG).get(chR);

			if (chB<0)
				imageVec[y][x].B = 255 + (int)chB;
			else
				imageVec[y][x].B = chB;

			if (chG<0)
				imageVec[y][x].G = 255 + (int)chG;
			else
				imageVec[y][x].G = chG;

			if (chR<0)
				imageVec[y][x].R = (int)chR + 255;
			else
				imageVec[y][x].R = (int)chR;

			//256個RGB的個數
			(counts[imageVec[y][x].B].B)++;
			(counts[imageVec[y][x].G].G)++;
			(counts[imageVec[y][x].R].R)++;

			sum.B += imageVec[y][x].B;
			sum.G += imageVec[y][x].G;
			sum.R += imageVec[y][x].R;
		}
	}

	file.close();
	return true;
}


int main() {
	char* fileNameEnter = new char[100];
	cout << "Please enter file name (ex:kodim01.bmp): ";
	cin >> fileNameEnter;

	uint32_t bmpWidth;
	uint32_t bmpHeight;
	if (!readWidthHeight(fileNameEnter, bmpWidth, bmpHeight)) {
		cout << "Error!!!No such file." << endl;
		return -1;
	}
	//cout << "圖寬:" << bmpWidth << "\t圖長:" << bmpHeight << endl;

	const size_t sizey = bmpWidth;
	const size_t sizex = bmpHeight;
	Color counts[256] = {0};
	float STDR = 0.0;
	float STDG = 0.0;
	float STDB = 0.0;

	vector<vector<Color> > imageVec(sizey, vector<Color>(sizex));
	if (!bmpRead(imageVec, fileNameEnter, counts)) {
		cout << "Error!!" << endl;
		return -1;
	}

	//Mean calculate
	float meanB = (float)sum.B / (bmpWidth*bmpHeight);
	float meanR = (float)sum.R / (bmpWidth*bmpHeight);
	float meanG = (float)sum.G / (bmpWidth*bmpHeight);
	cout << "\nMeanR: " << fixed << setprecision(2) <<  meanR;
	cout << "\nMeanG: " << meanG;
	cout << "\nMeanB: " << meanB;
	
	//計算標準差
	for (size_t y = 0; y != imageVec.size(); ++y) {
		for (size_t x = 0; x != imageVec[0].size(); ++x) {
			STDR += (imageVec[y][x].R - meanR)*(imageVec[y][x].R - meanR);
			STDG += (imageVec[y][x].G - meanG)*(imageVec[y][x].G - meanR);
			STDB += (imageVec[y][x].B - meanB)*(imageVec[y][x].B - meanR);
		}
	}
	STDR /= (bmpWidth*bmpHeight);
	STDR = sqrt(STDR);
	STDG /= (bmpWidth*bmpHeight);
	STDG = sqrt(STDG);
	STDB /= (bmpWidth*bmpHeight);
	STDB = sqrt(STDB);
	cout << "\nSTDs: " << STDR << " " << STDG << " " << STDB << "\n";

	//存為csv檔案
	string fName = fileNameEnter;
	fName = "CSV_" + fName + ".csv";
	ofstream csvFile;
	csvFile.open(fName);
	csvFile << "Index" << "," << "R" << "," << "G" << "," << "B" << "\n";
	for (int i = 0; i < 256; i++) 
		csvFile << i << "," << counts[i].R << "," << counts[i].G << "," << counts[i].B << "\n";
	csvFile << fixed << setprecision(2);
	csvFile << "Mean" << "," << (float)sum.R / (bmpWidth*bmpHeight) << "," << (float)sum.G / (bmpWidth*bmpHeight) << "," << (float)sum.B / (bmpWidth*bmpHeight) << "\n";
	csvFile << "STD" << "," << STDR << "," << STDG << "," << STDB << "\n";
	csvFile.close();
	cout << "\nCSV file writing completed.\n";
	delete[] fileNameEnter;
	return 0;
}
