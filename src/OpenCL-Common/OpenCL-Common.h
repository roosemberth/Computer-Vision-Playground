/*
 * "OpenCL Platform Common Functions header" - OpenCV-Playground
 *
 * Copyright (C) 2014-2015 Roosemberth Palacios <roosemberth@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * The following file is an edit of Mali OpenCL SDK v1.1.0 Common Source
 * Files, these were released under the following terms:
 *
 * [Quote]
 * [...]
 * 1. DEFINITIONS.
 * “Authorised Purpose” means; (i) with respect to Deliverables other than the Samples, the right to
 * use internally to execute Open CL applications and kernel programs on ARM based devices; and (ii)
 * with respect to the Samples the right to develop Software Applications. “Deliverables” means the
 * software source code accompanying this Licence, and any printed, electronic or online documentation
 * supplied with it, in all cases relating to the MALI OPENCL SDK.
 * [...]
 * 2. LICENCE GRANT.
 * DELIVERABLES: ARM hereby grants to you, subject to the terms and conditions of this Licence, a
 * nonexclusive, nontransferable, royalty free, worldwide licence to use and copy, internally only,
 * the Deliverables solely for the Authorised Purpose.
 * [...]
 * [/Quote]
 *
 * Edited on: 29 oct. 2014
 */

#ifndef SRC_OPENCL_COMMON_OPENCL_COMMON_H_
#define SRC_OPENCL_COMMON_OPENCL_COMMON_H_

