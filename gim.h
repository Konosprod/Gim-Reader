#ifndef GIM_H_INCLUDED
#define GIM_H_INCLUDED

#include <stdint.h>
#include <SDL/SDL.h>

#define RGBA5650 0x00
#define RGBA5551 0x01
#define RGBA4444 0x02
#define RGBA8888 0x03
#define INDEX4 0x04
#define INDEX8 0x05
#define INDEX16 0x06
#define INDEX32 0x07
#define CINDEX4 0x04
#define CINDEX8 0x08
#define CINDEX16 0x10
#define CINDEX32 0x20
#define C256 0x100
#define C16 0x10
#define EOF_PART 0x02
#define FI_PART 0x03
#define PALETTE 0x05
#define IMAGE_DATA 0x04
#define FI_DATA 0xFF
#define PIXEL_FASTER 0x01
#define PIXEL_NORMAL 0x00

typedef struct EOF_Part
{
    int eof_addr;
    int part_size;
}EOF_Part;

typedef struct FI_Part
{
    int fi_addr;
    int part_size;
}FI_Part;

typedef struct Palette
{
    int part_size;
    int data_offset;
    int palette_format;
    int palette_colors;
    int* data;
}Palette;

typedef struct Image_Data
{
    int part_size;
    int data_offset;
    char image_format;
    char pixel_order;
    int width;
    int height;
    int color_depth;
    int* data;
}Image_Data;

typedef struct FI_Data
{
    int part_size;
}FI_Data;

typedef struct Gim
{
    unsigned char sign[5];
    unsigned char version[5];
    unsigned char plateforme[4];
    EOF_Part eof;
    FI_Part fi;
    Palette palette;
    FI_Data fidata;
    Image_Data image_data;
}Gim;


void load_gim(Gim* image, FILE* gim);
void blit_gim(SDL_Surface* surface, Gim* image);
void convert_bmp(SDL_Surface* ecran, char* name);

#endif

