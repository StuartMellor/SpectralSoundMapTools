#include "smaudio.h"


////
int smaudio::readWavFile(std::string filepath, std::vector<uint32_t>* out, long* sampleRate, long* channels, long* duration, long* bitspersample)
{
	std::ifstream audioFile(filepath.c_str(), std::ios::binary);
	riff_header riffdata;
	audioFile.read(reinterpret_cast<char*>(&riffdata), sizeof(riffdata));
	// Now find fmt

	std::string twobytes;

	std::array<char, 3> fmtseq = { 102, 109, 116 };
	auto res = std::search(std::istream_iterator<char>(audioFile), std::istream_iterator<char>(), fmtseq.begin(), fmtseq.end());

	const std::streampos pos(audioFile.tellg() - static_cast<std::streamoff>(5));
	std::cout << (int)pos << std::endl;

	std::cout << "position: " << audioFile.tellg() << std::endl;
	audioFile.seekg(pos, std::ios::beg);
	std::cout << "position: " << audioFile.tellg() << std::endl;

	fmt_header fmtData;
	audioFile.read(reinterpret_cast<char*>(&fmtData), sizeof(fmtData));
	std::cout << "channels: " << fmtData.num_channels << std::endl;
	std::cout << "sample_rate: " << fmtData.sample_rate << std::endl;
	std::cout << "byte_rate: " << fmtData.byte_rate << std::endl;
	std::cout << "bit_depth: " << fmtData.bit_depth << std::endl;
	std::cout << "data_bytes: " << fmtData.data_bytes << std::endl;
	std::cout << "sample_alignment: " << fmtData.sample_alignment << std::endl;
	std::cout << "audio_format: " << fmtData.audio_format << std::endl;


	// Now the rest of the file should be bytes:
	// lets read them...
	const int testN = 480000;
	const int NBytes = (fmtData.bit_depth / 8);
	const int testNBytes = NBytes * testN;

	uint8_t* bytes = new uint8_t[testNBytes];
	std::cout << "file pos before read: " << audioFile.tellg() << std::endl;
	audioFile.read(reinterpret_cast<char*>(&bytes[0]), testNBytes);
	std::cout << "file pos after read: " << audioFile.tellg() << std::endl;

	if (out->size() < testN) {
		out->resize(testN);
	}

	// Need to add some error catching!
	int outByteIndex = 0;
	for (int i = 0; i < testNBytes; i += NBytes) {
		// WARNING IF IT'S A 32 BIT OR 16 BIT AUDIO FILE, THIS NEEDS THE FOLLOWING TWO LINES NEED TO ACCOUNT FOR 
		// 4 OR 2 BYTES, RESPECTIVELY... I.E. GO UP TO i + 3 OR ONLY GO UP TO i + 1;
		outByteIndex = i / NBytes;
		(*out)[outByteIndex] = ((uint32_t)bytes[i + 2] << 16) + ((uint32_t)bytes[i + 1] << 8) + ((uint32_t)bytes[i]);

		if ((*out)[outByteIndex] >= 8388608) {
			(*out)[outByteIndex] -= 8388606;
		}
		else {
			(*out)[outByteIndex] += 8388606;
		}
	}

	delete[] bytes;
	std::cout << "Finished loading file - lol!" << std::endl;
	return 0;
}