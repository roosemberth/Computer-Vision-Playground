#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
	VideoCapture VideoStream;
	switch (argc){
	case 1:
		VideoStream.open(0);
		if (!VideoStream.isOpened()){
			cout << "Couldn't Open Capture Device" << endl;
			return -1;
		}
		cout << "Opened Capture Device Number 0" << endl;
		break;
	case 2:
		VideoStream.open(argv[1]);
		if (!VideoStream.isOpened()){
			cout << "Couldn't Open Video Stream" << endl;
			return -1;
		}
		cout << "Opened Video Stream " << '"' << argv[1] << '"' << endl;
		break;
	default:
		//TODO: Implement other arguments handler.
		break;
	}

	const char Window_InVideo[] = "Input Video Stream Viewer";
	const char Window_OutVideo[] = "Output Stream Viewer";

	namedWindow(Window_InVideo, CV_WINDOW_KEEPRATIO);
	namedWindow(Window_OutVideo, CV_WINDOW_KEEPRATIO);

	Mat Frame;
	Mat GrayFrame;
	Mat OutputFrame;

	const int Amplificator = 10;

	while (waitKey(100)!=27){
		VideoStream.read(Frame);
		cvtColor(Frame, GrayFrame, CV_RGB2GRAY);
		imshow(Window_InVideo, Frame);
		imshow(Window_OutVideo, GrayFrame);

	}

	cout << "Program End" << endl;
	destroyAllWindows();
	VideoStream.release();
	return 0;
}
