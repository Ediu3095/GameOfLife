#include "cuda_runtime.h"
#include "device_launch_parameters.h"

cudaError_t updateWithCuda(const int rows, const int cols, const bool* in, int* out);
