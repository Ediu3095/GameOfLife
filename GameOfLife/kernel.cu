#include <iostream>

#include "kernel.cuh"

__global__ void updateKernel(const int rows, const int cols, const bool* in, bool* out)
{
	// Get the row and column with which to work
	int i = threadIdx.x + blockIdx.x * blockDim.x;
	int j = threadIdx.y + blockIdx.y * blockDim.y;

	// Stop threads that overstep the bounds of the grid
	if (i >= rows || j >= cols)
		return;

	// Count the amount of living neighbours around this position
	int neighbours = -in[i * cols + j];
	for (int ioff = -1; ioff <= 1; ioff++)
		for (int joff = -1; joff <= 1; joff++)
		{
			// Handle out of bounds indices
			int iaux = i + ioff, jaux = j + joff;

			if (iaux < 0)
				iaux = rows - 1;
			else if (iaux >= rows)
				iaux = 0;

			if (jaux < 0)
				jaux = cols - 1;
			else if (jaux >= cols)
				jaux = 0;

			// Count a new neighbour
			neighbours += in[iaux * cols + jaux];
		}

	// Store the amount of neighbours in the output
	out[i * cols + j] = (neighbours == 3 || (neighbours == 2 && in[i * cols + j]));
}

cudaError_t updateWithCuda(const int rows, const int cols, const bool* in, bool* out)
{
	bool* dev_in;
	bool* dev_out;
	cudaError_t cudaStatus;

	try
	{
		// Choose which GPU to run on, change this on a multi-GPU system.
		cudaStatus = cudaSetDevice(0);
		if (cudaStatus != cudaSuccess)
		{
			throw std::exception("cudaSetDevice failed! Do you have a CUDA-capable GPU installed?");
		}

		// Allocate GPU buffers for three vectors (two input, one output).
		cudaStatus = cudaMalloc((void**)&dev_in, rows * cols * sizeof(bool));
		if (cudaStatus != cudaSuccess)
		{
			throw std::exception("cudaMalloc failed!");
		}

		cudaStatus = cudaMalloc((void**)&dev_out, rows * cols * sizeof(bool));
		if (cudaStatus != cudaSuccess)
		{
			throw std::exception("cudaMalloc failed!");
		}

		// Copy input vectors from host memory to GPU buffers.
		cudaStatus = cudaMemcpy(dev_in, in, rows * cols * sizeof(bool), cudaMemcpyHostToDevice);
		if (cudaStatus != cudaSuccess)
		{
			throw std::exception("cudaMemcpy failed!");
		}

		// Launch a kernel on the GPU with one thread for each element.
		int temp = std::min(rows, cols);
		float tileSize = (temp < 32) ? temp : 32;

		dim3 gridSize(std::ceil(rows / tileSize), std::ceil(cols / tileSize));
		dim3 blockSize(tileSize, tileSize);
		updateKernel <<<gridSize, blockSize>>>(rows, cols, dev_in, dev_out);

		// Check for any errors launching the kernel
		cudaStatus = cudaGetLastError();
		if (cudaStatus != cudaSuccess)
		{
			//fprintf(stderr, "updateKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
			throw std::exception("updateKernel launch failed");
		}

		// cudaDeviceSynchronize waits for the kernel to finish, and returns
		// any errors encountered during the launch.
		cudaStatus = cudaDeviceSynchronize();
		if (cudaStatus != cudaSuccess)
		{
			//fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
			throw std::exception("cudaDeviceSynchronize returned an error code after launching addKernel!");
		}

		// Copy output vector from GPU buffer to host memory.
		cudaStatus = cudaMemcpy(out, dev_out, rows * cols * sizeof(bool), cudaMemcpyDeviceToHost);
		if (cudaStatus != cudaSuccess)
		{
			throw std::exception("cudaMemcpy failed!");
		}
	}
	catch (std::exception ex)
	{
		fprintf(stderr, ex.what());
	}

	cudaFree(dev_in);
	cudaFree(dev_out);

	return cudaStatus;
}
