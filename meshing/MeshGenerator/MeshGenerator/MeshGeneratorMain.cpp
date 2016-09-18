/*	MeshGenerator�� ���� �Լ�
	�ۼ� : 20160916 23:56 ��̼�
*/
#pragma once
#include <iostream>
#include <pxcsession.h>
#include <pxcsensemanager.h>

using namespace std;

/*	���� �Լ�
	�ۼ� : 20160917 00:32 ��̼�
	���� : 20160917 00:32 ��̼�
*/
int main(void)
{
	//Session ����
	PXCSession* session = PXCSession::CreateInstance();
	PXCSession::ImplVersion ver = session->QueryVersion();

	cout << "SDK Version " << ver.major << "." << ver.minor << "\n";

	//Sense Manager ����
	PXCSenseManager *sm = PXCSenseManager::CreateInstance();

	//Depth Stream Ȱ��ȭ 
	sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH,0,0,0);
	sm->Init();

	for (;;) {
		pxcStatus sts = sm->AcquireFrame(true);
		if (sts < PXC_STATUS_NO_ERROR)
		{
			cout << "���� �߻�\n";
			break;
		}
		PXCCapture::Sample *sample;
		sample = sm->QuerySample();

		// process image

		// ���� ����������
		sm->ReleaseFrame();
	}

	sm->Close();


	//��ü �ı�. �ı��� ��� Release() ����� ��.
	sm->Release();
	session->Release();
	return 0;
}