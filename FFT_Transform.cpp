#pragma once
#include "FFT_Transform.h"

#include <algorithm>
#include <iterator>
#include <type_traits>
#include "pocketfft_hdronly.h"
#include <algorithm>
#include <fstream>

using shape_t = std::vector<std::size_t>;
using stride_t = std::vector<std::ptrdiff_t>;

// Private
int FFT_Transform::DToC_transform(std::vector<float>* in, int dataN, std::vector<std::complex<float>>* out, int chunkSize, int overlap, bool DEBUG_MODE) {
	// Double to Complex transform. If chunkSize and overlap are zero, this will spit out
	// a straight up transformation.

	// TODO implement maximum size.
	if (chunkSize) {
		if (overlap) {
			return 0;
		}
		else {
			return 0;
		}
	}
	else {
		// A normal run of the mill full array transformation.
		// May be slow, depending on file size.
		std::cout << in->size() << std::endl;
		shape_t shape{ in->size() };
		stride_t stridef(shape.size());
		stridef[0] = sizeof(float);
		if (shape[0] <= 0) {
			return -1;
		}
		shape_t axes{ 0 };

		if (out->size() <= 0) {
			out->resize(in->size(), 0);
		}

		

		pocketfft::r2c<float>(shape, stridef, stridef, axes, pocketfft::FORWARD, in->data(), out->data(), 1.f);


		if (DEBUG_MODE) {
			std::cout << "Out size: [" << out->size() << "]";
		}
	}
	return 0;
}



// Public

int FFT_Transform::GetFreqMag(std::vector<uint32_t>* in, std::vector<uint32_t>* out, int desiredLength, int* actualLength, bool destructive, bool DEBUG_MODE) {
	// Get Frequency Magnitude Spectrum.
	// Need to implement destructive
	int dataN = in->size();

	if (desiredLength > dataN) {
		// Desired length should be shorter than input length.
		return -1;
	}

	std::vector<std::complex<float>> c_out(dataN);

	size_t N = in->size();
	size_t halfDataN = N / 2;

	std::vector<float> inF(in->size());

	std::ofstream myfile;
	myfile.open("prefloat.csv", std::ios::trunc);
	for (int i = 0; i < in->size(); i++) {
		myfile << (*in)[i] << "\n";
	}
	myfile.close();

	for (int i = 0; i < N; i++) {
		inF[i] = static_cast<float>((*in)[i] / 16777216.0f - 0.5f);
	}

	myfile.open("postfloat.csv", std::ios::trunc);
	for (int i = 0; i < inF.size(); i++) {
		myfile << inF[i] << "\n";
	}
	myfile.close();

	FFT_Transform::DToC_transform(&inF, dataN, &c_out, 0, 0, DEBUG_MODE);

	out->resize(N / 2);
	float res = 0;

	// Discard symmetry
	inF.resize(halfDataN);
	for (int i = 0; i < halfDataN; i++) {
		//res = 
		inF[i] = sqrt(pow(c_out[i].real(), 2) + pow(c_out[i].imag(), 2));
		if (inF[i] < 0) {
			std::cout << c_out[i] << std::endl;
		}
			//(res * 16777216.0f);
	}

	myfile.open("postfft.csv", std::ios::trunc);
	for (int i = 0; i < halfDataN; i++) {
		myfile << inF[i] << "\n";
	}
	myfile.close();

	//std::vector<float> testIn(12);
	//for (int i = 0; i < 12; i++) {
	//	testIn[i] = i;
	//}

	resample_inplace(&inF, desiredLength, SPECTSOUND_DOWNSCALING_ALGORITHM::MOVING_AVG);
	//resample_inplace(&testIn, 4, SPECTSOUND_DOWNSCALING_ALGORITHM::MOVING_AVG);

	out->resize(desiredLength);

	for (int i = 0; i < desiredLength; i++) {
		(*out)[i] = inF[i] * 16777216.0f;
	}

	return 0;
}


