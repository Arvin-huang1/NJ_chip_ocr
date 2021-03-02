#include "preprocess.h"
#include "findfile.h"


//分割子图
void img_segment(string raw_picture_path, string save_path)
{
	static int img_part_num = 0;
	//文件夹不存在，创建文件夹
	if (_access(save_path.c_str(), 0) == -1)
	{
		_mkdir(save_path.c_str());
	}

	vector<string> img_names;
	fileSearch(raw_picture_path.data(), img_names);//获取所有图片名称
	for (int m = 0; m < img_names.size(); m++)
	{
		cout << img_names[m] << endl;
	}
	//分割子图
	for (int m = 0; m < img_names.size(); m++)
	{
		cout << m << endl;
		Mat img = imread(raw_picture_path + "/" + img_names[m]);
		cout << img_names[m] << endl;
		assert(!img.empty());//读取图片是否成功

		Mat img_gray, img_gray_bin;//原图的灰度图和二值图
								   //将图像转成二值图像，用于统计连通域

		cvtColor(img, img_gray, COLOR_BGR2GRAY);
		Scalar mean_pix = mean(img_gray); //获取灰度图像素平均值
		threshold(img_gray, img_gray_bin, 150, 255, THRESH_BINARY);

		Mat out, stats, centroids;//获取连通域统计信息

		vector<Rect> rect;//存储子图矩形坐标
		//namedWindow("img", 0);
		//imshow("img", img_gray_bin);
		//waitKey(0);
		int number = connectedComponentsWithStats(img_gray_bin, out, stats, centroids, 8, CV_16U);
		int index = 0;
		//获取子图连通域矩形
		for (int i = 1; i < number; i++)
		{
			// 中心位置
			double center_x = centroids.at<double>(i, 0);
			double center_y = centroids.at<double>(i, 1);
			//矩形边框
			int x = stats.at<int>(i, CC_STAT_LEFT);
			int y = stats.at<int>(i, CC_STAT_TOP);
			int w = stats.at<int>(i, CC_STAT_WIDTH);
			int h = stats.at<int>(i, CC_STAT_HEIGHT);
			int area = stats.at<int>(i, CC_STAT_AREA);
			//if (area > 100000 && x > 10 && y > 10 && (x + w)<4090 && (y + h) < 2950)//1
			//if (area > 10000 && area < 20000 && x >10 && y >10 && (x + w)<4090 && (y + h) <2950 && w>420 && h > 360)//2,th82
			//if (area > 20000 && area < 100000 && w>700 && x > 10 && y > 10 && (x + w)<4090 && (y + h) < 2950)//3,th74
			if (area > 100000 && w>700 && x > 7 && y > 7 && (x + w)<4080 && (y + h) < 2984) //th147
			{
				// 外接矩形
				rect.push_back(Rect(x-4, y - 4 , w+8 , h+8));
				//rectangle(img_copy, rect_flag, Scalar(0, 255, 255), 4, 8, 0);
				cout << "rect_flag: " << rect[index] << endl;
				cout << "index: " << index << ",area: " << area << endl;
				index++;
			}

		}

		vector<Mat> img_part; //存储子图
		vector<string> img_name = SplitStrToVec(img_names[m], ".");
		for (int i = 0; i < rect.size(); i++)
		{
			Mat img_part_resize;
			img_part.push_back(img(rect[i]));
			resize(img_part[i], img_part_resize, Size(1350, 389));
			imwrite(save_path + "/" + img_name[0] + '_' +  to_string(i) + ".bmp", img_part_resize);
			img_part_num++;
		}


	}
	cout << "countAll:" << img_part_num << endl;

}


