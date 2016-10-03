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
#include "CameraModel.h"
#include "CameraModelLegacy.h"
#include "CameraController.h"
#include "CameraEventListener.h"
#include "CameraControl.h"

using namespace std;
using namespace cv;

bool threadExit;
CCameraControlApp theApp;

bool printingPicture(){
	namedWindow("DSLR", 1);

	//IplImage* dslr = 0;

	BITMAP Evf_Bmp;
	int LVBw, LVBh, w, h;
	float LVBratio, LVration;

	(*theApp._controller).actionPerformed(*new ActionEvent("evfAFOn"));
	
	//CvCapture* capture = cvCaptureFromCAM(0);

	while (1){
		if (threadExit)
			break;

		/*cvGrabFrame(capture);
		dslr = cvRetrieveFrame(capture);

		if (!dslr)
			break;

		cvShowImage("DSLR", dslr);

		char c = cvWaitKey(33);
		if (c == 0x1b) break;*/
	}

	//cvReleaseCapture(&capture);
	cvDestroyAllWindows();

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