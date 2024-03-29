#include "ocr.h"
#include "findfile.h"
#include "preprocess.h"


//获取ocr字符位置
vector<ImgLocation> getOcrLocation(Mat &img)
{
	vector<ImgLocation> ocrs_location;
	ImgLocation ocr_location;
	ocr_location.x = 0;
	ocr_location.y = 0;
	Mat img_gray, img_gray_bin;
	Scalar mean_pix;
	cvtColor(img, img_gray, COLOR_BGR2GRAY);
	//mean_pix = mean(img_gray); //获取灰度图像素平均值

	
	Mat img_gaus;
	GaussianBlur(img, img_gaus, Size(3, 3), 0);
	cvtColor(img_gaus, img_gray, COLOR_BGR2GRAY);
/*
	Mat img_sobel;
	Laplacian(img_gray, img_sobel, CV_8U);
	threshold(img_sobel, img_gray_bin, 5, 255, THRESH_BINARY_INV);
*/
	threshold(img_gray, img_gray_bin, 70, 255, THRESH_BINARY);//230

	Mat out, stats, centroids;
	int number = 0, index = 0;
	number = connectedComponentsWithStats(img_gray_bin, out, stats, centroids, 8, CV_16U);
	Mat img_copy;
	img.copyTo(img_copy);
	for (int i = 1; i < number; i++)
	{
		// 中心位置
		int center_x = centroids.at<double>(i, 0);
		int center_y = centroids.at<double>(i, 1);
		//矩形边框
		int x = stats.at<int>(i, CC_STAT_LEFT);
		int y = stats.at<int>(i, CC_STAT_TOP);
		int w = stats.at<int>(i, CC_STAT_WIDTH);
		int h = stats.at<int>(i, CC_STAT_HEIGHT);
		int area = stats.at<int>(i, CC_STAT_AREA);

		if (area > 1500 && x <img.cols /2 && y < img.rows /4 && w > 95 && w < 125 && h > 35 && h < 65)
		{
			index++;
			// 外接矩形
			Rect rect_flag(x, y, w, h);
			rectangle(img_copy, rect_flag, Scalar(0, 0, 255), 2, 8, 0);
			//rectangle(img_copy, rect, Scalar(0, 0, 255), 1, 8, 0);
			//cout << "rect_flag: " << rect_flag << ",center_x: " << center_x << ",center_y: " << center_y << endl;
			if (0)
			{
				cout << "index: " << index << ",area: " << area << endl;
				cout << rect_flag << endl;
			}
			
			ocr_location.x = x + 15;
			ocr_location.y = y + 2;
			ocrs_location.push_back(ocr_location);
		}

	}
	sort(ocrs_location.begin(), ocrs_location.end());
	ocr_location.x = 0;
	ocr_location.y = 0;
	ocrs_location.push_back(ocr_location);//如果1个都没找到，则添加两个，防止程序出错
	ocrs_location.push_back(ocr_location);
	
	if (0)
	{
		for (int i = 0; i < ocrs_location.size(); i++)
		{
			//cout << ocrs_location[i].x << ' ' << ocrs_location[i].y << endl;
		}
	}

	return ocrs_location;
}


