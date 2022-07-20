#pragma once

#include <string>
#include <vector>
#include "HSLA_Pixel.h"

using namespace std;

namespace graphics {

	class Chunk {
	public:
		uint32_t length, size;
		uint8_t* type;
		uint8_t* data;
		uint32_t crc;

		

		Chunk() : length(0), size(0), type(nullptr), data(nullptr), crc(0) {};
		Chunk(uint32_t len, uint32_t size, uint8_t* _type, uint8_t* _data, uint32_t crc)
			: length(len), size(size), data(data), crc(crc) 
		{
			type = new uint8_t[4];
			for (size_t i = 0; i < 4; i++) {
				type[i] = _type[i];
			}
			data = new uint8_t[size];
			for (size_t i = 0; i < size; i++) {
				data[i] = _data[i];
			}
		};
		//uint32_t& length() { return length; }


		void print_chunk();
	};


	class PNG {
	private:
		uint8_t png_file_sig[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };

		vector<Chunk> chunks;
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