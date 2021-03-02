#include "preprocess.h"
#include "findfile.h"
#include "ocr.h"


//#include <cassert>



void main() 
{

	//const char* find_path = "new_picture/3_new/";

	//vector<string> img_path_name;
	//fileSearch(find_path, img_path_name);
	//for (int i = 11; i < img_path_name.size(); i++)
	//{
	//	img_draw_rect_debug(find_path + img_path_name[i], " ");
	//}

	//img_segment("new_picture/3_new_", "cut_resize");
	//img_affine("3_cut_resize", "3_cut_correct");

	//get_pix_min_max("3_new_template", "4_result");
	//test_in_min_max("4_result", "NG", "4_result");
	
	//charsSegmentDebug("3_new_cut_resize", "img_chars");

	/*
	string path = "D:\\work\\Projects\\nanjing_chip\\chip_detection\\nanjing_ocr.pb";
	Net net = readNetFromTensorflow(path);
	string imgs_path = "D:\\work\\Projects\\nanjing_chip\\chip_detection\\rename";
	string img_path = "D:\\work\\Projects\\nanjing_chip\\chip_detection\\rename\\01_06.png";
	Mat img = imread(img_path);
	net.setPreferableTarget(DNN_TARGET_CUDA);
	net.setPreferableBackend(DNN_BACKEND_CUDA);
	cout << "模型加载成功" << endl;
	imgsRenameDebug(imgs_path, "rename.txt", img_path, net);
	//string img_name = getImgName(img, net);
	//cout << img_name << endl;
	//getOcrLocation(img);*/
	char *buffer;
	//也可以将buffer作为输出参数
	if ((buffer = _getcwd(NULL, 0)) == NULL)
	{
		perror("getcwd error");
	}
	else
	{
		string str = buffer;
		printf("%s\n", buffer);
		cout << str << endl;
		free(buffer);
	}

	system("pause");
}