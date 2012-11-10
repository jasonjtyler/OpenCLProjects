#include "benchmarker.h"

struct Benchmarker* create_benchmarker()
{
    struct Benchmarker* newMarker;
    newMarker = (struct Benchmarker*)malloc(sizeof(struct Benchmarker));
    RtlZeroMemory(newMarker, sizeof(struct Benchmarker));

    QueryPerformanceFrequency( &(newMarker->frequency));

    return newMarker;
}

void start_benchmarker(struct Benchmarker* benchmarker)
{
    QueryPerformanceCounter( &(benchmarker->startTime));
}

double stop_benchmarker(struct Benchmarker* benchmarker)
{
    QueryPerformanceCounter( &(benchmarker->endTime));
    return (double)(benchmarker->endTime.QuadPart - benchmarker->startTime.QuadPart) / (double)benchmarker->frequency.QuadPart;
}