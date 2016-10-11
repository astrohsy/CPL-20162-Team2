#include <iostream>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

const int MAXSTR = 100;
const char filePath[] = "./pattern\\%d.jpg";

void calibration(int board_w, int board_h, int n_boards, float measure);
void printCalibrationResult(int n_boards, Size imageSize, double reproj_err, Mat intrinsic_Matrix, Mat distortion_coeffs);