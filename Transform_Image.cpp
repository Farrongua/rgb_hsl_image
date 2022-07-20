#include "Transform_Image.h"

using namespace graphics;

PNG grayscale(PNG image) {
    for (unsigned x = 0; x < image.width_(); x++) {
        for (unsigned y = 0; y < image.height_(); y++) {
            HSLAPixel& pixel = image.getPixel(x, y);
            pixel.s = 0;
        }
    }

    return image;
}