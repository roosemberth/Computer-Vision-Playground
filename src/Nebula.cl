__constant sampler_t NebulaSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__kernel void NebulaKernel(__read_only image2d_t Input,__global uchar* Output, const uint RowPitch, const uint Amplificator){

    const int PosX = get_global_id(0);
    const int PosY = get_global_id(1);
    
    float x1y1 = read_imagef(Input, NebulaSampler, (int2){PosX-1, PosY-1}).s0;
    float x2y1 = read_imagef(Input, NebulaSampler, (int2){PosX+0, PosY-1}).s0;
    float x3y1 = read_imagef(Input, NebulaSampler, (int2){PosX+1, PosY-1}).s0;
    float x1y2 = read_imagef(Input, NebulaSampler, (int2){PosX-1, PosY+0}).s0;
    float x2y2 = read_imagef(Input, NebulaSampler, (int2){PosX+0, PosY+0}).s0;
    float x3y2 = read_imagef(Input, NebulaSampler, (int2){PosX+1, PosY+0}).s0;
    float x1y3 = read_imagef(Input, NebulaSampler, (int2){PosX-1, PosY+1}).s0;
    float x2y3 = read_imagef(Input, NebulaSampler, (int2){PosX+0, PosY+1}).s0;
    float x3y3 = read_imagef(Input, NebulaSampler, (int2){PosX+1, PosY+1}).s0;
    
    float Mean = (x1y1+x2y1+x3y1 + x1y2+x2y2+x3y2 + x1y3+x2y3+x3y3)/9;
    
    float MeanDeviation = (fabs(x1y1 - Mean) + fabs(x2y1 - Mean) + fabs(x3y1 - Mean) + \
    					   fabs(x1y2 - Mean) + fabs(x2y2 - Mean) + fabs(x3y2 - Mean) + \
    					   fabs(x1y3 - Mean) + fabs(x2y3 - Mean) + fabs(x3y3 - Mean) )/9;
    
	Output[PosY*RowPitch + PosX] = convert_uchar(Amplificator*MeanDeviation);
//	write_imageui(Output, (int2){PosX, PosY}, MeanDeviation);
//	write_imageui(Output, (int2){PosX, PosY}, (uint4){0, convert_int(255*PosX/480), convert_int(255*PosY/640), 0});   

}
