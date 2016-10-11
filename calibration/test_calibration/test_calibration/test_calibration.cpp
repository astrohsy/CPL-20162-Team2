#include "calibration.h"

int main()
{
	int board_w, board_h, n_boards;
	float measure;
	char str[MAXSTR];

	cout << "How many cross points of width direction ?" << endl;
	cin >> board_w;
	cout << "How many cross points of Height direction?" << endl;
	cin >> board_h;

	cout << "How many board?" << endl;
	cin >> n_boards;

	cout << "What mm ?" << endl;
	cin >> measure;

	cout << "w=" << board_w << " h=" << board_h << " n=" << n_boards << " " << measure << "mm" << endl;
	cout << "-----------FindChessboardCorner------------" << endl;
	
	calibration(board_w, board_h, n_boards, measure);
}