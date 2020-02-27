#ifndef LBPCUDA_H_
#define LBPCUDA_H_

#include "AbstractLbp.h"
#include "cuda.h"

class LbpCuda : public AbstractLbp
{
public:
	~LbpCuda();

	LbpCuda(const std::vector<byte>& pixels, unsigned width, unsigned height);

	std::vector<float> calculateNormalizedLBPs(float radius, unsigned samples, unsigned blockEdge);
	

private:
	byte *d_pixels;
	
	int_pair *d_offsets;
		void calcHistGridAndBlockSize(dim3& grid, dim3& block, unsigned& remainder);
		void calcLbpGridAndBlockSize(dim3& grid, dim3& block);
		struct cudaDeviceProp getDeviceProps();
};

#endif

