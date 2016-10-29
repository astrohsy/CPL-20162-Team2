#include <iostream>
#include <vector>
#include <string>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

Mat calibration(int board_w, int board_h, float measure, string src_name, string dst_name);
Mat changePicture(string src_name, string dst_name, Mat H);