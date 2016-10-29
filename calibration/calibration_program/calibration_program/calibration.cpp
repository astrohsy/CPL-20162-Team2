#include "calibration.h"

Mat calibration(int board_w, int board_h, float measure, string src_name, string dst_name)
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

	Mat H = findHomography(src_corners, dst_corners);

	return H;
}

Mat changePicture(string src_name, string dst_name, Mat H)
{
	Mat src = imread(src_name);
	Mat dst = imread(dst_name);

	Mat ret;

	warpPerspective(src, ret, H, dst.size());
	return ret;
}