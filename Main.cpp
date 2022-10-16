#include <vector>
#include <complex>
#include <iostream>

#include "FFT_Transform.h"
#include "FFT_Transform.cpp"
#include "AudioFile.h"
#include <fstream>

#include "dataToFile.h"
#include "rgba_bitmap.h"
#include "rgba_bitmap.c"

#ifndef NDEBUG
// debug mode
#define SM_DEBUG_MODE true
#else
// release
#define SM_DEBUG_MODE false
#endif


int main() {
	if (SM_DEBUG_MODE) {
		std::cout << "Starting in debug mode..." << std::endl;
	}

	//std::vector<float> test;
	//for (int i = 0; i < 64; i++) {
	//	test.push_back(static_cast<float>(32767.0 / rand()));
	//}

	std::cout << "Size of float in bits: " << sizeof(float) * 8 << std::endl;

	AudioFile<float> audioFile;
	audioFile.load("C:/Users/stuar/Downloads/Bass-Drum-1.wav");
	int dataN = audioFile.getNumSamplesPerChannel();
	std::cout << "dataN: " << dataN << std::endl;
	std::cout << "audioFile.samples[0].size(): " << audioFile.samples[0].size() << std::endl;
	std::cout << "Data Start Address: " << &audioFile.samples[0][0] << std::endl;
	

	int newTestSize = 0;
	//FFT_Transform<float>::resample_inplace(&out, 3, &newTestSize, SPECTSOUND_DOWNSCALING_ALGORITHM::MOVING_AVG);
	std::vector<float> spectrumMag;

	FFT_Transform<float>::GetFreqMag(&(audioFile.samples[0]), &spectrumMag, 2500, &newTestSize, false, true);

	// dataToFile::
	dataToFile::writeArray_raw(&spectrumMag, "C:/Users/stuar", "spectmag", true);

	//std::ofstream myfile;
	//myfile.open("C:/Users/stuar/spectmag.csv", std::ios::trunc);
	//for (int i = 0; i < spectrumMag.size(); i++) {
	//	myfile << spectrumMag[i] << "\n";
	//}
	//myfile.close();
}