#include "stdafx.h"
#include "camera.h"

Mat image;
char s2_number[10];
char c_rgb[20];
int c_number = 0;

Camera::Camera(){
	err = EDS_ERR_OK;
	camera = NULL;
	cameraList = NULL;
	count = 0;
	isSDKLoaded = false;
}

void Camera::setErr(EdsError err)
{	Camera::err = err;
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

Mat Camera::downloadEvfData()
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
		Mat img = Mat(coords.height, coords.width, CV_8UC3, data);
		image = imdecode(img, CV_LOAD_IMAGE_COLOR);
		return image;
		//imshow("main", image);
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
	return image;
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
		cout << "SDK initialised" << endl;
	}
	if (err == EDS_ERR_OK)
	{
		err = getFirstCamera();
		if (err == EDS_ERR_OK)
			cout << "getFirstCamera ok" << endl;
	}
	if (err == EDS_ERR_OK)
	{
		err = EdsOpenSession(camera);
		if (err == EDS_ERR_OK)
			cout << "session open" << endl;
	}
}

void Camera::releaseSDK()
{
	EdsCloseSession(camera);
	EdsTerminateSDK();
}

void Camera::takePicture()
{
	EdsInt32 saveTarget = kEdsSaveTo_Host;
	err = EdsSetPropertyData(camera, kEdsPropID_SaveTo, 0, 4, &saveTarget);
	EdsCapacity newCapacity = { 0x7FFFFFFF, 0x1000, 1 };
	err = EdsSetCapacity(camera, newCapacity);

	sprintf(s2_number, "%d", ++c_number);
	strcpy(c_rgb, "c_rgb");
	strcat(c_rgb, s2_number);
	strcat(c_rgb, ".bmp");
	imwrite(c_rgb, image);

	string str_path = "./Img.jpg";
	const char* ch_dest = str_path.c_str();
	EdsCreateFileStream(ch_dest, kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, 0);

	EdsSendCommand(camera, kEdsCameraCommand_TakePicture, 0);
}