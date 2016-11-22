
// TestDlg.h : ��� ����
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

// CTestDlg ��ȭ ����
class CTestDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CTestDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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
