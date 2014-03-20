#include <opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

typedef struct CannyEdgesParameters{
	int Threshold1;
	int Threshold2;
	int apertureSize;
	int L2Gradient;
} CannyEParams;

void L2GradChange(bool val){

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

	Mat FrameD; 			Stream>>FrameD; 		Mat *Frame=&FrameD;					// I miss C Compound Literals!
	Mat OutputFrameD; 		Stream>>OutputFrameD; 	Mat *OutputFrame=&OutputFrameD;
	CannyEParams CannyParamsD={60, 85, 0, true};	CannyEParams *CannyParams=&CannyParamsD;
	int pyrDownCicles=1;
	int CannyCicles=1;

	namedWindow("Canny Edges Parameters", CV_WINDOW_FREERATIO);


	while (waitKey(1)!=27){
		Stream >> *Frame;
		Stream >> *OutputFrame;
		if (!Frame->data) cout<<"Invalid Frame!"<<endl;
		imshow("Video Stream Viewer", *Frame);

		createTrackbar("pyrDown Cicles", "Output Stream Viewer", &pyrDownCicles, 10, NULL);
		createTrackbar("Canny Cicles", "Output Stream Viewer", &CannyCicles, 6, NULL);

		createTrackbar("Thrhold 1", "Canny Edges Parameters", &CannyParams->Threshold1, 100, NULL);
		createTrackbar("Thrhold 2", "Canny Edges Parameters", &CannyParams->Threshold2, 100, NULL);
		createTrackbar("Ap Size", "Canny Edges Parameters", &CannyParams->apertureSize, 2, NULL);
		createTrackbar("L2 Grad", "Canny Edges Parameters", &CannyParams->L2Gradient, 1, NULL);

		for (char i=0; i<pyrDownCicles; ++i) pyrDown(*OutputFrame, *OutputFrame);
		for (char i=0; i<CannyCicles; ++i) Canny(*OutputFrame, *OutputFrame, CannyParams->Threshold1, CannyParams->Threshold2, 2*CannyParams->apertureSize+3, CannyParams->L2Gradient>0?true:false);

		imshow("Output Stream Viewer", *OutputFrame);
	}

	cout<<"Good bye cruel world! u.u"<<endl;
	destroyAllWindows();
	Stream.release();
	return 0;
}
