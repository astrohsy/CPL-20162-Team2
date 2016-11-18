#include "calibration.h"

const int w = 8;
const int h = 5;
const float mm = 3.5;
const string src = "camera.bmp";
const string dst = "rgb.bmp";

void main()
{
	Calibration cali(w, h, mm, src, dst);

	cali.run_calibration();
	Mat result = cali.changePicture();
	cali.saveResult();
	cout << cali.getH() << endl;
	cali.loadResult();
	cout << cali.getH() << endl;

	imwrite("result.bmp", result);
	system("pause");
}