#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "CL/cl.h"

CONSOLE_SCREEN_BUFFER_INFO csbi;
HANDLE hStdOutput;

char* getPlatformInfo(cl_platform_id, cl_platform_info);

void main(void)
{
    cl_int error = 0;   // Used to handle error codes.
    cl_uint entryCount = 1;
    cl_uint platformCount;
    cl_platform_id* platforms;
    char *profile;
    char *version;
    char *name;
    char *vendor;
    char *extensions;
    int index;
	
    //Display console		
    hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!GetConsoleScreenBufferInfo(hStdOutput, &csbi))
    {
        printf("GetConsoleScreenBufferInfo failed: %d\n", GetLastError());
        exit(-1);
    }

    //Platform count
    error = clGetPlatformIDs(0, NULL, &platformCount);
    if (error != CL_SUCCESS) {
        printf("Error getting platform count");
        exit(error);
    }

    platforms = (cl_platform_id*)calloc(platformCount, sizeof(cl_platform_id));

    //Platform
    error = clGetPlatformIDs(platformCount, platforms, NULL);
    if (error != CL_SUCCESS) {
	    printf("Error getting platform id");
        exit(error);
    }

    printf("\nPlatform count: %i", platformCount);
    printf("\n");

    for (index = 0; index < platformCount; index++) {

        profile = getPlatformInfo(platforms[index], CL_PLATFORM_PROFILE);
        version = getPlatformInfo(platforms[index], CL_PLATFORM_VERSION);
        name = getPlatformInfo(platforms[index], CL_PLATFORM_NAME);
        vendor = getPlatformInfo(platforms[index], CL_PLATFORM_VENDOR);
        extensions = getPlatformInfo(platforms[index], CL_PLATFORM_EXTENSIONS);

        printf("\nPlatform profile: %s ", profile);
        printf("\nPlatform version: %s ", version);
        printf("\nPlatform name: %s ", name);
        printf("\nPlatform vendor: %s ", vendor);
        printf("\nPlatform extensions: %s ", extensions);
        printf("\n");
    
        free(profile);
        free(version);
        free(name);
        free(vendor);
        free(extensions);

    }

    free(platforms);

    printf("\n\n\tPress any key to exit...\n");
    getch();
}

char* getPlatformInfo(cl_platform_id platform, cl_platform_info infoId)
{
    cl_int error;
    size_t byteCount;
    char* info;

    //Determine the byte count of the char[] for the info being queried.
    clGetPlatformInfo(platform, infoId, 0,  NULL, &byteCount);
    info = (char*)calloc(byteCount, sizeof(char));

    //Populate the info char*.
    error = clGetPlatformInfo(platform, infoId, byteCount, info, &byteCount);
    if (error != CL_SUCCESS) {
	    printf("Failed to get platform info.");
	    exit(error);
    }

    return info;
}