//子图仿射变换
void img_affine(string file_path, string save_path)
{
	//文件夹不存在，创建文件夹
	if (_access(save_path.c_str(), 0) == -1)
	{
		_mkdir(save_path.c_str());
	}
	static int img_part_num = 0;
	vector<string> img_names;
	fileSearch(file_path.data(), img_names);//获取所有图片名称

	for (int m =0; m < img_names.size(); m++)
	{
		Mat img = imread(file_path + "/" + img_names[m]);
		assert(!img.empty());

		Mat img_gray, img_bin, img_copy;
		cvtColor(img, img_gray, COLOR_BGR2GRAY);
		img.copyTo(img_copy);
		Scalar mean_pix = mean(img); //获取灰度图像素平均值						 
		threshold(img_gray, img_bin, 74, 255, THRESH_BINARY); //二值化

		vector<vector<Point> > contours;
		vector<Rect> boundRect(contours.size());
		//注意第3个参数为RETR_EXTERNAL，只检索外框  
		findContours(img_bin, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE); //找轮廓

		//cout << contours.size() << endl;

		for (int i = 0; i < contours.size(); i++)
		{
			float min_point = 500;
			//需要获取的坐标  
			Point2f rectpoint[4];
			RotatedRect rect = minAreaRect(Mat(contours[i]));
			rect.points(rectpoint);//获取4个顶点坐标
			//for (int i = 0; i < 4 ; i++)
			//{
			//	min_point = min(rectpoint[i].x, min_point);
			//}
			//
			//if (min_point > 8)
			//{
			//	continue;
			//}
			//
			//与水平线的角度  
			float angle = rect.angle;
			

			int line1 = sqrt((rectpoint[1].y - rectpoint[0].y)*(rectpoint[1].y - rectpoint[0].y) + (rectpoint[1].x - rectpoint[0].x)*(rectpoint[1].x - rectpoint[0].x));
			int line2 = sqrt((rectpoint[3].y - rectpoint[0].y)*(rectpoint[3].y - rectpoint[0].y) + (rectpoint[3].x - rectpoint[0].x)*(rectpoint[3].x - rectpoint[0].x));
			//rectangle(img_copy, rectpoint[0], rectpoint[3], Scalar(255), 2);
			//面积太小的直接pass
			if (line1 * line2 < 10000 )
			{
				continue;
			}
			cout << "rect" << rectpoint[0] << rectpoint[1] << rectpoint[2] << rectpoint[3] << endl;
			cout << angle << endl;
			//为了让正方形横着放，所以旋转角度是不一样的。竖放的，给他加90度，翻过来
			if (line1 > line2)
			{
				angle = 90 + angle;
			}

			//创建一个仿射变换后的图像  
			Mat img_correction(img.rows, img.cols, CV_8UC1);
			img_correction.setTo(0);
			//对RoiSrcImg进行旋转  
			Point2f center = rect.center;  //中心点  
			Mat M2 = getRotationMatrix2D(center, angle, 1);//计算旋转加缩放的变换矩阵 
			warpAffine(img, img_correction, M2, img.size(), 1, 0, Scalar(0));//仿射变换 
			//rectangle(img_correction, rect_inner, Scalar(0, 0, 255), 3, 8, 0);
			//namedWindow("dst1", 0);
			//imshow("dst1", img_correction);
			//waitKey(0);
			//destroyAllWindows();
			imwrite(save_path + "/" + img_names[m], img_correction); //存储仿射变换后的图片
			Mat img_correction_gray, img_correction_bin;
			Mat img_resize;
			Mat out, stats, centroids;//获取连通域统计信息
			int number, index;
			cvtColor(img_correction, img_correction_gray, COLOR_BGR2GRAY);
			threshold(img_correction_gray, img_correction_bin, 74, 255, THRESH_BINARY); //二值化
			number = connectedComponentsWithStats(img_correction_bin, out, stats, centroids, 8, CV_16U);
			index = 0;
			//获取子图连通域矩形
			for (int i = 1; i < number; i++)
			{
				// 中心位置
				double center_x = centroids.at<double>(i, 0);
				double center_y = centroids.at<double>(i, 1);
				//矩形边框
				int x = stats.at<int>(i, CC_STAT_LEFT);
				int y = stats.at<int>(i, CC_STAT_TOP);
				int w = stats.at<int>(i, CC_STAT_WIDTH);
				int h = stats.at<int>(i, CC_STAT_HEIGHT);
				int area = stats.at<int>(i, CC_STAT_AREA);
				//if (area > 10000 && area < 20000 && (x + w) < 4090 && (y + h) < 2950 && w > 420 && h > 370)//2
				if (area > 10000&& x < 10)//3 
				{
					// 外接矩形
					Rect rect_print = Rect(x, y, w, h);
					//cout << m << ",rect_print: " << rect_print << endl;
					cout << "m: " << m <<"part_num" << img_part_num << ",area: " << area << endl;
					//resize(img_correction(rect_print), img_resize, Size(424, 374));//2
					resize(img_correction(rect_print), img_resize, Size(1342, 381));//3
					imwrite(save_path + "/"+ img_names[m], img_resize); //存储仿射变换后的图片
					index++;
					img_part_num++;
				}

			}
		}

	}
}


