#pragma once
#ifndef PREPROCESS_H
#define PREPROCESS_H

#include<iostream>
#include<string>
#include <fstream>
#include <sstream>
#include <vector>
#include <io.h>
#include<opencv.hpp>
#include <Windows.h>
#include <direct.h>

using namespace std;
using namespace cv;



void img_segment(string raw_picture_path, string save_path);
void img_affine(string file_path, string save_path);
void img_draw_rect(string file_path, string picture_path, string picture_name, string save_path, int picture_num);
void img_draw_rect_debug(string file_path, string save_path);
void img_gray_and_bin(string file_path, string gray_path, string bin_path);
void img_draw_rect_final(string txt_path, string picture_path, string save_perspective_path,
	string save_result_path, string save_point_path);
void get_pix_min_max(string file_path, string save_path);
void test_in_min_max(string min_max_path, string file_path, string save_path);
void analyze_imgs(const string &min_max_path, const string &file_path, const string save_path);
vector<Mat> add_mask(Mat &img);
void test(Mat &img);
void get_blurred_imgs(const string &imgs_path); 

void find_scratch(const string &file_path);
void generateImg(Mat &img_ok, Mat &img_ng);
void getRawImgs(const string &imgs_path, const string &save_path);
#endif // PREPROCESS_H
