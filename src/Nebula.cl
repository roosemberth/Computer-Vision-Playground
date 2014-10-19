__constant sampler_t NebulaSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__kernel void NebulaKernel(__read_only image2d_t Input,__write_only image2d_t Output){

    const int PosX = get_global_id(0)+1;
    const int PosY = get_global_id(1)+1;
    
    uint4 x1y1 = read_imageui(Input, NebulaSampler, (int2){PosX-1, PosY-1});
    uint4 x2y1 = read_imageui(Input, NebulaSampler, (int2){PosX+0, PosY-1});
    uint4 x3y1 = read_imageui(Input, NebulaSampler, (int2){PosX+1, PosY-1});
    uint4 x1y2 = read_imageui(Input, NebulaSampler, (int2){PosX-1, PosY+0});
    uint4 x2y2 = read_imageui(Input, NebulaSampler, (int2){PosX+0, PosY+0});
    uint4 x3y2 = read_imageui(Input, NebulaSampler, (int2){PosX+1, PosY+0});
    uint4 x1y3 = read_imageui(Input, NebulaSampler, (int2){PosX-1, PosY+1});
    uint4 x2y3 = read_imageui(Input, NebulaSampler, (int2){PosX+0, PosY+1});
    uint4 x3y3 = read_imageui(Input, NebulaSampler, (int2){PosX+1, PosY+1});
    
    float4 Mean = (x1y1+x2y1+x3y1 +\
    			   x1y2+x2y2+x3y2 +\
    			   x1y3+x2y3+x3y3 )/9;
    
    float4 Deviation = (abs_diff(x1y1, Mean) + abs_diff(x2y1, Mean) + abs_diff(x3y1, Mean) + \
    				    abs_diff(x1y2, Mean) + abs_diff(x2y2, Mean) + abs_diff(x3y2, Mean) + \
    				    abs_diff(x1y3, Mean) + abs_diff(x2y3, Mean) + abs_diff(x3y3, Mean) );
    
    write_imageui(Output, (int2){PosX, PosY}, (unsigned char)(x2y2-Deviation);
    
}