//获取单张子图ocr识别名称和预测准确度，共返回6个字符，前5个字符为名称，后1个字符为结果
string getImgName(Mat &img, Net &net)
{
	string img_name;//ocr识别图片的名称
	char predict_accuracy = 'T';//ocr识别置信度
	vector<ImgLocation> ocrs_location = getOcrLocation(img);

	Mat img_chars1 = img(Rect(ocrs_location[0].x, ocrs_location[0].y, 80, 48));//字符的像素坐标
	Mat img_chars2 = img(Rect(ocrs_location[1].x, ocrs_location[1].y, 80, 48));
	//Mat img_chars1 = img(Rect(194, 64, 80, 48));//字符的像素坐标
	//Mat img_chars2 = img(Rect(507, 64, 80, 48));
	vector<Rect> char_rect;//每个字符的矩形方框
	vector<Mat> imgs_char;
	int num[3] = { 0, 40,80 };//分割字符的x坐标

	for (int i = 0; i < 2; i++)
	{
		Rect rc = Rect(num[i], 0, num[i + 1] - num[i], 48);//获取每个字符矩形方框
		char_rect.push_back(rc);
	}

	for (int i = 0; i < 4; i++)
	{
		Mat img_resize;
		if (i < 2)
		{
			resize(img_chars1(char_rect[i%2]), img_resize, Size(32, 32));//resize每个字符的尺寸为32*32
			imgs_char.push_back(img_resize);
		}
		else
		{
			resize(img_chars2(char_rect[i%2]), img_resize, Size(32, 32));//resize每个字符的尺寸为32*32
			imgs_char.push_back(img_resize);
		}
		
	}
	for (int i = 0; i < imgs_char.size(); i++)
	{
		Mat img_resize;
		int result = 0;
		cvtColor(imgs_char[i], img_resize, COLOR_BGR2GRAY);
		Mat blob = blobFromImage(img_resize,
			1.0 / 255.0,//进行归一化
			Size(32, 32),
			Scalar(0));
		net.setInput(blob);//设置输入
		Mat out = net.forward();//前向传播获取预测结果

		//softmax函数，把前向计算得到的out转化为预测的softmax概率数组
		float softmax_sum = 0;//softmax函数分母
		vector<float> vec_out;//out转化为vec
		vector<float> vec_out_softmax;//softmax预测概率
		for (int nrow = 0; nrow < out.rows; nrow++)
		{
			float* data = out.ptr<float>(nrow);
			for (int ncol = 0; ncol < out.cols * out.channels(); ncol++)
			{
				cout << data[ncol] << endl;
				vec_out.push_back(data[ncol]);
			}
		}
		for (int i = 0; i < vec_out.size(); i++)
		{
			softmax_sum += exp(vec_out[i]);
		}
		for (int i = 0; i < vec_out.size(); i++)
		{
			vec_out_softmax.push_back(exp(vec_out[i]) / softmax_sum);
		}

		for (int i = 0; i < vec_out_softmax.size(); i++)
		{
			cout << vec_out_softmax[i] << endl;
		}

		//获取预测最大的概率predict_max
		vector<float>::iterator predict_max = max_element(begin(vec_out_softmax), end(vec_out_softmax));
		if (*predict_max < 0.8)
		{
			predict_accuracy = 'F';
		}
		result = int(distance(begin(vec_out_softmax), predict_max));	//获取预测的字符索引

		char c_result;//预测字符
		if (result < 10)
		{
			c_result = result + 0x30;
		}
		else
		{
			c_result = '_';
			predict_accuracy = 'F';
		}
		img_name.push_back(c_result);

		if (i == 1)
		{
			img_name.push_back(' ');
		}
	}
	img_name.push_back(predict_accuracy);

	return img_name;
}

int strToNum(string str)
{
	int num;
	char c = str.c_str()[0];
	if (c < 'A')
	{
		num = c - '0';
	}

	if (c == '_')
	{
		num = 10;
	}
	return num;
}

int charToNum(char  c)
{
	int num;
	if (c < 'A')
	{
		num = c - '0';
	}
	if (c == '_')
	{
		num = 10;
	}
	return num;
}

char numToChar(int num)
{
	char c_num;
	if (num < 10)
	{
		c_num = num + 0x30;
	}
	else
	{
		c_num = '_';
	}
	return c_num;
}

