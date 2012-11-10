#include <windows.h>

struct Benchmarker
{
    LARGE_INTEGER startTime;
    LARGE_INTEGER endTime;
    LARGE_INTEGER frequency;
};

struct Benchmarker* create_benchmarker();

void start_benchmarker(struct Benchmarker* benchmarker);

double stop_benchmarker(struct Benchmarker* benchmarker);
