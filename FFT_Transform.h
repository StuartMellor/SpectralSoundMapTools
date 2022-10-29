#pragma once
#include <cmath>
#include <vector>
#include <complex>
#include <iostream>
#include "SpectSoundTypes.h"


class FFT_Transform {
	// Will be private but for testing public
	//private:
public:
	FFT_Transform() {};
	static int DToC_transform(std::vector<float>* in, int dataN, std::vector<std::complex<float>>* out, int chunkSize, int overlap, bool DEBUG_MODE);
	static int GetFreqMag(std::vector<uint32_t>* in, std::vector<uint32_t>* out, int desiredLength, int* actualLength, bool destructive, bool DEBUG_MODE);
	static int resample_inplace(std::vector<float>* in, int length, SPECTSOUND_DOWNSCALING_ALGORITHM SSDA);
	static int resample(std::vector<float>* in, std::vector<float>* out, int newSampleRatio, int* actualLength, SPECTSOUND_DOWNSCALING_ALGORITHM SSDA);
};
