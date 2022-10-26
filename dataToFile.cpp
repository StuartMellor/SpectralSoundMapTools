#include "dataToFile.h"
#include <fstream>
#define TINY_DNG_WRITER_IMPLEMENTATION
#include "tiny_dng_writer.h"
#include <string>
#include "rgba_bitmap.h"
#include "base64.h"


template<typename T>
void printArray(std::vector<T> input) {
	int N = input.size();
	if (N <= 0) {
		return;
	}
	std::cout << "Array: [";
	for (int i = 0; i < N - 1; i++) {
		std::cout << static_cast<int>(input[i]) << ", ";
	}
	std::cout << input[N - 1] << "];" << std::endl;
}

void printArray(const unsigned char* input, int N) {
	if (N <= 0) {
		return;
	}
	std::cout << "Array: [";
	for (int i = 0; i < N - 1; i++) {
		std::cout << static_cast<int>(input[i]) << ", ";
	}
	std::cout << input[N - 1] << "];" << std::endl;
}

void printArray(const char* input, int N) {
	if (N <= 0) {
		return;
	}
	std::cout << "Array: [";
	for (int i = 0; i < N - 1; i++) {
		std::cout << static_cast<int>(input[i]) << ", ";
	}
	std::cout << input[N - 1] << "];" << std::endl;
}



int dataToFile::writeArray(std::vector<float>* in, std::string folder, std::string fileSuffix, int useColorChannels, int byteCutoff)
{
	// NEEDS WORK - Avoid using...
	// As shaders have access to a single pixel at a time,
	// Data would be spread across multiple - Floats contain 4 bytes and RGB is 3 bytes.
	// TBH, as I'm focusing on FFT data, I can probably submit to a lower bit depth for each floating point.
	// These things need thinking about.
	// One way to combat this would be to load the texture twice and offset the other
	// texture by 1 pixel across.. but as you can see, you would also need to have one shifted down
	// when the overlap wraps around. You could specify for this algorithm to m
	float N = in->size();

	// 10 floats = 40 bits
	int floatSize = sizeof(N);
	int pixels = ceil((N * floatSize) / static_cast<double>(useColorChannels));
	std::cout << "Total pixels needed: " << pixels << std::endl;
	int widthHeight = static_cast<int>(floor(sqrt(pixels)));

	int total = widthHeight * widthHeight;
	int leftOver = pixels - total;
	int extraColumns = static_cast<int>(ceil(leftOver / widthHeight));
	int actualWidth = widthHeight + extraColumns;
	total += extraColumns * widthHeight;

	std::cout << "LeftOver: " << leftOver << std::endl;

	std::cout << "Creating image with width and height: " << widthHeight << std::endl;


	tinydngwriter::DNGImage dngImage;
	dngImage.SetBigEndian(false);
	dngImage.SetSubfileType(false, false, false);
	dngImage.SetImageWidth(actualWidth);
	dngImage.SetImageLength(widthHeight);
	dngImage.SetRowsPerStrip(widthHeight);
	dngImage.SetSamplesPerPixel(3);
	uint16_t bps[3] = { 16, 16, 16 };
	dngImage.SetBitsPerSample(3, bps);
	dngImage.SetPlanarConfig(tinydngwriter::PLANARCONFIG_CONTIG);
	dngImage.SetCompression(tinydngwriter::COMPRESSION_NONE);
	dngImage.SetPhotometric(tinydngwriter::PHOTOMETRIC_RGB);
	dngImage.SetXResolution(1.0);
	dngImage.SetYResolution(1.0); // fractioal test
	dngImage.SetResolutionUnit(tinydngwriter::RESUNIT_NONE);
	dngImage.SetImageDescription("bora");

	unsigned char* image = (unsigned char*)malloc(total);
	int floatIndex = 0;
	int totalCheck = 0;
	int floatInnerIndex = 0;
	// Gonna go with little endian;
	unsigned char* initialFloat = reinterpret_cast<unsigned char*>(&(*in)[0]);

	// Kick off the last one
	int NtoUse = static_cast<int>(floor(N / useColorChannels) * useColorChannels);
	for (int i = 0; i < NtoUse; i++) {
		//char[
	}

	/* for (int i = 0; i < total; i++) {
		 image[i] = initialFloat[floatInnerIndex];
		 floatInnerIndex += 1;
		 if (floatInnerIndex >= floatSize && i != 0) {
			 floatIndex += 1;
			 floatInnerIndex = 0;
			 initialFloat = reinterpret_cast<unsigned char*>(&(*in)[floatIndex]);
		 }
	 }*/

	std::cout << "Final Float Index: " << floatIndex << std::endl;
	std::cout << "Initial N: " << N << std::endl;

	dngImage.SetImageData(image, total * sizeof(unsigned char));

	tinydngwriter::DNGWriter dng_writer(false);
	bool ret = dng_writer.AddImage(&dngImage);

	std::cout << "ret - Add Image: " << ret << std::endl;
	assert(ret);

	std::string err;

	std::string output_filename = folder + "/" + fileSuffix + ".tiff";
	ret = dng_writer.WriteToFile(output_filename.c_str(), &err);
	std::cout << "err - WriteToFile: " << err << std::endl;
	std::cout << "ret - Add Image: " << ret << std::endl;

	if (!err.empty()) {
		std::cerr << err;
	}

	return 0;
}