/*
 * This confidential and proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 *   (C) COPYRIGHT 2013 ARM Limited
 *       ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include "../SystechDefinitions.h"

using namespace std;

string errorNumberToString(cl_int errorNumber) {
	switch (errorNumber) {
	case CL_SUCCESS:
		return "CL_SUCCESS";
	case CL_DEVICE_NOT_FOUND:
		return "CL_DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:
		return "CL_DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:
		return "CL_COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:
		return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:
		return "CL_OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:
		return "CL_OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:
		return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:
		return "CL_MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:
		return "CL_IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:
		return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:
		return "CL_BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:
		return "CL_MAP_FAILURE";
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
		return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case CL_INVALID_VALUE:
		return "CL_INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE:
		return "CL_INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:
		return "CL_INVALID_PLATFORM";
	case CL_INVALID_DEVICE:
		return "CL_INVALID_DEVICE";
	case CL_INVALID_CONTEXT:
		return "CL_INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:
		return "CL_INVALID_QUEUE_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE:
		return "CL_INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR:
		return "CL_INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT:
		return "CL_INVALID_MEM_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
		return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:
		return "CL_INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:
		return "CL_INVALID_SAMPLER";
	case CL_INVALID_BINARY:
		return "CL_INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:
		return "CL_INVALID_BUILD_OPTIONS";
	case CL_INVALID_PROGRAM:
		return "CL_INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE:
		return "CL_INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME:
		return "CL_INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION:
		return "CL_INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL:
		return "CL_INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:
		return "CL_INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:
		return "CL_INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE:
		return "CL_INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS:
		return "CL_INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION:
		return "CL_INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE:
		return "CL_INVALID_WORK_GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE:
		return "CL_INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET:
		return "CL_INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST:
		return "CL_INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:
		return "CL_INVALID_EVENT";
	case CL_INVALID_OPERATION:
		return "CL_INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT:
		return "CL_INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE:
		return "CL_INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL:
		return "CL_INVALID_MIP_LEVEL";
	default:
		return "Unknown error";
	}
}

inline bool checkSuccess(cl_int errorNumber) {
	if (errorNumber != CL_SUCCESS) {
		DEBUG_CRIT("OpenCL error: %s", errorNumberToString(errorNumber).c_str());
		return false;
	}
	return true;
}

string imageChannelOrderToString(cl_channel_order channelOrder) {
	switch (channelOrder) {
	case CL_R:
		return "CL_R";
	case CL_A:
		return "CL_A";
	case CL_RG:
		return "CL_RG";
	case CL_RA:
		return "CL_RA";
	case CL_RGB:
		return "CL_RGB";
	case CL_RGBA:
		return "CL_RGBA";
	case CL_BGRA:
		return "CL_BGRA";
	case CL_ARGB:
		return "CL_ARGB";
	case CL_INTENSITY:
		return "CL_INTENSITY";
	case CL_LUMINANCE:
		return "CL_LUMINANCE";
	case CL_Rx:
		return "CL_Rx";
	case CL_RGx:
		return "CL_RGx";
	case CL_RGBx:
		return "CL_RGBx";
	default:
		return "Unknown image channel order";
	}
}

string imageChannelDataTypeToString(cl_channel_type channelDataType) {
	switch (channelDataType) {
	case CL_SNORM_INT8:
		return "CL_SNORM_INT8";
	case CL_SNORM_INT16:
		return "CL_SNORM_INT16";
	case CL_UNORM_INT8:
		return "CL_UNORM_INT8";
	case CL_UNORM_INT16:
		return "CL_UNORM_INT16";
	case CL_UNORM_SHORT_565:
		return "CL_UNORM_SHORT_565";
	case CL_UNORM_SHORT_555:
		return "CL_UNORM_SHORT_555";
	case CL_UNORM_INT_101010:
		return "CL_UNORM_INT_101010";
	case CL_SIGNED_INT8:
		return "CL_SIGNED_INT8";
	case CL_SIGNED_INT16:
		return "CL_SIGNED_INT16";
	case CL_SIGNED_INT32:
		return "CL_SIGNED_INT32";
	case CL_UNSIGNED_INT8:
		return "CL_UNSIGNED_INT8";
	case CL_UNSIGNED_INT16:
		return "CL_UNSIGNED_INT16";
	case CL_UNSIGNED_INT32:
		return "CL_UNSIGNED_INT32";
	case CL_HALF_FLOAT:
		return "CL_HALF_FLOAT";
	case CL_FLOAT:
		return "CL_FLOAT";
	default:
		return "Unknown image channel data type";
	}
}

bool printProfilingInfo(cl_event event) {
	cl_ulong queuedTime = 0;
	if (!checkSuccess(clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), \
			&queuedTime, NULL))) {
		DEBUG_CRIT("Retrieving CL_PROFILING_COMMAND_QUEUED OpenCL profiling information failed");
		return false;
	}

	cl_ulong submittedTime = 0;
	if (!checkSuccess(clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), \
			&submittedTime, NULL))) {
		DEBUG_CRIT("Retrieving CL_PROFILING_COMMAND_SUBMIT OpenCL profiling information failed");
		return false;
	}

	cl_ulong startTime = 0;
	if (!checkSuccess(clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), \
			&startTime, NULL))) {
		DEBUG_CRIT("Retrieving CL_PROFILING_COMMAND_START OpenCL profiling information failed");
		return false;
	}

	cl_ulong endTime = 0;
	if (!checkSuccess(clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), \
			&endTime, NULL))) {
		DEBUG_CRIT("Retrieving CL_PROFILING_COMMAND_END OpenCL profiling information failed");
		return false;
	}

	DEBUG_INFO("Profiling information:\n\n"\
				"\tQueued time:\t%f ms\n"\
				"\t  Wait time:\t%f ms\n"\
				"\t   Run time:\t%f ms", \
	/* OpenCL returns times in nano seconds. Print out the times in milliseconds (divide by a million). */
				(float)((submittedTime - queuedTime	  ) / 1000000.0), \
				(float)((    startTime - submittedTime) / 1000000.0), \
				(float)((      endTime - startTime	  ) / 1000000.0));
	return true;
}

