#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

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

bool printingPicture(){
	//namedWindow("Display window", 1);


	while (1){
		if (threadExit)
			break;


	}

	return true;
}

int main()
{
	CCameraControlApp theApp;
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