//根据第一幅图获取的圆心模板画框
void img_draw_rect(string file_path, string picture_path, string picture_name, string save_path, int picture_index)
{
	//文件夹不存在，创建文件夹
	if (_access(save_path.c_str(), 0) == -1)
	{
		_mkdir(save_path.c_str());
	}


	Mat img = imread(picture_path);//
	assert(!img.empty());
	resize(img, img, Size(844, 974));
	Mat img_copy;
	Mat img_gray, img_gray_bin;//原图的灰度图和二值图
	Mat out, stats, centroids;

	int number;//记录图像所有的连通域
	int index;//记录图像有效的连通域

	Scalar mean_pix;//获取灰度图像素平均值
	img.copyTo(img_copy);

	if (_access(file_path.c_str(), 0) != 0 && picture_index == 0)//文件不存在
	{
		ofstream outfile(file_path, ios::out);
		assert(outfile.is_open());				//若失败,则输出错误消息,并终止程序运行

		cvtColor(img, img_gray, COLOR_BGR2GRAY);
		mean_pix = mean(img_gray); //获取灰度图像素平均值
		threshold(img_gray, img_gray_bin, mean_pix[0] * 1.1, 255, THRESH_BINARY);
		number = connectedComponentsWithStats(img_gray_bin, out, stats, centroids, 8, CV_16U);
		index = 0;
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
			if (area > 50 && area < 350 && x > 35 && x < 815 && y > 172 && y < 808)
			{
				index++;
				// 外接矩形
				Rect rect_flag(x, y, w, h);
				//rectangle(img_copy, rect_flag, Scalar(0, 0, 255), 2, 8, 0);
				Rect rect(center_x - 17, center_y - 17, 35, 34);
				rectangle(img_copy, rect, Scalar(0, 0, 255), 1, 8, 0);
				//cout << "rect_flag: " << rect_flag << ",center_x: " << center_x << ",center_y: " << center_y << endl;
				cout << "index: " << index << ",area: " << area << endl;
				outfile << center_x << " " << center_y << endl;
			}

		}
		outfile.close();
	}

	ifstream infile;
	infile.open(file_path, ios::in);	//输入文件
	assert(infile.is_open());//若失败,则输出错误消息,并终止程序运行
	/*
	//发光孔偏移校正
	int x_min = 500, y_min = 500, x_max = 500, y_max = 500;//记录每幅图的左上发光孔的位置
	static int x_min_0 = 500, y_min_0 = 500, x_max_0 = 500, y_max_0 = 500;//记录标定图的3个点

																		  //记录与标定图的偏移
	cvtColor(img, img_gray, COLOR_BGR2GRAY);
	mean_pix = mean(img_gray); //获取灰度图像素平均值
	threshold(img_gray, img_gray_bin, mean_pix[0] * 0.85, 255, THRESH_BINARY);
	number = connectedComponentsWithStats(img_gray_bin, out, stats, centroids, 8, CV_16U);
	index = 0;
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
		if (area > 30 && area < 150 && x > 150 && x < 750 && y > 50 && y < 730)
		{
			index++;
			if (m == 0)
			{
				x_min_0 = min(x, x_min_0);
				y_min_0 = min(y, y_min_0);
				x_max_0 = max(x, x_max_0);
				y_max_0 = max(y, y_max_0);
			}
			// 外接矩形
			Rect rect_flag(x, y, w, h);
			x_min = min(x, x_min);
			y_min = min(y, y_min);
			x_max = max(x, x_max);
			y_max = max(y, y_max);

			if (index == 1)
			{
				cout << center_x << "," << center_y << endl;
			}

		}

	}
	*/

	//读取中心坐标,画框
	int center_x, center_y;
	index = 0;
	while (!infile.eof())
	{
		infile >> center_x >> center_y;//读取一行数据
		if (index < 305)
		{
			Rect rect(center_x - 17, center_y - 17, 35, 34);
			rectangle(img_copy, rect, Scalar(0, 0, 255), 1, 8, 0);
			imwrite(save_path + picture_name[0] +  picture_name[1]+ picture_name[2]+ picture_name[3]+
					picture_name[4] +  "_" + to_string(index) +  ".png" , img(rect));
			index++;
		}
	}
	cout << picture_index << endl;


	//imwrite(save_path + "/" + to_string(m) + ".bmp", img_copy);
	//imshow("img_copy", img_copy);
	//waitKey(0);
	//destroyAllWindows();
		
}

