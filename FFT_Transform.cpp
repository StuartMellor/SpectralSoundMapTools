#pragma once
#include "FFT_Transform.h"

#include <algorithm>
#include <iterator>

#include "pocketfft_hdronly.h"

using shape_t = std::vector<std::size_t>;
using stride_t = std::vector<std::ptrdiff_t>;

// Private
template<typename T>
int FFT_Transform<T>::DToC_transform(std::vector<T>* in, int dataN, std::vector<std::complex<T>>* out, int chunkSize, int overlap, bool DEBUG_MODE) {
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
		stridef[0] = sizeof(T);
		if (shape[0] <= 0) {
			return -1;
		}
		shape_t axes{ 0 };

		if (out->size() <= 0) {
			out->resize(in->size(), 0);
		}
		
		if (std::is_same < T, float>()) {
			pocketfft::r2c<T>(shape, stridef, stridef, axes, pocketfft::FORWARD, in->data(), out->data(), 1.f);
		}
		else if (std::is_same<T, double>()) {
			pocketfft::r2c<T>(shape, stridef, stridef, axes, pocketfft::FORWARD, in->data(), out->data(), 1., 8);
		}
		else if (std::is_same<T, long>()) {
			pocketfft::r2c<T>(shape, stridef, stridef, axes, pocketfft::FORWARD, in->data(), out->data(), 1.L);
		}
		if (DEBUG_MODE) {
			std::cout << "Out size: [" << out->size() << "]";
		}
	}
	return 0;
}
template<typename T>
int FFT_Transform<T>::CToD_transform() {
	// Complex to Double transform.
	return 10;
}

template<typename T>
int FFT_Transform<T>::DToC_general_transform(std::vector<T>* in, int dataN, std::vector<std::complex<T>>* out, int chunkSize, int overlap, bool DEBUG_MODE) {
	return 0;
}


// Public
template<typename T>
int FFT_Transform<T>::GetFreqMag(std::vector<T>* in, std::vector<T>* out, int desiredLength, int* actualLength, bool destructive, bool DEBUG_MODE) {
	// Get Frequency Magnitude Spectrum.
	// Need to implement destructive
	int dataN = in->size();

	if (desiredLength > dataN) {
		// Desired length should be shorter than input length.
		return -1;
	}

	std::vector<std::complex<float>> c_out(dataN);


	FFT_Transform<float>::DToC_transform(in, dataN, &c_out, 0, 0, DEBUG_MODE);
	size_t halfDataN = static_cast<size_t>(ceil(dataN / 2));
	if (out->size() != halfDataN) {
		out->resize(halfDataN);
	}

	for (int i = 0; i < halfDataN; i++) {
		(*out)[i] = sqrt(pow(c_out[i].real(), 2) + pow(c_out[i].imag(), 2));
	}


	if (desiredLength > 0) {
		double sampleRatio = halfDataN / desiredLength;
		int actualLength = 0;
		resample_inplace(out, sampleRatio, &actualLength, SPECTSOUND_DOWNSCALING_ALGORITHM::MOVING_AVG);
	}

	return 0;
}

template<typename T>
int FFT_Transform<T>::resample_inplace(std::vector<T>* in, int newSampleRatio, int* actualLength, SPECTSOUND_DOWNSCALING_ALGORITHM SSDA)
{
	// create a copy.
	// 10
	size_t currentN = in->size();
	//  10 / 3 = 3.333  ----> ceil ----> 4;
	size_t newN = static_cast<size_t>(ceil(currentN / static_cast<T>(newSampleRatio)));
	// 4 * 3 = 12
	size_t overN = newN * newSampleRatio; // overestimate
	// 4 * 2 = 8
	size_t underN = newN * (newSampleRatio - 1);

	// 2 left over that aren't accumulated;
	size_t diffN = overN - currentN;

	std::vector<T> inCopy;

	copy(in->begin(), in->end(), back_inserter(inCopy));

	// Resize input vector
	in->resize(newN);

	// Check in size
	if (in->size() != newN) {
		return -1;
	}

	int upToN = underN;
	if (diffN == 0) {
		upToN = currentN;
	}
	int innerIndex = 0;
	T accum = 0;
	for (int i = 0; i < upToN; i++) {
		accum += inCopy[i];
		if (i % newSampleRatio == 0 && i != 0) {
			innerIndex += 1;
			accum /= newSampleRatio;
			(*in)[innerIndex] = accum;
			accum = 0;
		}
	}
	
	if (diffN != 0) {
		accum = 0;
		for (int i = underN; i < currentN; i++) {
			accum += inCopy[i];
		}
		(*in)[newN - 1] = accum / diffN;
	}


	return 0;
}

template<typename T>
int FFT_Transform<T>::resample(std::vector<T>* in, std::vector<T>* out, int newSampleRatio, int* actualLength, SPECTSOUND_DOWNSCALING_ALGORITHM SSDA)
{

	return 0;
}
