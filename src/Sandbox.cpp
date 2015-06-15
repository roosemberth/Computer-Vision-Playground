#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>

#include <CL/cl.h>
#include "SystechDefinitions.h"

// ----------------------------------------------------------------------------- General Configuration
#ifndef DebugLevel
	#define DebugLevel 				0x0E	// 0x1110b
#endif

// ---------------------------------------------------------------------- End of General Configuration

// --------------------------------------------------------------------------- Preprocessor Directives

// Debug Functions
#define OpenCLFault() cleanUpOpenCL(SandboxCLContext, MaliCommandQueue, NebulaProgram, \
		NebulaKernel, FrameImages, NumFrameImages)

#define CatchCLFault(cond, fmt, args...) if (cond) {OpenCLFault(); \
	DEBUG_ERROR("[OpenCL Fault] " fmt, ## args); return -1;}

#define CatchGeneralFault(cond, fmt, args...) if (cond) {DEBUG_ERROR(fmt, ## args); return -1;}
// ------------------------------------------------------------------- End of  Preprocessor Directives

#include "OpenCL-Common.h"

using namespace cv;
using namespace std;

cl_mem FrameImages[6] = {0,0,0,0,0,0};	// [0-2] Left Frame, [3-5] Right Frame: In, Gray, Borders
char NumFrameImages = 6;

int main(int argc, char** argv){
	DEBUG_INFO("Program Entry Point, Compiled on: %s:%s", __DATE__, __TIME__);
	// ------------------------------------------------------------------------- OpenCV Initialization
	DEBUG_INFO("Starting OpenCV Initialization");
	VideoCapture LStream;
	VideoCapture RStream;
	if (argc<3){
		cerr << "Please specify Left and Right Streams as Arguments" << endl;
		return 1;
	} else {
		LStream.open(argv[1]);		// Open Left Stream
		RStream.open(argv[2]);		// Open Right Stream
		cout << "Opened " << '"' << argv[1] << '"' << " for Left Stream" << endl;
		cout << "Opened " << '"' << argv[2] << '"' << " for Right Stream" << endl;
	}

	const char Window_LStream_Title[] = "Left Stream Viewer";
	const char Window_RStream_Title[] = "Right Stream Viewer";
	const char Window_DepthMap_Title[] = "Depth Map Viewer";

	const char Window_LBorders_Title[] = "Left Stream Border Map";
	const char Window_RBorders_Title[] = "Right Stream Border Map";

	namedWindow(Window_LStream_Title, CV_WINDOW_NORMAL);
	namedWindow(Window_RStream_Title, CV_WINDOW_NORMAL);
	namedWindow(Window_LBorders_Title, CV_WINDOW_NORMAL);
	namedWindow(Window_RBorders_Title, CV_WINDOW_NORMAL);
//	namedWindow(Window_DepthMap_Title, CV_WINDOW_KEEPRATIO);

	Mat LFrame, RFrame;
	LStream.read(LFrame);
	RStream.read(RFrame);

//	Mat LBorders, RBorders;
	Mat LBorders(LFrame.rows, LFrame.cols, CV_8UC1, NULL, (unsigned int)LFrame.step/3);
	Mat RBorders(RFrame.rows, RFrame.cols, CV_8UC1, NULL, (unsigned int)RFrame.step/3);
	DEBUG_INFO("End of OpenCV Initialization");
	// --------------------------------------------------------------------- End OpenCV Initialization

	// ------------------------------------------------------------------------- OpenCL Initialization
	DEBUG_INFO("OpenCL Initialization");
	cl_context SandboxCLContext = 0;
	cl_command_queue MaliCommandQueue = 0;
	cl_program NebulaProgram = 0;
	cl_program BGRtoGrayProgram = 0;
	cl_device_id DeviceID = 0;
	cl_kernel BGRtoGrayKernel = 0;
	cl_kernel NebulaKernel = 0;
	cl_int errorCode;

	CatchCLFault(!createContext(&SandboxCLContext), "Failed to create an OpenCL Context!")
	CatchCLFault(!createCommandQueue(SandboxCLContext, &MaliCommandQueue, &DeviceID), \
			"Failed to create the OpenCL Command Queue")
	CatchCLFault(!createProgram(SandboxCLContext, DeviceID, "src/Nebula.cl", &NebulaProgram), \
			"Failed to create OpenCL \"Nebula\" program")
	CatchCLFault(!createProgram(SandboxCLContext, DeviceID, "src/BGRtoGray.cl", &BGRtoGrayProgram), \
			"Failed to create OpenCL \"BGR2Gray\" program")

	NebulaKernel = clCreateKernel(NebulaProgram, "NebulaKernel", &errorCode);
    CatchCLFault(!checkSuccess(errorCode), "Failed to create OpenCL kernel!")

	BGRtoGrayKernel = clCreateKernel(BGRtoGrayProgram, "BGRtoGrayKernel", &errorCode);
    CatchCLFault(!checkSuccess(errorCode), "Failed to create OpenCL kernel!")

    // TODO: Fix Nebula Events Name Conventions
    typedef struct S_SandboxCLEvents{
		cl_event SandboxCLEventsArray[12];

    	cl_event& MapLInFrame()		{return SandboxCLEventsArray[0];}
    	cl_event& UnMapLInFrame()	{return SandboxCLEventsArray[1];}
    	cl_event& LBGRtoGrayKernel()	{return SandboxCLEventsArray[2];}
    	cl_event& LNebulaKernel()	{return SandboxCLEventsArray[3];}
    	cl_event& MapLOutFrame()		{return SandboxCLEventsArray[4];}
    	cl_event& UnMapLOutFrame()	{return SandboxCLEventsArray[5];}
    	cl_event& MapRInFrame()		{return SandboxCLEventsArray[6];}
    	cl_event& UnMapRInFrame()	{return SandboxCLEventsArray[7];}
    	cl_event& RBGRtoGrayKernel()	{return SandboxCLEventsArray[8];}
    	cl_event& RNebulaKernel()	{return SandboxCLEventsArray[9];}
    	cl_event& MapROutFrame()		{return SandboxCLEventsArray[10];}
    	cl_event& UnMapROutFrame()	{return SandboxCLEventsArray[11];}
    } T_SandboxCLEvents;

    T_SandboxCLEvents SandboxCLEvents;

    uchar* CL_LInFrame;
    uchar* CL_RInFrame;
    uchar* CL_LOutFrame;
    uchar* CL_ROutFrame;
    cl_image_format CL_GrayFrameFormat;
    CL_GrayFrameFormat.image_channel_order = CL_RGBA;
    CL_GrayFrameFormat.image_channel_data_type = CL_FLOAT;

    size_t RGBFrameSize 	 = 3*sizeof(unsigned char)*LFrame.cols*LFrame.rows;
    size_t GrayFrameSize 	 = 1*sizeof(unsigned char)*LFrame.cols*LFrame.rows;
	size_t RGBImageRowPitch  = 3*sizeof(unsigned char)*LFrame.cols;
	size_t GrayImageRowPitch = 1*sizeof(unsigned char)*LFrame.cols;

    struct S_ImageFrameDescriptor{
    	size_t ImageOrigin[3];
    	size_t ImageRegion[3];
    	size_t ImageRowPitch;
    	size_t ImageSlicePitch;
    }ImageFrameDescriptor;

    ImageFrameDescriptor.ImageOrigin[0] = 0;
    ImageFrameDescriptor.ImageOrigin[1] = 0;
    ImageFrameDescriptor.ImageOrigin[2] = 0;
    ImageFrameDescriptor.ImageRegion[0] = LFrame.cols;
    ImageFrameDescriptor.ImageRegion[1] = LFrame.rows;
    ImageFrameDescriptor.ImageRegion[2] = 1;
    ImageFrameDescriptor.ImageRowPitch = 4*LFrame.cols * sizeof(uchar);
    ImageFrameDescriptor.ImageSlicePitch = 0;

    size_t globalWorkSize[3];
    globalWorkSize[0] = LFrame.cols;
    globalWorkSize[1] = LFrame.rows;
    globalWorkSize[2] = 1;

    DEBUG_INFO("End of OpenCL Initialization");
	// --------------------------------------------------------------------- End OpenCL Initialization

    // ----------------------------------------------------------------------------- Nebula Constrains
    DEBUG_INFO("Setting up Nebula Constrains");

    DEBUG_INFO("Fetching Guide Frame");
	//LStream.read(LFrame); Already Readed!
	unsigned int NebulaAmplificator = 10;
	bool CLOpsSuccess = true;

	DEBUG_INFO("Creating OpenCL Image Objects");
	FrameImages[0] = clCreateBuffer(SandboxCLContext, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, \
			RGBFrameSize, NULL, &errorCode);									// Left InFrame
	CLOpsSuccess &= checkSuccess(errorCode);

	FrameImages[1] = clCreateImage2D(SandboxCLContext, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, \
			&CL_GrayFrameFormat, LFrame.cols, LFrame.rows, 0, NULL, &errorCode); // Left GrayFrame
	CLOpsSuccess  &= checkSuccess(errorCode);

	FrameImages[2] = clCreateBuffer(SandboxCLContext, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR, \
			GrayFrameSize, NULL, &errorCode);								// Left Border Map
	CLOpsSuccess  &= checkSuccess(errorCode);

	FrameImages[3] = clCreateBuffer(SandboxCLContext, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, \
			RGBFrameSize, NULL, &errorCode);									// Right InFrame
	CLOpsSuccess &= checkSuccess(errorCode);

	FrameImages[4] = clCreateImage2D(SandboxCLContext, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, \
			&CL_GrayFrameFormat, LFrame.cols, LFrame.rows, 0, NULL, &errorCode); // Right GrayFrame
	CLOpsSuccess  &= checkSuccess(errorCode);

	FrameImages[5] = clCreateBuffer(SandboxCLContext, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR, \
			GrayFrameSize, NULL, &errorCode);								// Right Border Map
	CLOpsSuccess  &= checkSuccess(errorCode);


	CatchCLFault(!CLOpsSuccess, "Failed to create OpenCL Image/Buffer Objects")
	DEBUG_INFO("Successfully Created Image and Buffer Objects");

	clock_t Start_Time, End_Time;
	int FramesElapsed = 0;
	// ---------------------------------------------------------------------- End of Nebula Constrains
	// TODO: [Important] Redefine Process Lifetime
	while (waitKey(1)!=27 && FramesElapsed++<LStream.get(CV_CAP_PROP_FRAME_COUNT)){
		Start_Time = clock();
		// TODO: [Important] Improve Event Handling

		DEBUG_INFO("Enqueuing OpenCL Map InFrame Image Buffer");
		CL_LInFrame = (uchar*)clEnqueueMapBuffer(MaliCommandQueue, FrameImages[0], CL_FALSE, CL_MAP_WRITE, \
				0, RGBFrameSize, 0, NULL, &SandboxCLEvents.MapLInFrame(), &errorCode);
		CLOpsSuccess &= errorCode;
		CL_RInFrame = (uchar*)clEnqueueMapBuffer(MaliCommandQueue, FrameImages[3], CL_FALSE, CL_MAP_WRITE, \
				0, RGBFrameSize, 0, NULL, &SandboxCLEvents.MapRInFrame(), &errorCode);
				CLOpsSuccess &= errorCode;
		CatchCLFault(!checkSuccess(CLOpsSuccess), "Failed to Map OpenCL [L-R]InFrame Image Buffers.")
		DEBUG_INFO("OpenCL [L-R]InFrame Image Buffer Successfully Mapped!");
		LFrame.data = CL_LInFrame;							// Pointing to OpenCL Allocated Memory
		RFrame.data = CL_RInFrame;							// Pointing to OpenCL Allocated Memory
		clWaitForEvents(1, &SandboxCLEvents.MapLInFrame());	// Wait until GPU Mem is Mapped on Host
		clWaitForEvents(1, &SandboxCLEvents.MapRInFrame());	// Wait until GPU Mem is Mapped on Host
		LStream.read(LFrame);							// Get LFrame
		RStream.read(RFrame);							// Get RFrame

		CatchCLFault(clEnqueueUnmapMemObject(MaliCommandQueue, FrameImages[0], CL_LInFrame, 0, NULL, \
				&SandboxCLEvents.UnMapLInFrame()), "Error While Unmapping LInFrame Buffer Memory!")
		CatchCLFault(clEnqueueUnmapMemObject(MaliCommandQueue, FrameImages[3], CL_RInFrame, 0, NULL, \
				&SandboxCLEvents.UnMapRInFrame()), "Error While Unmapping RInFrame Buffer Memory!")
		DEBUG_INFO("OpenCL InFrame Image Buffer Successfully UnMapped!");


		DEBUG_INFO("Setting LBGR2Gray OpenCL Kernel Arguments");
		CLOpsSuccess &= checkSuccess(clSetKernelArg(BGRtoGrayKernel, 0, sizeof(cl_mem), &FrameImages[0]));
		CLOpsSuccess &= checkSuccess(clSetKernelArg(BGRtoGrayKernel, 1, sizeof(cl_mem), &FrameImages[1]));
		CLOpsSuccess &= checkSuccess(clSetKernelArg(BGRtoGrayKernel, 2, sizeof(size_t), &RGBImageRowPitch));
		CatchCLFault(!checkSuccess(CLOpsSuccess), "Failed Setting LBGR2Gray OpenCL Kernel Arguments")
		DEBUG_INFO("Success Setting LBGR2Gray OpenCL Kernel Arguments");

		DEBUG_INFO("Enqueueing LBGR2Gray OpenCL Kernel");
		CatchCLFault(!checkSuccess(clEnqueueNDRangeKernel(MaliCommandQueue, BGRtoGrayKernel, 3, NULL, \
				globalWorkSize, NULL, 0, NULL, &SandboxCLEvents.LBGRtoGrayKernel())), "Could Not Enqueue" \
				"LBGRtoGray Kernel!")
		DEBUG_INFO("Setting RBGR2Gray OpenCL Kernel Arguments");
		CLOpsSuccess &= checkSuccess(clSetKernelArg(BGRtoGrayKernel, 0, sizeof(cl_mem), &FrameImages[3]));
		CLOpsSuccess &= checkSuccess(clSetKernelArg(BGRtoGrayKernel, 1, sizeof(cl_mem), &FrameImages[4]));
		CLOpsSuccess &= checkSuccess(clSetKernelArg(BGRtoGrayKernel, 2, sizeof(size_t), &RGBImageRowPitch));
		CatchCLFault(!checkSuccess(CLOpsSuccess), "Failed Setting RBGR2Gray OpenCL Kernel Arguments")
		DEBUG_INFO("Success Setting RBGR2Gray OpenCL Kernel Arguments");

		DEBUG_INFO("Enqueueing RBGR2Gray OpenCL Kernel");
		CatchCLFault(!checkSuccess(clEnqueueNDRangeKernel(MaliCommandQueue, BGRtoGrayKernel, 3, NULL, \
				globalWorkSize, NULL, 0, NULL, &SandboxCLEvents.LBGRtoGrayKernel())), "Could Not Enqueue" \
				"RBGRtoGray Kernel!")

		CatchCLFault(!checkSuccess(clFinish(MaliCommandQueue)), "Failure while waiting for Command" \
				"Queue to Finish")
		DEBUG_INFO("Finished OpenCL Command Queue!");

		DEBUG_INFO("Setting Nebula OpenCL Kernel Arguments");
		CLOpsSuccess &= checkSuccess(clSetKernelArg(NebulaKernel, 0, sizeof(cl_mem), &FrameImages[1]));
		CLOpsSuccess &= checkSuccess(clSetKernelArg(NebulaKernel, 1, sizeof(cl_mem), &FrameImages[2]));
		CLOpsSuccess &= checkSuccess(clSetKernelArg(NebulaKernel, 2, sizeof(size_t), &GrayImageRowPitch));
		CLOpsSuccess &= checkSuccess(clSetKernelArg(NebulaKernel, 3, sizeof(size_t), &NebulaAmplificator));
		CatchCLFault(!checkSuccess(CLOpsSuccess), "Failed Setting LNebula OpenCL Kernel Arguments")
		DEBUG_INFO("Success Setting LNebula OpenCL Kernel Arguments");

		DEBUG_INFO("Enqueueing LNebula OpenCL Kernel");
		CatchCLFault(!checkSuccess(clEnqueueNDRangeKernel(MaliCommandQueue, NebulaKernel, 3, NULL, \
				globalWorkSize, NULL, 0, NULL, &SandboxCLEvents.LNebulaKernel())), "Could Not Enqueue" \
				" LNebulaKernel!")
		DEBUG_INFO("Setting Nebula OpenCL Kernel Arguments");
		CLOpsSuccess &= checkSuccess(clSetKernelArg(NebulaKernel, 0, sizeof(cl_mem), &FrameImages[4]));
		CLOpsSuccess &= checkSuccess(clSetKernelArg(NebulaKernel, 1, sizeof(cl_mem), &FrameImages[5]));
		CLOpsSuccess &= checkSuccess(clSetKernelArg(NebulaKernel, 2, sizeof(size_t), &GrayImageRowPitch));
		CLOpsSuccess &= checkSuccess(clSetKernelArg(NebulaKernel, 3, sizeof(size_t), &NebulaAmplificator));
		CatchCLFault(!checkSuccess(CLOpsSuccess), "Failed Setting RNebula OpenCL Kernel Arguments")
		DEBUG_INFO("Success Setting RNebula OpenCL Kernel Arguments");

		DEBUG_INFO("Enqueueing RNebula OpenCL Kernel");
		CatchCLFault(!checkSuccess(clEnqueueNDRangeKernel(MaliCommandQueue, NebulaKernel, 3, NULL, \
				globalWorkSize, NULL, 0, NULL, &SandboxCLEvents.RNebulaKernel())), "Could Not Enqueue" \
				" RNebulaKernel!")
		CatchCLFault(!checkSuccess(clFinish(MaliCommandQueue)), "Failure while waiting for Command" \
				"Queue to Finish")

		DEBUG_INFO("Finished OpenCL Command Queue!");

		DEBUG_INFO("Enqueuing OpenCL Map OutFrame Image Buffer");
		CL_LOutFrame = (uchar*)clEnqueueMapBuffer(MaliCommandQueue, FrameImages[2], CL_FALSE, \
				CL_MAP_READ, 0, GrayFrameSize, 0, NULL, &SandboxCLEvents.MapLOutFrame(), &errorCode);
		CLOpsSuccess &= errorCode;
		CL_ROutFrame = (uchar*)clEnqueueMapBuffer(MaliCommandQueue, FrameImages[5], CL_FALSE, \
				CL_MAP_READ, 0, GrayFrameSize, 0, NULL, &SandboxCLEvents.MapROutFrame(), &errorCode);
		CLOpsSuccess &= errorCode;
		CatchCLFault(!checkSuccess(CLOpsSuccess), "Failed to Map OpenCL OutFrame Image Buffer.")
		DEBUG_INFO("OpenCL OutFrame Image Buffer Successfully Mapped!");

		clWaitForEvents(1, &SandboxCLEvents.MapLOutFrame());
		clWaitForEvents(1, &SandboxCLEvents.MapROutFrame());
		LBorders.data = CL_LOutFrame;
		RBorders.data = CL_ROutFrame;

		imshow(Window_LStream_Title, LFrame);
		imshow(Window_LBorders_Title, LBorders);
		imshow(Window_RStream_Title, RFrame);
		imshow(Window_RBorders_Title, RBorders);

		CatchCLFault(clEnqueueUnmapMemObject(MaliCommandQueue, FrameImages[2], CL_LOutFrame, 0, \
				NULL, &SandboxCLEvents.UnMapLOutFrame()), "Error While Unmapping LOutFrame Memory!")
		CatchCLFault(clEnqueueUnmapMemObject(MaliCommandQueue, FrameImages[5], CL_ROutFrame, 0, \
				NULL, &SandboxCLEvents.UnMapROutFrame()), "Error While Unmapping ROutFrame Memory!")
		DEBUG_INFO("OpenCL OutFrame Image Buffer Successfully UnMapped!");
		End_Time = clock();
		printf("\rSeconds per Iteration: %6f, Iterations per Second: %6f", \
		(double)(End_Time-Start_Time)/CLOCKS_PER_SEC, (double)CLOCKS_PER_SEC/(End_Time-Start_Time));
//		cout << "Seconds per iteration: " << (double)(End_Time-Start_Time)/CLOCKS_PER_SEC << endl;
//		cout << "Iterations per Second: " << (double)CLOCKS_PER_SEC/(End_Time-Start_Time) << endl;
	}
	cout << "Elapsed Frames: " << FramesElapsed << endl;
	DEBUG_INFO("Printing Last Events Profiling Info\n");

	DEBUG_INFO("Printing Map LInFrame Profiling Info:");
	printProfilingInfo(SandboxCLEvents.MapLInFrame());
	DEBUG_INFO("Printing Map RInFrame Profiling Info:");
	printProfilingInfo(SandboxCLEvents.MapRInFrame());

	DEBUG_INFO("Printing UnMap LInFrame Profiling Info:");
	printProfilingInfo(SandboxCLEvents.UnMapLInFrame());
	DEBUG_INFO("Printing UnMap RInFrame Profiling Info:");
	printProfilingInfo(SandboxCLEvents.UnMapRInFrame());

	DEBUG_INFO("Printing LBGR2Gray Kernel Profiling Info:");
	printProfilingInfo(SandboxCLEvents.LBGRtoGrayKernel());
	DEBUG_INFO("Printing RBGR2Gray Kernel Profiling Info:");
	printProfilingInfo(SandboxCLEvents.RBGRtoGrayKernel());

	DEBUG_INFO("Printing LNebula Kernel Profiling Info:");
	printProfilingInfo(SandboxCLEvents.LNebulaKernel());
	DEBUG_INFO("Printing RNebula Kernel Profiling Info:");
	printProfilingInfo(SandboxCLEvents.RNebulaKernel());

	DEBUG_INFO("Printing Map LOutFrame Profiling Info:");
	printProfilingInfo(SandboxCLEvents.MapLOutFrame());
	DEBUG_INFO("Printing Map ROutFrame Profiling Info:");
	printProfilingInfo(SandboxCLEvents.MapROutFrame());

	DEBUG_INFO("Printing UnMap LOutFrame Profiling Info:");
	clWaitForEvents(1, &SandboxCLEvents.UnMapLOutFrame());
	printProfilingInfo(SandboxCLEvents.UnMapLOutFrame());
	DEBUG_INFO("Printing UnMap ROutFrame Profiling Info:");
	clWaitForEvents(1, &SandboxCLEvents.UnMapROutFrame());
	printProfilingInfo(SandboxCLEvents.UnMapROutFrame());
	DEBUG_INFO("End of Profiling Information\n");

	DEBUG_INFO("Releasing Event Objects...");
	CatchCLFault(!checkSuccess(clReleaseEvent(SandboxCLEvents.MapLInFrame())), \
			"Could not Release Nebula Kernel Run Event")
	CatchCLFault(!checkSuccess(clReleaseEvent(SandboxCLEvents.MapRInFrame())), \
			"Could not Release Nebula Kernel Run Event")
	CatchCLFault(!checkSuccess(clReleaseEvent(SandboxCLEvents.UnMapLInFrame())), \
			"Could not Release Nebula Kernel Run Event")
	CatchCLFault(!checkSuccess(clReleaseEvent(SandboxCLEvents.UnMapRInFrame())), \
			"Could not Release Nebula Kernel Run Event")
	CatchCLFault(!checkSuccess(clReleaseEvent(SandboxCLEvents.LBGRtoGrayKernel())), \
			"Could not Release Nebula Kernel Run Event")
	CatchCLFault(!checkSuccess(clReleaseEvent(SandboxCLEvents.RBGRtoGrayKernel())), \
				"Could not Release Nebula Kernel Run Event")
	CatchCLFault(!checkSuccess(clReleaseEvent(SandboxCLEvents.LNebulaKernel())), \
			"Could not Release Nebula Kernel Run Event")
	CatchCLFault(!checkSuccess(clReleaseEvent(SandboxCLEvents.RNebulaKernel())), \
				"Could not Release Nebula Kernel Run Event")
	CatchCLFault(!checkSuccess(clReleaseEvent(SandboxCLEvents.MapLOutFrame())), \
			"Could not Release Nebula Kernel Run Event")
	CatchCLFault(!checkSuccess(clReleaseEvent(SandboxCLEvents.MapROutFrame())), \
			"Could not Release Nebula Kernel Run Event")
	CatchCLFault(!checkSuccess(clReleaseEvent(SandboxCLEvents.UnMapLOutFrame())), \
			"Could not Release Nebula Kernel Run Event")
	CatchCLFault(!checkSuccess(clReleaseEvent(SandboxCLEvents.UnMapROutFrame())), \
			"Could not Release Nebula Kernel Run Event")
	DEBUG_INFO("Event Objects Released");

	DEBUG_INFO("\n\n Goodbye my dear and beloved world...\n\n");
	cleanUpOpenCL(SandboxCLContext, MaliCommandQueue, NebulaProgram, NebulaKernel, FrameImages, \
			NumFrameImages);
	destroyAllWindows();
	LStream.release();
	RStream.release();
	return 0;
}
