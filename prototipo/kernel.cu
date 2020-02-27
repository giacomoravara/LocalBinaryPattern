#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <iostream>
#include <bitset>
#include <chrono>
#include <stdexcept>
#include "lodepng.h"
#include "LbpSeq.h"
#include "LbpPar.h"
#include "LbpCuda.h"
#include "Benchmark.h"

static std::vector<byte> pixels;
static unsigned width;
static unsigned height;

static bool loadImage(const std::string& filename)
{
	unsigned error = lodepng::decode(pixels, width, height, filename);
	if (error) {
		std::cerr << lodepng_error_text(error) << std::endl;
		return false;
	}
	std::cout << "image size is " << width << "x" << height << std::endl;
	return true;
}

static void makeSampleOutput()
{
	LbpSeq output(pixels, width, height);
	LbpPar output2(pixels, width, height);
	LbpCuda output3(pixels, width, height);

	std::vector<byte> outputImage;
	unsigned outputWidth, outputHeight;
	output.calculateNormalizedLBPs(2, 8, 32, &outputImage, &outputWidth, &outputHeight);

	std::string outI;
	std::cout << "Insert Path for output" << std::endl;
	std::getline(std::cin, outI);

	lodepng::encode(outI, outputImage, outputWidth, outputHeight, LCT_GREY);

}



static void test()
{
	LbpSeq image(pixels, width, height);
	LbpPar d_Image(pixels, width, height);
	LbpCuda d_Image_cuda(pixels, width, height);
	int samp[] = { 4, 6, 8, 9, 10, 11, 12, 13, 14, 15 };
	float rads[] = { 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0 };
	//int edge[] = { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 };
	int edge[] = { 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 };
	for (int i = 0; i < 10; i++) {
		long cpuMillis[1000] = {};
		long totCPU = 0;
		long gpuMillis[1000] = {};
		long totGPU = 0;
		long gpuMillisCUDA[1000] = {};
		long totCUDA = 0;
		try {
			for (int j = 0; j < 1000; j++) {
				Benchmark::start();
				auto cpuHistograms = image.calculateNormalizedLBPs(rads[i], samp[i], edge[i]);
				Benchmark::stop();
				cpuMillis[j] = Benchmark::getMillis();
				totCPU += cpuMillis[j];
				Benchmark::start();
				auto gpuHistograms = d_Image.calculateNormalizedLBPs(rads[i], samp[i], edge[i]);
				Benchmark::stop();
				gpuMillis[j] = Benchmark::getMillis();
				totGPU += gpuMillis[j];
				Benchmark::start();
				auto gpuHistograms_CUDA = d_Image_cuda.calculateNormalizedLBPs(rads[i], samp[i], edge[i]);
				Benchmark::stop();
				gpuMillisCUDA[j] = Benchmark::getMillis();
				totCUDA += gpuMillisCUDA[j];
				std::cerr << "With conf {s=" << samp[j] << "; r=" << rads[j] << "; e=" << edge[j] << "} ";
				std::cerr << "\tSequential took " << totCPU / 1000 << "ms and Parallel " << totGPU / 1000 << "ms and Cuda " << totCUDA / 1000 << "ms";
			}
		}

		catch (const std::invalid_argument& e) {
			std::cerr << e.what() << std::endl;
			std::cerr << "Conf {s=" << samp[i] << "; r=" << rads[i] << "; e=" << edge[i] << "} is not supported" << std::endl;
			std::cerr << "End of main" << std::endl;
			return;
		}
	}
}

int main()
{
	std::string s;
	std::cout << "Insert Path" << std::endl;
	std::getline(std::cin, s);
	bool t = loadImage(s);
	makeSampleOutput();
	//test();
	cudaDeviceReset();
	return 0;
}

