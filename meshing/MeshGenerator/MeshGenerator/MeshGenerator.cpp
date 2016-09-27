/*	MeshGenerator
작성 : 20160924 07:56 김미수
수정 : 20160924 09:11 김미수
*/
#pragma once
#include <pxcsensemanager.h>
#include <pxcsession.h>
#include <stdlib.h>
#include <iostream>
#include <util_render.h>
#include <time.h>

#define MAX_FRAMES 1200

using namespace std;

//class MeshHandler :public PXCSenseManager::Handler
//{
//public:
//	virtual pxcStatus PXCAPI OnNewSample(pxcUID, PXCCapture::Sample* sample)
//	{
//		// NO ERROR 리턴하면 계속
//		// ERROR 발생하면 루프 종료
//		return PXC_STATUS_NO_ERROR;
//	}
//};

int main(void)
{
	clock_t t1, t2;

	PXCSession* session = PXCSession::CreateInstance();
	PXCSession::ImplVersion ver = session->QueryVersion();
	wprintf_s(L"SDK Version %d.%d\n", ver.major, ver.minor);

	UtilRender* renderColor = new UtilRender(L"COLOR STREAM");
	UtilRender* renderDepth = new UtilRender(L"DEPTH STREAM");

	PXCSenseManager* sm = 0;
	sm = PXCSenseManager::CreateInstance();
	if (sm == 0)
	{
		wprintf_s(L"Unable to create the PXCSenseManager\n");
		return 1;
	}
	//sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480);
	sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 320, 240, 60);

	if (sm->Init() != PXC_STATUS_NO_ERROR)
	{
		wprintf_s(L"Unable to Init the PXCSenseManager\n");
		return 2;
	}
	/*MeshHandler handler;
	sm->Init(&handler);

	sm->StreamFrames(true);*/

	PXCImage* colorlm;
	PXCImage* depthlm;
	
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		// 스트림이 준비될 때까지 block
		if (sm->AcquireFrame(true) < PXC_STATUS_NO_ERROR)
			break;

		// 가능한 이미지 샘플을 다 받기
		PXCCapture::Sample* sample = sm->QuerySample();

		// sample에서 이미지 받기
		//colorlm = sample->color;
		depthlm = sample->depth;

		// 렌더링
		/*if (!renderColor->RenderFrame(colorlm))
			break;*/
		if (!renderDepth->RenderFrame(depthlm))
			break;
		t1 = clock();
		/*while (true)
		{
			t2 = clock();
			if (t2 - t1 > CLOCKS_PER_SEC)
				break;
		}*/
		sm->ReleaseFrame();
	}

	// UtilRender 객체 삭제
	delete renderColor;
	delete renderDepth;

	// 파괴자 대신 Release() 써줘야 함.
	sm->Release();
	session->Release();
}
