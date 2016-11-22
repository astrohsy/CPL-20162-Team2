
// TestDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Test.h"
#include "TestDlg.h"
#include "afxdialogex.h"
/////////////////////////// MFC
#include "camera.h"

#include <thread>
#include <future>
#include <string>
/////////////////////// rgb sensor

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
#include <string.h>

const int w = 8;
const int h = 5;
const float mm = 3.5;
const string src = "camera.bmp";
const string dst = "rgb.bmp";

int d_number = 0;
char s_number[10];
char d_rgb[20];
char d_depth[20];
char d_ply[20];

bool programExit;
bool takingPicture;
bool depthExit;
bool takingDepth;

bool inputDoing();
void evf(Camera dslr);
void mat2Ply(Mat image);
void ConvertPXCImageToOpenCVMat(PXCImage *inImg, Mat *outImg);

char * depth_capture(void);

///////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VideoCapture *cap1;
CImage *mfcImg;
Camera dslr;
Mat frame1;
int cnt;
Calibration cali;

///////////////////////
Mat rgbMat;
Mat depthMat;
PXCSession* session;
PXCSenseManager* sm = 0;
PXCImage* colorlm;
PXCImage* depthlm;
/////////////////////
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestDlg 대화 상자



CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, m_picture);
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDCANCEL, &CTestDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCALI, &CTestDlg::OnBnClickedCali)
END_MESSAGE_MAP()


// CTestDlg 메시지 처리기

BOOL CTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//calibration
	cali.CalibrationInit(w, h, mm);
	
	///dslr
	dslr.initializeSDK();
	dslr.setErr(dslr.startLiveview());
	///depth
	PXCSession* session = PXCSession::CreateInstance();
	PXCSession::ImplVersion ver = session->QueryVersion();
	wprintf_s(L"SDK Version %d.%d\n", ver.major, ver.minor);
	sm = PXCSenseManager::CreateInstance();


	if (sm == 0)
	{
		wprintf_s(L"Unable to create the PXCSenseManager\n");
	}

	// STREAM 옵션에 따라서 되기도 하고 안되기도 함.
	// 현재 설정은 무리없이 되는 듯.
	sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480, 30/*, PXCCapture::Device::STREAM_OPTION_UNRECTIFIED*/);
	sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 320, 240, 30);

	if (sm->Init() != PXC_STATUS_NO_ERROR)
	{
		wprintf_s(L"Unable to Init the PXCSenseManager\n");
	}

	mfcImg = NULL;
	SetTimer(1000, 30, NULL);


	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void displayImage(CStatic * mPicture, Mat frame)
{
	CImage *cimage_mfc;
	cimage_mfc = NULL;

	int bpp = 8 * frame.elemSize();
	assert((bpp == 8 || bpp == 24 || bpp == 32));

	int padding = 0;
	//32 bit image is always DWORD aligned because each pixel requires 4 bytes
	if (bpp < 32)
		padding = 4 - (frame.cols % 4);

	if (padding == 4)
		padding = 0;

	int border = 0;
	//32 bit image is always DWORD aligned because each pixel requires 4 bytes
	if (bpp < 32)
	{
		border = 4 - (frame.cols % 4);
	}

	Mat mat_temp;
	if (border > 0 || frame.isContinuous() == false)
	{
		// Adding needed columns on the right (max 3 px)
		cv::copyMakeBorder(frame, mat_temp, 0, 0, 0, border, cv::BORDER_CONSTANT, 0);
	}
	else
	{
		mat_temp = frame;
	}


	RECT r;
	mPicture->GetClientRect(&r);
	cv::Size winSize(r.right, r.bottom);

	if (cimage_mfc)
	{
		cimage_mfc->ReleaseDC();
		delete cimage_mfc;
		cimage_mfc = nullptr;
	}

	cimage_mfc = new CImage();
	cimage_mfc->Create(winSize.width, winSize.height, 24);


	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = bpp;
	bitInfo.bmiHeader.biWidth = mat_temp.cols;
	bitInfo.bmiHeader.biHeight = -mat_temp.rows;
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biSizeImage = 0;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0;


	// Image is bigger or smaller than into destination rectangle
	// we use stretch in full rect

	if (mat_temp.cols == winSize.width  && mat_temp.rows == winSize.height)
	{
		// source and destination have same size
		// transfer memory block
		// NOTE: the padding border will be shown here. Anyway it will be max 3px width

		SetDIBitsToDevice(cimage_mfc->GetDC(),
			//destination rectangle
			0, 0, winSize.width, winSize.height,
			0, 0, 0, mat_temp.rows,
			mat_temp.data, &bitInfo, DIB_RGB_COLORS);
	}
	else
	{
		// destination rectangle
		int destx = 0, desty = 0;
		int destw = winSize.width;
		int desth = winSize.height;

		// rectangle defined on source bitmap
		// using imgWidth instead of mat_temp.cols will ignore the padding border
		int imgx = 0, imgy = 0;
		int imgWidth = mat_temp.cols - border;
		int imgHeight = mat_temp.rows;

		StretchDIBits(cimage_mfc->GetDC(),
			destx, desty, destw, desth,
			imgx, imgy, imgWidth, imgHeight,
			mat_temp.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);
	}

	cimage_mfc->BitBlt(::GetDC(mPicture->m_hWnd), 0, 0);

	if (cimage_mfc)
	{
		cimage_mfc->ReleaseDC();
		delete cimage_mfc;
		cimage_mfc = nullptr;
	}
}

void CTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnTimer(nIDEvent);

	//참고한 사이트 http://stackoverflow.com/a/29006218  
	//참고한 사이트 http://webnautes.tistory.com/824

	//cap1->read(frame1);
	frame1 = dslr.downloadEvfData();
	/////
	////depth
	// 스트림이 준비될 때까지 block
	if (!(sm->AcquireFrame(true) < PXC_STATUS_NO_ERROR)){

		// 가능한 이미지 샘플을 다 받기
		PXCCapture::Sample* sample = sm->QuerySample();

		// sample에서 이미지 받기
		colorlm = sample->color;
		depthlm = sample->depth;

		ConvertPXCImageToOpenCVMat(colorlm, &rgbMat);
		ConvertPXCImageToOpenCVMat(depthlm, &depthMat);
		//frame1 = rgbMat;
		//imshow("RealSense RGB window", rgbMat);
		sm->ReleaseFrame();
	}
	waitKey(33);

	displayImage(&m_picture, frame1);

	/*RECT r;
	m_picture.GetClientRect(&r);
	cv::Size winSize(r.right, r.bottom);

	cv::Mat cvImgTmp(winSize, CV_8UC3);
	if (frame1.size() != winSize)
	{
		cv::resize(frame1, cvImgTmp, winSize);
	}
	else
	{
		cvImgTmp = frame1.clone();
	}

	cv::flip(cvImgTmp, cvImgTmp, 0);

	if (mfcImg)
	{
		mfcImg->ReleaseDC();
		delete mfcImg;
		mfcImg = nullptr;
	}

	mfcImg = new CImage();
	mfcImg->Create(winSize.width, winSize.height, 24);

	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 24;
	bitInfo.bmiHeader.biWidth = winSize.width;
	bitInfo.bmiHeader.biHeight = winSize.height;
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biSizeImage = 0;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0;

	StretchDIBits(mfcImg->GetDC(), 0, 0,
		winSize.width, winSize.height, 0, 0,
		winSize.width, winSize.height,
		cvImgTmp.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY
		);

	mfcImg->BitBlt(::GetDC(m_picture.m_hWnd), 0, 0);


	if (mfcImg)
	{
		mfcImg->ReleaseDC();
		delete mfcImg;
		mfcImg = nullptr;
	}*/
}


void CTestDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	KillTimer(1);

	// TODO: Add your message handler code here
}

void CTestDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
	CTestDlg::OnDestroy();
	
	dslr.setErr(dslr.endLiveview());
	dslr.releaseSDK();

	//cap1->release();
}

void CTestDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
	char * src = dslr.takePicture();
	char * dst = depth_capture();

	if (cali.getH().size().height == 0){
		cali.loadResult();
	}
	Mat result = cali.changePicture(src, dst);

	char resultName[20];
	sprintf(resultName, "c_%s", src);
	imwrite(resultName, result);
}

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
// 인풋은 320*240, cvDataType = CV_16U; cvDataWidth = 2; 임.
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

	strcpy(textureFile, d_rgb);

	Vertice vtrArr[DEPTH_HEIGHT][DEPTH_WIDTH];
	memset(vtrArr, 0, sizeof(Vertice)* DEPTH_WIDTH * DEPTH_HEIGHT);

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

	strcpy(d_ply, "mesh");
	strcat(d_ply, s_number);
	strcat(d_ply, ".ply");

	// ply로 출력하는 부분
	fPtr = fopen(d_ply, "wt+");

	fprintf(fPtr, "%s", header.c_str());
	fprintf(fPtr, "%s", content.c_str());

	fclose(fPtr);
}
char * depth_capture(void)
{
	// OpenCV 창 생성
	//UtilRender* renderColor = new UtilRender(L"COLOR STREAM");
	//UtilRender* renderDepth = new UtilRender(L"DEPTH STREAM");
		sprintf(s_number, "%d", ++d_number);

		strcpy(d_depth, "depth");
		strcat(d_depth, s_number);
		strcat(d_depth, ".bmp");

		strcpy(d_rgb, "d_rgb");
		strcat(d_rgb, s_number);
		strcat(d_rgb, ".bmp");

		imwrite(d_depth, depthMat); //Scaling 안 해줬음 아직
		imwrite(d_rgb, rgbMat); //Scaling 안 해줬음 아직
		mat2Ply(depthMat);
		//break;%
	
		return d_rgb;
}

void CTestDlg::OnBnClickedCali()
{
	char * src = dslr.takePicture();
	char * dst = depth_capture();

	cali.run_calibration(src, dst);
	cali.saveResult();
}
