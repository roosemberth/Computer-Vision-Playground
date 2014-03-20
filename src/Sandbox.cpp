#include <opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

typedef struct gaussBlurParams{
	int ksize;
	int stdX;
	int stdY;
	int borderType;
} GaussBlurParams;

void ProcessFrames(Mat In, Mat Out, GaussBlurParams GBPs){
	GaussianBlur(In, Out, Size(GBPs.ksize==0?0:2*GBPs.ksize-1, GBPs.ksize==0?0:2*GBPs.ksize-1), GBPs.ksize==0?1:GBPs.stdX, GBPs.ksize==0?1:GBPs.stdY);
}

int main(int argc, char** argv){
	VideoCapture Stream;
	if (argc==1){
		Stream.open(0);				// Default Camera
		if (!Stream.isOpened()){
			cout << "Couldn't Open Capture Device" << endl;
			return -1;
		}
	} else {
		Stream.open(argv[1]);		// Open a Video File
	}
	namedWindow("Video Stream Viewer", WINDOW_NORMAL);
	namedWindow("Output Stream Viewer", WINDOW_NORMAL);
	Mat FrameD;
	Stream>>FrameD;
	Mat *Frame=&FrameD;
	Mat OutputFrameD;
	Stream>>OutputFrameD;
	Mat *OutputFrame=&OutputFrameD;
	GaussBlurParams DefGBP= {5, 3, 3, 0};
	GaussBlurParams *GBPs = &DefGBP;
	while (waitKey(1)!=27){
		Stream >> *Frame;
		if (!Frame->data) cout<<"Invalid Frame!"<<endl;
		imshow("Video Stream Viewer", *Frame);
		createTrackbar("Kernel Size", "Output Stream Viewer", &GBPs->ksize, 100, NULL);
		createTrackbar("STD X Sigma", "Output Stream Viewer", &GBPs->stdX, 100, NULL);
		createTrackbar("STD Y Sigma", "Output Stream Viewer", &GBPs->stdX, 100, NULL);
		ProcessFrames(*Frame, *OutputFrame, *GBPs);
		imshow("Output Stream Viewer", *OutputFrame);
	}
	cout<<"Good bye cruel world! u.u"<<endl;
	return 0;
}
