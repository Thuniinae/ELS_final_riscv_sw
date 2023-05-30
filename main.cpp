#include "string"
#include "string.h"
#include "cassert"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "map.h"
#include <cmath>
#include <iostream>
using namespace std;

unsigned char header[54] = {
    0x42,          // identity : B
    0x4d,          // identity : M
    0,    0, 0, 0, // file size
    0,    0,       // reserved1
    0,    0,       // reserved2
    54,   0, 0, 0, // RGB data offset
    40,   0, 0, 0, // struct BITMAPINFOHEADER size
    0,    0, 0, 0, // bmp width
    0,    0, 0, 0, // bmp height
    1,    0,       // planes
    24,   0,       // bit per pixel
    0,    0, 0, 0, // compression
    0,    0, 0, 0, // data size
    0,    0, 0, 0, // h resolution
    0,    0, 0, 0, // v resolution
    0,    0, 0, 0, // used colors
    0,    0, 0, 0  // important colors
};

unsigned int input_rgb_raw_data_offset;
const unsigned int output_rgb_raw_data_offset=54;
int width;
int height;
unsigned int width_bytes;
unsigned char bits_per_pixel;
unsigned short bytes_per_pixel;
unsigned char *source_bitmap;
unsigned char *target_bitmap;
const int WHITE = 255;
const int BLACK = 0;
const int THRESHOLD = 90;

bool _is_using_dma = true;
int read_bmp(std::string infile_name) {
  FILE *fp_s = NULL; // source file handler
  fp_s = fopen(infile_name.c_str(), "rb");
  if (fp_s == NULL) {
    printf("fopen %s error\n", infile_name.c_str());
    return -1;
  }
  // move offset to 10 to find rgb raw data offset
  fseek(fp_s, 10, SEEK_SET);
  assert(fread(&input_rgb_raw_data_offset, sizeof(unsigned int), 1, fp_s));

  // move offset to 18 to get width & height;
  fseek(fp_s, 18, SEEK_SET);
  assert(fread(&width, sizeof(unsigned int), 1, fp_s));
  assert(fread(&height, sizeof(unsigned int), 1, fp_s));

  // get bit per pixel
  fseek(fp_s, 28, SEEK_SET);
  assert(fread(&bits_per_pixel, sizeof(unsigned short), 1, fp_s));
  bytes_per_pixel = bits_per_pixel / 8;

  // move offset to input_rgb_raw_data_offset to get RGB raw data
  fseek(fp_s, input_rgb_raw_data_offset, SEEK_SET);

  source_bitmap =
      (unsigned char *)malloc((size_t)width * height * bytes_per_pixel);
  if (source_bitmap == NULL) {
    printf("malloc images_s error\n");
    return -1;
  }

  target_bitmap =
      (unsigned char *)malloc((size_t)width * height * bytes_per_pixel);
  if (target_bitmap == NULL) {
    printf("malloc target_bitmap error\n");
    return -1;
  }

  assert(fread(source_bitmap, sizeof(unsigned char),
               (size_t)(long)width * height * bytes_per_pixel, fp_s));
  fclose(fp_s);

  unsigned int file_size; // file size
  // file size
  file_size = width * height * bytes_per_pixel + output_rgb_raw_data_offset;
  header[2] = (unsigned char)(file_size & 0x000000ff);
  header[3] = (file_size >> 8) & 0x000000ff;
  header[4] = (file_size >> 16) & 0x000000ff;
  header[5] = (file_size >> 24) & 0x000000ff;

  // width
  header[18] = width & 0x000000ff;
  header[19] = (width >> 8) & 0x000000ff;
  header[20] = (width >> 16) & 0x000000ff;
  header[21] = (width >> 24) & 0x000000ff;

  // height
  header[22] = height & 0x000000ff;
  header[23] = (height >> 8) & 0x000000ff;
  header[24] = (height >> 16) & 0x000000ff;
  header[25] = (height >> 24) & 0x000000ff;

  // bit per pixel
  header[28] = bits_per_pixel;

  return 0;
}

int write_bmp(std::string outfile_name) {
  FILE *fp_t = NULL; // target file handler

  fp_t = fopen(outfile_name.c_str(), "wb");
  if (fp_t == NULL) {
    printf("fopen %s error\n", outfile_name.c_str());
    return -1;
  }

  // write header
  fwrite(header, sizeof(unsigned char), output_rgb_raw_data_offset, fp_t);

  // write image
  fwrite(target_bitmap, sizeof(unsigned char),
         (size_t)(long)width * height * bytes_per_pixel, fp_t);

  fclose(fp_t);
  return 0;
}

void write_data_to_ACC(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){  
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(buffer);
    *DMA_DST_ADDR = (uint32_t)(ADDR);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
  }else{
    // Directly Send
    memcpy(ADDR, buffer, sizeof(unsigned char)*len);
  }
}
void read_data_from_ACC(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(ADDR);
    *DMA_DST_ADDR = (uint32_t)(buffer);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
  }else{
    // Directly Read
    memcpy(buffer, ADDR, sizeof(unsigned char)*len);
  }
}

void write(word buffer, int channel) {
  switch(channel){
    case 1:
      write_data_to_ACC(GD_W_RGB_ADDR, buffer.uc, 4);
      break;
    case 2:
      write_data_to_ACC(NM_W_RGB_ADDR, buffer.uc, 4);
      break;
    default:
      cout << "unknown rgb channel!" << endl;
      break;
  }
}

