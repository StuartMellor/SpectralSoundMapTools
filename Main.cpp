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
#include <limits>

#include "smaudio.h"
#include "smaudio.cpp"


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

	std::string audiopath = "C:/Users/stuar/Downloads/220630_0496.wav";
	std::vector<uint16_t> out;
	long samplerate, bitspersample, channels, duration;
	smaudio<uint16_t>::readWavFile(audiopath, &out, &samplerate, &channels, &duration, &bitspersample);

	/*float testa = 123456;
	std::cout << "Test a: " << testa << std::endl;
	std::cout << "Float size: " << sizeof(float) << std::endl;
	std::cout << "uint16_t size: " << sizeof(uint16_t) << std::endl;
	std::cout << "long size: " << sizeof(long) << std::endl;
	unsigned char bytes[4];
	bytes[0] = (static_cast<long>(testa) >> 24) & 0xFF;
	bytes[1] = (static_cast<long>(testa) >> 16) & 0xFF;
	bytes[2] = (static_cast<long>(testa) >> 8) & 0xFF;
	bytes[3] = (static_cast<long>(testa)) & 0xFF;

	long testout[4] = { 0, 0, 0, 0 };
	float res = 0;

	for (int i = 0; i < 4; i++) {
		std::cout << static_cast<int>(bytes[i]) << " " << std::endl;
	}

	testout[0] = bytes[0] << 24;
	std::cout << "testout[0]: " << testout[0] << std::endl;
	testout[1] = bytes[1] << 16;
	std::cout << "testout[1]: " << testout[1] << std::endl;
	testout[2] = bytes[2] << 8;
	std::cout << "testout[2]: " << testout[2] << std::endl;
	testout[3] = bytes[3];
	std::cout << "testout[3]: " << testout[3] << std::endl;
	res += testout[0] + testout[1] + testout[2] + testout[3];
	std::cout << "res: " << res;
	
	*/




	//testb[0] = (testa >> 24) & 0xFF;

	//(testa >> 24) & 0xFF);
	//dest.put((value >> 16) & 0xFF);
	//dest.put((value >> 8) & 0xFF);
	//dest.put((value) & 0xFF);


	return 0;

	std::cout << "Size of float in bits: " << sizeof(float) * 8 << std::endl;

	AudioFile<float> audioFile;
	audioFile.load("C:/Users/stuar/Downloads/Bass-Drum-1.wav");
	int dataN = audioFile.getNumSamplesPerChannel();

	int newTestSize = 0;
	std::vector<float> spectrumMag;

	FFT_Transform<float>::GetFreqMag(&(audioFile.samples[0]), &spectrumMag, 2550, &newTestSize, false, true);

	printArray<float>(spectrumMag);

	dataToFile::writeArray_raw(&spectrumMag, { 1 }, "C:/Users/stuar", "spectMag9", false);
	std::vector<float> readTest;
	dataToFile::readData_raw("C:/Users/stuar/spectMag9", &readTest);
	std::ofstream myfile;
	myfile.open("C:/Users/stuar/spectmag100.csv", std::ios::trunc);
	for (int i = 0; i < readTest.size(); i++) {
		myfile << readTest[i] << "\n";
	}
	myfile.close();
}