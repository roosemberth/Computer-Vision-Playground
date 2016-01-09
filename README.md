Computer-Vision-Playground
=================

Current Target: Stream from [See3CAM_CU130](http://www.e-consystems.com/UltraHD-USB-Camera.asp) to a [Mali OpenCL device](http://www.hardkernel.com/main/products/prdt_info.php?g_code=G143452239825&tab_idx=2) using [Exynos5422's Hw JPEG Block](https://github.com/hardkernel/linux/blob/d717e460844165d6c2d24332ccd5cb2d2d0ece77/arch/arm/boot/dts/exynos5420.dtsi#L779-L795) [decoding](https://github.com/hardkernel/linux/tree/odroidxu4-v4.2-rc1/drivers/media/platform/s5p-jpeg)

The intended workflow is represented by the following pipeline:

Camera <-|-> s5p-jpeg Decoder In <--> s5p-jpeg Decoder Out <-|-> Mali OpenCL Cmd Queue

(where '|' implies an 'act')


Using the following state machine:

|Property | Camera             | DecoderIn              | DecoderOut             | GPU                 |
|---------|--------------------|------------------------|------------------------|---------------------|
|query    | getNewFrame()      | dequeueBuf()           | dequeueBuf()           | isMaliCmdQueueDone()|
|outcome  | -EAGAIN (no frame) | -EAGAIN (not done yet) | -EAGAIN (not done yet) | maliCmdQueueNotDone |
|         | newCamBuf          | usedDecBuf             | newDecodBuf            | maliCmdQueueDone    |
|actions  | enqCamBuffer(-)    | enqDIBuffer(-)         | enqDOBuffer(-)         | enqKernel(-)        |

\-EAGAIN indicates that there's currently a frame being processed


The following action table
camBuf is a "temporal" holder for the last camera buffer recieved
imgBuf is a "temporal" holder for the last decoded frame
maliBuf is a holder for the buffer in the mali pipeline

|Camera    | DecoderIn  | DecoderOut  | GPU                 | actOnCam | actOnDecIn | actOnDecOut | actOnGPU |
|----------|------------|-------------|---------------------|----------|------------|-------------|----------|
|-EAGAIN   | -EAGAIN    | -EAGAIN     | maliCmdQueueNotDone | NOP      | NOP        | NOP         | NOP      |
|newCamBuf | *          | *           | *                   | camJob() | NOP        | NOP         | NOP      |
|*         | usedDecBuf | *           | *                   | NOP      | decInJob() | NOP         | NOP      |
|*         | *          | newDecodBuf | *                   | NOP      | NOP        | decOutJob() | NOP      |
|*         | *          | *           | maliCmdQueueDone    | NOP      | NOP        | NOP         | maliJob()|

NOP = No Operation

Jobs:

camJob{
	enqCamBuf(camBuf);
	camBuf = newCamBuf
}

decInJob{
	encDIBuf(camBuf);
	camBuf = usedDecBuf
}

decOutJob{
	enqDOBuffer(imgBuf);
    imgBuf = newDecodBuf
}

maliJob{
    swap(maliBuf, imgBuf);
    enqKernel(maliBuf);
}

I'll try to update the variable, jobs and reference names as I implement it
