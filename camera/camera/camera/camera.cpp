#include "camera.h"

Camera::Camera(){
	err = EDS_ERR_OK;
	camera = NULL;
	cameraList = NULL;
	count = 0;
	isSDKLoaded = false;
}

void Camera::setErr(EdsError err)
{
	Camera::err = err;
}

EdsError Camera::getErr()
{
	return err;
}

void Camera::setCamera(EdsCameraRef camera)
{
	Camera::camera = camera;
}

EdsCameraRef Camera::getCamera()
{
	return camera;
}

void Camera::setCameraList(EdsCameraListRef cameraList)
{
	Camera::cameraList = cameraList;
}

EdsCameraListRef Camera::getCameraList()
{
	return cameraList;
}

void Camera::setCount(EdsUInt32 count)
{
	Camera::count = count;
}

EdsUInt32 Camera::getCount()
{
	return count;
}

void Camera::setIsSDKLoaded(bool isSDKLoaded)
{
	Camera::isSDKLoaded = isSDKLoaded;
}

bool Camera::getIsSDKLoaded()
{
	return isSDKLoaded;
}

EdsError Camera::getFirstCamera()
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
		err = EdsGetChildAtIndex(cameraList, 0, &camera);
	}
	// Release camera list
	if (cameraList != NULL)
	{
		EdsRelease(cameraList);
		cameraList = NULL;
	}
	return err;
}

EdsError Camera::startLiveview()
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

EdsError Camera::endLiveview()
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

EdsError Camera::downloadEvfData()
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

	// Display image
	if (data != NULL)
	{
		EdsGetLength(stream, &size);
		Mat image;
		Mat img = Mat(coords.height, coords.width, CV_8UC3, data);
		image = imdecode(img, CV_LOAD_IMAGE_COLOR);
		imshow("main", image);
	}

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

bool Camera::isOK()
{
	if (getErr() == EDS_ERR_OK)
		return true;
	else
		return false;
}

void Camera::initializeSDK()
{
	err = EdsInitializeSDK(); // If camera is initialised, err = EDS_ERR_OK
	if (err == EDS_ERR_OK)
	{
		isSDKLoaded = true; // isSDKLoaded is true if initialised
		printf("SDK initialised \n");
	}
	if (err == EDS_ERR_OK)
	{
		err = getFirstCamera();
		if (err == EDS_ERR_OK)
			printf("getFirstCamera ok \n");
	}
	if (err == EDS_ERR_OK)
	{
		err = EdsOpenSession(camera);
		if (err == EDS_ERR_OK)
			printf("session open \n");
	}
}