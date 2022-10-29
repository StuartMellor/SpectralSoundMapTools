#include <vector>
#include <complex>
#include <iostream>

#include "FFT_Transform.h"
#include "AudioFile.h"
#include <fstream>

#include "dataToFile.h"
#include "rgba_bitmap.h"
#include "rgba_bitmap.c"
#include <limits>

#include "smaudio.h"


#ifndef NDEBUG
// debug mode
#define SM_DEBUG_MODE true
#else
// release
#define SM_DEBUG_MODE false
#endif

const float maxFloatVal = std::numeric_limits<float>::max();

template<typename T>
void printArray(std::vector<T> input) {
	int N = input.size();
	if (N <= 0) {
		return;
	}
	std::cout << "Array: [";
	for (int i = 0; i < N - 1; i++) {
		std::cout << input[i] << ", ";
	}
	std::cout << input[N - 1] << "];" << std::endl;
}

int main() {
	if (SM_DEBUG_MODE) {
		std::cout << "Starting in debug mode..." << std::endl;
	}
	// 220630_0508
	// 220630_0496


	std::string audiopath = "C:/Users/stuar/Downloads/220630_0475.wav";
	std::vector<uint32_t> out;
	long samplerate, bitspersample, channels, duration;

	smaudio::readWavFile(audiopath, &out, &samplerate, &channels, &duration, &bitspersample);

	std::vector<uint32_t> spectrumMag;
	int newTestSize = 0;
	FFT_Transform::GetFreqMag(&out, &spectrumMag, 250 * 250, &newTestSize, false, true);

	std::ofstream myfile;
	myfile.open("spectmag.csv", std::ios::trunc);
	for (int i = 0; i < spectrumMag.size(); i++) {
		myfile << spectrumMag[i] << "\n";
	}
	myfile.close();

	return 0;
}