int dataToFile::writeArray_32bmp(std::vector<float>* in, std::string folder, std::string fileSuffix)
{

	float N = in->size();

	int widthHeight = static_cast<int>(floor(sqrt(N)));

	int total = widthHeight * widthHeight;
	int leftOver = N - total;
	int extraColumns = static_cast<int>(ceil(leftOver / static_cast<double>(widthHeight)));
	int actualWidth = widthHeight + extraColumns;
	total += extraColumns * widthHeight;

	std::cout << "\nLeftOver: " << leftOver << std::endl;

	std::cout << "Creating image with width: " << actualWidth << " and height: " << widthHeight << std::endl;

	// resize input to match total
	in->resize(total);
	const unsigned char* bitmap = reinterpret_cast<unsigned char*>(&((*in)[0]));
	size_t bitmap_size = 0;
	const char* bitmap_file_data = reinterpret_cast<const char*>(rgba_encode_bitmap_to_file_data(bitmap, actualWidth, widthHeight, 0, bitmap_buffer_format::bitmap_buffer_format_RGBA, &bitmap_size));

	std::ofstream bmp_file("C:/Users/stuar/Documents/test.rgba", std::ios::out | std::ios::binary);
	bmp_file.write(bitmap_file_data, bitmap_size);
	bmp_file.close();

	return 0;
}


int dataToFile::writeArray_raw(std::vector<float>* in, std::string folder, std::string fileSuffix, bool base64) {
	float N = in->size();

	int widthHeight = static_cast<int>(floor(sqrt(N)));

	size_t total = widthHeight * widthHeight;
	int leftOver = N - total;
	int extraColumns = static_cast<int>(ceil(leftOver / static_cast<double>(widthHeight)));
	int actualWidth = widthHeight + extraColumns;
	total += extraColumns * widthHeight;

	std::cout << "\nLeftOver: " << leftOver << std::endl;

	std::cout << "Creating image with width: " << actualWidth << " and height: " << widthHeight << std::endl;

	// resize input to match total
	in->resize(total);
	

	if (base64) {
		std::cout << "in.size() = " << in->size() << std::endl;
		const unsigned char* bitmap = reinterpret_cast<unsigned char*>(&((*in)[0]));

		printArray(bitmap, in->size() * sizeof(unsigned char));
		std::string out = base64_encode(bitmap, in->size() * 4);

		std::cout << out << std::endl;

		std::vector<BYTE> testData = base64_decode(out);
		std::vector<float> backToFloatTest;
		printArray(testData);
		for (int i = 0; i < testData.size() - 1; i += 4) {
			BYTE toFloaty[4] = { testData[i], testData[i + 1], testData[i + 2], testData[i + 3] };
			backToFloatTest.push_back(*(float*)toFloaty);
		}

		printArray(backToFloatTest);
		std::cout << "TestData.size: " << testData.size() << std::endl;

		std::string filePath = folder + "/" + fileSuffix;
		std::ofstream rawFile(filePath, std::ios::out | std::ios::binary);
		rawFile.write(out.data() , in->size() * 4);
		rawFile.close();
	}
	else {
		const unsigned char* bitmap = reinterpret_cast<unsigned char*>(&((*in)[0]));
		std::cout << bitmap << std::endl;
		printArray(bitmap, in->size() * 4);
		std::string filePath = folder + "/" + fileSuffix;
		std::ofstream rawFile(filePath, std::ios::out | std::ios::binary);
		rawFile.write((char*)&bitmap[0], in->size() * 4);
		rawFile.close();
	}
	return 0;
}

std::ostream&
output32BitUInt(std::ostream& dest, uint32_t value)
{
	dest.put((value >> 24) & 0xFF);
	dest.put((value >> 16) & 0xFF);
	dest.put((value >> 8) & 0xFF);
	dest.put((value) & 0xFF);
	return dest;
}

int dataToFile::writeArray_raw(std::vector<float>* in, std::vector<int> dims, std::string folder, std::string fileSuffix, bool base64) {
	if (dims.size() == 1) {
		// Just write array 1 x N
		
		//std::cout << bitmap << std::endl;
		////printArray(bitmap, in->size() * 4);
		//std::string filePath = folder + "/" + fileSuffix;
		//std::ofstream rawFile(filePath, std::ios::out | std::ios::binary);
		//rawFile.write((char*)&bitmap[0], in->size() * 4);
		//rawFile.close();
	}
	return 0;
}

float DecodeFloatRGBA(int enc[4])
{
	float kDecodeDot[4] = { 1.0, 1 / 255.0, 1 / 65025.0, 1 / 160581375.0 };
	//float4 kDecodeDot = float4(1 / 160581375.0, 1 / 65025.0, 1 / 255.0, 1.0);
	float out = 0;
	for (int i = 0; i < 4; i++) {
		out += kDecodeDot[i] * (double) enc[i];
	}
	return out;
}


void dataToFile::readData_raw(std::string filepath, std::vector<float>* in) {
	std::ifstream inFile(filepath, std::ios::binary);
	std::vector<char> bytes(
		(std::istreambuf_iterator<char>(inFile)),
		(std::istreambuf_iterator<char>()));

	inFile.close();
	std::cout << "Bytees: " << std::endl;
	//printArray(bytes);
	
	int bytesForFloat[4] = { 0, 0, 0, 0 };
	in->resize(bytes.size() / 4);
	std::cout << "Bytes Count: " << bytes.size() << std::endl;
	float temp = 0;
	
	for (int i = 0; i < bytes.size() - 1; i += 4) {
		float temp = bytes[i + 3] + bytes[i + 2] ;
		//const unsigned char tempchar[4] = { bytes[i], bytes[i + 1], bytes[i + 2], bytes[i + 3] };
		//if (i == 4) {
		//	std::cout << "tempchar: " << (int)tempchar[0] << ", " << (int)tempchar[1] << ", " << (int)tempchar[2] << ", " << (int)tempchar[3] << std::endl;
		//}
		//memcpy(&temp, tempchar, sizeof(float));
		(*in)[i / 4] = temp;
	}
	std::cout << "\nBytes End!" << std::endl;
	
	//printArray(*in);
}