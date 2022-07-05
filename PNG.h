#pragma once

#include <string>
#include <vector>
#include "HSLA_Pixel.h"


namespace graphics {
	class PNG {
	private:
		unsigned int width;
		unsigned int height;
		HSLAPixel* imageData;
		HSLAPixel defaultPixel;

	public:
		//Create empty PNG
		PNG();
		//Create a PNG image of the specified dimensions
		PNG(unsigned int width, unsigned int heigth);

	};
}