bool printSupported2DImageFormats(cl_context context) {
	/* Get the number of supported image formats in order to allocate the correct amount of memory. */
	cl_uint numberOfImageFormats;
	if (!checkSuccess(clGetSupportedImageFormats(context, 0, CL_MEM_OBJECT_IMAGE2D, 0,NULL, \
			&numberOfImageFormats))) {
		DEBUG_CRIT("Getting the number of supported 2D image formats failed");
		return false;
	}

	/* Get the list of supported image formats. */
	cl_image_format* imageFormats = new cl_image_format[numberOfImageFormats];
	if (!checkSuccess(clGetSupportedImageFormats(context, 0, CL_MEM_OBJECT_IMAGE3D, \
			numberOfImageFormats, imageFormats, NULL))) {
		DEBUG_CRIT("Getting the list of supported 2D image formats failed");
		return false;
	}

	DEBUG_INFO("%d Image Formats Supported", numberOfImageFormats);

	if (numberOfImageFormats > 0) {
		cout << " (channel order, channel data type):" << endl;
	} else {
		cout << "." << endl;
	}

	for (unsigned int i = 0; i < numberOfImageFormats; i++) {
		cout << imageChannelOrderToString(imageFormats[i].image_channel_order)
				<< ", "
				<< imageChannelDataTypeToString(
						imageFormats[i].image_channel_data_type) << endl;
	}

	delete[] imageFormats;

	return true;
}

bool cleanUpOpenCL(cl_context context, cl_command_queue commandQueue,
		cl_program program, cl_kernel kernel, cl_mem* memoryObjects,
		int numberOfMemoryObjects) {
	bool returnValue = true;
	if (context != 0) {
		if (!checkSuccess(clReleaseContext(context))) {
			DEBUG_CRIT("Releasing the OpenCL context failed");
			returnValue = false;
		}
	}

	if (commandQueue != 0) {
		if (!checkSuccess(clReleaseCommandQueue(commandQueue))) {
			DEBUG_CRIT("Releasing the OpenCL command queue failed");
			returnValue = false;
		}
	}

	if (kernel != 0) {
		if (!checkSuccess(clReleaseKernel(kernel))) {
			DEBUG_CRIT("Releasing the OpenCL kernel failed");
			returnValue = false;
		}
	}

	if (program != 0) {
		if (!checkSuccess(clReleaseProgram(program))) {
			DEBUG_CRIT("Releasing the OpenCL program failed");
			returnValue = false;
		}
	}

	for (int index = 0; index < numberOfMemoryObjects; index++) {
		if (memoryObjects[index] != 0) {
			if (!checkSuccess(clReleaseMemObject(memoryObjects[index]))) {
				DEBUG_CRIT("Releasing the OpenCL memory object %d failed", index);
				returnValue = false;
			}
		}
	}

	return returnValue;
}

bool createContext(cl_context* context) {
	cl_int errorNumber = 0;
	cl_uint numberOfPlatforms = 0;
	cl_platform_id firstPlatformID = 0;

	/* Retrieve a single platform ID. */
	if (!checkSuccess(
			clGetPlatformIDs(1, &firstPlatformID, &numberOfPlatforms))) {
		DEBUG_CRIT("Retrieving OpenCL platforms failed");
		return false;
	}

	if (numberOfPlatforms <= 0) {
		DEBUG_CRIT("No OpenCL platforms found");
		return false;
	}

	/* Get a context with a GPU device from the platform found above. */
	cl_context_properties contextProperties[] = { CL_CONTEXT_PLATFORM,
			(cl_context_properties) firstPlatformID, 0 };
	*context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU,
			NULL, NULL, &errorNumber);
	if (!checkSuccess(errorNumber)) {
		DEBUG_CRIT("Creating an OpenCL context failed");
		return false;
	}

	return true;
}

