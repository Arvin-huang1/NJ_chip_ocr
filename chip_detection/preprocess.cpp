#include "preprocess.h"
#include "findfile.h"


//�ָ���ͼ
void img_segment(string raw_picture_path, string save_path)
{
	static int img_part_num = 0;
	//�ļ��в����ڣ������ļ���
	if (_access(save_path.c_str(), 0) == -1)
	{
		_mkdir(save_path.c_str());
	}

	vector<string> img_names;
	fileSearch(raw_picture_path.data(), img_names);//��ȡ����ͼƬ����
	for (int m = 0; m < img_names.size(); m++)
	{
		cout << img_names[m] << endl;
	}
	//�ָ���ͼ
	for (int m = 0; m < img_names.size(); m++)
	{
		cout << m << endl;
		Mat img = imread(raw_picture_path + "/" + img_names[m]);
		cout << img_names[m] << endl;
		assert(!img.empty());//��ȡͼƬ�Ƿ�ɹ�

		Mat img_gray, img_gray_bin;//ԭͼ�ĻҶ�ͼ�Ͷ�ֵͼ
								   //��ͼ��ת�ɶ�ֵͼ������ͳ����ͨ��

		cvtColor(img, img_gray, COLOR_BGR2GRAY);
		Scalar mean_pix = mean(img_gray); //��ȡ�Ҷ�ͼ����ƽ��ֵ
		threshold(img_gray, img_gray_bin, 150, 255, THRESH_BINARY);

		Mat out, stats, centroids;//��ȡ��ͨ��ͳ����Ϣ

		vector<Rect> rect;//�洢��ͼ��������
		//namedWindow("img", 0);
		//imshow("img", img_gray_bin);
		//waitKey(0);
		int number = connectedComponentsWithStats(img_gray_bin, out, stats, centroids, 8, CV_16U);
		int index = 0;
		//��ȡ��ͼ��ͨ�����
		for (int i = 1; i < number; i++)
		{
			// ����λ��
			double center_x = centroids.at<double>(i, 0);
			double center_y = centroids.at<double>(i, 1);
			//���α߿�
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
				// ��Ӿ���
				rect.push_back(Rect(x-4, y - 4 , w+8 , h+8));
				//rectangle(img_copy, rect_flag, Scalar(0, 255, 255), 4, 8, 0);
				cout << "rect_flag: " << rect[index] << endl;
				cout << "index: " << index << ",area: " << area << endl;
				index++;
			}

		}

		vector<Mat> img_part; //�洢��ͼ
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


//��ͼ����任
void img_affine(string file_path, string save_path)
{
	//�ļ��в����ڣ������ļ���
	if (_access(save_path.c_str(), 0) == -1)
	{
		_mkdir(save_path.c_str());
	}
	static int img_part_num = 0;
	vector<string> img_names;
	fileSearch(file_path.data(), img_names);//��ȡ����ͼƬ����

	for (int m =0; m < img_names.size(); m++)
	{
		Mat img = imread(file_path + "/" + img_names[m]);
		assert(!img.empty());

		Mat img_gray, img_bin, img_copy;
		cvtColor(img, img_gray, COLOR_BGR2GRAY);
		img.copyTo(img_copy);
		Scalar mean_pix = mean(img); //��ȡ�Ҷ�ͼ����ƽ��ֵ						 
		threshold(img_gray, img_bin, 74, 255, THRESH_BINARY); //��ֵ��

		vector<vector<Point> > contours;
		vector<Rect> boundRect(contours.size());
		//ע���3������ΪRETR_EXTERNAL��ֻ�������  
		findContours(img_bin, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE); //������

		//cout << contours.size() << endl;

		for (int i = 0; i < contours.size(); i++)
		{
			float min_point = 500;
			//��Ҫ��ȡ������  
			Point2f rectpoint[4];
			RotatedRect rect = minAreaRect(Mat(contours[i]));
			rect.points(rectpoint);//��ȡ4����������
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
			//��ˮƽ�ߵĽǶ�  
			float angle = rect.angle;
			

			int line1 = sqrt((rectpoint[1].y - rectpoint[0].y)*(rectpoint[1].y - rectpoint[0].y) + (rectpoint[1].x - rectpoint[0].x)*(rectpoint[1].x - rectpoint[0].x));
			int line2 = sqrt((rectpoint[3].y - rectpoint[0].y)*(rectpoint[3].y - rectpoint[0].y) + (rectpoint[3].x - rectpoint[0].x)*(rectpoint[3].x - rectpoint[0].x));
			//rectangle(img_copy, rectpoint[0], rectpoint[3], Scalar(255), 2);
			//���̫С��ֱ��pass
			if (line1 * line2 < 10000 )
			{
				continue;
			}
			cout << "rect" << rectpoint[0] << rectpoint[1] << rectpoint[2] << rectpoint[3] << endl;
			cout << angle << endl;
			//Ϊ���������κ��ŷţ�������ת�Ƕ��ǲ�һ���ġ����ŵģ�������90�ȣ�������
			if (line1 > line2)
			{
				angle = 90 + angle;
			}

			//����һ������任���ͼ��  
			Mat img_correction(img.rows, img.cols, CV_8UC1);
			img_correction.setTo(0);
			//��RoiSrcImg������ת  
			Point2f center = rect.center;  //���ĵ�  
			Mat M2 = getRotationMatrix2D(center, angle, 1);//������ת�����ŵı任���� 
			warpAffine(img, img_correction, M2, img.size(), 1, 0, Scalar(0));//����任 
			//rectangle(img_correction, rect_inner, Scalar(0, 0, 255), 3, 8, 0);
			//namedWindow("dst1", 0);
			//imshow("dst1", img_correction);
			//waitKey(0);
			//destroyAllWindows();
			imwrite(save_path + "/" + img_names[m], img_correction); //�洢����任���ͼƬ
			Mat img_correction_gray, img_correction_bin;
			Mat img_resize;
			Mat out, stats, centroids;//��ȡ��ͨ��ͳ����Ϣ
			int number, index;
			cvtColor(img_correction, img_correction_gray, COLOR_BGR2GRAY);
			threshold(img_correction_gray, img_correction_bin, 74, 255, THRESH_BINARY); //��ֵ��
			number = connectedComponentsWithStats(img_correction_bin, out, stats, centroids, 8, CV_16U);
			index = 0;
			//��ȡ��ͼ��ͨ�����
			for (int i = 1; i < number; i++)
			{
				// ����λ��
				double center_x = centroids.at<double>(i, 0);
				double center_y = centroids.at<double>(i, 1);
				//���α߿�
				int x = stats.at<int>(i, CC_STAT_LEFT);
				int y = stats.at<int>(i, CC_STAT_TOP);
				int w = stats.at<int>(i, CC_STAT_WIDTH);
				int h = stats.at<int>(i, CC_STAT_HEIGHT);
				int area = stats.at<int>(i, CC_STAT_AREA);
				//if (area > 10000 && area < 20000 && (x + w) < 4090 && (y + h) < 2950 && w > 420 && h > 370)//2
				if (area > 10000&& x < 10)//3 
				{
					// ��Ӿ���
					Rect rect_print = Rect(x, y, w, h);
					//cout << m << ",rect_print: " << rect_print << endl;
					cout << "m: " << m <<"part_num" << img_part_num << ",area: " << area << endl;
					//resize(img_correction(rect_print), img_resize, Size(424, 374));//2
					resize(img_correction(rect_print), img_resize, Size(1342, 381));//3
					imwrite(save_path + "/"+ img_names[m], img_resize); //�洢����任���ͼƬ
					index++;
					img_part_num++;
				}

			}
		}

	}
}


//���ݵ�һ��ͼ��ȡ��Բ��ģ�廭��
void img_draw_rect(string file_path, string picture_path, string picture_name, string save_path, int picture_index)
{
	//�ļ��в����ڣ������ļ���
	if (_access(save_path.c_str(), 0) == -1)
	{
		_mkdir(save_path.c_str());
	}


	Mat img = imread(picture_path);//
	assert(!img.empty());
	resize(img, img, Size(844, 974));
	Mat img_copy;
	Mat img_gray, img_gray_bin;//ԭͼ�ĻҶ�ͼ�Ͷ�ֵͼ
	Mat out, stats, centroids;

	int number;//��¼ͼ�����е���ͨ��
	int index;//��¼ͼ����Ч����ͨ��

	Scalar mean_pix;//��ȡ�Ҷ�ͼ����ƽ��ֵ
	img.copyTo(img_copy);

	if (_access(file_path.c_str(), 0) != 0 && picture_index == 0)//�ļ�������
	{
		ofstream outfile(file_path, ios::out);
		assert(outfile.is_open());				//��ʧ��,�����������Ϣ,����ֹ��������

		cvtColor(img, img_gray, COLOR_BGR2GRAY);
		mean_pix = mean(img_gray); //��ȡ�Ҷ�ͼ����ƽ��ֵ
		threshold(img_gray, img_gray_bin, mean_pix[0] * 1.1, 255, THRESH_BINARY);
		number = connectedComponentsWithStats(img_gray_bin, out, stats, centroids, 8, CV_16U);
		index = 0;
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
			if (area > 50 && area < 350 && x > 35 && x < 815 && y > 172 && y < 808)
			{
				index++;
				// ��Ӿ���
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
	infile.open(file_path, ios::in);	//�����ļ�
	assert(infile.is_open());//��ʧ��,�����������Ϣ,����ֹ��������
	/*
	//�����ƫ��У��
	int x_min = 500, y_min = 500, x_max = 500, y_max = 500;//��¼ÿ��ͼ�����Ϸ���׵�λ��
	static int x_min_0 = 500, y_min_0 = 500, x_max_0 = 500, y_max_0 = 500;//��¼�궨ͼ��3����

																		  //��¼��궨ͼ��ƫ��
	cvtColor(img, img_gray, COLOR_BGR2GRAY);
	mean_pix = mean(img_gray); //��ȡ�Ҷ�ͼ����ƽ��ֵ
	threshold(img_gray, img_gray_bin, mean_pix[0] * 0.85, 255, THRESH_BINARY);
	number = connectedComponentsWithStats(img_gray_bin, out, stats, centroids, 8, CV_16U);
	index = 0;
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
			// ��Ӿ���
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

	//��ȡ��������,����
	int center_x, center_y;
	index = 0;
	while (!infile.eof())
	{
		infile >> center_x >> center_y;//��ȡһ������
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

//������ͨ�򻭿�������
void img_draw_rect_debug(string file_path, string save_path)
{
	////�ļ��в����ڣ������ļ���
	//if (_access(save_path.c_str(), 0) == -1)
	//{
	//	_mkdir(save_path.c_str());
	//}

	Mat img = imread(file_path);
	assert(!img.empty());
	
	
	for (int j = 147; j < 255 ; j++,j++,j++)
	{
		Mat img_copy;
		Mat img_gray, img_gray_bin;//ԭͼ�ĻҶ�ͼ�Ͷ�ֵͼ


		int number;//��¼ͼ�����е���ͨ��
		int index;//��¼ͼ����Ч����ͨ��

		Scalar mean_pix;//��ȡ�Ҷ�ͼ����ƽ��ֵ

		img.copyTo(img_copy);

		Mat out, stats, centroids;

		cvtColor(img, img_gray, COLOR_BGR2GRAY);
		mean_pix = mean(img_gray); //��ȡ�Ҷ�ͼ����ƽ��ֵ
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
			// ����λ��
			int center_x = centroids.at<double>(i, 0);
			int center_y = centroids.at<double>(i, 1);
			//���α߿�
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
				// ��Ӿ���
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


//��ȡԭͼ�ĻҶ�ͼ�Ͷ�ֵͼ
void img_gray_and_bin(string file_path, string gray_path, string bin_path)
{
	//�ļ��в����ڣ������ļ���
	if (_access(gray_path.c_str(), 0) == -1)
	{
		_mkdir(gray_path.c_str());
	}
	//�ļ��в����ڣ������ļ���
	if (_access(bin_path.c_str(), 0) == -1)
	{
		_mkdir(bin_path.c_str());
	}
	vector<string> img_names;
	fileSearch(file_path.data(), img_names);//��ȡ����ͼƬ����

	for (int m = 0; m < img_names.size(); m++)
	{
		cout << m << endl;
		Mat img = imread(file_path + "/" + img_names[m]);//
		assert(!img.empty());
		Mat img_copy;
		Mat img_gray, img_gray_bin;//ԭͼ�ĻҶ�ͼ�Ͷ�ֵͼ
		Scalar mean_pix;
		cvtColor(img, img_gray, COLOR_BGR2GRAY);
		mean_pix = mean(img_gray); //��ȡ�Ҷ�ͼ����ƽ��ֵ
		threshold(img_gray, img_gray_bin, mean_pix[0] * 0.85, 255, THRESH_BINARY);

		imwrite(gray_path + "/" + to_string(m) + ".bmp", img_gray);
		imwrite(bin_path + "/" + to_string(m) + ".bmp", img_gray_bin);
	}
}


//ʹ�ö�㵥ӳ�䣬����
void img_draw_rect_final(string txt_path, string picture_path, string save_perspective_path, string save_result_path, string save_point_path)
{

	vector<int> vec_x[18];//����ͼƬ�������������
	vector<int> vec_y[18];//����ͼƬ�������������

	vector<string> img_names;
	fileSearch(picture_path.data(), img_names);//��ȡ����ͼƬ����

	//��ȡ����ͼƬ�������������
	for (int m = 0; m < img_names.size(); m++)
	{
		Mat img = imread(picture_path + "/" + img_names[m]);//
		assert(!img.empty());
		Mat img_copy;
		Mat img_gray, img_gray_bin;//ԭͼ�ĻҶ�ͼ�Ͷ�ֵͼ
		Mat out, stats, centroids;

		int number;//��¼ͼ�����е���ͨ��
		int index;//��¼ͼ����Ч����ͨ��

		Scalar mean_pix;//��ȡ�Ҷ�ͼ����ƽ��ֵ
		img.copyTo(img_copy);

		//ifstream infile;
		//infile.open(txt_path, ios::in);	//�����ļ�
		//assert(infile.is_open());//��ʧ��,�����������Ϣ,����ֹ��������

		cvtColor(img, img_gray, COLOR_BGR2GRAY);
		mean_pix = mean(img_gray); //��ȡ�Ҷ�ͼ����ƽ��ֵ
		threshold(img_gray, img_gray_bin, mean_pix[0] * 0.85, 255, THRESH_BINARY);
		number = connectedComponentsWithStats(img_gray_bin, out, stats, centroids, 8, CV_16U);
		index = 0;

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
			if (area > 30 && area < 150 && x > 150 && x < 750 && y > 50 && y < 730)
			{
				index++;
				vec_x[m].push_back(center_x);
				vec_y[m].push_back(center_y);

			}

		}

	}

	vector<int> vec_x_form[18];//��ű�׼ͼ��������꣬��0.bmp
	vector<int> vec_y_form[18];
	vector<int> vec_x_match[18];//���������Ҫӳ��ͼƬ�ķ��������
	vector<int> vec_y_match[18];

	//��ȡ͸�ӱ任�ĵ�
	for (int m = 0; m < size(vec_x); m++)
	{
		if (m == 0)//��׼ͼ����
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

	vec_x[0].swap(vector<int>());//��պ��ű�׼ͼ�����ķ��������
	vec_y[0].swap(vector<int>());//��պ��ű�׼ͼ�����ķ��������

	ifstream infile;
	infile.open(txt_path, ios::in);	//�����ļ�
	assert(infile.is_open());//��ʧ��,�����������Ϣ,����ֹ��������

							 //��ȡ��������,����
	int center_x, center_y;
	int index = 0;
	while (!infile.eof())
	{
		infile >> center_x >> center_y;//��ȡһ������
		if (index < 368)
		{
			vec_x[0].push_back(center_x);
			vec_y[0].push_back(center_y);
			index++;
		}
	}
	infile.close();

	//���ҵ���ͼ
	for (int m = 0; m < 18; m++)
	{
		//����͸�ӱ任
		Mat img = imread(picture_path + "/" + to_string(m) + ".bmp");
		Mat img_copy;
		Mat h;//�任����
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



		if (save_perspective_path != "") //��Ϊ�ձ���͸�ӱ任ͼƬ
		{
			//�ļ��в����ڣ������ļ���
			if (_access(save_perspective_path.c_str(), 0) == -1)
			{
				_mkdir(save_perspective_path.c_str());
			}
			imwrite(save_perspective_path + "/" + to_string(m) + ".bmp", img_copy);
		}

		//���������
		for (int j = 0; j < vec_x[0].size(); j++)
		{
			Rect rect(vec_x[0][j] - 16, vec_y[0][j] - 15, 32, 30);//������׻���
			rectangle(img_copy, rect, Scalar(0, 0, 255), 1, 8, 0);

			if (save_point_path != "") //��Ϊ�ձ��淢���������ͼƬ
			{
				//�ļ��в����ڣ������ļ���
				if (_access(save_point_path.c_str(), 0) == -1)
				{
					_mkdir(save_point_path.c_str());
				}
				imwrite(save_point_path + "/" + to_string(m * 368 + j) + ".bmp", img(rect));
			}
		}

		if (save_result_path != "") //��Ϊ�ձ�����ͼƬ
		{
			//�ļ��в����ڣ������ļ���
			if (_access(save_result_path.c_str(), 0) == -1)
			{
				_mkdir(save_result_path.c_str());
			}
			imwrite(save_result_path + "/" + to_string(m) + ".bmp", img_copy);
		}

		cout << m << endl;
	}
}


//��ȡϵ��ͼƬ���ص����ֵ����Сֵ
void get_pix_min_max(string file_path, string save_path)
{
	//�ļ��в����ڣ������ļ���
	if (_access(save_path.c_str(), 0) == -1)
	{
		_mkdir(save_path.c_str());
	}
	vector<string> img_names;
	fileSearch(file_path.data(), img_names);//��ȡ����ͼƬ����
	vector<Mat> imgs;
	Mat img;
	//�ָ���ͼ
	for (int m = 0; m < img_names.size(); m++)
	{
		img = imread(file_path + "/" + img_names[m], IMREAD_GRAYSCALE);
		assert(!img.empty());
		imgs.push_back(img);
	}

	Mat pix_min, pix_max;
	imgs[0].copyTo(pix_min);
	imgs[0].copyTo(pix_max);
	//��ȡpix_min����pix_max
	for (int k = 0; k < size(imgs); k++)//�С���
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


//����ͼƬ
void test_in_min_max(string min_max_path, string file_path, string save_path)
{
	//�ļ��в����ڣ������ļ���
	if (_access(save_path.c_str(), 0) == -1)
	{
		_mkdir(save_path.c_str());
	}

	Mat pix_min, pix_max;
	pix_min = imread(min_max_path + "/pix_min.bmp", IMREAD_GRAYSCALE);
	pix_max = imread(min_max_path + "/pix_max.bmp", IMREAD_GRAYSCALE);
	assert(!pix_min.empty() && !pix_max.empty());

	vector<string> img_names;
	fileSearch(file_path.data(), img_names);//��ȡ����ͼƬ����

	//�ָ���ͼ
	for (int m = 0; m < img_names.size(); m++)
	{

		//�����ͼƬ
		Mat img_raw = imread(file_path + "/" + img_names[m], 1);
		Mat img_test = imread(file_path + "/" + img_names[m], 0);

		Mat img_test_copy(img_test.rows, img_test.cols, CV_8UC1); //ע���������ѡCV_8UC1
		img_test_copy.setTo(0); //��ɫ������Ϊ��ɫ  

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

		

		Mat out, stats, centroids;//��ȡ��ͨ��ͳ����Ϣ
		int number, index;
		Mat element = getStructuringElement(MORPH_RECT, Size(4,4));
		dilate(img_test_copy, img_test_copy, element);
		
		threshold(img_test_copy, img_test_copy, 82, 255, THRESH_BINARY); //��ֵ��
		number = connectedComponentsWithStats(img_test_copy, out, stats, centroids, 8, CV_16U);
		index = 0;
		//��ȡ��ͼ��ͨ�����
		for (int i = 1; i < number; i++)
		{
			// ����λ��
			double center_x = centroids.at<double>(i, 0);
			double center_y = centroids.at<double>(i, 1);
			//���α߿�
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
				// ��Ӿ���
				Rect rect_print = Rect(x, y, w, h);
				rectangle(img_raw, rect_print, Scalar(0, 0, 255), 1, 8, 0);
				index++;
			}

			if (area > 16 && x>152 && x<242 && y>122 && y<200)//���Ŀ�
			{
				// ��Ӿ���
				Rect rect_print = Rect(x, y, w, h);
				rectangle(img_raw, rect_print, Scalar(0, 0, 255), 1, 8, 0);
				index++;
			}
			if (area > 16 && x>474 && x<564 && y>122 && y<200)//���Ŀ�
			{
				// ��Ӿ���
				Rect rect_print = Rect(x, y, w, h);
				rectangle(img_raw, rect_print, Scalar(0, 0, 255), 1, 8, 0);
				index++;
			}
			if (area > 16 && x>792 && x<880 && y>122 && y<200)//���Ŀ�
			{
				// ��Ӿ���
				Rect rect_print = Rect(x, y, w, h);
				rectangle(img_raw, rect_print, Scalar(0, 0, 255), 1, 8, 0);
				index++;
			}
			if (area > 16 && x>1114 && x<1202 && y>122 && y<200)//���Ŀ�
			{
				// ��Ӿ���
				Rect rect_print = Rect(x, y, w, h);
				rectangle(img_raw, rect_print, Scalar(0, 0, 255), 1, 8, 0);
				index++;
			}
		}
		vector<string> img_name2;
		img_name2 = SplitStrToVec(img_names[m], ".");
		if (index > 0)
		{
			imwrite("NEWNG/" + img_names[m], img_raw);//ԭͼ
			//imwrite("NEWNG/" + img_name2[0] + "_." + img_name2[1], img_test_copy); //�ڰ�ͼ
		} 
		else
		{
			imwrite("NEWOK/"  + img_names[m], img_raw);//ԭͼ
			//imwrite("NEWOK/" + img_name2[0] + "_." + img_name2[1], img_test_copy); //�ڰ�ͼ
		}
		
	}
}


