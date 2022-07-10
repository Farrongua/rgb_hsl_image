
#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <winsock.h>
#include <errno.h>
#include "PNG.h"
#include "RGL_HSL.h"

using namespace std;



//Stoen from http://www.libpng.org/pub/png/spec/1.2/PNG-CRCAppendix.html
//----------------------------------------------------------------------
/* Table of CRCs of all 8-bit messages. */
unsigned long crc_table[256];

/* Flag: has the table been computed? Initially false. */
int crc_table_computed = 0;

/* Make the table for a fast CRC. */
void make_crc_table(void)
{
    unsigned long c;
    int n, k;

    for (n = 0; n < 256; n++) {
        c = (unsigned long)n;
        for (k = 0; k < 8; k++) {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table[n] = c;
    }
    crc_table_computed = 1;
}

/* Update a running CRC with the bytes buf[0..len-1]--the CRC
   should be initialized to all 1's, and the transmitted value
   is the 1's complement of the final running CRC (see the
   crc() routine below)). */

unsigned long update_crc(unsigned long crc, unsigned char* buf,
    int len)
{
    unsigned long c = crc;
    int n;

    if (!crc_table_computed)
        make_crc_table();
    for (n = 0; n < len; n++) {
        c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    }
    return c;
}

/* Return the CRC of the bytes buf[0..len-1]. */
unsigned long crc(unsigned char* buf, int len)
{
    return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}
//----------------------------------------------------------------------


//const uint8_t png_file_sig[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };


void read_buf(std::ifstream &input_file, void* buf, size_t buf_size) {

    input_file.read(reinterpret_cast<char*>(buf), buf_size);

}

void print_bytes(uint8_t* buf, size_t buf_size) {
    for (size_t i = 0; i < buf_size; i++) {
        printf("%u ", buf[i]);
    }
    printf("\n");
}

void reverse_bytes(void* buf, size_t buf_size) {
    uint8_t* buffer = reinterpret_cast<uint8_t*>(buf);
    uint8_t tmp;
    for (size_t i = 0; i < buf_size / 2; i++) {
        tmp = buffer[i];
        buffer[i] = buffer[buf_size - i - 1];
        buffer[buf_size - i - 1] = tmp;
    }
}



namespace graphics {
    PNG::PNG() {
        width = 0;
        height = 0;
        imageData = NULL;
    }

    PNG::PNG(unsigned int width_, unsigned int height_) {
        width = width_;
        height = height_;
        imageData = new HSLAPixel[width * height];
    }


    void CHUNK::print_chunk() {
        std::cout << "Chunk lenght : ";
        printf("%u \n", length);
        std::cout << "Chunk type : ";
        printf("%.*s \n", sizeof(chunk_type), chunk_type);
        std::cout << "Chunk data : ";
        print_bytes(data, length);
        std::cout << "Chunk CRC : ";
        printf("0x%08X\n|---------------------------------------------------|\n\n", crc);
    }

    void PNG::readFromFile(const char* filename) {
        delete[] imageData;

        make_crc_table();

        FILE* input = fopen(filename, "rb");

        std::ifstream input_file;

        input_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            input_file.open(filename, ios::binary);
        }
        catch (std::system_error& e) {
            std::cerr << e.code().message() << std::endl;
        }
        uint8_t png_sig[8];
        //unsigned char size_buf[8];  
        
        read_buf(input_file, png_sig, 8);
        
        if (memcmp(png_sig, png_file_sig, 8) != 0) {
            //error
            exit(1);
        }
        printf("Signature PNG : ");
        print_bytes(png_sig, sizeof(png_sig));
        //CHUNK ch;
        bool quit = false;
        while (!quit) {

            uint32_t chunk_size;
            read_buf(input_file, &chunk_size, sizeof(chunk_size));
            reverse_bytes(&chunk_size, sizeof(chunk_size));
            

            uint8_t chunk_type[4];
            read_buf(input_file, &chunk_type, sizeof(chunk_type));

            if (*(uint32_t*)chunk_type == 0x444E4549)
                quit = true;


            
            input_file.seekg(chunk_size, input_file.cur);
            int tell = input_file.tellg();
            if ( tell < 0 ) {
                exit(1);
            }
            

           /* uint8_t* sz_buf = new uint8_t[chunk_size];
            read_buf(input_file, sz_buf, chunk_size);
            printf("Chunk's data : ");
            print_bytes(sz_buf, chunk_size);*/



            uint32_t chunk_crc;
            read_buf(input_file, &chunk_crc, sizeof(chunk_crc));

            /*CHUNK chunk(chunk_size, chunk_type, sz_buf, chunk_crc);
            chunk.print_chunk();*/

            /*delete[] sz_buf;*/

            printf("Chunk size : %u \n", chunk_size);
            printf("Chunk type : %.*s (0x%08X)\n", (int)sizeof(chunk_type), chunk_type, *(uint32_t*)chunk_type);
            printf("Chunk CRC : 0x%08X\n|---------------------------------------------------|\n\n", chunk_crc);
            
        }
        /*uint8_t what[2];
        read_buf(input_file, what, sizeof(what));
        if (what[1] == 6)
            printf("ITS RGBA COLOR TYPE AND WE HAVE %.lf COLORS\n", pow(2, (int)what[0]));
        print_bytes(what, sizeof(what));*/


    }
    void PNG::writeToFile(string const& filename) {

    }



    unsigned int PNG::width_() const { return width; }
    unsigned int PNG::height_() const { return height; }

    HSLAPixel& PNG::getPixel(unsigned int x, unsigned int y) const {

        if (x == 0 || y == 0) {
            //error
        }
        if (x > width) {
            //error
        }
        if (y > height) {
            //error
        }
        
        unsigned index = x + (y * width);
        return imageData[index];
    }



    //void PNG::readFromFile(string const& filename) {

    //    delete[] imageData;

    //    fstream image;
    //    image.open(filename);
    //    string type = "", width_ = "", height_ = "", RGB = "";
    //    image >> type;
    //    image >> width_;
    //    image >> height_;
    //    image >> RGB;

    //    width = stoi(width_);
    //    height = stoi(height_);
    //    imageData = new HSLAPixel[width * height];

    //    vector<unsigned char> byteData;
    //    string red = "", green = "", blue = "", alpha = "";
    //    unsigned r = 0, g = 0, b = 0, a = 0;
    //    unsigned i = 0;
    //    while (!image.eof()) {

    //        image >> red;
    //        image >> green;
    //        image >> blue;
    //        //image >> alpha;

    //        stringstream redstream(red);
    //        stringstream greenstream(green);
    //        stringstream bluestream(blue);
    //        //stringstream alphastream(alpha);

    //        redstream >> r;
    //        greenstream >> g;
    //        bluestream >> b;
    //        //alphastream >> a;

    //        rgbaColor rgb;
    //        rgb.r = r;
    //        rgb.g = g;
    //        rgb.b = b;
    //        rgb.a = a;

    //        hslaColor hsl = RGB_TO_HSL(rgb);
    //        HSLAPixel& pixel = imageData[i / 4];
    //        pixel.h = hsl.h;
    //        pixel.s = hsl.s;
    //        pixel.l = hsl.l;
    //        pixel.a = hsl.a;
    //        i += 4;
    //    }



    //    image.close();



    //}

    //void PNG::writeToFile(string const& filename) {
    //    unsigned char* byteData = new unsigned char[width * height * 4];

    //    ofstream image(filename);
    //    image << "P3" << endl;
    //    image << width << " " << height << endl;
    //    image << "255" << endl;

    //    for (unsigned i = 0; i < width * height; i++) {
    //        hslaColor hsl;
    //        hsl.h = imageData[i].h;
    //        hsl.s = imageData[i].s;
    //        hsl.l = imageData[i].l;
    //        hsl.a = imageData[i].a;

    //        rgbaColor rgb = HSL_TO_RBG(hsl);

    //        byteData[(i * 4)] = rgb.r;
    //        byteData[(i * 4) + 1] = rgb.g;
    //        byteData[(i * 4) + 2] = rgb.b;
    //        byteData[(i * 4) + 3] = rgb.a;

    //        image << to_string(rgb.r) << " " << to_string(rgb.g) << " "
    //            << to_string(rgb.b) << " ";       //<< to_string(rgb.a) << " ";


    //    }
    //}




}