//debug调试发布函数
//图片重命名，并存储T的名称，
void imgsRenameDebug(const string &imgs_path, const string & file_path, const string &save_path, Net &net)
{
	/*
	//文件夹不存在，创建文件夹
	if (_access(save_path.c_str(), 0) == -1)
	{
		_mkdir(save_path.c_str());
	}*/
	map<string, int> glob_name_map;
	ofstream outfile(file_path, ios::out);
	vector<string> imgs_path_name;
	fileSearch(imgs_path.data(), imgs_path_name);
	for (int m = 0; m < imgs_path_name.size(); m++)//
	{
		
		
		Mat img = imread(imgs_path + '/' + imgs_path_name[m]);
		string img_name = getImgName(img, net);
		if (img_name[5] == 'T')
		{
			outfile <<img_name.substr(0, 5) << endl;
		} 

		

		//存储rename后的原始图片
		map<string, int>::iterator ret_iter = glob_name_map.find(img_name.substr(0, 5));
		if (ret_iter != glob_name_map.end())
		{
			int name_cnt = ret_iter->second;
			char new_name[256];
			sprintf_s(new_name, "%d.bmp", name_cnt + 1);
			//cout << "new_name " << new_name << endl;
			string tmp = imgs_path + "/" + imgs_path_name[m];
			string rst =  imgs_path + "/" + "repeat_" + img_name.substr(0, 5) + new_name;
			rename(tmp.data(), rst.data());

			//imwrite(save_path + '\\' + img_name.substr(0,5) + new_name, img);
			glob_name_map[img_name.substr(0, 5)]++;
		}
		else 
		{
			if (img_name[5] == 'F')
			{
				string tmp = imgs_path + "/" + imgs_path_name[m];
				string rst =  imgs_path + "/" + "manual_" + img_name.substr(0,5) + ".bmp";
				rename(tmp.data(), rst.data());
			}
			else
			{
				string tmp = imgs_path + "/" + imgs_path_name[m];
				string rst = imgs_path + "/" + img_name.substr(0, 5) + ".bmp";
				rename(tmp.data(), rst.data());
			}
			
			//imwrite(save_path + '\\' + img_name.substr(0,5) + ".bmp", img);
			glob_name_map[img_name.substr(0, 5)]++;
		}
		
		cout << m <<": "<< img_name.substr(0, 5) + ".bmp" << endl;
		//ImgLocation xy;
		//xy = imgNmaeToLocation(img_name);
		


	}
	outfile.close();
}
//获取字符图片
void charsSegmentDebug(const string &imgs_path, const string &save_path)
{
	//文件夹不存在，创建文件夹
	if (_access(save_path.c_str(), 0) == -1)
	{
		_mkdir(save_path.c_str());
	}
	vector<string> imgs_path_name;
	fileSearch(imgs_path.data(), imgs_path_name);
	int count_chars = 0;
	for (int m = 0; m < imgs_path_name.size(); m++)
	{
		cout << m << endl;
		Mat img = imread(imgs_path + '/' + imgs_path_name[m]);
		string img_name;//ocr识别图片的名称

		vector<ImgLocation> ocrs_location = getOcrLocation(img);
		Mat img_chars1 = img(Rect(ocrs_location[0].x, ocrs_location[0].y, 80, 48));//字符的像素坐标
		Mat img_chars2 = img(Rect(ocrs_location[1].x, ocrs_location[1].y, 80, 48));

		vector<Rect> char_rect;//每个字符的矩形方框
		int num[3] = { 0, 40,80 };//分割字符的x坐标

		for (int i = 0; i < 2; i++)
		{
			Rect rc = Rect(num[i], 0, num[i + 1] - num[i], 48);//获取每个字符矩形方框
			char_rect.push_back(rc);
		}

		for (int i = 0; i < 4; i++)
		{
			Mat img_resize;
			if (i<2)
			{
				resize(img_chars1(char_rect[i%2]), img_resize, Size(32, 32));//resize每个字符的尺寸为32*32
				imwrite(save_path + '/' + to_string(count_chars) + ".bmp", img_resize);
			} 
			else
			{
				resize(img_chars2(char_rect[i%2]), img_resize, Size(32, 32));//resize每个字符的尺寸为32*32
				imwrite(save_path + '/' + to_string(count_chars) + ".bmp", img_resize);
			}
			count_chars++;
		}
	}
}
//字符分类
void charsClassificationDebug(const string &chars_path, const string & save_path, Net &net)
{
	//文件夹不存在，创建文件夹
	if (_access(save_path.c_str(), 0) == -1)
	{
		_mkdir(save_path.c_str());
	}
	vector<string> imgs_path_name;
	fileSearch(chars_path.data(), imgs_path_name);

	for (int m = 0; m < imgs_path_name.size(); m++)
	{
		cout << m << endl;
		Mat img_char = imread(chars_path + '/' + imgs_path_name[m]);
		Mat img_char_gray;
		cvtColor(img_char, img_char_gray, COLOR_BGR2GRAY);
		Mat blob = blobFromImage(img_char_gray,
			1.0 / 255.0,
			Size(32, 32),
			Scalar(0));
		net.setInput(blob);
		Mat out = net.forward();
		Point maxclass;
		minMaxLoc(out, NULL, NULL, NULL, &maxclass);
		char c_result = numToChar(maxclass.x);

		imwrite(save_path + '/' + c_result + '/' + imgs_path_name[m], img_char);
	}
}

