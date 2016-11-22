#include "stdafx.h"

Calibration::Calibration()
{
	board_w = 0;
	board_h = 0;
	measure = 0;
	H = Mat::zeros(Size(0, 0), 0);
}

Calibration::Calibration(int w, int h, float mm)
{
	board_w = w;
	board_h = h;
	measure = mm;
	H = Mat::zeros(Size(0, 0), 0);
}

void Calibration::CalibrationInit(int w, int h, float mm)
{
	board_w = w;
	board_h = h;
	measure = mm;
	H = Mat::zeros(Size(0, 0), 0);
}

void Calibration::run_calibration(char * src_name, char * dst_name)
{
	vector< vector< Point2f> > imagePoints;
	vector< vector< Point3f> > objectPoints;
	Size imageSize;

	//image load   
	Mat srcgray;
	Mat srcimg2;
	Mat srcimg = imread(src_name);
	imageSize = Size(srcimg.cols, srcimg.rows);
	cvtColor(srcimg, srcgray, CV_RGB2GRAY);
	srcimg.copyTo(srcimg2);

	Mat dstgray;
	Mat dstimg2;
	Mat dstimg = imread(dst_name);
	imageSize = Size(dstimg.cols, dstimg.rows);
	cvtColor(dstimg, dstgray, CV_RGB2GRAY);
	dstimg.copyTo(dstimg2);

	vector< Point2f> src_corners;
	vector< Point2f> dst_corners;

	//체스보드 코너점을 찾기 findChessboardCorners(inputArray image, Size patternSize, OutputArray corners)
	bool sCorner = findChessboardCorners(srcgray, Size(board_w, board_h), src_corners);
	bool dCorner = findChessboardCorners(dstgray, Size(board_w, board_h), dst_corners);

	//if find corner success, then   
	if (sCorner)
	{
		// 코너점 구체화 cornerSubPix(InputArray image, InputoutputArray corners, Size winSize, Size zeroZone, termCriteria criteria)
		cornerSubPix(srcgray, src_corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		cornerSubPix(dstgray, dst_corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
	}

	H = findHomography(src_corners, dst_corners);
}

Mat Calibration::changePicture(char * srcf, char * dstf)
{
	Mat src = imread(srcf);
	Mat dst = imread(dstf);

	Mat ret;

	warpPerspective(src, ret, H, dst.size());
	return ret;
}

void Calibration::saveResult()
{
	ofstream fout;
	fout.open(result_name);

	fout << H.type() << endl;
	
	fout.precision(15);

	for (int i = 0; i < 3; i++){
		for (int k = 0; k < 3; k++){
			fout << H.at<double>(i, k) << endl;
		}
	}
}

Mat Calibration::getH()
{
	return H;
}

void Calibration::loadResult()
{
	ifstream fin;
	fin.open(result_name);
	
	int type;
	fin >> type;

	H = Mat::zeros(Size(3, 3), type);

	for (int i = 0; i < 3; i++){
		for (int k = 0; k < 3; k++){
			fin >> H.at<double>(i, k);
		}
	}
}