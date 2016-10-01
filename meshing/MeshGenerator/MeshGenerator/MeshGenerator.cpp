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
#include <thread>

#define MAX_FRAMES 1200

using namespace std;

//void rgbRawStream()
//{
//	PXCSession* session = PXCSession::CreateInstance();
//	PXCSession::ImplVersion ver = session->QueryVersion();
//	wprintf_s(L"SDK Version %d.%d\n", ver.major, ver.minor);
//
//	UtilRender* renderColor = new UtilRender(L"COLOR STREAM");
//	//UtilRender* renderDepth = new UtilRender(L"DEPTH STREAM");
//
//	PXCSenseManager* sm = 0;
//	sm = PXCSenseManager::CreateInstance();
//	if (sm == 0)
//	{
//		wprintf_s(L"Unable to create the PXCSenseManager\n");
//	}
//	sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480);
//	//sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 320, 240, 60);
//
//	if (sm->Init() != PXC_STATUS_NO_ERROR)
//	{
//		wprintf_s(L"Unable to Init the PXCSenseManager\n");
//	}
//
//
//	PXCImage* colorlm;
//	PXCImage* depthlm;
//
//	for (int i = 0; i < MAX_FRAMES; i++)
//	{
//		// 스트림이 준비될 때까지 block
//		if (sm->AcquireFrame(true) < PXC_STATUS_NO_ERROR)
//			break;
//
//		// 가능한 이미지 샘플을 다 받기
//		PXCCapture::Sample* sample = sm->QuerySample();
//
//		// sample에서 이미지 받기
//		colorlm = sample->color;
//		//depthlm = sample->depth;
//
//		// 렌더링
//		if (!renderColor->RenderFrame(colorlm))
//			break;
//		/*if (!renderDepth->RenderFrame(depthlm))
//			break;*/
//		sm->ReleaseFrame();
//	}
//
//	// UtilRender 객체 삭제
//	delete renderColor;
//	//delete renderDepth;
//
//	// 파괴자 대신 Release() 써줘야 함.
//	sm->Release();
//	session->Release();
//}
//
//void depthRawStream()
//{
//	PXCSession* session = PXCSession::CreateInstance();
//	PXCSession::ImplVersion ver = session->QueryVersion();
//	wprintf_s(L"SDK Version %d.%d\n", ver.major, ver.minor);
//
//	//UtilRender* renderColor = new UtilRender(L"COLOR STREAM");
//	UtilRender* renderDepth = new UtilRender(L"DEPTH STREAM");
//
//	PXCSenseManager* sm = 0;
//	sm = PXCSenseManager::CreateInstance();
//	if (sm == 0)
//	{
//		wprintf_s(L"Unable to create the PXCSenseManager\n");
//	}
//	//sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480);
//	sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 320, 240, 60);
//
//	if (sm->Init() != PXC_STATUS_NO_ERROR)
//	{
//		wprintf_s(L"Unable to Init the PXCSenseManager\n");
//	}
//
//
//	//PXCImage* colorlm;
//	PXCImage* depthlm;
//
//	for (int i = 0; i < MAX_FRAMES; i++)
//	{
//		// 스트림이 준비될 때까지 block
//		if (sm->AcquireFrame(true) < PXC_STATUS_NO_ERROR)
//			break;
//
//		// 가능한 이미지 샘플을 다 받기
//		PXCCapture::Sample* sample = sm->QuerySample();
//
//		// sample에서 이미지 받기
//		//colorlm = sample->color;
//		depthlm = sample->depth;
//
//		// 렌더링
//		/*if (!renderColor->RenderFrame(colorlm))
//			break;*/
//		if (!renderDepth->RenderFrame(depthlm))
//			break;
//		sm->ReleaseFrame();
//	}
//
//	// UtilRender 객체 삭제
//	//delete renderColor;
//	delete renderDepth;
//
//	// 파괴자 대신 Release() 써줘야 함.
//	sm->Release();
//	session->Release();
//}

int main(void)
{
	//thread rgbTh(rgbRawStream);
	//thread depthTh(depthRawStream);

	//rgbTh.join();
	//depthTh.join();
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
	}
	
	// STREAM 옵션에 따라서 되기도 하고 안되기도 함.
	// 현재 설정은 무리없이 되는 듯.
	sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480, 30/*, PXCCapture::Device::STREAM_OPTION_UNRECTIFIED*/);
	sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 320, 240, 30);

	if (sm->Init() != PXC_STATUS_NO_ERROR)
	{
		wprintf_s(L"Unable to Init the PXCSenseManager\n");
	}


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
		colorlm = sample->color;
		depthlm = sample->depth;

		// 렌더링
		if (!renderColor->RenderFrame(colorlm))
			break;
		if (!renderDepth->RenderFrame(depthlm))
			break;

		sm->ReleaseFrame();
	}

	// UtilRender 객체 삭제
	delete renderColor;
	delete renderDepth;

	// 파괴자 대신 Release() 써줘야 함.
	sm->Release();
	session->Release();


	return 0;
}
