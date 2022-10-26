#pragma once
#include <vector>
#include <string>
#include "SpectSoundTypes.h"


class dataToFile
{
public:
	static int writeArray(std::vector<float>* in, std::string folder, std::string fileSuffix, int useColorChannels, int byteCutoff);
	static int writeArray_raw(std::vector<float>* in, std::string folder, std::string fileSuffix, bool base64);
	static int writeArray_raw(std::vector<float>* in, std::vector<int> dims, std::string folder, std::string fileSuffix, bool base64);
	static void readData_raw(std::string filepath, std::vector<float>* in);
	static int writeArray_raw(std::vector<float>* in, int dims[], std::string folder, std::string fileSuffix, bool base64);
	static int writeArray_32bmp(std::vector<float>* in, std::string folder, std::string fileSuffix);
	static int readArray(std::vector<float>* out, std::string filePath);
};

