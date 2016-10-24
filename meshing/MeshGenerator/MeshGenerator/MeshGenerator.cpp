/*	MeshGenerator
�ۼ� : 20160924 07:56 ��̼�
���� : 20161024 20:07 ��̼�
*/
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

#define MAX_FRAMES 10200

using namespace std;
using namespace cv;

typedef struct
{
	bool isThere;
	int vtrNum;
} Vertice;

// PXCImage -> Mat�� ��ȯ�ϴ� �Լ�
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
	case PXCImage::PIXEL_FORMAT_DEPTH_RAW: /* Raw Stream ĸ�ĸ� ����ؼ� Depth �̹����� �������� ����. ����� ������ Scaling �ʿ�. 16-bit unsigned integer with device specific precision (call device->QueryDepthUnit()) */
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

// Realsense ī�޶󿡼� ���� ���� ����(mat)�� PLY�� ��ȯ(�޽� ����)
// ��ǲ�� 320*240, cvDataType = CV_16U; cvDataWidth = 2; ��.
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

	Vertice vtrArr[240][320];
	memset(vtrArr, 0, sizeof(Vertice) * 320 * 240);

	//Triangle triArr[4 * 320 * 240];

	for (y = 0; y < image.rows; y++)
	{
		for (x = 0; x < image.cols; x++)
		{
			z = image.at<unsigned short>(y, x);
			if (z > 0 && z < USHRT_MAX/10)
			{
				sprintf(temp, "%f %f %f\n", (float)x / 300, -(float)y / 300, -(float)z / USHRT_MAX * 60);
				content += temp;
				vtrArr[y][x].isThere = true;
				vtrArr[y][x].vtrNum = vtrAmount;
				vtrAmount++;
			}
		}
	}

	// �׽�Ʈ�� �ڵ�
	/*content += "3 0 1 2\n";
	triAmount++;
	content += "3 100 1 2\n";
	triAmount++;
	content += "3 0 100 2\n";
	triAmount++;
	content += "3 0 1 200\n";
	triAmount++;*/
	//

	//  face ����
	for (y = 0; y < image.rows; y++)
	{
		for (x = 0; x < image.cols - 1; x++)
		{
			bool firstTry = false;
			if (y < image.rows - 1)
			{
				if (vtrArr[y][x].isThere == true && vtrArr[y][x + 1].isThere == true && vtrArr[y + 1][x].isThere == true)
				{
					sprintf(temp, "3 %d %d %d\n", vtrArr[y][x].vtrNum, vtrArr[y][x + 1].vtrNum, vtrArr[y + 1][x].vtrNum);
					content += temp;
					firstTry = true;
					triAmount++;
				}
				if (vtrArr[y+1][x].isThere == true && vtrArr[y+1][x + 1].isThere == true && vtrArr[y][x + 1].isThere == true)
				{
					sprintf(temp, "3 %d %d %d\n", vtrArr[y+1][x].vtrNum, vtrArr[y+1][x + 1].vtrNum, vtrArr[y][x + 1].vtrNum);
					content += temp;
					firstTry = true;
					triAmount++;
				}

				if (firstTry == false)
				{
					if (vtrArr[y][x].isThere == true && vtrArr[y][x + 1].isThere == true && vtrArr[y + 1][x + 1].isThere == true)
					{
						sprintf(temp, "3 %d %d %d\n", vtrArr[y][x].vtrNum, vtrArr[y][x + 1].vtrNum, vtrArr[y + 1][x + 1].vtrNum);
						content += temp;
						triAmount++;
					}
					if (vtrArr[y+1][x].isThere == true && vtrArr[y+1][x + 1].isThere == true && vtrArr[y][x].isThere == true)
					{
						sprintf(temp, "3 %d %d %d\n", vtrArr[y+1][x].vtrNum, vtrArr[y+1][x + 1].vtrNum, vtrArr[y][x].vtrNum);
						content += temp;
						triAmount++;
					}
				}
			}
		}
	}

	// ��� ����
	header += "format ascii 1.0\n";
	sprintf(temp, "element vertex %d\n", vtrAmount);
	header += temp;
	header += "property float x\n";
	header += "property float y\n";
	header += "property float z\n";
	sprintf(temp, "element face %d\n", triAmount);
	header += temp;
	header += "property list uchar int vertex_indices\n";
	header += "end_header\n";

	cout << header;

	// ply�� ����ϴ� �κ�
	fPtr = fopen("mesh.ply", "wt+");

	fprintf(fPtr, "%s", header.c_str());
	fprintf(fPtr, "%s", content.c_str());

	fclose(fPtr);
}

int main(void)
{
	Mat rgbMat;
	Mat depthMat;
	bool doSave = false;

	PXCSession* session = PXCSession::CreateInstance();
	PXCSession::ImplVersion ver = session->QueryVersion();
	wprintf_s(L"SDK Version %d.%d\n", ver.major, ver.minor);

	// OpenCV â ����
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

	// STREAM �ɼǿ� ���� �Ǳ⵵ �ϰ� �ȵǱ⵵ ��.
	// ���� ������ �������� �Ǵ� ��.
	sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480, 30/*, PXCCapture::Device::STREAM_OPTION_UNRECTIFIED*/);
	sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 320, 240, 30);

	if (sm->Init() != PXC_STATUS_NO_ERROR)
	{
		wprintf_s(L"Unable to Init the PXCSenseManager\n");
	}

	PXCImage* colorlm;
	PXCImage* depthlm;

	while (1)
	{
		// Ű���� �Է� �ƹ��ų� ������ Mesh ���� �� ����
		if (_kbhit() != 0)
		{
			doSave = true;
		}

		// ��Ʈ���� �غ�� ������ block
		if (sm->AcquireFrame(true) < PXC_STATUS_NO_ERROR)
			break;

		// ������ �̹��� ������ �� �ޱ�
		PXCCapture::Sample* sample = sm->QuerySample();

		// sample���� �̹��� �ޱ�
		colorlm = sample->color;
		depthlm = sample->depth;

		ConvertPXCImageToOpenCVMat(colorlm, &rgbMat);
		ConvertPXCImageToOpenCVMat(depthlm, &depthMat);
		imshow("RealSense RGB window", rgbMat);
		imshow("RealSense Depth window", depthMat);
		sm->ReleaseFrame();

		if (doSave == true)
		{
			imwrite("depth.bmp", depthMat); //Scaling �� ������ ����
			mat2Ply(depthMat);
			cout << "Save Complete" << endl;
			break;
		}
		waitKey(33); // waitKey �� �ɾ��ָ� ����
	}

	// UtilRender ��ü ����
	//delete renderColor;
	//delete renderDepth;
	// �ı��� ��� Release() ����� ��.
	sm->Release();
	session->Release();
	return 0;
}
