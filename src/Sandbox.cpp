#include <opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

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

	Mat FrameD; 		Stream>>FrameD; 		Mat *Frame=&FrameD;					// I miss C Compound Literals!
	Mat OutputFrameD; 	Stream>>OutputFrameD; 	Mat *OutputFrame=&OutputFrameD;
	int pyrDownCicles=1;

	while (waitKey(1)!=27){
		Stream >> *Frame;
		Stream >> *OutputFrame;
		if (!Frame->data) cout<<"Invalid Frame!"<<endl;
		imshow("Video Stream Viewer", *Frame);
		createTrackbar("Number of pyrDown Cicles", "Output Stream Viewer", &pyrDownCicles, 10, NULL);
		for (char i=0; i<pyrDownCicles; ++i) pyrDown(*OutputFrame, *OutputFrame);
		imshow("Output Stream Viewer", *OutputFrame);
	}

	cout<<"Good bye cruel world! u.u"<<endl;
	return 0;
}
