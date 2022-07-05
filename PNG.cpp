#include "PNG.h"
#include "RGL_HSL.h"

namespace graphics {
    PNG::PNG() {
        width = 0;
        height = 0;
        imageData = NULL;
    }

    PNG::PNG(unsigned int width, unsigned int height) {
        width = width;
        height = height;
        imageData = new HSLAPixel[width * height];
    }
}
