#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <stdlib.h>
#include <stdio.h>
using namespace std;
using namespace cv;

void main()
{
	int i, j;
	int board_w, board_h;
	int n_boards;
	float measure;
	Size imageSize;

	vector< vector< Point2f> > imagePoints;
	vector< vector< Point3f> > objectPoints;

	printf("How many cross points of width direction? \n");
	scanf("%d", &board_w);
	printf("How many cross points of Height direction? \n");
	scanf("%d", &board_h);

	printf("How many board? (board will be read by this namimg-> ./pattern/p1.jpg, ./pattern/p2.jpg...\n");
	scanf("%d", &n_boards);

	printf("What mm ?\n");
	scanf("%f", &measure);

	printf("w=%d h=%d n=%d %lfmm\n", board_w, board_h, n_boards, measure);
	printf("-----------FindChessboardCorner------------\n");
	char str[100];
	for (int i = 0; i < n_boards; ++i)
	{
		//image load   
		sprintf(str, "c1.bmp");
		printf("%s\n", str);
		Mat img2;
		Mat img = imread(str);
		imageSize = Size(img.cols, img.rows);
		Mat gray;
		cvtColor(img, gray, CV_RGB2GRAY);
		img.copyTo(img2);
		vector< Point2f> corners;

		//체스보드 코너점을 찾기 findChessboardCorners(inputArray image, Size patternSize, OutputArray corners)
		bool sCorner = findChessboardCorners(gray, Size(board_w, board_h), corners);

		//if find corner success, then   
		if (sCorner)
		{
			// 코너점 구체화 cornerSubPix(InputArray image, InputoutputArray corners, Size winSize, Size zeroZone, termCriteria criteria)
			cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
			// 점 그리기 , 쭈욱이어서 drawChessboardCorners(InputOutputArray image, Size patternSize, InputArray corners, bool patternWasFound)
			drawChessboardCorners(img2, Size(board_w, board_h), corners, sCorner);
			if (corners.size() == board_w*board_h) // 코너점의 수가 초기에 입력한수와 같다면
			{
				vector< Point2f> v_tImgPT;
				vector< Point3f> v_tObjPT;
				//save 2d coordenate and world coordinate   
				for (int j = 0; j < corners.size(); ++j)
				{
					Point2f tImgPT;
					Point3f tObjPT;
					// 코너의 좌표 x,y
					tImgPT.x = corners[j].x;
					tImgPT.y = corners[j].y;

					tObjPT.x = j%board_w*measure;
					tObjPT.y = j / board_w*measure;
					tObjPT.z = 0;

					v_tImgPT.push_back(tImgPT);
					v_tObjPT.push_back(tObjPT);
				}
				imagePoints.push_back(v_tImgPT);
				objectPoints.push_back(v_tObjPT);
			}
			sprintf(str, "c1.bmp");
			imwrite(str, img2);
			imshow("pattern", img2);
			cvWaitKey(10);
		}
	}
	vector< Mat> rvecs, tvecs;
	double reproj_err;
	Mat intrinsic_Matrix(3, 3, CV_64F); // 카메라 매트릭스
	Mat distortion_coeffs(8, 1, CV_64F); // 왜곡 계수
	reproj_err = calibrateCamera(objectPoints, imagePoints, imageSize, intrinsic_Matrix, distortion_coeffs, rvecs, tvecs);

	//결과
	printf("-----------Undistorted images------------\n");
	for (int i = 0; i < n_boards; ++i)
	{
		sprintf(str, "c1.bmp");
		printf("%s\n", str);
		Mat img4; //결과
		Mat img3 = imread(str);
		imageSize = Size(img3.cols, img3.rows);
		undistort(img3, img4, intrinsic_Matrix, distortion_coeffs);
		sprintf(str, "c1.bmp");
		imwrite(str, img4);
	}

	//
	printf("--------Calibration Parameters---------\n");
	printf("Forcal length :");
	for (i = 0; i < intrinsic_Matrix.rows; ++i)
	{
		for (j = 0; j < intrinsic_Matrix.cols; ++j)
		{
			if ((i == 0 && j == 0) || (i == 1 && j == 1)){
				printf("  %lf", intrinsic_Matrix.at<  double >(i, j));
			}
		}
	}
	printf("\n");
	printf("Prin cipal Point :");
	for (i = 0; i < intrinsic_Matrix.rows; ++i)
	{
		for (j = 0; j < intrinsic_Matrix.cols; ++j)
		{
			if ((i == 0 && j == 2) || (i == 1 && j == 2)){
				printf("  %lf", intrinsic_Matrix.at<  double >(i, j));
			}
		}
	}
	printf("\n");

	printf("Distortion :");
	for (i = 0; i < distortion_coeffs.rows; ++i)
	{
		for (j = 0; j < distortion_coeffs.cols; ++j)
		{
			printf("  %lf", distortion_coeffs.at<  double >(i, j));
		}
	}
	printf("\n");
	printf("Reprojection Error : %lf \n", reproj_err);
}