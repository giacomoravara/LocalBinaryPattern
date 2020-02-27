#ifndef LBPSEQ_H_
#define LBPSEQ_H_

#include "AbstractLbp.h"

class LbpSeq : public AbstractLbp
{
public:
	~LbpSeq();
	LbpSeq(const std::vector<byte>& pixels, unsigned width, unsigned height);

	std::vector<float> calculateNormalizedLBPs(float radius, unsigned samples, unsigned blockEdge,
		std::vector<byte> *outputImage, unsigned *outputWidth, unsigned *outputHeight);

	std::vector<float> calculateNormalizedLBPs(float radius, unsigned samples, unsigned blockEdge);

private:
	byte pixelAt(unsigned row, unsigned col);
	float *getHistogram(float *histograms, unsigned row, unsigned col);
	unsigned compareWithNeighborhood(unsigned row, unsigned col);
};

#endif

