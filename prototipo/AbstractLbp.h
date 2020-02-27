#ifndef ABSTRACTLBP_H_
#define ABSTRACTLBP_H_

#include <vector>
#include <cmath>
#include <string>
#include "Types.h"
#include <thread>
class AbstractLbp
{
public:
	virtual ~AbstractLbp();

	virtual std::vector<float> calculateNormalizedLBPs(float radius, unsigned samples, unsigned blockEdge) = 0;

	long getHistogramLength() const;

	int_pair getGridSize() const;

	long getNumberHistograms() const;

	static void saveHistogramsToFile(std::vector<float> histograms, long histLength, long numHists, const std::string& filename);

protected:
	std::vector<byte> pixels;
	unsigned width, height;
	float radius;
	unsigned samples;
	unsigned blockEdge;
	image_region region;
	std::vector<int_pair> offsets;
	std::vector<float> histograms;

	std::vector<std::thread> threads;
protected:
	AbstractLbp(const std::vector<byte>& pixels, unsigned width, unsigned height);
	void prepare(float radius, unsigned samples, unsigned blockEdge);
	long getHistogramsSizeInBytes();

private:
	static std::vector<byte> toGrayscale(const std::vector<byte>& pixels, unsigned width, unsigned height);
	bool checkMinimumSize();
	void calcSamplingOffsets();
	void calcImageRegion();
	void allocateHistograms();
};

#endif