//根据连通域画框，做调试
void img_draw_rect_debug(string file_path, string save_path)
{
	////文件夹不存在，创建文件夹
	//if (_access(save_path.c_str(), 0) == -1)
	//{
	//	_mkdir(save_path.c_str());
	//}

	Mat img = imread(file_path);
	assert(!img.empty());
	
	
	for (int j = 147; j < 255 ; j++,j++,j++)
	{
		Mat img_copy;
		Mat img_gray, img_gray_bin;//原图的灰度图和二值图


		int number;//记录图像所有的连通域
		int index;//记录图像有效的连通域

		Scalar mean_pix;//获取灰度图像素平均值

		img.copyTo(img_copy);

		Mat out, stats, centroids;

		cvtColor(img, img_gray, COLOR_BGR2GRAY);
		mean_pix = mean(img_gray); //获取灰度图像素平均值
		threshold(img_gray, img_gray_bin, j, 255, THRESH_BINARY);
		//namedWindow("img_copy", 0);
		//resizeWindow("img_copy", 1500, 1000);
		//imshow("img_copy", img_gray_bin);
		//waitKey(0);
		//destroyAllWindows();
		number = connectedComponentsWithStats(img_gray_bin, out, stats, centroids, 8, CV_16U);
		index = 0;
		cout << j<<endl;
		for (int i = 0; i < number; i++)
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
			//if (area > 10000 && area < 20000 && x > 10 && y > 10 && (x+w)<4090 && (y+h) < 2950)//2
			//if (area > 20000 && area < 100000 && w>700 && x > 10 && y > 10 && (x + w)<4090 && (y + h) < 2950)//3 th74
			if (area > 100000  && w>700 && x > 1 && y > 1 && (x + w)<4095 && (y + h) < 2999) //th147
			{
				index++;
				// 外接矩形
				Rect rect_flag(x, y, w, h);
				//rectangle(img_copy, rect_flag, Scalar(0, 0, 255), 2, 8, 0);
				//Rect rect(center_x - 16, center_y - 15, 32, 30);
				rectangle(img_copy, rect_flag, Scalar(0, 0, 255), 2, 8, 0);
				//circle(img_copy, Point(center_x, center_y),6, Scalar(0, 0, 255),3,8,0);
				//cout << "rect_flag: " << rect_flag << ",center_x: " << center_x << ",center_y: " << center_y << endl;
				cout << "index: " << index << ",area: " << area << endl;

			}

		}
		namedWindow("img_gray_bin", 0);
		resizeWindow("img_gray_bin", 1500, 1000);
		imshow("img_gray_bin", img_gray_bin);
		namedWindow("img_copy", 0);
		resizeWindow("img_copy", 1500, 1000);
		imshow("img_copy", img_copy);
		waitKey(0);
		destroyAllWindows();
	}
	
	//imwrite(save_path + "/" + to_string(m) + ".bmp", img_copy);



}


//获取原图的灰度图和二值图
void img_gray_and_bin(string file_path, string gray_path, string bin_path)
{
	//文件夹不存在，创建文件夹
	if (_access(gray_path.c_str(), 0) == -1)
	{
		_mkdir(gray_path.c_str());
	}
	//文件夹不存在，创建文件夹
	if (_access(bin_path.c_str(), 0) == -1)
	{
		_mkdir(bin_path.c_str());
	}
	vector<string> img_names;
	fileSearch(file_path.data(), img_names);//获取所有图片名称

	for (int m = 0; m < img_names.size(); m++)
	{
		cout << m << endl;
		Mat img = imread(file_path + "/" + img_names[m]);//
		assert(!img.empty());
		Mat img_copy;
		Mat img_gray, img_gray_bin;//原图的灰度图和二值图
		Scalar mean_pix;
		cvtColor(img, img_gray, COLOR_BGR2GRAY);
		mean_pix = mean(img_gray); //获取灰度图像素平均值
		threshold(img_gray, img_gray_bin, mean_pix[0] * 0.85, 255, THRESH_BINARY);

		imwrite(gray_path + "/" + to_string(m) + ".bmp", img_gray);
		imwrite(bin_path + "/" + to_string(m) + ".bmp", img_gray_bin);
	}
}


