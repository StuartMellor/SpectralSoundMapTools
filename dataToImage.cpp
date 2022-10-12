#include "dataToImage.h"
#include <fstream>
#define TINY_DNG_WRITER_IMPLEMENTATION
#include "tiny_dng_writer.h"

#include "rgba_bitmap.h"


int dataToImage::writeArray(std::vector<float>* in, std::string folder, std::string fileSuffix, int useColorChannels, int byteCutoff)
{
	// 1 2 3 4
	// 

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

int dataToImage::writeArray_32bmp(std::vector<float>* in, std::string folder, std::string fileSuffix)
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


	return 0;
}
