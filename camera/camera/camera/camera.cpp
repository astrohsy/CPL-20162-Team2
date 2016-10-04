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

/*
EdsError startLiveview(EdsCameraRef camera)
{
	EdsError err = EDS_ERR_OK;
	// Get the output device for the live view image
	EdsUInt32 device;
	err = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	// PC live view starts by setting the PC as the output device for the live view image.
	if (err == EDS_ERR_OK)
	{
		device |= kEdsEvfOutputDevice_PC;
		err = EdsSetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	}
	// A property change event notification is issued from the camera if property settings are made successfully.
	// Start downloading of the live view image once the property change notification arrives.

	return err;
}
*/


Mat downloadEvfData(EdsCameraRef camera)
{
	EdsError err = EDS_ERR_OK;
	EdsStreamRef stream = NULL;
	EdsEvfImageRef evfImage = NULL;

	unsigned char *data = NULL;
	EdsUInt32 size = 0;
	EdsSize coords;

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
		// Get the zoom ratio
		EdsUInt32 zoom;
		EdsGetPropertyData(evfImage, kEdsPropID_Evf_ZoomPosition, 0, sizeof(zoom), &zoom);
		// Get the focus and zoom border position
		EdsPoint point;
		EdsGetPropertyData(evfImage, kEdsPropID_Evf_ZoomPosition, 0, sizeof(point), &point);
	}

	// get image coordinates
	EdsGetPropertyData(evfImage, kEdsPropID_Evf_CoordinateSystem, 0, sizeof(coords), &coords);

	// get buffer pointer and size
	EdsGetPointer(stream, (EdsVoid**)&data);
	//EdsGetLenth(stream, &size);

	// create mat object
	Mat img(coords.height, coords.width, CV_8U, data);
	Mat image = imdecode(img, CV_LOAD_IMAGE_COLOR);

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


/*
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
*/


bool printingPicture(){
	IplImage* dslr = 0;
	namedWindow("DSLR", 1);

	theApp._model->setEvfMode(0x01);
	theApp._model->setEvfAFMode(0x01);
	theApp._model->setEvfOutputDevice(2147483650);
	theApp._model->setEvfDepthOfFieldPreview(0);

	//startLiveview(theApp._model->getCameraObject());
	(*theApp._controller).actionPerformed(*new ActionEvent("startEVF"));


	while (1){
		if (threadExit)
			break;

		imshow("DSLR", downloadEvfData(theApp._model->getCameraObject()));
		char c = cvWaitKey(50);
	}

	(*theApp._controller).actionPerformed(*new ActionEvent("endEVF"));
	cvDestroyAllWindows();

	cout << "thread end" << endl;

	return true;
}

/*
bool printingPicture(){
	IplImage* dslr = 0;
	namedWindow("DSLR", 1);

	EdsError err = EDS_ERR_OK;

	EdsEvfImageRef evfImage = NULL;
	EdsStreamRef stream = NULL;
	EdsUInt32 bufferSize = 2 * 1024 * 1024;

	unsigned char *imageData = NULL;

	theApp._model->setEvfMode(0x01);
	theApp._model->setEvfAFMode(0x01);
	theApp._model->setEvfOutputDevice(2147483650);
	theApp._model->setEvfDepthOfFieldPreview(0);
	(*theApp._controller).actionPerformed(*new ActionEvent("startEVF"));

	cout << theApp._model->getEvfOutputDevice() << ' ' << kEdsEvfOutputDevice_PC << ' ' << (theApp._model->getEvfOutputDevice() & kEdsEvfOutputDevice_PC) << endl;

	while (1){
		if (threadExit)
			break;

		// Exit unless during live view.
		if ((theApp._model->getEvfOutputDevice() & kEdsEvfOutputDevice_PC) == 0)
		{
			return true;
		}

		// Create memory stream.
		err = EdsCreateMemoryStream(bufferSize, &stream);

		// When creating to a file.
		// err = EdsCreateFileStream("C:\\test.jpg", kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, &stream);

		// Create EvfImageRef.
		if (err == EDS_ERR_OK)
		{
			err = EdsCreateEvfImageRef(stream, &evfImage);
		}

		// Download live view image data.
		if (err == EDS_ERR_OK)
		{
			err = EdsDownloadEvfImage(theApp._model->getCameraObject(), evfImage);
		}

		if (err == EDS_ERR_OK)
		{
			cout << "okay1" << endl;

			EVF_DATASET dataSet = { 0 };

			dataSet.stream = stream;

			cout << "okay2" << endl;

			// Get position of image data. (when enlarging)
			// Upper left coordinate using JPEG Large size as a reference.
			EdsGetPropertyData(evfImage, kEdsPropID_Evf_ImagePosition, 0, sizeof(dataSet.imagePosition), &dataSet.imagePosition);

			// Get the size as a reference of the coordinates of rectangle of the focus border.
			EdsGetPropertyData(evfImage, kEdsPropID_Evf_CoordinateSystem, 0, sizeof(dataSet.sizeJpegLarge), &dataSet.sizeJpegLarge);

			cout << "okay3" << endl;

			// Live view image transfer complete notification.
			if (err == EDS_ERR_OK)
			{
				CameraEvent e("EvfDataChanged", &dataSet);
				theApp._model->notifyObservers(&e);
			}
			
			cout << "okay4" << endl;
			
			imageData = &stream;
			Mat img(dataSet.sizeJpegLarge.height, dataSet.sizeJpegLarge.width, CV_8U, imageData);
			Mat image = imd	ecode(img, CV_LOAD_IMAGE_COLOR);
			
			cout << "okay5" << ' ' << image << endl;
			
			imshow("DSLR", image);

			cout << "okay6" << endl;
		}

		cout << "err" << endl;

		char c = cvWaitKey(50);
	}

	if (stream != NULL)
	{
		EdsRelease(stream);
		stream = NULL;
	}

	if (evfImage != NULL)
	{
		EdsRelease(evfImage);
		evfImage = NULL;
	}

	(*theApp._controller).actionPerformed(*new ActionEvent("endEVF"));
	cvDestroyAllWindows();

	cout << "thread end" << endl;

	return true;
}
*/

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

/*
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
*/