/*
 * v4l-wrapper.hpp
 *
 *  Created on: 6 nov. 2015
 *      Author: roosemberth
 */

#ifndef SRC_V4L_WRAPPER_HPP_
#define SRC_V4L_WRAPPER_HPP_

#include <cstdint>
#include <stddef.h>

namespace v4l2 {

    typedef struct S_streamFormat {
        unsigned int height;
        unsigned int width;
        unsigned int PixelFormat;
    } streamFormat;

    class videoCapture {
    private:
        struct v4l2_buffer *v4l2Buffer;
        streamFormat currentStreamFormat;

        size_t bufferSize;
        void   **buffers;
        int    fd;
        uint   nBuffers;
        uint   lastBufferIndex;

        int exchangeUsrBuf(unsigned long request, void *buffer, size_t bufSize,
                           uint index, v4l2_buf_type type);

        void construct(char *deviceName);
        void construct(char *deviceName, streamFormat sFmt);
        void construct(char *deviceName, streamFormat sFmt,
             void *buf[], size_t bufSize, uint bufCount);

    public:
        /*
         * videoCapture:
         * \brief Creates a videoCapture from a device path and a buffer
         *
         * \param *deviceName 	Image Device Path (ie. "/dev/video0")
         *
         * \param resolution    Use the specified resolution
         *
         * \param ***buffer 	Buffer array where to store image data
         * \param bufferSize 	Allocated Buffer Size
         * \param bufferCount 	Number of allocated buffers in the buffers array
         */
        videoCapture(char *deviceName);
        videoCapture(char *deviceName, streamFormat sFmt);
        videoCapture(char *deviceName, streamFormat sFmt, void **buf, size_t
        bufSize, uint bufCount);

        /*
         * capture:
         * \brief Captures a frame and puts it on buffer index
         *
         * \param nextBuff      Capture image from camera and store it in this buffer
         * \param bufferSize    Allocated Buffer size
         *
         * \return              Null if frame is not yet ready
         *                      Pointer to a frame if ready
         */
        void *capture();

        void *capture(void *nextBuff, size_t bufferSize);

        /*
         * GetStreamFormats
         * \brief Gets supported stream format for specified device
         * \sa streamFormat
         *
         * \param *deviceName 	Image Device Path (ie. "/dev/video0")
         *
         * \return              NULL-terminated array with supported stream formats
         */
        static streamFormat **GetStreamFormats(char *deviceName);

        /*
         *
         *
         */
        ~videoCapture();
    };

    class hwVideoDecode {

    };

} // namespace v4l2

#endif /* SRC_V4L_WRAPPER_HPP_ */
