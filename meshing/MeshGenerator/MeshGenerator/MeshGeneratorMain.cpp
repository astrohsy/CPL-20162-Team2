/*	MeshGenerator용 메인 함수
	작성 : 20160916 23:56 김미수
*/
#pragma once
#include <iostream>
#include <pxcsession.h>
#include <pxcsensemanager.h>

using namespace std;

/*	메인 함수
	작성 : 20160917 00:32 김미수
	수정 : 20160917 00:32 김미수
*/
int main(void)
{
	//Session 생성
	PXCSession* session = PXCSession::CreateInstance();
	PXCSession::ImplVersion ver = session->QueryVersion();

	cout << "SDK Version " << ver.major << "." << ver.minor << "\n";

	//Sense Manager 생성
	PXCSenseManager *sm = PXCSenseManager::CreateInstance();

	//Depth Stream 활성화 
	sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH,0,0,0);
	sm->Init();

	for (;;) {
		pxcStatus sts = sm->AcquireFrame(true);
		if (sts < PXC_STATUS_NO_ERROR)
		{
			cout << "에러 발생\n";
			break;
		}
		PXCCapture::Sample *sample;
		sample = sm->QuerySample();

		// process image

		// 다음 프레임으로
		sm->ReleaseFrame();
	}

	sm->Close();


	//객체 파괴. 파괴자 대신 Release() 써줘야 함.
	sm->Release();
	session->Release();
	return 0;
}