#include <iostream>

#include "EDSDK.h"
#include "EDSDKErrors.h"
#include "EDSDKTypes.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Camera{
private:
	EdsError err;
	EdsCameraRef camera;
	EdsCameraListRef cameraList;
	EdsUInt32 count;
	bool isSDKLoaded;

public:
	Camera();

	void setErr(EdsError err);
	EdsError getErr();
	void setCamera(EdsCameraRef camera);
	EdsCameraRef getCamera();
	void setCameraList(EdsCameraListRef cameraList);
	EdsCameraListRef getCameraList();
	void setCount(EdsUInt32 count);
	EdsUInt32 getCount();
	void setIsSDKLoaded(bool isSDKLoaded);
	bool getIsSDKLoaded();

	EdsError getFirstCamera();
	EdsError startLiveview();
	EdsError endLiveview();
	Mat downloadEvfData();

	char * takePicture();

	void initializeSDK();
	bool isOK();
	void releaseSDK();
};