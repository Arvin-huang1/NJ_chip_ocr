#include "ocr.h"
#include "findfile.h"

//��ȡocr�ַ�λ��
vector<ImgLocation> getOcrLocation(Mat &img)
{
	vector<ImgLocation> ocrs_location;
	ImgLocation ocr_location;
	ocr_location.x = 0;
	ocr_location.y = 0;
	Mat img_gray, img_gray_bin;
	Scalar mean_pix;
	cvtColor(img, img_gray, COLOR_BGR2GRAY);
	mean_pix = mean(img_gray); //��ȡ�Ҷ�ͼ����ƽ��ֵ

	threshold(img_gray, img_gray_bin, mean_pix[0]*0.6, 255, THRESH_BINARY);//230

	Mat out, stats, centroids;
	int number = 0, index = 0;
	number = connectedComponentsWithStats(img_gray_bin, out, stats, centroids, 8, CV_16U);
	Mat img_copy;
	img.copyTo(img_copy);
	for (int i = 1; i < number; i++)
	{
		// ����λ��
		int center_x = centroids.at<double>(i, 0);
		int center_y = centroids.at<double>(i, 1);
		//���α߿�
		int x = stats.at<int>(i, CC_STAT_LEFT);
		int y = stats.at<int>(i, CC_STAT_TOP);
		int w = stats.at<int>(i, CC_STAT_WIDTH);
		int h = stats.at<int>(i, CC_STAT_HEIGHT);
		int area = stats.at<int>(i, CC_STAT_AREA);

		if (area > 3000 && x <img.cols /2 && y < img.rows /4 && w > 100 && w < 120 && h > 40 && h < 60)
		{
			index++;
			// ��Ӿ���
			Rect rect_flag(x, y, w, h);
			//rectangle(img_copy, rect_flag, Scalar(0, 0, 255), 2, 8, 0);
			//rectangle(img_copy, rect, Scalar(0, 0, 255), 1, 8, 0);
			//cout << "rect_flag: " << rect_flag << ",center_x: " << center_x << ",center_y: " << center_y << endl;
			cout << "index: " << index << ",area: " << area << endl;
			cout << rect_flag << endl;
			ocr_location.x = x + 15;
			ocr_location.y = y + 2;
			ocrs_location.push_back(ocr_location);
		}

	}
	sort(ocrs_location.begin(), ocrs_location.end());
	ocr_location.x = 0;
	ocr_location.y = 0;
	ocrs_location.push_back(ocr_location);//���1����û�ҵ����������������ֹ�������
	ocrs_location.push_back(ocr_location);
	for (int i = 0; i < ocrs_location.size(); i++)
	{
		cout << ocrs_location[i].x << ' ' << ocrs_location[i].y << endl;
	}
	return ocrs_location;
}

//��ȡocr�ַ�λ��
vector<ImgLocation> getOcrLocation_img2(Mat &img)
{
	vector<ImgLocation> ocrs_location;
	ImgLocation ocr_location;
	ocr_location.x = 0;
	ocr_location.y = 0;
	Mat img_gray, img_gray_bin;
	Scalar mean_pix;
	cvtColor(img, img_gray, COLOR_BGR2GRAY);
	mean_pix = mean(img_gray); //��ȡ�Ҷ�ͼ����ƽ��ֵ

	threshold(img_gray, img_gray_bin, mean_pix[0] * 0.6, 255, THRESH_BINARY);//230

	Mat out, stats, centroids;
	int number = 0, index = 0;
	number = connectedComponentsWithStats(img_gray_bin, out, stats, centroids, 8, CV_16U);
	Mat img_copy;
	img.copyTo(img_copy);
	for (int i = 1; i < number; i++)
	{
		// ����λ��
		int center_x = centroids.at<double>(i, 0);
		int center_y = centroids.at<double>(i, 1);
		//���α߿�
		int x = stats.at<int>(i, CC_STAT_LEFT);
		int y = stats.at<int>(i, CC_STAT_TOP);
		int w = stats.at<int>(i, CC_STAT_WIDTH);
		int h = stats.at<int>(i, CC_STAT_HEIGHT);
		int area = stats.at<int>(i, CC_STAT_AREA);

		if (area > 3000 && x < img.cols / 4 && y < img.rows / 2 && w > 100 && w < 120 && h > 40 && h < 60)
		{
			index++;
			// ��Ӿ���
			//Rect rect_flag(x, y, w, h);
			//rectangle(img_copy, rect_flag, Scalar(0, 0, 255), 2, 8, 0);
			//rectangle(img_copy, rect, Scalar(0, 0, 255), 1, 8, 0);
			//cout << "rect_flag: " << rect_flag << ",center_x: " << center_x << ",center_y: " << center_y << endl;
			//cout << "index: " << index << ",area: " << area << endl;
			//cout << rect_flag << endl;
			ocr_location.x = x + 15;
			ocr_location.y = y + 2;
			ocrs_location.push_back(ocr_location);
		}

	}
	sort(ocrs_location.begin(), ocrs_location.end());
	return ocrs_location;
}

