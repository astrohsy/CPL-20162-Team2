#include "calibration.h"

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

	calibration(board_w, board_h, measure, src_name, dst_name);
}