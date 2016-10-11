#include "camera.h"

#include <iostream>
#include <thread>
#include <future>
#include <string>

bool evfValue;

bool evf(Camera dslr){
	

	return 1;
}

int main()
{
	string doing;
	namedWindow("main", WINDOW_AUTOSIZE);
	Camera dslr;
	evfValue = false;

	// Initialize SDK
	dslr.initializeSDK();

	thread evfThread(evf, dslr);

	/*while (1){
		cin >> doing;
		if (doing == "z"){
			evfValue = true;
			break;
		}
	}*/

	dslr.setErr(dslr.startLiveview());
	if (dslr.isOK())
	{
		cout << "start live view" << endl;
	}

	// live view
	while (1){
		dslr.downloadEvfData();
		int c = cvWaitKey(10);
		if (c == 32)
			break;

		if (evfValue)
			break;
	}
	//

	dslr.setErr(dslr.endLiveview());
	if (dslr.isOK())
	{
		cout << "end live view" << endl;
	}

	evfThread.join();

	return 0;
}