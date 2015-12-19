/*
 * v4l-wrapper.cpp
 *
 * TODO: Description
 */

#include "SystechDefinitions.h"

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include "v4l-wrapper.hpp"

#define ERROR_EXIT(...) do{         \
    DEBUG_ERROR(__VA_ARGS__);       \
    assert(0);                      \
    } while(0)

/*
 * IsValidFormat
 * \brief Checks if resolution specified by req is valid
 *
 * \param validFmts     Null-Terminated Array of pointers to valid streamFormat
 * \param req           Requested streamFormat to be checked
 * \param freeMem       Free that memory contained in validFmts after this call
 *
 * \return true if it's a valid streamFormat, false otherwise
 *
 */
static bool IsValidFormat(
  v4l2::streamFormat **validFmts, v4l2::streamFormat req, bool freeMem) {
    if (validFmts == NULL)
        return false;
    if (req.width <= 0 || req.height <= 0)
        return false;

    v4l2::streamFormat *validFmt = validFmts[0];
    int                i         = 0;
    do {
        if (req.width == validFmt->width &&
            req.height == validFmt->height &&
            req.PixelFormat == validFmt->PixelFormat)
            return true;
        if (freeMem)
            free(validFmt);
        validFmt = validFmts[++i];
    } while (validFmt != NULL);
    if (freeMem)
        free(validFmts);
    return false;
}

// Dummy class constructors

v4l2::videoCapture::videoCapture(char *deviceName, streamFormat format) {
    memset(this, 0, sizeof(v4l2::videoCapture));
    v4l2::videoCapture::construct(deviceName, format);
}

v4l2::videoCapture::videoCapture(char *deviceName, streamFormat sFmt,
  void *buf[], size_t bufSize, uint bufCount) {
    memset(this, 0, sizeof(v4l2::videoCapture));
    v4l2::videoCapture::construct(deviceName, sFmt, buf, bufSize, bufCount);
}

v4l2::videoCapture::videoCapture(char *deviceName) {
    memset(this, 0, sizeof(v4l2::videoCapture));
    v4l2::videoCapture::construct(deviceName);
}

// "Real" class constructors
void
v4l2::videoCapture::construct(char *deviceName) {
    v4l2::streamFormat *maxResolutionFmt = NULL;
    {
        v4l2::streamFormat **validFmts = GetStreamFormats(deviceName);
        unsigned int       maxHeight   = 0;
        unsigned int       maxWidth    = 0;
        unsigned int       i           = 0;
        v4l2::streamFormat *validFmt   = validFmts[0];
        do {
            if ((validFmt->width * validFmt->height) >
                (maxHeight * maxWidth)) {
                maxWidth  = validFmt->width;
                maxHeight = validFmt->height;
                if (maxResolutionFmt != NULL)
                    free(maxResolutionFmt);
                maxResolutionFmt = validFmt;
            } else
                free(validFmt);
            validFmt = validFmts[++i];
        } while (validFmt != NULL);
        free(validFmts);
    }
    v4l2::videoCapture::construct(deviceName, *maxResolutionFmt);
}

void
v4l2::videoCapture::construct(char *deviceName, streamFormat format) {
    if (!IsValidFormat(
      v4l2::videoCapture::GetStreamFormats(deviceName), format, true)) {
        // TODO: Throw an exception?
    }
    const unsigned int bufCount = 3;
    const unsigned int bufSize  = static_cast<unsigned int>(
      3 * sizeof(char) * format.width * format.height);

    void *buf[bufCount];
    for (unsigned int i = 0; i < bufCount; ++i) {
        buf[i] = malloc(bufSize);
    }
    v4l2::videoCapture::construct(deviceName, format, buf, bufSize, bufCount);
}

