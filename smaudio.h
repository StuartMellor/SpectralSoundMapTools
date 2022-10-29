#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <bit>
#include <array>
#include <algorithm>

#ifndef RIFFSIZE
#define RIFFSIZE 12
#endif

typedef struct RIFF_header {
	// RIFF Header
	// Offset = 0
	char riff_header[4]; // Contains "RIFF"
	// Offset = 4
	int wav_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
	// Offset = 8
	char wave_header[4]; // Contains "WAVE"
} riff_header;

typedef struct FMT_header {
	// Format Header
	// Offset = 12
	char fmt_header[4]; // Contains "fmt " (includes trailing space)
	// Offset = 
	int fmt_chunk_size; // Should be 16 for PCM
	short audio_format; // Should be 1 for PCM. 3 for IEEE Float
	short num_channels;
	int sample_rate;
	int byte_rate; // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
	short sample_alignment; // num_channels * Bytes Per Sample
	short bit_depth; // Number of bits per sample

	// Data
	char data_header[4]; // Contains "data"
	int data_bytes; // Number of bytes in data. Number of samples * num_channels * sample byte size
	// uint8_t bytes[]; // Remainder of wave file is bytes
} fmt_header;

class smaudio
{
public:
	static int readWavFile(std::string filepath, std::vector<uint32_t>* out, long* sampleRate, long* channels, long* duration, long* bitspersample);
};




