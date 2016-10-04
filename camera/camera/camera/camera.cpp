#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <Windows.h>

#include "EDSDK.h"
#include "EDSDKTypes.h"
#include "EDSDKErrors.h"

#include "DownloadEvfCommand.h"
#include "StartEvfCommand.h"
#include "EndEvfCommand.h"

#include "CameraModel.h"
#include "CameraModelLegacy.h"
#include "CameraController.h"
#include "CameraEventListener.h"
#include "CameraControl.h"

#include "Command.h"
#include "CameraEvent.h"

using namespace std;
using namespace cv;

bool threadExit;
CCameraControlApp theApp;

bool printingPicture(){
	IplImage* dslr = 0;

	theApp._model->setEvfMode(0x01);
	theApp._model->setEvfAFMode(0x01);
	//theApp._model->setEvfOutputDevice(0x00000500);
	(*theApp._controller).actionPerformed(*new ActionEvent("startEVF"));
	(*theApp._controller).actionPerformed(*new ActionEvent("downloadEVF"));

	cout << theApp._model->getEvfOutputDevice() << ' ' << theApp._model->getEvfAFMode() << ' ' << theApp._model->getEvfMode() << endl;

	namedWindow("DSLR", 1);

	while (1){
		if (threadExit)
			break;

		char c = cvWaitKey(50);
	}

	(*theApp._controller).actionPerformed(*new ActionEvent("endEVF"));
	cvDestroyAllWindows();

	cout << "thread end" << endl;

	return true;
}

int main()
{
	theApp.InitInstance();

	string doing;
	threadExit = false;

	auto pthread = async(&printingPicture);

	while (1){
		cin >> doing;
		if (doing == "t"){
			cout << "Take picture" << endl;
			(*theApp._controller).actionPerformed(*new ActionEvent("TakePicture"));
		}

		else if (doing == "z"){
			cout << "Exit program" << endl;
			threadExit = true;
			break;
		}
	}

	bool check = pthread.get();

	theApp.release();
}

/*
VideoCapture vcap;
Mat image;
if (!vcap.open(theApp._model->getEvfOutputDevice())) {
	cout << "Error opening video stream or file" << std::endl;
	return -1;
}

while (1){
	vcap.read(image);
	imshow("DSLR", image);

	cout << image << endl;

	char c = cvWaitKey(50);
}
*/

/*
CvCapture* capture = cvCaptureFromCAM(theApp._model->getEvfOutputDevice());

while (1){
	//1
	cvGrabFrame(capture);
	dslr = cvRetrieveFrame(capture);

	//2
	dslr = cvQueryFrame(capture);
	cvShowImage("DSLR", dslr);

	if (!dslr)
		break;

	char c = cvWaitKey(50);
}
cvReleaseCapture(&capture);
*/