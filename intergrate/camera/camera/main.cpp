#pragma once
#pragma warning(disable:4996)

#include <pxcsensemanager.h>
#include <pxcsession.h>
#include <stdlib.h>
#include <iostream>
#include <conio.h>
#include <util_render.h>
#include <time.h>
#include <thread>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
/////////////////////// depth sensor
#include "camera.h"

#include <thread>
#include <future>
#include <string>
/////////////////////// rgb sensor
#define DEPTH_WIDTH 480
#define DEPTH_HEIGHT 360
#define RGB_WIDTH 1920
#define RGB_HEIGHT 1080
#define FRAME_RATE 30

using namespace std;
using namespace cv;

typedef struct
{
	bool isThere;
	int vtrNum;
} Vertice;
////////////////////// depth sensor

bool programExit;
bool takingPicture;
bool depthExit;
bool takingDepth;

bool inputDoing();
void evf(Camera dslr);
void ConvertPXCImageToOpenCVMat(PXCImage *inImg, Mat *outImg);
void mat2Ply(Mat image);
int depth_main(void);

///////////////////// rgb sesnor

int main()
{
	namedWindow("main", WINDOW_AUTOSIZE);
	Camera dslr;
	programExit = false;
	takingPicture = false;
	depthExit = false;
	takingDepth = false;

	// Initialize SDK
	dslr.initializeSDK();
	
	thread doingThread2(depth_main);//depth sensor 시작
	thread doingThread(inputDoing); //카메라 버튼대기 쓰레드

	evf(dslr);
	
	doingThread2.join();
	doingThread.join();
	dslr.releaseSDK();

	return 0;
}

bool inputDoing(){
	string doing;

	while (1){
		cin >> doing;
		if (doing == "z"){
			programExit = true;
			depthExit = true;
			break;
		}
		if (doing == "t"){
			takingPicture = true;
			takingDepth = true;
		}
	}

	return 1;
}

void evf(Camera dslr){
	dslr.setErr(dslr.startLiveview());
	if (dslr.isOK())
	{
		cout << "start live view" << endl;
	}

	// live view
	while (1){
		dslr.downloadEvfData();
		int c = cvWaitKey(10);

		if (takingPicture){
			takingPicture = false;
			dslr.takePicture();
		}

		if (programExit)
			break;
	}
	//

	dslr.setErr(dslr.endLiveview());
	if (dslr.isOK())
	{
		cout << "end live view" << endl;
	}
}
///////////////////////////////// rgb sensor

