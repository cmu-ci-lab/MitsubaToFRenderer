#include <cstdlib>
#include <cstdio>
#include <vector>
#include <iostream>
#include <string>
// Uncomment if you want to use system provided zlib.
// #define TINYEXR_USE_MINIZ (0)
// #include <zlib.h>

#define TINYEXR_IMPLEMENTATION
#include "tinyexr_adi.h"

#ifdef __clang__
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#endif

#define SIMPLE_API_EXAMPLE
//#define TEST_ZFP_COMPRESSION

#ifdef SIMPLE_API_EXAMPLE

#if 0
static void
SaveAsPFM(const char* filename, int width, int height, float* data)
{
#ifdef _WIN32
  FILE* fp = NULL;
  fopen_s(&fp, filename, "wb");
#else
  FILE* fp = fopen(filename, "wb");
#endif
  if (!fp) {
    fprintf(stderr, "failed to write a PFM file.\n");
    return;
  }

  fprintf(fp, "PF\n");
  fprintf(fp, "%d %d\n", width, height);
  fprintf(fp, "-1\n"); // -1: little endian, 1: big endian

  // RGBA -> RGB
  std::vector<float> rgb(static_cast<size_t>(width*height*3));

  for (size_t i = 0; i < static_cast<size_t>(width * height); i++) {
    rgb[3*i+0] = data[4*i+0];
    rgb[3*i+1] = data[4*i+1];
    rgb[3*i+2] = data[4*i+2];
  }
  
  fwrite(&rgb.at(0), sizeof(float), static_cast<size_t>(width * height * 3), fp);

  fclose(fp);
}
#endif

#else

static const char* GetPixelType(int id)
{
  if (id == TINYEXR_PIXELTYPE_HALF) {
    return "HALF";
  } else if (id == TINYEXR_PIXELTYPE_FLOAT) {
    return "FLOAT";
  } else if (id == TINYEXR_PIXELTYPE_UINT) {
    return "UINT";
  }

  return "???";
}


// Simple tile -> scanline converter. Assumes FLOAT pixel type for all channels.
static void
TiledImageToScanlineImage(EXRImage* src, const EXRHeader* header)
{
  assert(header->data_window[2] - header->data_window[0] + 1 >= 0);
  assert(header->data_window[3] - header->data_window[1] + 1 >= 0);
  size_t data_width  = static_cast<size_t>(header->data_window[2] - header->data_window[0] + 1);
  size_t data_height = static_cast<size_t>(header->data_window[3] - header->data_window[1] + 1);

  src->images = static_cast<unsigned char**>(malloc(sizeof(float*) * static_cast<size_t>(header->num_channels)));
  for (size_t c = 0; c < static_cast<size_t>(header->num_channels); c++) {
    assert(header->pixel_types[c] == TINYEXR_PIXELTYPE_FLOAT);
    src->images[c] = static_cast<unsigned char*>(malloc(sizeof(float) * data_width * data_height));
    memset(src->images[c], 0, sizeof(float) * data_width * data_height);
  }

  for (size_t tile_idx = 0; tile_idx < static_cast<size_t>(src->num_tiles); tile_idx++) {

    size_t sx = static_cast<size_t>(src->tiles[tile_idx].offset_x * header->tile_size_x);
    size_t sy = static_cast<size_t>(src->tiles[tile_idx].offset_y * header->tile_size_y);
    size_t ex = static_cast<size_t>(src->tiles[tile_idx].offset_x * header->tile_size_x + src->tiles[tile_idx].width);
    size_t ey = static_cast<size_t>(src->tiles[tile_idx].offset_y * header->tile_size_y + src->tiles[tile_idx].height);

    for (size_t c = 0; c < static_cast<size_t>(header->num_channels); c++) {
      float *dst_image = reinterpret_cast<float*>(src->images[c]);
      const float *src_image = reinterpret_cast<const float*>(src->tiles[tile_idx].images[c]);
      for (size_t y = 0; y < static_cast<size_t>(ey - sy); y++) {
        for (size_t x = 0; x < static_cast<size_t>(ex - sx); x++) {
          dst_image[(y + sy) * data_width + (x + sx)] = src_image[y * static_cast<size_t>(header->tile_size_x) + x];
        }
      }
    }

  }

}
#endif

int
main(int argc, char** argv)
{
  const char* err = NULL;
  int ret;

  if (argc < 3) {
    fprintf(stderr, "Needs input prefix and number of files.\n");
    exit(-1);
  }
  std::string fileprefix = argv[1];
  int numberOfFiles = atoi(argv[2]);
  std::string outfilename;
  std::string infilename;
  EXRVersion exr_version;
  EXRHeader exr_header;
  EXRHeader tmp_header;

  int width, height;
  float* meanImage;
  float* secondMoment;

  if (argc = 3) {
    outfilename = fileprefix + "_mean.exr";
  }
  for(int i = 0; i < numberOfFiles; i++){
    infilename = fileprefix + "_" + std::to_string(i) + ".exr";
  //  std::cout << "Input File:" << infilename << std::endl;
    if(i == 0){
        int ret = LoadMultiEXR(&meanImage, &secondMoment, &width, &height, numberOfFiles, infilename.c_str(), &err);
        if(ret != 0)
                std::cout << "Error loading the file:" << infilename.c_str() << std::endl;
        ret = ParseEXRHeaderFromFile(&exr_header, &exr_version, infilename.c_str(), &err);
        if(ret != 0)
                std::cout << "Error parsing the header:" << infilename.c_str() << std::endl;
    }else{
        int ret = MomentsEXR(&meanImage, &secondMoment, width, height, numberOfFiles, infilename.c_str(), &err);
        if(ret != 0)
                std::cout << "Error loading the file:" << infilename.c_str() << std::endl;
    }

  }
  ret = SaveMultiEXR(meanImage, width, height, exr_header, 1, (fileprefix + "_mean.exr").c_str());
  if(ret != 0)
    std::cout << "Error saving the file:" << (fileprefix + "_mean.exr").c_str() << std::endl;
  for (size_t i = 0; i < width * height; i++) {
    for (size_t j = 0; j < exr_header.num_channels; j++) {
        secondMoment[exr_header.num_channels * i + j] -= meanImage[exr_header.num_channels * i + j]*meanImage[exr_header.num_channels * i + j];

    }
  }
  ret = SaveMultiEXR(secondMoment, width, height, exr_header, 1, (fileprefix + "_var.exr").c_str());
  if(ret != 0)
    std::cout << "Error saving the file:" << (fileprefix + "_var.exr").c_str() << std::endl;
  return ret;
}
