#define _USE_MATH_DEFINES
#include "AbstractLbp.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <iomanip>
#include <cmath>

AbstractLbp::AbstractLbp(const std::vector<byte>& pixels, unsigned width, unsigned height) :
	pixels(toGrayscale(pixels, width, height)),
	width(width),
	height(height),
	radius(0.f),
	samples(0),
	blockEdge(0) {
}

AbstractLbp::~AbstractLbp() {
}

std::vector<byte> AbstractLbp::toGrayscale(const std::vector<byte>& pixels, unsigned width, unsigned height)
{
	int channels;
	if (pixels.size() == width * height) {
		return pixels;
	}
	else if (pixels.size() == 3 * width * height) {
		channels = 3;
	}
	else if (pixels.size() == 4 * width * height) {
		channels = 4;
	}
	else {
		throw std::invalid_argument("Not a 1, 3 or 4 (of 1 byte) channels image");
	}
	std::vector<byte> grayPixels;
	grayPixels.reserve(pixels.size() / channels);

	for (int i = 0; i < pixels.size(); i += channels)
	{
		byte gray = (byte)round((pixels[i] + pixels[i + 1] + pixels[i + 2]) / 3.f);
		grayPixels.push_back(gray);
	}

	return grayPixels;
}

void AbstractLbp::prepare(float radius, unsigned samples, unsigned blockEdge)
{
	this->radius = radius;
	this->samples = samples;
	this->blockEdge = blockEdge;

	if (!checkMinimumSize()) {
		throw new std::invalid_argument("The image is too small");
	}
	// Def neighborhood
	calcSamplingOffsets();
	// Def net area
	calcImageRegion();
	// Alloc host mem
	allocateHistograms();
}

bool AbstractLbp::checkMinimumSize()
{
	int minSize = 2 * ((int)round(radius)) + blockEdge;

	if (width >= minSize && height >= minSize) {
		return true;
	}
	return false;
}

void AbstractLbp::calcSamplingOffsets()
{
	this->offsets.clear();
	float angle = 2 * M_PI / samples;

	// Loop the unit circle clockwise
	for (int sample = 0; sample < samples; sample++)
	{
		offsets.push_back({ 0, 0 });
		offsets[sample].y = (-1) * (int)round(radius * cos(sample * angle));
		offsets[sample].x = (+1) * (int)round(radius * sin(sample * angle));
	}
}

void AbstractLbp::calcImageRegion()
{
	int radiusInt = (int)round(radius);
	// Define the processed sub-region of the image
	region.gaps_pixel.y = (2 * radiusInt + (height - 2 * radiusInt) % blockEdge) / 2;
	region.gaps_pixel.x = (2 * radiusInt + (width - 2 * radiusInt) % blockEdge) / 2;
	region.grid_size.y = (height - 2 * radiusInt) / blockEdge;
	region.grid_size.x = (width - 2 * radiusInt) / blockEdge;
	region.end_pixels.y = region.gaps_pixel.y + region.grid_size.y * blockEdge;
	region.end_pixels.x = region.gaps_pixel.x + region.grid_size.x * blockEdge;
}

void AbstractLbp::allocateHistograms()
{
	long size = getHistogramLength() * getNumberHistograms();
	try {
		histograms = std::vector<float>(size, 0.f);
	}
	catch (const std::bad_alloc& e) {
		throw std::invalid_argument("std::vector<float>(" + std::to_string(size) + ", 0.f) failed");
	}
}

long AbstractLbp::getNumberHistograms() const
{
	return region.grid_size.x * region.grid_size.y;
}

int_pair AbstractLbp::getGridSize() const
{
	return region.grid_size;
}

long AbstractLbp::getHistogramLength() const
{
	return pow(2, samples);
}

long AbstractLbp::getHistogramsSizeInBytes()
{
	return sizeof(float) * getHistogramLength() * getNumberHistograms();
}

void AbstractLbp::saveHistogramsToFile(std::vector<float> histograms, long histogramLength, long numberHistograms, const std::string& filename)
{
	std::ofstream file(filename);
	file << std::fixed << std::setprecision(3);

	// Write histograms to text rows
	for (int i = 0; i < numberHistograms; i++)
	{
		float *histogram = (histograms.data() + i * histogramLength);
		for (int j = 0; j < histogramLength; j++)
		{
			file << histogram[j] << ' ';
		}
		file << '\n';
	}

	file.close();
}
