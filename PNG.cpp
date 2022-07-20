
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

void write_buf(std::ofstream& output_file, void* buf, size_t buf_size) {
    output_file.write(reinterpret_cast<char*>(buf), buf_size);
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


    void Chunk::print_chunk() {
        printf("Chunk size : %u \n", size);
        printf("Chunk type : %.*s (0x%08X)\n", (int)sizeof(type), type, *(uint32_t*)type);
        printf("Chunk CRC : 0x%08X\n|---------------------------------------------------|\n\n", crc);
    }


    void PNG::readFromFile(const char* filename) {
        delete[] imageData;

        make_crc_table();

        std::ifstream input_file;
        input_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            input_file.open(filename, ios::binary);
        }
        catch (std::system_error& e) {
            std::cerr << e.code().message() << std::endl;
            exit(1);
        }

        uint8_t png_sig[8]; 
        read_buf(input_file, png_sig, 8);
        
        if (memcmp(png_sig, png_file_sig, 8) != 0) {
            //error
            exit(1);
        }
        printf("Signature PNG : ");
        print_bytes(png_sig, sizeof(png_sig));
        bool quit = false;
        while (!quit) {

            uint32_t chunk_size, correct_size;
            read_buf(input_file, &chunk_size, sizeof(chunk_size));
            correct_size = chunk_size;
            reverse_bytes(&correct_size, sizeof(correct_size));

            uint8_t chunk_type[4];
            read_buf(input_file, &chunk_type, sizeof(chunk_type));      
            if (*(uint32_t*)chunk_type == 0x444E4549)
                quit = true;   
            uint8_t* chunk_data = new uint8_t[correct_size];
            read_buf(input_file, chunk_data, correct_size);
            uint32_t chunk_crc;
            read_buf(input_file, &chunk_crc, sizeof(chunk_crc));

            chunks.push_back(Chunk(chunk_size, correct_size, chunk_type, chunk_data, chunk_crc));
           
            delete[] chunk_data;
        }
        for (size_t i = 0; i < chunks.size(); i++) {
            chunks[i].print_chunk();
        }
        input_file.close();
    }

    void PNG::writeToFile(string const& filename) {
        make_crc_table();

        std::ofstream output_file;
        output_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            output_file.open(filename, ios::out | ios::binary);
        }
        catch (std::system_error& e) {
            std::cerr << e.code().message() << std::endl;
        }

        output_file.write(reinterpret_cast<char*>(png_file_sig), 8);

        for (size_t i = 0; i < chunks.size(); i++) {
            output_file.write(reinterpret_cast<char*>(&chunks[i].length), sizeof(chunks[i].length));
            output_file.write(reinterpret_cast<char*>(chunks[i].type), sizeof(chunks[i].type));
            output_file.write(reinterpret_cast<char*>(chunks[i].data), chunks[i].size);
            output_file.write(reinterpret_cast<char*>(&chunks[i].crc), sizeof(chunks[i].crc));
        }
        write_buf(output_file, png_file_sig, 8);
        output_file.close();

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


}