//使用多点单映射，画框
void img_draw_rect_final(string txt_path, string picture_path, string save_perspective_path, string save_result_path, string save_point_path)
{

	vector<int> vec_x[18];//所有图片发光孔中心坐标
	vector<int> vec_y[18];//所有图片发光孔中心坐标

	vector<string> img_names;
	fileSearch(picture_path.data(), img_names);//获取所有图片名称

	//获取所有图片发光孔中心坐标
	for (int m = 0; m < img_names.size(); m++)
	{
		Mat img = imread(picture_path + "/" + img_names[m]);//
		assert(!img.empty());
		Mat img_copy;
		Mat img_gray, img_gray_bin;//原图的灰度图和二值图
		Mat out, stats, centroids;

		int number;//记录图像所有的连通域
		int index;//记录图像有效的连通域

		Scalar mean_pix;//获取灰度图像素平均值
		img.copyTo(img_copy);

		//ifstream infile;
		//infile.open(txt_path, ios::in);	//输入文件
		//assert(infile.is_open());//若失败,则输出错误消息,并终止程序运行

		cvtColor(img, img_gray, COLOR_BGR2GRAY);
		mean_pix = mean(img_gray); //获取灰度图像素平均值
		threshold(img_gray, img_gray_bin, mean_pix[0] * 0.85, 255, THRESH_BINARY);
		number = connectedComponentsWithStats(img_gray_bin, out, stats, centroids, 8, CV_16U);
		index = 0;

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
			if (area > 30 && area < 150 && x > 150 && x < 750 && y > 50 && y < 730)
			{
				index++;
				vec_x[m].push_back(center_x);
				vec_y[m].push_back(center_y);

			}

		}

	}

	vector<int> vec_x_form[18];//存放标准图发光孔坐标，即0.bmp
	vector<int> vec_y_form[18];
	vector<int> vec_x_match[18];//存放其它需要映射图片的发光孔坐标
	vector<int> vec_y_match[18];

	//获取透视变换的点
	for (int m = 0; m < size(vec_x); m++)
	{
		if (m == 0)//标准图跳过
		{
			continue;
		}
		for (int i = size(vec_x[0]) - 1; i > 0; i -= 3)
		{
			for (int j = 0; j < size(vec_x[m]); j++)
			{
				if (((vec_x[0][i] - 5 < vec_x[m][j]) && (vec_x[0][i] + 5 > vec_x[m][j])) &&
					((vec_y[0][i] - 5 < vec_y[m][j]) && (vec_y[0][i] + 5 > vec_y[m][j])) && vec_x_match[m].size() < 115)
				{
					vec_x_form[m].push_back(vec_x[0][i]);
					vec_y_form[m].push_back(vec_y[0][i]);
					vec_x_match[m].push_back(vec_x[m][j]);
					vec_y_match[m].push_back(vec_y[m][j]);
				}
			}
		}
	}

	vec_x[0].swap(vector<int>());//清空后存放标准图排序后的发光孔坐标
	vec_y[0].swap(vector<int>());//清空后存放标准图排序后的发光孔坐标

	ifstream infile;
	infile.open(txt_path, ios::in);	//输入文件
	assert(infile.is_open());//若失败,则输出错误消息,并终止程序运行

							 //读取中心坐标,画框
	int center_x, center_y;
	int index = 0;
	while (!infile.eof())
	{
		infile >> center_x >> center_y;//读取一行数据
		if (index < 368)
		{
			vec_x[0].push_back(center_x);
			vec_y[0].push_back(center_y);
			index++;
		}
	}
	infile.close();

	//给找到的图
	for (int m = 0; m < 18; m++)
	{
		//进行透视变换
		Mat img = imread(picture_path + "/" + to_string(m) + ".bmp");
		Mat img_copy;
		Mat h;//变换矩阵
		img.copyTo(img_copy);
		assert(!img.empty());
		vector<Point2f> srcTri;
		vector<Point2f> dstTri;
		if (m > 0)
		{
			for (int i = 0; i < size(vec_x_form[m]); i++)
			{
				srcTri.push_back(Point2f(vec_x_match[m][i], vec_y_match[m][i]));
				dstTri.push_back(Point2f(vec_x_form[m][i], vec_y_form[m][i]));
			}
			h = findHomography(srcTri, dstTri, RANSAC);
			warpPerspective(img, img_copy, h, img.size());
		}



		if (save_perspective_path != "") //不为空保存透视变换图片
		{
			//文件夹不存在，创建文件夹
			if (_access(save_perspective_path.c_str(), 0) == -1)
			{
				_mkdir(save_perspective_path.c_str());
			}
			imwrite(save_perspective_path + "/" + to_string(m) + ".bmp", img_copy);
		}

		//遍历发光孔
		for (int j = 0; j < vec_x[0].size(); j++)
		{
			Rect rect(vec_x[0][j] - 16, vec_y[0][j] - 15, 32, 30);//给发光孔画框
			rectangle(img_copy, rect, Scalar(0, 0, 255), 1, 8, 0);

			if (save_point_path != "") //不为空保存发光孔样本点图片
			{
				//文件夹不存在，创建文件夹
				if (_access(save_point_path.c_str(), 0) == -1)
				{
					_mkdir(save_point_path.c_str());
				}
				imwrite(save_point_path + "/" + to_string(m * 368 + j) + ".bmp", img(rect));
			}
		}

		if (save_result_path != "") //不为空保存结果图片
		{
			//文件夹不存在，创建文件夹
			if (_access(save_result_path.c_str(), 0) == -1)
			{
				_mkdir(save_result_path.c_str());
			}
			imwrite(save_result_path + "/" + to_string(m) + ".bmp", img_copy);
		}

		cout << m << endl;
	}
}


