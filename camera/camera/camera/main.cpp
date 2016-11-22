#include "camera.h"

#include <thread>
#include <future>
#include <string>

bool programExit;
bool takingPicture;

bool inputDoing();
void evf(Camera dslr);

int main()
{
	namedWindow("main", WINDOW_AUTOSIZE);
	Camera dslr;
	programExit = false;
	takingPicture = false;

	// Initialize SDK
	dslr.initializeSDK();

	thread doingThread(inputDoing);
	evf(dslr);
	
	doingThread.join();
	dslr.releaseSDK();

	return 0;
}

bool inputDoing(){
	string doing;

	while (1){
		cin >> doing;
		if (doing == "z"){
			programExit = true;
			break;
		}
		if (doing == "t"){
			takingPicture = true;
		}
	}

	return 1;
}

void evf(Camera dslr){
	dslr.setErr(dslr.startLiveview());
	if (dslr.isOK())
	{
		cout << "start live view" << endl;
	}

	// live view
	while (1){
		Mat image = dslr.downloadEvfData();
		int c = cvWaitKey(10);

		if (takingPicture){
			takingPicture = false;
			dslr.takePicture();
			saveMat(image);
		}

		if (programExit)
			break;
	}
	//

	dslr.setErr(dslr.endLiveview());
	if (dslr.isOK())
	{
		cout << "end live view" << endl;
	}
}