// PXCImage -> Mat로 변환하는 함수
void ConvertPXCImageToOpenCVMat(PXCImage *inImg, Mat *outImg) {
	int cvDataType;
	int cvDataWidth;


	PXCImage::ImageData data;
	inImg->AcquireAccess(PXCImage::ACCESS_READ, &data);
	PXCImage::ImageInfo imgInfo = inImg->QueryInfo();

	switch (data.format) {
		/* STREAM_TYPE_COLOR */
	case PXCImage::PIXEL_FORMAT_YUY2: /* YUY2 image  */
	case PXCImage::PIXEL_FORMAT_NV12: /* NV12 image */
		throw(0); // Not implemented
	case PXCImage::PIXEL_FORMAT_RGB32: /* BGRA layout on a little-endian machine */
		cvDataType = CV_8UC4;
		cvDataWidth = 4;
		break;
	case PXCImage::PIXEL_FORMAT_RGB24: /* BGR layout on a little-endian machine */
		cvDataType = CV_8UC3;
		cvDataWidth = 3;
		break;
	case PXCImage::PIXEL_FORMAT_Y8:  /* 8-Bit Gray Image, or IR 8-bit */
		cvDataType = CV_8U;
		cvDataWidth = 1;
		break;

		/* STREAM_TYPE_DEPTH */
	case PXCImage::PIXEL_FORMAT_DEPTH: /* 16-bit unsigned integer with precision mm. */
	case PXCImage::PIXEL_FORMAT_DEPTH_RAW: /* Raw Stream 캡쳐를 사용해서 Depth 이미지는 이쪽으로 들어옴. 제대로 보려면 Scaling 필요. 16-bit unsigned integer with device specific precision (call device->QueryDepthUnit()) */
		cvDataType = CV_16U;
		cvDataWidth = 2;
		break;
	case PXCImage::PIXEL_FORMAT_DEPTH_F32: /* 32-bit float-point with precision mm. */
		cvDataType = CV_32F;
		cvDataWidth = 4;
		break;

		/* STREAM_TYPE_IR */
	case PXCImage::PIXEL_FORMAT_Y16:          /* 16-Bit Gray Image */
		cvDataType = CV_16U;
		cvDataWidth = 2;
		break;
	case PXCImage::PIXEL_FORMAT_Y8_IR_RELATIVE:    /* Relative IR Image */
		cvDataType = CV_8U;
		cvDataWidth = 1;
		break;
	}

	// suppose that no other planes
	if (data.planes[1] != NULL) throw(0); // not implemented
	// suppose that no sub pixel padding needed
	if (data.pitches[0] % cvDataWidth != 0) throw(0); // not implemented

	outImg->create(imgInfo.height, data.pitches[0] / cvDataWidth, cvDataType);

	memcpy(outImg->data, data.planes[0], imgInfo.height*imgInfo.width*cvDataWidth * sizeof(pxcBYTE));

	inImg->ReleaseAccess(&data);
}
// Realsense 카메라에서 받은 뎁스 영상(mat)를 PLY로 변환(메쉬 생성)
// Realsense 카메라에서 받은 뎁스 영상(mat)를 PLY로 변환(메쉬 생성)
// 인풋은 480*360, cvDataType = CV_16U; cvDataWidth = 2; 임.
void mat2Ply(Mat image)
{
	int x = 0, y = 0;
	unsigned short z = 0;
	int vtrAmount = 0;
	int triAmount = 0;
	FILE* fPtr;
	string header = "ply\n";
	string content = "";
	char temp[100];
	char textureFile[50] = "rgb.bmp";

	Vertice vtrArr[DEPTH_HEIGHT][DEPTH_WIDTH];
	memset(vtrArr, 0, sizeof(Vertice) * DEPTH_WIDTH * DEPTH_HEIGHT);

	for (y = 0; y < image.rows; y++)
	{
		for (x = 0; x < image.cols; x++)
		{
			z = image.at<unsigned short>(y, x);
			if (z > 0 && z < USHRT_MAX / 80)
			{
				// 텍스쳐 위치 관련은 여기서. u--일수록 텍스쳐가 오른쪽(열었을 때)으로 밀림. 
				// Texture coordinate가 아주 정확한 건 아님. 마음에 안 들면 4, 5번째 %f를 변경하길 바람.
				sprintf(temp, "%f %f %f %f %f\n",
					(float)x / 300, -(float)y / 300, -(float)z / USHRT_MAX * 180, 0.04828 + ((float)(x + 1)) / DEPTH_WIDTH * 3 / 4, -0.0101 - ((float)(y + 1)) / DEPTH_HEIGHT);
				content += temp;
				vtrArr[y][x].isThere = true;
				vtrArr[y][x].vtrNum = vtrAmount;
				vtrAmount++;
			}
		}
	}

	//  face 생성
	for (y = 0; y < image.rows - 1; y++)
	{
		for (x = 0; x < image.cols - 1; x++)
		{
			bool firstTry = false;

			if (vtrArr[y][x].isThere == true && vtrArr[y][x + 1].isThere == true && vtrArr[y + 1][x].isThere == true)
			{
				sprintf(temp, "3 %d %d %d\n", vtrArr[y + 1][x].vtrNum, vtrArr[y][x + 1].vtrNum, vtrArr[y][x].vtrNum);
				content += temp;
				firstTry = true;
				triAmount++;
			}
			if (vtrArr[y + 1][x].isThere == true && vtrArr[y + 1][x + 1].isThere == true && vtrArr[y][x + 1].isThere == true)
			{
				sprintf(temp, "3 %d %d %d\n", vtrArr[y + 1][x].vtrNum, vtrArr[y + 1][x + 1].vtrNum, vtrArr[y][x + 1].vtrNum);
				content += temp;
				firstTry = true;
				triAmount++;
			}

			if (firstTry == false)
			{
				if (vtrArr[y][x].isThere == true && vtrArr[y][x + 1].isThere == true && vtrArr[y + 1][x + 1].isThere == true)
				{
					sprintf(temp, "3 %d %d %d\n", vtrArr[y + 1][x + 1].vtrNum, vtrArr[y][x + 1].vtrNum, vtrArr[y][x].vtrNum);
					content += temp;
					triAmount++;
				}
				if (vtrArr[y + 1][x].isThere == true && vtrArr[y + 1][x + 1].isThere == true && vtrArr[y][x].isThere == true)
				{
					sprintf(temp, "3 %d %d %d\n", vtrArr[y + 1][x].vtrNum, vtrArr[y + 1][x + 1].vtrNum, vtrArr[y][x].vtrNum);
					content += temp;
					triAmount++;
				}
			}
		}
	}

	// 헤더 선언
	header += "format ascii 1.0\n";
	sprintf(temp, "comment TextureFile %s\n", textureFile);
	header += temp;
	sprintf(temp, "element vertex %d\n", vtrAmount);
	header += temp;
	header += "property float x\n";
	header += "property float y\n";
	header += "property float z\n";
	header += "property float texture_u\n";
	header += "property float texture_v\n";
	sprintf(temp, "element face %d\n", triAmount);
	header += temp;
	header += "property list uchar int vertex_indices\n";
	header += "end_header\n";

	cout << header;

	// ply로 출력하는 부분
	fPtr = fopen("mesh.ply", "wt+");

	fprintf(fPtr, "%s", header.c_str());
	fprintf(fPtr, "%s", content.c_str());

	fclose(fPtr);
}

