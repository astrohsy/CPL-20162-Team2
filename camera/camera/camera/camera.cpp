#include <iostream>
#include <string>
#include <thread>
#include <future>

#include "EDSDK.h"
#include "EDSDKTypes.h"
#include "CameraModel.h"
#include "CameraModelLegacy.h"
#include "CameraController.h"
#include "CameraEventListener.h"

#include "CameraControl.h"

using namespace std;

bool threadExit;

bool printingPicture(){
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