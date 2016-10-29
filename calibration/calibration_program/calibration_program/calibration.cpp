#include "calibration.h"

void calibration(int board_w, int board_h, float measure, string src_name, string dst_name)
{
	char str[MAXSTR];
	vector< vector< Point2f> > imagePoints;
	vector< vector< Point3f> > objectPoints;
	Size imageSize;
	
	//image load   
	Mat gray;

	Mat srcimg2;
	Mat srcimg = imread(src_name);
	imageSize = Size(srcimg.cols, srcimg.rows);
	cvtColor(srcimg, gray, CV_RGB2GRAY);
	srcimg.copyTo(srcimg2);

	Mat dstimg2;
	Mat dstimg = imread(dst_name);
	imageSize = Size(dstimg.cols, dstimg.rows);
	cvtColor(dstimg, gray, CV_RGB2GRAY);
	dstimg.copyTo(dstimg2);

	vector< Point2f> corners;

	//체스보드 코너점을 찾기 findChessboardCorners(inputArray image, Size patternSize, OutputArray corners)
	bool sCorner = findChessboardCorners(gray, Size(board_w, board_h), corners);

	//if find corner success, then   
	if (sCorner)
	{
		// 코너점 구체화 cornerSubPix(InputArray image, InputoutputArray corners, Size winSize, Size zeroZone, termCriteria criteria)
		cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
	}
}