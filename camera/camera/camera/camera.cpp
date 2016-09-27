#include <iostream>
#include <string>

#include "EDSDK.h"
#include "EDSDKTypes.h"
#include "CameraModel.h"
#include "CameraModelLegacy.h"
#include "CameraController.h"
#include "CameraEventListener.h"

#include "CameraControl.h"

using namespace std;

void take_picture(CCameraControlApp theApp){


}

int main()
{
	CCameraControlApp theApp;
	theApp.InitInstance();

	string doing;

	while (1){ 
		cin >> doing;

		if (doing == "t"){
			theApp._controller
		}

		else if (doing == "z")
			break;
	}

	theApp.release();
}