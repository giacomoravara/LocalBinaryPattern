#include "LbpPar.h"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <exception>
#include <thread>


LbpPar::LbpPar(const std::vector<byte> &pixels, unsigned width, unsigned height) :
	AbstractLbp(pixels, width, height) {}

LbpPar::~LbpPar() {

}

float *LbpPar::getHistogram(float *histograms, unsigned row, unsigned col) {
	// Each pixel block has its own histogram
	int_pair block;
	block.y = (row - region.gaps_pixel.y) / blockEdge;
	block.x = (col - region.gaps_pixel.x) / blockEdge;
	auto offset = getHistogramLength() * (block.y * region.grid_size.x + block.x);
	return (histograms + offset);
}

byte LbpPar::pixelAt(byte *pixels, int row, unsigned width, int col) {
	return this->pixels[row * width + col];
}

unsigned LbpPar::compareWithNeighborhood(byte *pixels, int_pair gaps_pixels, unsigned width, unsigned row, unsigned col,
	unsigned samples, int_pair *offsets) {
	byte pixel = pixelAt(pixels, row, width, col);
	unsigned result = 0;

	for (int i = 0; i < samples; i++) {
		unsigned nrow = row + offsets[i].y;
		unsigned ncol = col + offsets[i].x;
		byte neighbor = pixelAt(pixels, nrow, width, ncol);

		if (pixel <= neighbor) {
			int shift = samples - 1 - i;
			result = result | (0x1 << shift);
		}
	}

	return result;
}

void LbpPar::computeLBPs(int i, int_pair *offsets, std::vector<byte> *outputImage) {
	for (int k = region.gaps_pixel.y + i * ((region.end_pixels.y - region.gaps_pixel.y) / getNumThread());
		k < region.gaps_pixel.y + (i + 1) * ((region.end_pixels.y - region.gaps_pixel.y) / getNumThread()); k++) {
		for (int j = region.gaps_pixel.x; j < region.end_pixels.x; j++) {
			unsigned row = k;
			unsigned col = j;
			float *histogram = getHistogram(histograms.data(), row, col);
			unsigned pattern = compareWithNeighborhood(pixels.data(), region.gaps_pixel, width, row,
				col, samples, this->offsets.data());

			histogram[pattern] += 1;

			if (outputImage != nullptr) {
				float check = pattern / powf(2.f, samples - 8.f);
				outputImage->push_back((byte)roundf(check));
			}
		}
	}
}


std::vector<float> LbpPar::calculateNormalizedLBPs(float radius, unsigned samples, unsigned blockEdge) {
	return LbpPar::calculateNormalizedLBPs(radius, samples, blockEdge, nullptr, nullptr, nullptr);
}


std::vector<float>
LbpPar::calculateNormalizedLBPs(float radius, unsigned samples, unsigned blockEdge, std::vector<byte> *outputImage,
	unsigned *outputWidth, unsigned *outputHeight) {
	prepare(radius, samples, blockEdge);

	if (outputImage != nullptr) {
		outputImage->clear();
		*outputWidth = region.grid_size.x * blockEdge;
		*outputHeight = region.grid_size.y * blockEdge;
		outputImage->reserve(*outputWidth * *outputHeight);
	}
	unsigned remainder;

	int MAX_THREAD = getNumThread();
	for (int j = 0; j < MAX_THREAD; j++)
		threads.emplace_back(std::thread(&LbpPar::computeLBPs, this, j, this->offsets.data(), outputImage));
	for (auto &t : threads)
		if (t.joinable())
			t.join();

	int size = getHistogramLength() * getNumberHistograms();
	for (int i = 0; i < size; i++) {
		histograms[i] /= (blockEdge*blockEdge);
	}


	return std::move(histograms);
}


const int LbpPar::getNumThread() {

	std::thread thread;
	unsigned maxThreads = thread.hardware_concurrency();
	return maxThreads;
}