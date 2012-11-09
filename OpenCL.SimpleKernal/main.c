#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "CL/cl.h"

struct vector4
{
    float x;
    float y;
    float z;
    float w;
};

CONSOLE_SCREEN_BUFFER_INFO csbi;
HANDLE hStdOutput;

cl_program compileProgram(cl_context context, cl_device_id device);
cl_int executeKernal(cl_program program);
BOOL initializePlatformDevice(cl_platform_id* platform, cl_device_id* device, cl_context* context, cl_command_queue* queue);
char* readFile(size_t* fileSize);

void main(void)
{
	cl_platform_id platform = NULL;
	cl_context context = NULL;
	cl_command_queue queue = NULL;
	cl_device_id device = NULL;
    cl_program program = NULL;
    struct vector4* sourceVectorsA;
    struct vector4* sourceVectorsB;
    float* results;
    cl_mem bufferA;
    cl_mem bufferB;
    cl_mem bufferResults;
    int i;
    size_t size = 500000;
    size_t local_ws = 512;
    size_t global_ws = 500000;
    cl_int error = 0;

	//Display console
    hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!GetConsoleScreenBufferInfo(hStdOutput, &csbi)) {
            printf("GetConsoleScreenBufferInfo failed: %d\n", GetLastError());
            exit(-1);
    }

	//Get all necessary components needed to use the OpenCL platform
    if (!initializePlatformDevice(&platform, &device, &context, &queue)) {
        exit(-1);
    }

    sourceVectorsA = (struct vector4*)calloc(size, sizeof(struct vector4));
    sourceVectorsB = (struct vector4*)calloc(size, sizeof(struct vector4));
    results = (float*)calloc(size, sizeof(float));
        
    // Initialize both vectors
    for (i = 0; i < size; i++) {
        sourceVectorsA[i].x = (float)(rand() % 100);
        sourceVectorsB[i].x = (float)(rand() % 100);
        sourceVectorsA[i].y = (float)(rand() % 100);
        sourceVectorsB[i].y = (float)(rand() % 100);
        sourceVectorsA[i].z = (float)(rand() % 100);
        sourceVectorsB[i].z = (float)(rand() % 100);
    }

    // Allocates a buffer of size mem_size and copies mem_size bytes from src_a_h
    bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(struct vector4)*size, sourceVectorsA, &error);
    bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(struct vector4)*size, sourceVectorsB, &error);
    bufferResults = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float)*size, NULL, &error);

    //Compile the opencl program.
    program = compileProgram(context, device);
    if (program == NULL) {
        printf("Error compiling program.");
        exit(-1);
    }

    //Execute the simple kernal on the buffered data.
    error = executeKernal(program, queue, bufferA, bufferB, bufferResults, size);
    if (error != CL_SUCCESS) {
        printf("Error executing kernal.");
        exit(error);
    }

    //Get the kernal results.
    clEnqueueReadBuffer(queue, bufferResults, CL_TRUE, 0, sizeof(float)*size, results, 0, NULL, NULL);

    for (i = 0; i < size; i+=1000)
    {
        printf("[%i] %f\n", i, results[i]);
    }

    printf("\n\n\n\tPress any key to exit...\n");
    getch();
}

cl_program compileProgram(cl_context context, cl_device_id device)
{
    char* programSource = NULL;
    char* buildLog = NULL;
    size_t* programSourceSize = NULL;
    cl_program program = NULL;
    size_t logSize = 0;
    cl_int error;

    programSourceSize = (size_t*)calloc(1, sizeof(size_t));
    programSource = readFile(&(programSourceSize[0]));

    program = clCreateProgramWithSource(context, 1, &programSource, programSourceSize, &error);
    error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    // First call to know the proper size
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
    buildLog = (char*)calloc(logSize+1, sizeof(char));

    // Second call to get the log
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, buildLog, NULL);
    buildLog[logSize] = '\0';

    //Output the log
    printf("%s\n\n", buildLog);

    free(buildLog);

    return program;
}

cl_int executeKernal(cl_program program, cl_command_queue queue, cl_mem bufferA, cl_mem bufferB, cl_mem bufferResults, size_t size)
{
    cl_kernel vector_add_kernel = NULL;
    cl_int error = 0;

    vector_add_kernel = clCreateKernel(program, "vector_dot_product", &error);

    error = clSetKernelArg(vector_add_kernel, 0, sizeof(cl_mem), &bufferA);
    error |= clSetKernelArg(vector_add_kernel, 1, sizeof(cl_mem), &bufferB);
    error |= clSetKernelArg(vector_add_kernel, 2, sizeof(cl_mem), &bufferResults);
    error |= clSetKernelArg(vector_add_kernel, 3, sizeof(size_t), &size);

    error = clEnqueueNDRangeKernel(queue, vector_add_kernel, 1, NULL, &size, NULL, 0, NULL, NULL);

    return error;
}

BOOL initializePlatformDevice(cl_platform_id* platform, cl_device_id* device, cl_context* context, cl_command_queue* queue)
{
    cl_uint platformCount = 0;
    cl_int error = 0;

    //Platform
	error = clGetPlatformIDs(1, platform, &platformCount);
	if (error != CL_SUCCESS) 
    {
		printf("Error getting platform id");
		return FALSE;
	}

    //Device
	error = clGetDeviceIDs(*platform, CL_DEVICE_TYPE_GPU, 1, device, NULL);
	if (error != CL_SUCCESS) {
		printf("Error getting device IDs.");
		return FALSE;
    }

    // Context
	*context = clCreateContext(0, 1, device, NULL, NULL, &error);
	if (error != CL_SUCCESS) {
		printf("Error creating the context.");
		return FALSE;
    }

    // Command-queue
	*queue = clCreateCommandQueue(*context, *device, 0, &error);
	if (error != CL_SUCCESS) {
		printf("Error creating the command queue.");
		return FALSE;
    }
    
    return TRUE;
}

char* readFile(size_t* fileSize)
{   
    FILE *fp;
    char *buffer;

    fp = fopen ( "simple.cl" , "rb" );
    if( !fp ) perror("simple.cl"),exit(1);

    fseek( fp , 0L , SEEK_END);
    (* fileSize) = ftell( fp );
    rewind( fp );

    // allocate memory for entire content
    buffer = (char*)calloc( 1, (* fileSize) + 1 );
    if( !buffer ) fclose(fp),fputs("memory alloc fails",stderr),exit(1);

    /* copy the file into the buffer */
    if( 1!=fread( buffer , (* fileSize), 1 , fp) )
        fclose(fp),free(buffer),fputs("entire read fails",stderr),exit(1);


    fclose(fp);
    
    return buffer;

}