//��ȡ������ͼocrʶ�����ƺ�Ԥ��׼ȷ�ȣ�������6���ַ���ǰ5���ַ�Ϊ���ƣ���1���ַ�Ϊ���
string getImgName(Mat &img, Net &net)
{
	string img_name;//ocrʶ��ͼƬ������
	char predict_accuracy = 'T';//ocrʶ�����Ŷ�
	vector<ImgLocation> ocrs_location = getOcrLocation(img);

	Mat img_chars1 = img(Rect(ocrs_location[0].x, ocrs_location[0].y, 80, 48));//�ַ�����������
	Mat img_chars2 = img(Rect(ocrs_location[1].x, ocrs_location[1].y, 80, 48));
	vector<Rect> char_rect;//ÿ���ַ��ľ��η���
	vector<Mat> imgs_char;
	int num[3] = { 0, 40,80 };//�ָ��ַ���x����

	for (int i = 0; i < 2; i++)
	{
		Rect rc = Rect(num[i], 0, num[i + 1] - num[i], 48);//��ȡÿ���ַ����η���
		char_rect.push_back(rc);
	}

	for (int i = 0; i < 4; i++)
	{
		Mat img_resize;
		if (i < 2)
		{
			resize(img_chars1(char_rect[i%2]), img_resize, Size(32, 32));//resizeÿ���ַ��ĳߴ�Ϊ32*32
			imgs_char.push_back(img_resize);
		}
		else
		{
			resize(img_chars2(char_rect[i%2]), img_resize, Size(32, 32));//resizeÿ���ַ��ĳߴ�Ϊ32*32
			imgs_char.push_back(img_resize);
		}
		
	}
	for (int i = 0; i < imgs_char.size(); i++)
	{
		Mat img_resize;
		int result = 0;
		cvtColor(imgs_char[i], img_resize, COLOR_BGR2GRAY);
		Mat blob = blobFromImage(img_resize,
			1.0 / 255.0,//���й�һ��
			Size(32, 32),
			Scalar(0));
		net.setInput(blob);//��������
		Mat out = net.forward();//ǰ�򴫲���ȡԤ����

		//softmax��������ǰ�����õ���outת��ΪԤ���softmax��������
		float softmax_sum = 0;//softmax������ĸ
		vector<float> vec_out;//outת��Ϊvec
		vector<float> vec_out_softmax;//softmaxԤ�����
		for (int nrow = 0; nrow < out.rows; nrow++)
		{
			float* data = out.ptr<float>(nrow);
			for (int ncol = 0; ncol < out.cols * out.channels(); ncol++)
			{
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

		//��ȡԤ�����ĸ���predict_max
		vector<float>::iterator predict_max = max_element(begin(vec_out_softmax), end(vec_out_softmax));
		if (*predict_max < 0.9)
		{
			predict_accuracy = 'F';
		}
		result = int(distance(begin(vec_out_softmax), predict_max));	//��ȡԤ����ַ�����

		char c_result;//Ԥ���ַ�
		if (result < 10)
		{
			c_result = result + 0x30;
		}
		else if (10 <= result && result <= 35)
		{
			c_result = result + 0x37;
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
	if ('A' <= c && c <= 'Z')
	{
		num = c - 'A' + 10;
	}
	if (c == '_')
	{
		num = 36;
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
	if ('A' <= c && c <= 'Z')
	{
		num = c - 'A' + 10;
	}
	if (c == '_')
	{
		num = 36;
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
	else if (10 <= num && num <= 35)
	{
		c_num = num + 0x37;
	}
	else
	{
		c_num = '_';
	}
	return c_num;
}

//debug���Է�������
//ͼƬ�����������洢T�����ƣ�
void imgsRenameDebug(const string &imgs_path, const string & file_path, const string &save_path, Net &net)
{
	/*
	//�ļ��в����ڣ������ļ���
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
		
		cout << m << endl;
		Mat img = imread(imgs_path + '/' + imgs_path_name[m]);
		string img_name = getImgName(img, net);
		if (img_name[5] == 'T')
		{
			outfile <<img_name.substr(0, 5) << endl;
		} 

		

		//�洢rename���ԭʼͼƬ
		map<string, int>::iterator ret_iter = glob_name_map.find(img_name.substr(0, 5));
		if (ret_iter != glob_name_map.end())
		{
			int name_cnt = ret_iter->second;
			char new_name[256];
			sprintf_s(new_name, "%d.bmp", name_cnt + 1);
			cout << "new_name " << new_name << endl;
			string tmp = imgs_path + "/" + imgs_path_name[m];
			string rst =  imgs_path + "/" + "manual_" + img_name.substr(0, 5) + new_name;
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
		

		//ImgLocation xy;
		//xy = imgNmaeToLocation(img_name);
		


	}

	outfile.close();



}
//��ȡ�ַ�ͼƬ
void charsSegmentDebug(const string &imgs_path, const string & save_path)
{
	//�ļ��в����ڣ������ļ���
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
		string img_name;//ocrʶ��ͼƬ������

		vector<ImgLocation> ocrs_location = getOcrLocation(img);
		Mat img_chars1 = img(Rect(ocrs_location[0].x, ocrs_location[0].y, 80, 48));//�ַ�����������
		Mat img_chars2 = img(Rect(ocrs_location[1].x, ocrs_location[1].y, 80, 48));
		vector<Rect> char_rect;//ÿ���ַ��ľ��η���
		int num[3] = { 0, 40,80 };//�ָ��ַ���x����

		for (int i = 0; i < 2; i++)
		{
			Rect rc = Rect(num[i], 0, num[i + 1] - num[i], 48);//��ȡÿ���ַ����η���
			char_rect.push_back(rc);
		}

		for (int i = 0; i < 4; i++)
		{
			Mat img_resize;
			if (i<2)
			{
				resize(img_chars1(char_rect[i%2]), img_resize, Size(32, 32));//resizeÿ���ַ��ĳߴ�Ϊ32*32
				imwrite(save_path + '/' + to_string(count_chars) + ".bmp", img_resize);
			} 
			else
			{
				resize(img_chars2(char_rect[i%2]), img_resize, Size(32, 32));//resizeÿ���ַ��ĳߴ�Ϊ32*32
				imwrite(save_path + '/' + to_string(count_chars) + ".bmp", img_resize);
			}
			count_chars++;
		}
	}
}
//�ַ�����
void charsClassificationDebug(const string &chars_path, const string & save_path, Net &net)
{
	//�ļ��в����ڣ������ļ���
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

//дExcel
void writeExcel()
{
	Book* book = xlCreateXMLBook();
	book->setKey("GCCG", "windows-282123090cc0e6036db16b60a1t3u0h9");
	//��������˸ÿ⣬��������Ӧ��key����û�й�����������
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