#pragma once

#include <string>
#include <vector>
#include "HSLA_Pixel.h"

using namespace std;

namespace graphics {

	class CHUNK {
	public:
		uint32_t length;
		uint8_t* chunk_type;
		uint8_t* data;
		uint32_t crc;

		

		//CHUNK() : length(0), chunk_type({}), data({}) {};
		CHUNK(uint32_t len, uint8_t* type, uint8_t* data, uint32_t crc) : length(len), chunk_type(type), data(data), crc(crc) {};
		//uint32_t& length() { return length; }


		void print_chunk();
	};


	class PNG {
	private:
		const uint8_t png_file_sig[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };

		//vector<CHUNK> chunks;
		unsigned int width;
		unsigned int height;

		HSLAPixel* imageData;
		HSLAPixel defaultPixel;

	public:
		//Create empty PNG
		PNG();
		//Create a PNG image of the specified dimensions
		PNG(unsigned int width_, unsigned int heigth_);


		void readFromFile(const char* filename);

		void writeToFile(string const& filename);

		unsigned int width_() const;
		unsigned int height_() const;

		HSLAPixel& getPixel(unsigned int x, unsigned int y) const;
	};


	

}