int FFT_Transform::resample_inplace(std::vector<float>* in, int length, SPECTSOUND_DOWNSCALING_ALGORITHM SSDA)
{
	if (in->size() < length) {
		return -1;
	}
	if (in->size() == length) {
		return 0;
	}
	// If an array is 3.75 times the size of a given array
	// [1]               [2]             [3]                   [  ]
	// 
	// [1] [2] [3] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [15]
	// 3.75 of the larger array will contribute to each of the other array.
	// i.e. [1] [1] [1] [.75]
	// then             [.25] [1] [1] [1] [.5]
	// then                               [.5] [1] [1] [1] [.25]
	// then                                                [.75] [1] [1] [1]

	// [ 1] [ 2] [ 3]
	// [ 1] [ 2] [ 3] [ 4] [ 5] [ 6] [ 7] [ 8] [  9 ] [10] [11] [12] [13] [14] [15] [16] [17] [ 18 ] [19] [20] [21] [22] [23] [24] [25] [26]
	// [ 1] [ 2] [ 3] [ 4] [ 5] [ 6] [ 7] [ 8] [.66r]
	//                                         [.33r] [ 1] [ 2] [ 3] [ 4] [ 5] [ 6] [ 7] [ 8] [.33r]
	//                                                                                        [.66r] [ 1] [ 2] [ 3] [ 4] [ 5] [ 6] [ 7] [ 8]

	// [---------------------1] 
	//				  [-------------------------------2] 
	//				  |		  |			       [-------------------------------3] 
	//				  |		  |				   |	   |			    [--------------------------4] 
	//				  |		  |				   |	   |				|		|			[-------------------------------5]
	//				  |		  |				   |	   |				|		|			|		|				 [-------------------------------6]
	//				  |		  |				   |	   |				|		|			|		|				 |		 |				  [---------------------7]
	// [ 1] [ 2] [ 3] [      4] [ 5] [ 6] [ 7] [      8] [ 9] [10] [11] [     12] [13] [14] [     15] [16] [17] [18] [     19] [20] [21] [22] [     23] [24] [25] [26]
	// [ 1] [ 2] [ 3] [.714285]                |       |                |       |           |		|				 |		 |				  |		  |				 |
	//                [.285714] [ 1] [ 2] [ 3] [.428571]                |       |           |       |				 |		 |				  |		  |				 |
	//                                         [.571428] [ 1] [ 2] [ 3] [.142857]           |		|				 |		 |				  |		  |				 |
	//                                                                  [.857142] [ 1] [ 2] [.857142]				 |		 |				  |		  |				 |
	//                                                                                      [.142857] [ 1] [ 2] [ 3] [.571428]				  |		  |				 |
	//                                                                                                               [.428571] [ 1] [ 2] [ 3] [.285714]				 |
	//                                                                                                                                        [.714285] [ 1] [ 2] [ 3]
	//                                                                                                      
	float sampleRatio = in->size() / static_cast<float>(length);
	
	std::vector<float> outF(length);


	//std::cout << "outF[" << 0 << "]: " << outF[0] << std::endl;
	long offset = 0;
	long sampsToIterate = floor(sampleRatio);
	float overflow = sampleRatio - sampsToIterate;
	float underflow = 0;

	// Do first whole samples
	for (int i = 0; i < sampsToIterate; ++i) {
		outF[0] += (*in)[i];
	}

	// Add overflow
	outF[0] += (*in)[sampsToIterate] * overflow;
	outF[0] /= sampleRatio;

	underflow = 1 - overflow;
	offset = sampsToIterate;
	
	float temp = 0;

	for (int i = 1; i < length - 1; i++) {
		// Calculate samps to iterate and overflow
		temp = sampleRatio - underflow;
		overflow = temp - floor(temp);
		sampsToIterate = temp - overflow;
		
		// Add underflow
		outF[i] = (*in)[offset] * underflow;

		for (int j = 0; j < sampsToIterate; ++j) {
			outF[i] += (*in)[offset + j + 1];
		}

		// Add overflow
		outF[i] += (*in)[sampsToIterate] * overflow;
		outF[i] /= sampleRatio;

		underflow = 1 - overflow;
		offset += sampsToIterate + 1;
	}

	// Do final;
	temp = sampleRatio - underflow;
	overflow = temp - floor(temp);
	sampsToIterate = temp - overflow;

	// Add underflow
	outF[length - 1] = (*in)[offset] * underflow;

	for (int j = 0; j < sampsToIterate; ++j) {
		outF[length - 1] += (*in)[offset + j + 1];
	}

	outF[length - 1] /= sampleRatio;

	/*for (int i = 0; i < length; i++) {
		std::cout << "outF[" << i << "]: " << outF[i] << "\n";
	}
	std::cout << std::endl;*/
	*in = outF;

	return 0;
}

int FFT_Transform::resample(std::vector<float>* in, std::vector<float>* out, int newSampleRatio, int* actualLength, SPECTSOUND_DOWNSCALING_ALGORITHM SSDA)
{

	return 0;
}
