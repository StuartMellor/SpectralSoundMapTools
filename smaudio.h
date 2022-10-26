#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <bit>

#ifndef RIFFSIZE
#define RIFFSIZE 12
#endif

typedef struct wav_header {
    // RIFF Header
    // Offset = 0
    char riff_header[4]; // Contains "RIFF"
    // Offset = 4
    int wav_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    // Offset = 8
    char wave_header[4]; // Contains "WAVE"

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
} wav_header;

template<class T>
class smaudio
{
public:
	static int readWavFile(std::string filepath, std::vector<T>* out, long *sampleRate, long *channels, long *duration, long *bitspersample);
};


template<class T>
inline int smaudio<T>::readWavFile(std::string filepath, std::vector<T>* out, long* sampleRate, long* channels, long* duration, long* bitspersample)
{
    std::ifstream audioFile(filepath.c_str(), std::ios::binary);
    std::cout << "Size of Wavheader: " << sizeof(wav_header) << std::endl;
    const size_t wavHeadSize = sizeof(wav_header);

    char riffHeader[RIFFSIZE];

    // Read riffheader.
    audioFile.read(riffHeader, RIFFSIZE);
    
    // read next three bytes.
    char fmtOrBext[3];
    audioFile.read(fmtOrBext, 3);

    if (fmtOrBext[0] == 'f') {
        // fmt is here
    }
    else {
        // find fmt.
        std::cout << "Wav file header has junk chunk!\nReading till end of junk..." << std::endl;
        
        // Read in first 1000 bytes following first 16 bytes.

        std::cout << "sizeof(int): " << sizeof(int) << std::endl;

        const int initialReadLimit = 3000;
        char data[initialReadLimit];
        audioFile.read(&data[0], initialReadLimit);
        std::string fmtbit(data, initialReadLimit);
        int fmtOffset = fmtbit.find("fmt");

        const int fmtHeaderSize = (sizeof(wav_header) - RIFFSIZE);

        if (fmtOffset < 0 || fmtOffset > initialReadLimit - fmtHeaderSize) {
            std::cout << "fmtOffset not found, ... searching further ... " << std::endl;
            // TODO: Implement read again ...
            return 2;
        }

        std::cout << "fmtOffset: " << fmtOffset << std::endl;
        std::cout << fmtbit.substr(fmtOffset, fmtHeaderSize) << std::endl;

        std::string wholeWavHeaderData(riffHeader, RIFFSIZE);
        wholeWavHeaderData.append(fmtbit.substr(fmtOffset, fmtHeaderSize));

        std::cout << "whole header: {\n" << wholeWavHeaderData << "\n}" << std::endl;

        const char* fmtheader = fmtbit.substr(fmtOffset, fmtHeaderSize).c_str();

        struct wav_header wavehead;
        int offset = 0;
        memcpy(&wavehead.riff_header, &wholeWavHeaderData[offset], sizeof(wavehead.riff_header));
        offset = sizeof(wavehead.riff_header);
        memcpy(&wavehead.wav_size, &wholeWavHeaderData[offset], sizeof(wavehead.wav_size));
        offset += sizeof(wavehead.wav_size);
        memcpy(&wavehead.wave_header, &wholeWavHeaderData[offset], sizeof(wavehead.wave_header));
        offset += sizeof(wavehead.wave_header);
        memcpy(&wavehead.fmt_header, &wholeWavHeaderData[offset], sizeof(wavehead.fmt_header));
        offset += sizeof(wavehead.fmt_header);
        memcpy(&wavehead.fmt_chunk_size, &wholeWavHeaderData[offset], sizeof(wavehead.fmt_chunk_size));
        offset += sizeof(wavehead.fmt_chunk_size);
        memcpy(&wavehead.audio_format, &wholeWavHeaderData[offset], sizeof(wavehead.audio_format));
        offset += sizeof(wavehead.audio_format);

        std::cout << wavehead.riff_header << '\0' << std::endl;
        std::cout << wavehead.wav_size << std::endl;
        std::cout << wavehead.wave_header << std::endl;
        std::cout << wavehead.fmt_header << std::endl;
        std::cout << wavehead.fmt_chunk_size << std::endl;
    }

    return 0;

    // now read rest of data
    char headerbytes[wavHeadSize - RIFFSIZE - 4];
    audioFile.read(headerbytes, wavHeadSize - RIFFSIZE - 4);

    // copy data into wavhead struct
    //memcpy(&wavhead, headerbytes, RIFFSIZE);

    // identify header junk

    

    /*struct wav_header wavhead;
    memcpy(&wavhead, headerbytes, wavHeadSize);
    std::cout << wavhead.riff_header << std::endl;*/
    return 0;
}

