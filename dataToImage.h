#pragma once
#include <vector>
#include <string>
#include "SpectSoundTypes.h"


class dataToImage
{
public:
	static int writeArray(std::vector<float>* in, std::string folder, std::string fileSuffix, int useColorChannels, int byteCutoff);
	static int writeArray_32bmp(std::vector<float>* in, std::string folder, std::string fileSuffix);
	static int readArray(std::vector<float>* out, std::string filePath);
};