void
v4l2::videoCapture::construct(char *deviceName, streamFormat sFmt, void *buf[],
                              size_t bufSize, uint bufCount) {

    assert(buf);
    assert(deviceName);
    assert(bufSize > 0);
    assert(bufCount > 0);
    assert(IsValidFormat(GetStreamFormats(deviceName), sFmt, true));

    struct v4l2_capability devCaps = {};

    bufferSize          = bufSize;
    nBuffers            = bufCount;
    buffers             = buf;
    lastBufferIndex     = 0;
    currentStreamFormat = sFmt;
    v4l2Buffer          = reinterpret_cast<struct v4l2_buffer*>(
      malloc(sizeof(struct v4l2_buffer)));

    if ((fd = open(deviceName, O_RDWR)) < 0)
        ERROR_EXIT("Failed opening V4L2 Interface: %s", deviceName);

    if (ioctl(fd, VIDIOC_QUERYCAP, &devCaps) < 0)
        ERROR_EXIT("Failed quering VIDEOC_QUERYCAP");

    if (!(devCaps.capabilities & V4L2_CAP_VIDEO_CAPTURE))
        ERROR_EXIT("%s is not a video capture device!", deviceName);

    if (!(devCaps.capabilities & V4L2_CAP_STREAMING))
        ERROR_EXIT("%s can not stream", deviceName);

    if (!(devCaps.capabilities & V4L2_CAP_READWRITE))
        ERROR_EXIT("%s can not accept read i/o", deviceName);

    struct v4l2_format format = {};
    format.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = sFmt.PixelFormat;
    format.fmt.pix.width       = static_cast<__u32>(sFmt.width);
    format.fmt.pix.height      = static_cast<__u32>(sFmt.height);
    format.fmt.pix.field       = V4L2_FIELD_NONE;
    format.fmt.pix.field       = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &format) < 0)
        ERROR_EXIT("IOCTL: VIDIOC_S_FMT");

    if (format.fmt.pix.width != sFmt.width ||
        format.fmt.pix.height != sFmt.height) {
        DEBUG_WARN("Size of stream is different than reported size");
        sFmt.width  = format.fmt.pix.width;
        sFmt.height = format.fmt.pix.height;
        // assert(bufSize >= sFmt.width * sFmt.height);
    }

    struct v4l2_requestbuffers *requestBuffers =
      reinterpret_cast<struct v4l2_requestbuffers *>(
        malloc(sizeof(struct v4l2_requestbuffers)));
    memset(requestBuffers, 0, sizeof(struct v4l2_requestbuffers));

    requestBuffers->count  = reinterpret_cast<__u32>(bufCount);
    requestBuffers->type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    requestBuffers->memory = V4L2_MEMORY_USERPTR;

    if (ioctl(fd, VIDIOC_REQBUFS, &requestBuffers) < 0)
        ERROR_EXIT("VIDIOC_REQBUFS");

    /*
     * FIXME: [CRITICAL]
     * Need to keep track of this buffers, as we'll be passing new
     * buffers on each call to capture method which will issue an 
     * ioctl call so the old buffer will be orphan and will cause 
     * a memory leak. Possible fix: deallocate them while dequeuing
     */
    int err = 0;

    for (unsigned int i   = 0; i < bufCount; ++i)
        if ((err = exchangeUsrBuf(
          VIDIOC_QBUF, buf[i], bufSize, i, V4L2_BUF_TYPE_VIDEO_CAPTURE)))
            ERROR_EXIT("First Enqueue buffer %d: ioctl: %d", i, err);

    if (ioctl(fd, VIDIOC_STREAMON, requestBuffers->type) < 0)
        ERROR_EXIT("VIDIOC_STREAMON");
}

/*
 * exchangeUsbBuff
 * \brief exchange an user buffer with the v4l2 driver
 *
 * \param request   request to the driver: VIDIOC_QBUF or VIDIOC_DQBUF
 * \param buffer    pointer to user buffer
 * \param bufSize   buffer size
 * \param type      buffer type
 */
int v4l2::videoCapture::exchangeUsrBuf(
  unsigned long request, void *buffer, size_t bufSize,
  uint index, v4l2_buf_type type) {

    // Improvement: make this more type-safe
    if (request != VIDIOC_QBUF && request != VIDIOC_DQBUF) {
        DEBUG_ERROR("Called with wrong argument type! expecting VIDIOC_QBUF"
                      "(%lu) or VIDIOC_DQBUFF(%lu) but recieved (%lu)!",
                    VIDIOC_QBUF, VIDIOC_DQBUF, request);
    }
    assert(bufSize);
    assert(index < nBuffers);

    // TODO: Hold a spinlock/mutex on v4l2Buffer
    memset(&v4l2Buffer, 0, sizeof(v4l2Buffer));

    v4l2Buffer->type      = type;
    v4l2Buffer->memory    = V4L2_MEMORY_USERPTR;
    v4l2Buffer->index     = index;
    v4l2Buffer->length    = static_cast<__u32>(bufSize);
    v4l2Buffer->m.userptr = reinterpret_cast<unsigned long>(buffer);

    int ret = ioctl(static_cast<int>(request),
                    reinterpret_cast<unsigned long>(v4l2Buffer));
    // TODO: Release spinlock/mutex on v4l2Buffer
    if (ret != EAGAIN)
        DEBUG_ERROR("ioctl error: request: %lu", request);
    return ret;
}

