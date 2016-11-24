
// TestDlg.h : 헤더 파일
//
#pragma once

#include "opencv2/opencv.hpp"    
#include <iostream>    
#include "afxwin.h"

#define DEPTH_WIDTH 480
#define DEPTH_HEIGHT 360
#define RGB_WIDTH 1920
#define RGB_HEIGHT 1080
#define FRAME_RATE 30

using namespace cv;
using namespace std;

typedef struct
{
	bool isThere;
	int vtrNum;
} Vertice;


////////////////////// depth sensor

void displayImage(CStatic * mPicture, Mat frame);

// CTestDlg 대화 상자
class CTestDlg : public CDialogEx
{
// 생성입니다.
public:
	CTestDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCancel();
	CStatic m_picture;
	CStatic m_picture2;
	afx_msg void OnBnClickedOk();
	afx_msg void OnStnClickedPicture();
	afx_msg void OnBnClickedCali();
};
