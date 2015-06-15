// Ensure x belongs to [min, max]
#define SafeLocation(x, min, max) x>min?x<max?x:max:min
#define MatIndexFromLocation(PosX, PosY, SizeX, SizeY, Channels) \
	(SafeLocation(PosX, 0, SizeX-1)*Channels)+(SizeX*SafeLocation(PosY, 0, SizeY-1)*Channels)

__kernel void ConvolutionOperationKernel(__constant uchar* InputImage, 			\
										 __constant uchar* ConvolutionKernel, 	\
										 __global uchar* OutputImage, 			\
										 __constant uint[3] ImageDimensions, 	\
										 __constant uint[3] KernelDimensions, 	\
										 __constant int PixelAmplificator 		){
	
	if (KernelDimensions[2]!=ImageDimensions[2]) return;
		 
	const int PosX=get_global_id(0);
	const int PosY=get_global_id(1);
	
	int pointerX=(KernelDimensions[0]-1)/2;
	int pointerY=(KernelDimensions[1]-1)/2;
	
	uint KernelElements=KernelDimensions[0]*KernelDimensions[1]*KernelDimensions[2];
	PixelNeighborhiid NextImagePixels[KernelElements];
	
	int xKernelShift=(KernelDimensions[0]-1)/2;
	int yKernelShift=(KernelDimensions[1]-1)/2;


/* 
	Basically Fill for the first time CurrentImagePixels Stack
	This is in order to make a non-blocking memory prefetch and speed up memory operations
*/
	for (int xTmp=-xKernelShift; xTmp<=xKernelShift; ++xTmp){
		for (int yTmp=-yKernelShift; yTmp<=yKernelShift; ++yTmp){
			for (int ch=0; ch<KernelDimensions[2]; ++ch){
				CurrentImagePixels[MatIndexFromLocation(xTmp+xKernelShift, yTmp+yKernelShift, \
														 KernelDimensions[0], KernelDimensions[1], \
														 KernelDimensions[2])] =\
				InputImage[MatIndexFromLocation(xTmp, yTmp, \
												 ImageDimensions[0], ImageDimensions[2],\
												 ImageDimensions[3])];
			}
		}	
	}  
	
/*
	Main Loop: We will do Element-wise multiplication of the Neighborgh Pixels and Matrix Elements
	with center on Image Current Pixel and Convolution Kernel Matrix Central Element 
*/
	
	for (int xTmp=-xKernelShift; xTmp<=xKernelShift; ++xTmp){
		for (int yTmp=-yKernelShift; yTmp<=yKernelShift; ++yTmp){
			for (int ch=0; ch<KernelDimensions[2]; ++ch){
// Fill Next Set
				NextImagePixels[MatIndexFromLocation(xTmp+xKernelShift, yTmp+yKernelShift, \
														 KernelDimensions[0], KernelDimensions[1], \
														 KernelDimensions[2])] =\
				&InputImage[MatIndexFromLocation(xTmp, yTmp, \
												 ImageDimensions[0], ImageDimensions[2],\
												 ImageDimensions[3])];
												 
				
			}
		}	
	}	
	
}	