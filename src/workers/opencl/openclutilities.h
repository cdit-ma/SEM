#ifndef OPENCL_RE_UTILITIES_H
#define OPENCL_RE_UTILITIES_H

#include "cl2.hpp"
#include <string>
#include <iostream>
#include <core/modellogger.h>

/*void LogOpenCLError(std::string message, cl_int errorCode) {
	std::cerr << "OpenCL error (" << errorCode << "): " << message << std::endl;
}*/

/*void LogOpenCLError(std::string message) {
	std::cerr << "OpenCL error: " << message << std::endl;
}*/

static std::string clErrorNames[] = {
	"CL_SUCCESS",
	"CL_DEVICE_NOT_FOUND",
	"CL_DEVICE_NOT_AVAILABLE",
	"CL_COMPILER_NOT_AVAILABLE",
	"CL_MEM_OBJECT_ALLOCATION_FAILURE",
	"CL_OUT_OF_RESOURCES",
	"CL_OUT_OF_HOST_MEMORY",
	"CL_PROFILING_INFO_NOT_AVAILABLE",
	"CL_MEM_COPY_OVERLAP",
	"CL_IMAGE_FORMAT_MISMATCH",
	"CL_IMAGE_FORMAT_NOT_SUPPORTED",
	"CL_BUILD_PROGRAM_FAILURE",
	"CL_MAP_FAILURE",
	"CL_MISALIGNED_SUB_BUFFER_OFFSET",
	"CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"CL_INVALID_VALUE",
	"CL_INVALID_DEVICE_TYPE",
	"CL_INVALID_PLATFORM",
	"CL_INVALID_DEVICE",
	"CL_INVALID_CONTEXT",
	"CL_INVALID_QUEUE_PROPERTIES",
	"CL_INVALID_COMMAND_QUEUE",
	"CL_INVALID_HOST_PTR",
	"CL_INVALID_MEM_OBJECT",
	"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
	"CL_INVALID_IMAGE_SIZE",
	"CL_INVALID_SAMPLER",
	"CL_INVALID_BINARY",
	"CL_INVALID_BUILD_OPTIONS",
	"CL_INVALID_PROGRAM",
	"CL_INVALID_PROGRAM_EXECUTABLE",
	"CL_INVALID_KERNEL_NAME",
	"CL_INVALID_KERNEL_DEFINITION",
	"CL_INVALID_KERNEL",
	"CL_INVALID_ARG_INDEX",
	"CL_INVALID_ARG_VALUE",
	"CL_INVALID_ARG_SIZE",
	"CL_INVALID_KERNEL_ARGS",
	"CL_INVALID_WORK_DIMENSION",
	"CL_INVALID_WORK_GROUP_SIZE",
	"CL_INVALID_WORK_ITEM_SIZE",
	"CL_INVALID_GLOBAL_OFFSET",
	"CL_INVALID_EVENT_WAIT_LIST",
	"CL_INVALID_EVENT",
	"CL_INVALID_OPERATION",
	"CL_INVALID_GL_OBJECT",
	"CL_INVALID_BUFFER_SIZE",
	"CL_INVALID_MIP_LEVEL",
	"CL_INVALID_GLOBAL_WORK_SIZE",
	"CL_INVALID_PROPERTY"
};

static void LogOpenCLError(Worker* worker_reference,
	std::string function_signature,
	std::string error_message,
	cl_int cl_error_code)
{
	std::string message = error_message + " (" + clErrorNames[-cl_error_code] + ")";

	if (worker_reference != NULL) {
		ModelLogger::get_model_logger()->LogWorkerEvent(worker_reference,
			function_signature,
			ModelLogger::WorkloadEvent::MESSAGE,
			-1,		// Need to expose something like get_current_work_id() 
			message);
	}
	else {
		std::cerr << function_signature << ": " << message << std::endl;
	}
}

static void LogOpenCLError(Worker* worker_reference,
	std::string function_signature,
	std::string error_message)
{
	if (worker_reference != NULL) {
		ModelLogger::get_model_logger()->LogWorkerEvent(worker_reference,
			function_signature,
			ModelLogger::WorkloadEvent::MESSAGE,
			-1,		// Need to expose something like get_current_work_id() 
			error_message);
	}
	else {
		std::cerr << function_signature << ": " << error_message << std::endl;
	}
}

#endif
