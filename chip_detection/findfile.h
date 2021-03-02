#pragma once
#ifndef FINDFILE_H
#define FINDFILE_H

#include<string>
#include<vector>
#include<io.h>
#include<direct.h>

using namespace std;


void fileSearch(const char* path_dir, vector<string> &file_vec);//查找文件夹
vector<string> SplitStrToVec(const string &s, const string &seperator);//分割坐标

#endif // FINDFILE_H
