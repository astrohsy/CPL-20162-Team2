#include "calibration.h"
#include <fstream>

const int w = 8;
const int h = 5;
const float mm = 3.5;
const string src = "rgb.bmp";
const string dst = "camera.bmp";

void main()
{
	int board_w, board_h;
	float measure;
	Size imageSize;
	string src_name, dst_name;

	vector< vector< Point2f> > imagePoints;
	vector< vector< Point3f> > objectPoints;

	cout << "How many cross points of width direction?" << endl;
	cin >> board_w;
	cout << "How many cross points of Height direction?" << endl;
	cin >> board_h;
	cout << "What mm ?" << endl;
	cin >> measure;

	cout << "What is source picture name?" << endl;
	cin >> src_name;
	cout << "What is destination picture name?" << endl;
	cin >> dst_name;

	Mat H = calibration(board_w, board_h, measure, src_name, dst_name);
	Mat result = changePicture(src_name, dst_name, H);

	ofstream fout;
	fout.open("cali.Mat");

	fout << H;

	//imshow("result", result);
	imwrite("result.bmp", result);
	system("pause");
}