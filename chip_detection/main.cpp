#include "preprocess.h"
#include "findfile.h"
#include "ocr.h"

void main() 
{
	//getRawImgs(R"(D:\work\Darknet\new_scratch_detection\data\OK2_result)", R"(D:\work\Darknet\new_scratch_detection\data\OK2)");

	//const char* find_path = R"(D:\work\Resource\nanjing_chip\temp_cut\)";

	//vector<string> img_path_name;
	//fileSearch(find_path, img_path_name);
	//for (int i = 0; i < img_path_name.size(); i++)
	//{
	//	img_draw_rect_debug(find_path + img_path_name[i], " ");
	//}

	//img_segment(R"(D:\work\Resource\nanjing_chip\new_picture\temp)", R"(D:\work\Resource\nanjing_chip\temp_cut)");
	//img_affine("3_cut_resize", "3_cut_correct");

	get_pix_min_max("D:\\work\\Resource\\nanjing_chip\\temp_0", "D:\\work\\Resource\\nanjing_chip\\temp_min_max");
	//test_in_min_max("D:\\work\\Resource\\nanjing_chip\\4_result", "D:\\work\\Resource\\nanjing_chip\\3_new_cut_resize", "4_result");
	//analyze_imgs("D:\\work\\Resource\\nanjing_chip\\temp_min_max", R"(D:\work\Resource\nanjing_chip\new_picture\temp_ok\OK)", "4_result");
	//find_scratch(R"(D:\work\Darknet\new_scratch_detection\data\img_test)");

	//string img_path = R"(D:\work\Resource\nanjing_chip\new_picture\temp\0.736_-9.207.png)";

	//Mat img_ok = imread(R"(D:\work\Resource\nanjing_chip\point\ok1.png)");
	//Mat img_ng = imread(R"(D:\work\Resource\nanjing_chip\point\ng2.png)");
	//add_mask(img);
	//test(img);
	//get_blurred_imgs(R"(D:\work\Resource\nanjing_chip\new_picture\temp)");
	//generateImg(img_ok,img_ng);
	//charsSegmentDebug(R"(D:\work\Resource\nanjing_chip\temp_cut)", R"(D:\work\Resource\nanjing_chip\_)");

#if 0
	//print_flag();
	string path = "D:\\work\\Py_Projects\\ocr\\src\\nanjing_point\\point.pb";
	Net net = readNetFromTensorflow(path);
	string imgs_path = R"(D:\OCR\bin\sub)";
	string img_path = "D:\\work\\Resource\\nanjing_chip\\rename\\01_06.png";
	Mat img = imread(img_path);
	net.setPreferableTarget(DNN_TARGET_CUDA);
	net.setPreferableBackend(DNN_BACKEND_CUDA);
	cout << "模型加载成功" << endl;
	//imgsRenameDebug(imgs_path, "rename.txt", img_path, net);
	//charsClassificationDebug(R"(D:\work\Resource\nanjing_chip\img_chars)", R"(D:\work\Resource\nanjing_chip\img_char)", net);
	getPointsAccuracy(R"(D:\work\Resource\nanjing_chip\PointRaw)", net);
#endif
	//string img_name = getImgName(img, net);
	//cout << img_name << endl;
	//getOcrLocation(img);
	/*
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

