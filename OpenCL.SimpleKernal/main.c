#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "CL/cl.h"

CONSOLE_SCREEN_BUFFER_INFO csbi;
HANDLE hStdOutput;

char* readFile(size_t* fileSize);

void main(void)
{
		cl_int error = 0;   // Used to handle error codes
		cl_uint entryCount = 1;
		cl_uint platformCount;
		cl_platform_id platform;
		cl_context context;
		cl_command_queue queue;
		cl_device_id device;
        cl_program program = NULL;
        cl_kernel vector_add_kernel = NULL;
		size_t byteCount;
        char* build_log;
        size_t log_size;
        float* src_a_h;
        float* src_b_h;
        float* res_h;
        float* check;
        cl_mem src_a_d;
        cl_mem src_b_d;
        cl_mem res_d;
        char* programText;
        size_t* programTextSize;
        int mem_size;
        int i;
        int size = 3;
        size_t local_ws = 512;
        size_t global_ws = 512;


		//Display console
        hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        if (!GetConsoleScreenBufferInfo(hStdOutput, &csbi))
        {
                printf("GetConsoleScreenBufferInfo failed: %d\n", GetLastError());
                exit(-1);
        }

		// Platform
		error = clGetPlatformIDs(entryCount, &platform, &platformCount);
		if (error != CL_SUCCESS) {
		  printf("Error getting platform id");
		   exit(error);
		}

		// Device
		error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
		if (error != CL_SUCCESS) {
		   printf("Error getting device IDs.");
		   exit(error);
		}

		// Context
		context = clCreateContext(0, 1, &device, NULL, NULL, &error);
		if (error != CL_SUCCESS) {
		   printf("Error creating the context.");
		   exit(error);
		}

		// Command-queue
		queue = clCreateCommandQueue(context, device, 0, &error);
		if (error != CL_SUCCESS) {
		   printf("Error creating the command queue.");
		   exit(error);
        }

        src_a_h = (float*)calloc(size, sizeof(float));
        src_b_h = (float*)calloc(size, sizeof(float));
        res_h = (float*)calloc(size, sizeof(float));
        
        // Initialize both vectors
        for (i = 0; i < size; i++) {
            src_a_h[i] = (float)5;
            src_b_h[i] = (float)5;
        }

        mem_size = sizeof(float)*size;
        // Allocates a buffer of size mem_size and copies mem_size bytes from src_a_h
        src_a_d = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, src_a_h, &error);
        src_b_d = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, src_b_h, &error);
        res_d = clCreateBuffer(context, CL_MEM_WRITE_ONLY, mem_size, NULL, &error);

        programTextSize = (size_t*)calloc(1, sizeof(size_t));
        programText = readFile(&(programTextSize[0]));

        program = clCreateProgramWithSource(context, 1, &programText, programTextSize, &error);
        error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);

        // First call to know the proper size
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        build_log = (char*)calloc(log_size+1, sizeof(char));
        // Second call to get the log
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
        build_log[log_size] = '\0';
        printf("%s\n\n", build_log);
        free(build_log);

        vector_add_kernel = clCreateKernel(program, "vector_dot_product", &error);

        error = clSetKernelArg(vector_add_kernel, 0, sizeof(cl_mem), &src_a_d);
        error |= clSetKernelArg(vector_add_kernel, 1, sizeof(cl_mem), &src_b_d);
        error |= clSetKernelArg(vector_add_kernel, 2, sizeof(cl_mem), &res_d);
        error |= clSetKernelArg(vector_add_kernel, 3, sizeof(size_t), &size);

        error = clEnqueueNDRangeKernel(queue, vector_add_kernel, 1, NULL, &global_ws, &local_ws, 0, NULL, NULL);

        check = (float*)calloc(size, sizeof(float));
        clEnqueueReadBuffer(queue, res_d, CL_TRUE, 0, mem_size, check, 0, NULL, NULL);

        for (i = 0; i < 3; i++)
        {
            printf("%f", check[i]);
        }

        printf("\n\n\n\tPress any key to exit...\n");
        getch();
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

/*
int main(int argc, char **argv){
	/*
	cl_platform_id test;
	cl_uint num;
	cl_uint ok = 1;
	clGetPlatformIDs(ok, &test, &num);
	return 0;
	



}*/