//获取系列图片像素的最大值和最小值
void get_pix_min_max(string file_path, string save_path)
{
	//文件夹不存在，创建文件夹
	if (_access(save_path.c_str(), 0) == -1)
	{
		_mkdir(save_path.c_str());
	}
	vector<string> img_names;
	fileSearch(file_path.data(), img_names);//获取所有图片名称
	vector<Mat> imgs;
	Mat img;
	//分割子图
	for (int m = 0; m < img_names.size(); m++)
	{
		img = imread(file_path + "/" + img_names[m], IMREAD_GRAYSCALE);
		assert(!img.empty());
		imgs.push_back(img);
	}

	Mat pix_min, pix_max;
	imgs[0].copyTo(pix_min);
	imgs[0].copyTo(pix_max);
	//获取pix_min，和pix_max
	for (int k = 0; k < size(imgs); k++)//行、列
	{
		for (int i = 0; i < imgs[0].cols; i++)
		{
			for (int j = 0; j < imgs[0].rows; j++)
			{
				if (imgs[k].at<uchar>(j, i) < pix_min.at<uchar>(j, i))
				{
					pix_min.at<uchar>(j, i) = imgs[k].at<uchar>(j, i);
				}
				if (imgs[k].at<uchar>(j, i) > pix_max.at<uchar>(j, i))
				{
					pix_max.at<uchar>(j, i) = imgs[k].at<uchar>(j, i);
				}
			}
		}
		cout << k << endl;
	}
	imwrite(save_path + "/pix_min.bmp", pix_min);
	imwrite(save_path + "/pix_max.bmp", pix_max);

}


