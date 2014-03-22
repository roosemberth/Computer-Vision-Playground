#include <opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

float ArrayStdDeviation(uchar Neighborhood [], Size NSize){
	int Mean=0;
	for (int x=0;x<NSize.width;++x){
		for (int y=0;y<NSize.height;++y){
			Mean += Neighborhood[x*NSize.height+y];
		}
	}
	Mean /= NSize.width*NSize.height;

	int STD=0;
	for (int x=0;x<NSize.width;++x){
		for (int y=0;y<NSize.height;++y){
			STD += abs(Mean-Neighborhood[x*NSize.height+y]);
		}
	}
	STD /= NSize.width*NSize.height;

	return STD;
}

void UpdateNeighborhoodWidth(int newWidth, void *NHSizeO){
	Size* NHSize = (Size*) NHSizeO;
	NHSize->width=3+2*newWidth;
}
void UpdateNeighborhoodHeight(int newHeight, void *NHSizeO){
	Size* NHSize = (Size*) NHSizeO;
	NHSize->height=3+2*newHeight;
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

	namedWindow("Video Stream Viewer", CV_WINDOW_KEEPRATIO);
	namedWindow("Output Stream Viewer", CV_WINDOW_KEEPRATIO);

	Mat FrameD; 			Stream>>FrameD; 		Mat *Frame=&FrameD;					// I miss C Compound Literals!
	Mat OutputFrameD; 		Stream>>OutputFrameD; 	Mat *OutputFrame=&OutputFrameD;

	Size NHSizeD=Size(3,3);	Size *NHSize=&NHSizeD;										// NeighborhoodSize elements must be odd!
	int Amplificator=10;
	int NHSizeW=0;
	int NHSizeH=0;

	namedWindow("Border Processor Parameters", CV_WINDOW_FREERATIO);

	while (waitKey(1)!=27){
		Stream >> *Frame;
		*OutputFrame=Scalar(0,0,0);
		if (!Frame->data) cout<<"Invalid Frame!"<<endl;
		imshow("Video Stream Viewer", *Frame);

		createTrackbar("STD Amplificator", "Border Processor Parameters", &Amplificator, 10, NULL);
		createTrackbar("Size of Neighbohood", "Border Processor Parameters", &NHSizeW, 6, &UpdateNeighborhoodWidth, NHSize);
		createTrackbar("Height of Neighbohood", "Border Processor Parameters", &NHSizeH, 6, &UpdateNeighborhoodHeight, NHSize);

		uchar PixelNBH[NHSize->width*NHSize->height];
		for (int ColorPixel=0; ColorPixel<3; ++ColorPixel){
			for (int xPix=(NHSize->width-1)/2; xPix<=Frame->cols-(NHSize->width-1)/2; ++xPix){
				for (int yPix=(NHSize->height-1)/2; yPix<Frame->rows-(NHSize->height-1)/2; ++yPix){
					for (int Ny=0; Ny<NHSize->height; ++Ny){
						for (int Nx=0; Nx<NHSize->width; ++Nx){
							PixelNBH[Ny*NHSize->width + Nx]=Frame->data[Frame->step[0]*(yPix+Ny-(NHSize->height-1)/2) + Frame->step[1]*(xPix+Nx-(NHSize->width-1)/2) + ColorPixel];
						}
					}
					OutputFrame->data[OutputFrame->step[0]*yPix + OutputFrame->step[1]*xPix + 0] += ArrayStdDeviation(PixelNBH, *NHSize)*Amplificator;
					OutputFrame->data[OutputFrame->step[0]*yPix + OutputFrame->step[1]*xPix + 1] += ArrayStdDeviation(PixelNBH, *NHSize)*Amplificator;
					OutputFrame->data[OutputFrame->step[0]*yPix + OutputFrame->step[1]*xPix + 2] += ArrayStdDeviation(PixelNBH, *NHSize)*Amplificator;
				}
			}
		}
		imshow("Output Stream Viewer", *OutputFrame);
	}

	cout<<"Good bye cruel world! u.u"<<endl;
	destroyAllWindows();
	Stream.release();
	return 0;
}
