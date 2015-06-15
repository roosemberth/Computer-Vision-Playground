__constant sampler_t DefSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__kernel void BGRtoGrayKernel(__global uchar* Input,__write_only image2d_t Output, const uint RowPitch){

    const int PosX = get_global_id(0);
    const int PosY = get_global_id(1);

	uchar BluePixel  = Input[PosY*RowPitch + 3*PosX + 0];
	uchar GreenPixel = Input[PosY*RowPitch + 3*PosX + 1];
	uchar RedPixel 	 = Input[PosY*RowPitch + 3*PosX + 2];

//	uint4 RGBPixel = read_imageui(Input, DefSampler, (int2){PosX, PosY});
	float GrayPixel = BluePixel*(float)0.0722 + GreenPixel*(float)0.7152 + RedPixel*(float)0.2126;
    
	write_imagef(Output, (int2){PosX, PosY}, (float)GrayPixel);
    
}
