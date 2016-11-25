#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

class Calibration {
private:
	int board_w;
	int board_h;
	float measure;
	Mat H;

	const string result_name = "hormography.hr";
public:
	Calibration();
	Calibration(int w, int h, float mm);

	void CalibrationInit(int w, int h, float mm);
	void run_calibration(char * src_name, char * dst_name);
	Mat changePicture(char * src_name, char * dst_name);
	void saveResult();
	void loadResult();

	Mat getH();
};

