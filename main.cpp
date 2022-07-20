#include "PNG.h"
#include "Transform_Image.h"
#include <fstream>
#include <stdint.h>
#include <iostream>
using namespace graphics;



int main() {
	PNG image;
	image.readFromFile("PNG.png");
	
    //grayscale(image);

	image.writeToFile("transforme.png");
    
	


    
}