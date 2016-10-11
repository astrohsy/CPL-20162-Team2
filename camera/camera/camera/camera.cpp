#include <stdio.h>

#include "EDSDK.h"
#include "EDSDKErrors.h"
#include "EDSDKTypes.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

EdsError getFirstCamera(EdsCameraRef *camera)
{
	EdsError err = EDS_ERR_OK;
	EdsCameraListRef cameraList = NULL;
	EdsUInt32 count = 0;
	// Get camera list
	err = EdsGetCameraList(&cameraList);
	// Get number of cameras
	if (err == EDS_ERR_OK)
	{
		err = EdsGetChildCount(cameraList, &count);
		if (count == 0)
		{
			err = EDS_ERR_DEVICE_NOT_FOUND;
		}

	}
	// Get first camera retrieved
	if (err == EDS_ERR_OK)
	{
		err = EdsGetChildAtIndex(cameraList, 0, camera);
	}
	// Release camera list
	if (cameraList != NULL)
	{
		EdsRelease(cameraList);
		cameraList = NULL;
	}
	return err;
}

EdsError startLiveview(EdsCameraRef camera)
{
	EdsError err = EDS_ERR_OK;

	EdsUInt32 device;
	err = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);

	if (err == EDS_ERR_OK)
	{
		device |= kEdsEvfOutputDevice_PC;
		err = EdsSetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	}
	return err;

}

EdsError endLiveview(EdsCameraRef camera)
{
	EdsError err = EDS_ERR_OK;
	// Get the output device for the live view image
	EdsUInt32 device;
	err = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	// PC live view ends if the PC is disconnected from the live view image output device.
	if (err == EDS_ERR_OK)
	{
		device &= ~kEdsEvfOutputDevice_PC;
		err = EdsSetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	}
	return err;
}

EdsError downloadEvfData(EdsCameraRef camera)
{

	EdsError err = EDS_ERR_OK;
	EdsStreamRef stream = NULL;
	EdsEvfImageRef evfImage = NULL;
	EdsSize coords;
	EdsUInt64 size = 0;
	unsigned char *data = NULL;

	// Create memory stream.
	err = EdsCreateMemoryStream(0, &stream);
	// Create EvfImageRef.
	if (err == EDS_ERR_OK)
	{
		err = EdsCreateEvfImageRef(stream, &evfImage);
	}
	// Download live view image data.
	if (err == EDS_ERR_OK)
	{
		err = EdsDownloadEvfImage(camera, evfImage);
	}
	// Get the incidental data of the image.
	if (err == EDS_ERR_OK)
	{
		EdsGetPropertyData(evfImage, kEdsPropID_Evf_CoordinateSystem, 0, sizeof(coords), &coords);
		EdsGetPointer(stream, (EdsVoid**)&data);
	}
	if (data != NULL)
	{
		EdsGetLength(stream, &size);
		Mat image;
		Mat img = Mat(coords.height, coords.width, CV_8UC3, data);
		image = imdecode(img, CV_LOAD_IMAGE_COLOR);
		imshow("main", image);

	}
	//
	// Display image

	//
	// Release stream
	if (stream != NULL)
	{
		EdsRelease(stream);
		stream = NULL;
	}
	// Release evfImage
	if (evfImage != NULL)
	{
		EdsRelease(evfImage);
		evfImage = NULL;
	}
	return err;
}

int main()
{
	int c;
	bool show = true;
	namedWindow("main", WINDOW_AUTOSIZE);
	EdsError err = EDS_ERR_OK;
	EdsCameraRef camera = NULL;
	EdsCameraListRef cameraList = NULL;
	EdsUInt32 count = 0;
	bool isSDKLoaded = false;
	// Initialize SDK
	err = EdsInitializeSDK(); // If camera is initialised, err = EDS_ERR_OK
	if (err == EDS_ERR_OK)
	{
		isSDKLoaded = true; // isSDKLoaded is true if initialised
		printf("SDK initialised \n");
	}
	if (err == EDS_ERR_OK)
	{
		err = getFirstCamera(&camera);
		if (err == EDS_ERR_OK)
			printf("getFirstCamera ok \n");
	}
	if (err == EDS_ERR_OK)
	{
		err = EdsOpenSession(camera);
		if (err == EDS_ERR_OK)
			printf("session open \n");
	}
	err = startLiveview(camera);
	if (err == EDS_ERR_OK)
	{
		printf("start live view \n");
	}
	// live view
	for (;;)
	{
		downloadEvfData(camera);
		c = cvWaitKey(10);
		if (c == 32)
		{
			break;
		}
	}
	//
	err = endLiveview(camera);
	if (err == EDS_ERR_OK)
	{
		printf("end live view \n");
	}
	return 0;
}