int depth_main(void)
{
	Mat rgbMat;
	Mat depthMat;
	bool doSave = false;

	PXCSession* session = PXCSession::CreateInstance();
	PXCSession::ImplVersion ver = session->QueryVersion();
	wprintf_s(L"SDK Version %d.%d\n", ver.major, ver.minor);

	// OpenCV 창 생성
	namedWindow("RealSense RGB window", WINDOW_AUTOSIZE);
	namedWindow("RealSense Depth window", WINDOW_AUTOSIZE);
	//UtilRender* renderColor = new UtilRender(L"COLOR STREAM");
	//UtilRender* renderDepth = new UtilRender(L"DEPTH STREAM");

	PXCSenseManager* sm = 0;
	sm = PXCSenseManager::CreateInstance();

	if (sm == 0)
	{
		wprintf_s(L"Unable to create the PXCSenseManager\n");
	}

	// STREAM 옵션 중요. 30프레임으로 돌리면 RGB는 1080p, Depth는 480*360까지 받을 수 있음
	sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, RGB_WIDTH, RGB_HEIGHT, FRAME_RATE/*, PXCCapture::Device::STREAM_OPTION_UNRECTIFIED*/);
	sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, DEPTH_WIDTH, DEPTH_HEIGHT, FRAME_RATE);

	if (sm->Init() != PXC_STATUS_NO_ERROR)
	{
		wprintf_s(L"Unable to Init the PXCSenseManager\n");
	}

	PXCImage* colorlm;
	PXCImage* depthlm;

	while (1)
	{
		// 키보드 입력 아무거나 받으면 Mesh 생성 후 저장
		//if (_kbhit() != 0)
		if (takingDepth)
		{
			takingDepth = false;
			doSave = true;
		}
		else if (depthExit)
		{
			waitKey(33); // waitKey 안 걸어주면 터짐
			break;
		}

		// 스트림이 준비될 때까지 block
		if (sm->AcquireFrame(true) < PXC_STATUS_NO_ERROR)
			break;

		// 가능한 이미지 샘플을 다 받기
		PXCCapture::Sample* sample = sm->QuerySample();

		// sample에서 이미지 받기
		colorlm = sample->color;
		depthlm = sample->depth;

		ConvertPXCImageToOpenCVMat(colorlm, &rgbMat);
		ConvertPXCImageToOpenCVMat(depthlm, &depthMat);
		imshow("RealSense RGB window", rgbMat);
		imshow("RealSense Depth window", depthMat);
		sm->ReleaseFrame();

		if (doSave == true)
		{
			imwrite("depth.bmp", depthMat); //Scaling 안 해줬음 아직
			imwrite("rgb.bmp", rgbMat); //Scaling 안 해줬음 아직
			mat2Ply(depthMat);
			cout << "Save Complete" << endl;
			/////
			doSave = false;
			//break;
		}
		waitKey(33); // waitKey 안 걸어주면 터짐
	}

	// UtilRender 객체 삭제
	//delete renderColor;
	//delete renderDepth;
	// 파괴자 대신 Release() 써줘야 함.
	sm->Release();
	session->Release();
	return 0;
}
///////////////////////////////// depth sensor