bool createCommandQueue(cl_context context, cl_command_queue* commandQueue, cl_device_id* device) {
	cl_int errorNumber = 0;
	cl_device_id* devices = NULL;
	size_t deviceBufferSize = -1;

	/* Retrieve the size of the buffer needed to contain information about the devices in this OpenCL context. */
	if (!checkSuccess(clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize))) {
		DEBUG_CRIT("Failed to get OpenCL context information");
		return false;
	}

	if (deviceBufferSize == 0) {
		DEBUG_WARN("No OpenCL devices found");
		return false;
	}

	/* Retrieve the list of devices available in this context. */
	devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
	if (!checkSuccess(clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL))) {
		DEBUG_CRIT("Failed to get the OpenCL context information");
		delete[] devices;
		return false;
	}

	/* Use the first available device in this context. */
	*device = devices[0];
	delete[] devices;

	/* Set up the command queue with the selected device. */
	*commandQueue = clCreateCommandQueue(context, *device, CL_QUEUE_PROFILING_ENABLE, &errorNumber);
	if (!checkSuccess(errorNumber)) {
		DEBUG_CRIT("Failed to create the OpenCL command queue");
		return false;
	}

	return true;
}

bool createProgram(cl_context context, cl_device_id device, string filename, cl_program* program) {
	cl_int errorNumber = 0;
	ifstream kernelFile(filename.c_str(), ios::in);

	if (!kernelFile.is_open()) {
		DEBUG_CRIT("Unable to open %s", filename.c_str());
		return false;
	}

	/*
	 * Read the kernel file into an output stream.
	 * Convert this into a char array for passing to OpenCL.
	 */
	ostringstream outputStringStream;
	outputStringStream << kernelFile.rdbuf();
	string srcStdStr = outputStringStream.str();
	const char* charSource = srcStdStr.c_str();

	*program = clCreateProgramWithSource(context, 1, &charSource, NULL,
			&errorNumber);
	if (!checkSuccess(errorNumber) || program == NULL) {
		DEBUG_CRIT("Failed to create OpenCL program");
		return false;
	}

	/* Try to build the OpenCL program. */
	bool buildSuccess = checkSuccess(clBuildProgram(*program, 0, NULL, NULL, NULL, NULL));

	/* Get the size of the build log. */
	size_t logSize = 0;
	clGetProgramBuildInfo(*program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);

	/*
	 * If the build succeeds with no log, an empty string is returned (logSize = 1),
	 * we only want to print the message if it has some content (logSize > 1).
	 */
	if (logSize > 1) {
		char* log = new char[logSize];
		clGetProgramBuildInfo(*program, device, CL_PROGRAM_BUILD_LOG, logSize, log, NULL);

		string* stringChars = new string(log, logSize);
		cerr << "Build log:\n " << *stringChars << endl;

		delete[] log;
		delete stringChars;
	}

	if (!buildSuccess) {
		clReleaseProgram(*program);
		DEBUG_CRIT("Failed to build OpenCL program");
		return false;
	}

	return true;
}

bool isExtensionSupported(cl_device_id device, string extension) {
	if (extension.empty()) {
		return false;
	}

	/* First find out how large the ouput of the OpenCL device query will be. */
	size_t extensionsReturnSize = 0;
	if (!checkSuccess(clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, 0, NULL, &extensionsReturnSize))) {
		DEBUG_CRIT("Failed to get return size from clGetDeviceInfo for parameter CL_DEVICE_EXTENSIONS");
		return false;
	}

	/* Allocate enough memory for the output. */
	char* extensions = new char[extensionsReturnSize];

	/* Get the list of all extensions supported. */
	if (!checkSuccess(clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, extensionsReturnSize, extensions, NULL))) {
		DEBUG_CRIT("Failed to get data from clGetDeviceInfo for parameter CL_DEVICE_EXTENSIONS");
		delete[] extensions;
		return false;
	}

	/* See if the requested extension is in the list. */
	string* extensionsString = new string(extensions);
	bool returnResult = false;
	if (extensionsString->find(extension) != string::npos) {
		returnResult = true;
	}

	delete[] extensions;
	delete extensionsString;

	return returnResult;
}

#endif // #ifndef SRC_OPENCL_COMMON_OPENCL_COMMON_H_
