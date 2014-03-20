#include <highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
	VideoCapture Stream;
	if (argc==1){
		Stream.open(0);				// Default Camera
	} else {
		Stream.open(argv[1]);		// Open a Video File
	}
	if (!Stream.isOpened()){
		cout << "Couldn't Open Capture Device" << endl;
		return -1;
	}
	Mat frame;
	namedWindow("Video Stream Viewer", WINDOW_NORMAL);
	while (waitKey(1)!='c'){
		Stream >> frame;
		if (!frame.data) break;
		imshow("Video Stream Viewer", frame);
	}
	cout<<"Good bye cruel world! u.u"<<endl;
	return 0;
}
