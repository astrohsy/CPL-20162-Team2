#include "calibration.h"

void printCalibrationResult(int n_boards, Size imageSize, double reproj_err, Mat intrinsic_Matrix, Mat distortion_coeffs)
{
	char str[MAXSTR];

	//결과
	cout << "---------- - Undistorted images------------" << endl;
	for (int i = 0; i < n_boards; ++i)
	{
		sprintf(str, filePath, i + 1);
		cout << str << endl;
		Mat img4; //결과
		Mat img3 = imread(str);
		imageSize = Size(img3.cols, img3.rows);
		undistort(img3, img4, intrinsic_Matrix, distortion_coeffs);
		sprintf(str, "Undistorted images%d.jpg", i + 1);
		imwrite(str, img4);
	}

	//
	cout << "--------Calibration Parameters---------" << endl;
	cout << "Forcal length :" << endl;
	for (int i = 0; i < intrinsic_Matrix.rows; ++i)
	{
		for (int j = 0; j < intrinsic_Matrix.cols; ++j)
		{
			if ((i == 0 && j == 0) || (i == 1 && j == 1)){
				cout << "  " << intrinsic_Matrix.at<  double >(i, j);
			}
		}
	}
	cout << endl;

	cout << "Prin cipal Point :";
	for (int i = 0; i < intrinsic_Matrix.rows; ++i)
	{
		for (int j = 0; j < intrinsic_Matrix.cols; ++j)
		{
			if ((i == 0 && j == 2) || (i == 1 && j == 2)){
				cout << "  " << intrinsic_Matrix.at<  double >(i, j);
			}
		}
	}
	cout << endl;

	cout << "Distortion :";
	for (int i = 0; i < distortion_coeffs.rows; ++i)
	{
		for (int j = 0; j < distortion_coeffs.cols; ++j)
		{
			cout << "  " << distortion_coeffs.at<  double >(i, j);
		}
	}
	cout << endl;

	cout << "Reprojection Error : " << reproj_err << endl;
}

void calibration(int board_w, int board_h, int n_boards, float measure)
{
	char str[MAXSTR];
	vector< vector< Point2f> > imagePoints;
	vector< vector< Point3f> > objectPoints;
	Size imageSize;

	for (int i = 0; i < n_boards; ++i)
	{
		//image load   
		sprintf(str, filePath, i + 1);
		cout << str << endl;
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
			sprintf(str, "DrawChessboardCorners%d.jpg", i + 1);
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

	printCalibrationResult(n_boards, imageSize, reproj_err, intrinsic_Matrix, distortion_coeffs);
}