//测试图片
void test_in_min_max(string min_max_path, string file_path, string save_path)
{
	//文件夹不存在，创建文件夹
	if (_access(save_path.c_str(), 0) == -1)
	{
		_mkdir(save_path.c_str());
	}

	Mat pix_min, pix_max;
	pix_min = imread(min_max_path + "/pix_min.bmp", IMREAD_GRAYSCALE);
	pix_max = imread(min_max_path + "/pix_max.bmp", IMREAD_GRAYSCALE);
	assert(!pix_min.empty() && !pix_max.empty());

	vector<string> img_names;
	fileSearch(file_path.data(), img_names);//获取所有图片名称

	//分割子图
	for (int m = 0; m < img_names.size(); m++)
	{

		//处理的图片
		Mat img_raw = imread(file_path + "/" + img_names[m], 1);
		Mat img_test = imread(file_path + "/" + img_names[m], 0);

		Mat img_test_copy(img_test.rows, img_test.cols, CV_8UC1); //注意这里必须选CV_8UC1
		img_test_copy.setTo(0); //颜色都设置为黑色  

		int count_test = 0;
		int out_point = 0;
		for (int i = 0; i < img_test.cols; i++)
		{
			for (int j = 0; j < img_test.rows; j++)
			{
				if (img_test.at<uchar>(j, i) < int(pix_min.at<uchar>(j, i) - 30 )  || img_test.at<uchar>(j, i) > pix_max.at<uchar>(j, i) + 10)
				{
					img_test_copy.at<uchar>(j, i) = 255;
					count_test++;
					out_point++;
				}
			}
		}
		cout << "img" << m << "  ,count_test:" << count_test << endl;

		

		Mat out, stats, centroids;//获取连通域统计信息
		int number, index;
		Mat element = getStructuringElement(MORPH_RECT, Size(4,4));
		dilate(img_test_copy, img_test_copy, element);
		
		threshold(img_test_copy, img_test_copy, 82, 255, THRESH_BINARY); //二值化
		number = connectedComponentsWithStats(img_test_copy, out, stats, centroids, 8, CV_16U);
		index = 0;
		//获取子图连通域矩形
		for (int i = 1; i < number; i++)
		{
			// 中心位置
			double center_x = centroids.at<double>(i, 0);
			double center_y = centroids.at<double>(i, 1);
			//矩形边框
			int x = stats.at<int>(i, CC_STAT_LEFT);
			int y = stats.at<int>(i, CC_STAT_TOP);
			int w = stats.at<int>(i, CC_STAT_WIDTH);
			int h = stats.at<int>(i, CC_STAT_HEIGHT);
			int area = stats.at<int>(i, CC_STAT_AREA);
			if (area<2000 && ( (x>175 && x<225 && y>295 && y<350)|| (x>275 && x<325 && y>295 && y<350) ||
				(x>495 && x<545 && y>295 && y<350) || (x>595 && x<645 && y>295 && y<350) ||
				(x>810 && x<860 && y>295 && y<350) || (x>910 && x<960 && y>295 && y<350) ||
				(x>1125 && x<1175 && y>295 && y<350) || (x>1230 && x<1280 && y>295 && y<350)
				))
			{
				continue;
			}
			if (area > 200)
			{
				// 外接矩形
				Rect rect_print = Rect(x, y, w, h);
				rectangle(img_raw, rect_print, Scalar(0, 0, 255), 1, 8, 0);
				index++;
			}

			if (area > 16 && x>152 && x<242 && y>122 && y<200)//中心孔
			{
				// 外接矩形
				Rect rect_print = Rect(x, y, w, h);
				rectangle(img_raw, rect_print, Scalar(0, 0, 255), 1, 8, 0);
				index++;
			}
			if (area > 16 && x>474 && x<564 && y>122 && y<200)//中心孔
			{
				// 外接矩形
				Rect rect_print = Rect(x, y, w, h);
				rectangle(img_raw, rect_print, Scalar(0, 0, 255), 1, 8, 0);
				index++;
			}
			if (area > 16 && x>792 && x<880 && y>122 && y<200)//中心孔
			{
				// 外接矩形
				Rect rect_print = Rect(x, y, w, h);
				rectangle(img_raw, rect_print, Scalar(0, 0, 255), 1, 8, 0);
				index++;
			}
			if (area > 16 && x>1114 && x<1202 && y>122 && y<200)//中心孔
			{
				// 外接矩形
				Rect rect_print = Rect(x, y, w, h);
				rectangle(img_raw, rect_print, Scalar(0, 0, 255), 1, 8, 0);
				index++;
			}
		}
		vector<string> img_name2;
		img_name2 = SplitStrToVec(img_names[m], ".");
		if (index > 0)
		{
			imwrite("NEWNG/" + img_names[m], img_raw);//原图
			//imwrite("NEWNG/" + img_name2[0] + "_." + img_name2[1], img_test_copy); //黑白图
		} 
		else
		{
			imwrite("NEWOK/"  + img_names[m], img_raw);//原图
			//imwrite("NEWOK/" + img_name2[0] + "_." + img_name2[1], img_test_copy); //黑白图
		}
		
	}
}


