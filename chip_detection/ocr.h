#pragma once

#ifndef OCR_H
#define OCR_H

#include<iostream>
#include <vector>
#include <opencv.hpp>
#include "Windows.h"
#include "libxl.h"

using namespace std;
using namespace cv;
using namespace dnn;
using namespace libxl;

struct ImgLocation {
	int x;//Í¼Æ¬x×ø±ê
	int y;//Í¼Æ¬y×ø±ê

	bool operator == (const ImgLocation& other) const {
		return this->x == other.x && this->y == other.y;
	}
	bool operator < (const ImgLocation& other) const {
		return this->x < other.x ;
	}
};

struct ImgInfo {
	string img_name ;//Í¼Æ¬Ãû³Æ
	int x;//Í¼Æ¬x×ø±ê
	int y;//Í¼Æ¬y×ø±ê
	bool img_revise;//Í¼Æ¬×ø±êÊÇ·ñÐÞ¶©
};

vector<ImgLocation> getOcrLocation(Mat &img);
string getImgName(Mat &img, Net &net);

int charToNum(char c);
char numToChar(int num);
int strToNum(string str);

void writeExcel();

//debughº¯Êý
void imgsRenameDebug(const string &imgs_path, const string & file_path, const string & save_path, Net &net);
void charsSegmentDebug(const string &imgs_path, const string & save_path);
void charsClassificationDebug(const string &chars_path, const string & save_path, Net &net);
float getCharsAccuracy(const string &chars_path, Net &net);
float getPointsAccuracy(const string &chars_path, Net &net);
#endif // OCR_H
