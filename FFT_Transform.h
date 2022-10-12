#pragma once
#include <cmath>
#include <vector>
#include <complex>
#include <iostream>
#include "SpectSoundTypes.h"

template<typename T>
class FFT_Transform {
	// Will be private but for testing public
	//private:
public:
	FFT_Transform() {};
	static int DToC_transform(std::vector<T>* in, int dataN, std::vector<std::complex<T>>* out, int chunkSize, int overlap, bool DEBUG_MODE);
	static int CToD_transform();
	static int DToC_general_transform(std::vector<T>* in, int dataN, std::vector<std::complex<T>>* out, int chunkSize, int overlap, bool DEBUG_MODE);
	
	static int GetFreqMag(std::vector<T>* in, std::vector<T>* out, int desiredLength, int* actualLength, bool destructive,bool DEBUG_MODE);
	static int resample_inplace(std::vector<T>* in, int newSampleRatio, int* actualLength, SPECTSOUND_DOWNSCALING_ALGORITHM SSDA);
	static int resample(std::vector<T>* in, std::vector<T>* out, int newSampleRatio, int* actualLength, SPECTSOUND_DOWNSCALING_ALGORITHM SSDA);
};