float getCharsAccuracy(const string &chars_path, Net &net)
{

	long correct_num = 0, total_num = 0;
	long cor_num_class = 0, total_num_class = 0;
	float correct_rate = 0.0f;

	int result;

	intptr_t	hFile1 = 0;
	intptr_t	hFile2 = 0;
	struct _finddata_t fileinfo;  //存放文件信息;

	string p;
	string subDir;  //子目录路径名称
	string fileName;	//文件目录名称

	int classNum = 0;

	cout << "Start testing the test set...." << endl;

	if ((hFile1 = _findfirst(p.assign(chars_path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib == _A_SUBDIR)) //判断是否为文件夹
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) //存放用于训练的文件夹
				{
					cor_num_class = 0;
					total_num_class = 0;
					classNum = strToNum(fileinfo.name);

					hFile2 = 0;
					subDir.assign(chars_path).append("\\").append(fileinfo.name);  //子目录路径 path->subdir																			
					if ((hFile2 = _findfirst(p.assign(subDir).append("\\*").c_str(), &fileinfo)) != -1)		// 获取子目录下第一个文件句柄
					{
						do
						{
							if ((fileinfo.attrib != _A_SUBDIR))  //文件为图形文件
							{
								fileName.assign(subDir).append("\\").append(fileinfo.name); //文件绝对路径+文件名
								Mat temp_img = imread(fileName, 0);
								if (NULL == temp_img.data) {
									continue;
								}
								Mat blob = blobFromImage(temp_img,//1 - frame_32F / 255.0,
									1.0 / 255.0,//不管神经网络在哪里预处理，输入的x都是0-1的，必须要进行归一化
									Size(32, 32),
									Scalar(0));
								net.setInput(blob);
								Mat out = net.forward();
								Point maxclass;
								minMaxLoc(out, NULL, NULL, NULL, &maxclass);

								result = maxclass.x;	//根据训练好的神经网络预测结果

								total_num++;
								total_num_class++;
								if (result == classNum)
								{
									correct_num++;
									cor_num_class++;
								}
								else
								{
									cout << fileName << endl;
								}

							}
						} while (_findnext(hFile2, &fileinfo) == 0);  //寻找下一个文件
					}

					correct_rate = float(cor_num_class) / float(total_num_class);//每一类正确率的计算
					cout << "Character:   " << classNum << "     " << correct_rate << endl << endl << endl;

				}
			}
		} while (_findnext(hFile1, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
	}

	correct_rate = float(correct_num) / float(total_num);//每一类正确率的计算
	cout << "Correct Rate:   " << correct_rate << endl;

	return correct_rate;
}

float getPointsAccuracy(const string &chars_path, Net &net)
{

	long correct_num = 0, total_num = 0;
	long cor_num_class = 0, total_num_class = 0;
	float correct_rate = 0.0f;

	int result;

	intptr_t	hFile1 = 0;
	intptr_t	hFile2 = 0;
	struct _finddata_t fileinfo;  //存放文件信息;

	string p;
	string subDir;  //子目录路径名称
	string fileName;	//文件目录名称

	int classNum = 0;

	cout << "Start testing the test set...." << endl;

	if ((hFile1 = _findfirst(p.assign(chars_path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib == _A_SUBDIR)) //判断是否为文件夹
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) //存放用于训练的文件夹
				{
					cor_num_class = 0;
					total_num_class = 0;
					classNum = strToNum(fileinfo.name);

					hFile2 = 0;
					subDir.assign(chars_path).append("\\").append(fileinfo.name);  //子目录路径 path->subdir																			
					if ((hFile2 = _findfirst(p.assign(subDir).append("\\*").c_str(), &fileinfo)) != -1)		// 获取子目录下第一个文件句柄
					{
						do
						{
							if ((fileinfo.attrib != _A_SUBDIR))  //文件为图形文件
							{
								fileName.assign(subDir).append("\\").append(fileinfo.name); //文件绝对路径+文件名
								Mat temp_img = imread(fileName, 0);
								if (NULL == temp_img.data) {
									continue;
								}
								Mat blob = blobFromImage(temp_img,//1 - frame_32F / 255.0,
									1.0 / 255.0,//不管神经网络在哪里预处理，输入的x都是0-1的，必须要进行归一化
									Size(28, 28),
									Scalar(0));
								net.setInput(blob);
								Mat out = net.forward();
								Point maxclass;
								minMaxLoc(out, NULL, NULL, NULL, &maxclass);

								result = maxclass.x;	//根据训练好的神经网络预测结果

								total_num++;
								total_num_class++;
								if (result == classNum)
								{
									correct_num++;
									cor_num_class++;
								}
								else
								{
									cout << fileName << endl;
								}

							}
						} while (_findnext(hFile2, &fileinfo) == 0);  //寻找下一个文件
					}

					correct_rate = float(cor_num_class) / float(total_num_class);//每一类正确率的计算
					cout << "Character:   " << classNum << "     " << correct_rate << endl << endl << endl;

				}
			}
		} while (_findnext(hFile1, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
	}

	correct_rate = float(correct_num) / float(total_num);//每一类正确率的计算
	cout << "Correct Rate:   " << correct_rate << endl;

	return correct_rate;
}

//写Excel
void writeExcel()
{
	Book* book = xlCreateXMLBook();
	book->setKey("GCCG", "windows-282123090cc0e6036db16b60a1t3u0h9");
	//如果购买了该库，则设置相应的key，若没有购买，则不用这行
	Sheet * sheet = book->addSheet("sheet1");
	
	ifstream OK_file("OK.txt", ios::in);
	ifstream NG_file("NG.txt", ios::in);

	while (!OK_file.eof())
	{
		int _row, _col;
		OK_file >> _row >> _col;
		sheet->writeNum(87 - _row, _col, 1);
	}
	while (!NG_file.eof())
	{
		int _row, _col;
		NG_file >> _row >> _col;
		sheet->writeNum(87 - _row, _col, 0);
	}

	book->save("result.xlsx");
	book->release();
	
}