void *v4l2::videoCapture::capture() {
    // TODO: Auto allocate new buffer and return a call to capture(.., ..)
    return NULL;
}

void *v4l2::videoCapture::capture(void *nextBuff, size_t buffSize) {
    int ret = 0;
    ret = exchangeUsrBuf(VIDIOC_DQBUF, buffers[lastBufferIndex], bufferSize,
                         lastBufferIndex, V4L2_BUF_TYPE_VIDEO_CAPTURE);
    if (ret == EAGAIN)
        return NULL;
    else if (ret != EIO)
        ERROR_EXIT("Error dequeuing buffer %d: ioctl: %d",
                   lastBufferIndex, ret);

    ret = exchangeUsrBuf(VIDIOC_QBUF, nextBuff, buffSize,
                         lastBufferIndex, V4L2_BUF_TYPE_VIDEO_CAPTURE);
    if (ret < 0)
        ERROR_EXIT("Error enqueuing new replacement buffer %d: ioctl: %d",
                   lastBufferIndex, ret);
    lastBufferIndex = (lastBufferIndex + 1) % nBuffers;

    return reinterpret_cast<void *>(v4l2Buffer->m.userptr);
}

v4l2::videoCapture::~videoCapture() {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0)
        ERROR_EXIT("VIDIOC_STREAMOFF");
    if (close(fd) < 0)
        ERROR_EXIT("Error closing device!");
}

v4l2::streamFormat **v4l2::videoCapture::GetStreamFormats(
  char *deviceName) {
    assert(deviceName);

    struct S_ll_formats {
        v4l2::streamFormat  format;
        struct S_ll_formats *next;
        struct S_ll_formats *last;
    };

    struct S_ll_formats ll_supportedFormats = {};
    struct S_ll_formats *curFmt             = &ll_supportedFormats;
    unsigned int        nSizes              = 0;

    enum v4l2_buf_type      type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    struct v4l2_fmtdesc     fmt     = {};
    struct v4l2_frmsizeenum frmSize = {};

    fmt.index = 0;
    fmt.type  = type;

    int fd = open(deviceName, O_RDWR);
    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) >= 0) {
        frmSize.pixel_format = fmt.pixelformat;
        frmSize.index        = 0;
        while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmSize) >= 0) {
            if (nSizes++ > 0) {
                curFmt->next = reinterpret_cast<struct S_ll_formats *>(
                  malloc(sizeof(struct S_ll_formats)));
                memset(curFmt->next, 0, sizeof(struct S_ll_formats));
                curFmt->next->last = curFmt;
                curFmt = curFmt->next;
            }
            curFmt->format.PixelFormat = fmt.pixelformat;
            if (frmSize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                curFmt->format.width  = frmSize.discrete.width;
                curFmt->format.height = frmSize.discrete.height;
            } else if (frmSize.type == V4L2_FRMSIZE_TYPE_STEPWISE) {
                curFmt->format.width  = frmSize.stepwise.max_width;
                curFmt->format.height = frmSize.stepwise.max_height;
            }
            frmSize.index++;
        }
        fmt.index++;
    }
    close(fd);

    curFmt = &ll_supportedFormats;

    v4l2::streamFormat **supportedStreams =
      reinterpret_cast<v4l2::streamFormat **>(
        malloc((nSizes + 1) * sizeof(v4l2::streamFormat *)));

    for (unsigned int i = 0; i < nSizes; ++i) {
        supportedStreams[i] = reinterpret_cast<v4l2::streamFormat *>(
        malloc(sizeof(v4l2::streamFormat)));
        // Won't initialize as I'll fill all the fields...
        supportedStreams[i]->PixelFormat = curFmt->format.PixelFormat;
        supportedStreams[i]->width  = curFmt->format.width;
        supportedStreams[i]->height = curFmt->format.height;
        if ((curFmt = curFmt->next) == NULL)
            break;
        free(curFmt->last);
        curFmt->last = NULL;
    }
    supportedStreams[nSizes] = NULL;

    // Improvement: Here returning an array of pointers to streamFormat
    // allocated using malloc, if the other side doesn't free this pointers,
    // this will lead to a memory leak...
    return supportedStreams;
}