word pixel(unsigned int i, unsigned int j) {
  unsigned char R, G, B;      // color of R, G, B
  if (i >= 0 && i < width && j >= 0 && j < height) {
    R = *(source_bitmap +
          bytes_per_pixel * (width * j + i) + 2);
    G = *(source_bitmap +
          bytes_per_pixel * (width * j + i) + 1);
    B = *(source_bitmap +
          bytes_per_pixel * (width * j + i) + 0);
  } else {
    std::cout << "caution, pixel out of range" << std::endl;
    R = 0;
    G = 0;
    B = 0;
  }
  word buffer;
  buffer.uc[0] = R;
  buffer.uc[1] = G;
  buffer.uc[2] = B;
  return buffer;
}

void write_mean(mean_t buffer, int channel){
  // mean with length rgb(4 bytes) * 8 = 32 bytes
  switch(channel){
    case 1:
      write_data_to_ACC(GD_W_MEAN_ADDR, buffer.uc, 32);
      break;
    case 2:
      write_data_to_ACC(CP_W_MEAN_ADDR, buffer.uc, 32);
      break;
    default:
      cout << "unknown mean channel!" << endl;
      break;
  }
}

mean_t read_mean() {
  mean_t buffer;
  read_data_from_ACC(NM_R_MEAN_ADDR, buffer.uc, 32);
  return buffer;
}

unsigned char read_index() {
  unsigned char buffer;
  read_data_from_ACC(AM_R_INDEX_ADDR, &buffer, 1);
  return buffer;
}

void write_index(unsigned char buffer, int channel) {
  switch(channel){
    case 1:
      write_data_to_ACC(NM_W_INDEX_ADDR, &buffer, 1);
      break;
    case 2:
      write_data_to_ACC(CP_W_INDEX_ADDR, &buffer, 1);
      break;
    default:
      cout << "unknown mean channel!" << endl;
      break;
  }
}

distance_t read_distance(){
  distance_t buffer;
  read_data_from_ACC(GD_R_DISTANCE_ADDR, buffer.uc, 24);
  return buffer;
}

void write_distance(distance_t buffer){
  write_data_to_ACC(AM_W_DISTANCE_ADDR, buffer.uc, 24);
}


bool converge (mean_t mean1, mean_t mean2, int threshold) { // threshold usage 8-bit
  bool result;
  int error; // 16-bit usage
  // mean square error for 8 means
  result = true;
  for (int i = 0; i < 8; i++) { // for K-means
    error = 0; // initialize
    for (int j = 0; j < 3; j++){ // for R, G, B
      error += pow((int)mean1.uc[i * 4 + j] - (int)mean2.uc[i * 4 + j], 2);
    }
    if (error > threshold * threshold) {
      result = false;
      break;
    }
  }
  return result;
}

int main(int argc, char *argv[]) {

  read_bmp("lena_color_512.bmp");
  printf("======================================\n");
  printf("\t  Reading from array\n");
  printf("======================================\n");
	printf(" input_rgb_raw_data_offset\t= %d\n", input_rgb_raw_data_offset);
	printf(" width\t\t\t\t= %d\n", width);
	printf(" height\t\t\t\t= %d\n", height);
	printf(" bytes_per_pixel\t\t= %d\n",bytes_per_pixel);
  printf("======================================\n");

  mean_t mean;
  // initialize mean
  memcpy(&mean.uc[0], pixel(width>>1, height>>1).uc, 3);
  memcpy(&mean.uc[1], pixel(width>>2, height>>1).uc, 3);
  memcpy(&mean.uc[2], pixel(width>>1, height>>2).uc, 3);
  memcpy(&mean.uc[3], pixel(width>>2, height>>2).uc, 3);
  memcpy(&mean.uc[4], pixel((width>>1) + (width>>2), (height>>1) + (height>>2)).uc, 3);
  memcpy(&mean.uc[5], pixel((width>>1) + (width>>2), height>>1).uc, 3);
  memcpy(&mean.uc[6], pixel(width>>1, (height>>1) + (height>>2)).uc, 3);
  memcpy(&mean.uc[7], pixel((width>>1) + (width>>2), (height>>1) - (height>>2)).uc, 3);
  
  int cnt = 0;
  for(int k = 0; k < 30; k++) { // send sampled pixels until converge (maximum 30 times)
    for (unsigned int x = 0; x < width; x = x + 16) {
      for (unsigned int y = 0; y < height; y = y + 16) {
        write(pixel(x , y), 1);
        write_mean(mean, 1);
        write_distance(read_distance());
        unsigned char index = read_index();
        write(pixel(x , y), 2);
        write_index(index, 1);
      }
    }
    mean_t new_mean;
    new_mean = read_mean();
    if (!converge(mean, new_mean, 1)) // if not converge
      mean = new_mean;
    else {
      mean = new_mean;
      break;
    }
    cnt++;
  }
  // send all the pixels for coloring
  for (unsigned int x = 0; x < width; x++) {
    for (unsigned int y = 0; y < height; y++) {
      write(pixel(x , y), 1);
      write_mean(mean, 1);
      write_distance(read_distance());
      unsigned char index = read_index();
      write_index(index, 2);
      write_mean(mean, 2);
    }
  }
  write_bmp("lena_color_512_seg.bmp");
}
