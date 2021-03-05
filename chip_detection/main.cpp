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

	//get_pix_min_max("D:\\work\\Resource\\nanjing_chip\\3_new_template", "D:\\work\\Resource\\nanjing_chip\\3_new_min_max");
	//test_in_min_max("D:\\work\\Resource\\nanjing_chip\\4_result", "D:\\work\\Resource\\nanjing_chip\\3_new_cut_resize", "4_result");
	
	string img_path = "D:\\OCR\\bin\\blurred\\3-12_bin.bmp";
	Mat img = imread(img_path,0);
	//add_mask(img);
	//test(img);

	//get_blurred_imgs(R"(D:\work\Projects\huawei_chip_detection\huawei_chip_detection\picture)");

	//charsSegmentDebug("3_new_cut_resize", "img_chars");
	/*
	string path = "D:\\OCR\\template\\nanjing_ocr.pb";//"D:\\work\\Projects\\nanjing_chip\\chip_detection\\nanjing_ocr.pb";
	Net net = readNetFromTensorflow(path);
	string imgs_path = "D:\\OCR\\bin\\sub";//"D:\\work\\Resource\\nanjing_chip\\rename";
	string img_path = "D:\\work\\Resource\\nanjing_chip\\rename\\01_06.png";
	Mat img = imread(img_path);
	net.setPreferableTarget(DNN_TARGET_CUDA);
	net.setPreferableBackend(DNN_BACKEND_CUDA);
	cout << "模型加载成功" << endl;
	imgsRenameDebug(imgs_path, "rename.txt", img_path, net);
	//string img_name = getImgName(img, net);
	//cout << img_name << endl;
	//getOcrLocation(img);
	
	char *buffer;
	//也可以将buffer作为输出参数，获取当前文件目录
	if ((buffer = _getcwd(NULL, 0)) != NULL)
	{

		string str = buffer;
		printf("%s\n", buffer);
		cout << str << endl;
		free(buffer);
	}
	
	writeExcel();
*/
	system("pause");
}