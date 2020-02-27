#ifndef LBPPAR_H_
#define LBPPAR_H_


#include "AbstractLbp.h"
#include "AbstractLbp.h"
#include <thread>
#include <iostream>
#include <mutex>
class LbpPar : public AbstractLbp
{
public:
	~LbpPar();

	byte pixelAt(byte *pixels, int row, unsigned width, int col);
	LbpPar(const std::vector<byte>& pixels, unsigned width, unsigned height);
	void normalizeHistograms(int i);
	void computeLBPs(int i, int_pair *offsets, std::vector<byte>* outputImage);
	float* getHistogram(float *histograms, unsigned row, unsigned col);
	unsigned compareWithNeighborhood(byte *pixels, int_pair gaps_pixels, unsigned width, unsigned row, unsigned col, unsigned samples, int_pair *offsets);
	std::vector<float> calculateNormalizedLBPs(float radius, unsigned samples, unsigned blockEdge, std::vector<byte> *outputImage, unsigned *outputWidth, unsigned *outputHeight);
	const int getNumThread();
	std::vector<float> calculateNormalizedLBPs(float radius, unsigned samples, unsigned blockEdge) override;

};


#endif
