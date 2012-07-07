#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#include "gim.h"

/*TODO: - gérer tous les cas possibles*/

int end_swp(int nb)
{
    int a = nb & 0xFF0000;
    int b = nb & 0x00FF00;
    int c = nb & 0x0000FF;
    
    a = a >> 16;
    c = c << 16;
    
    return a | b | c;
}


char* rev(char* str)
{
  int end= strlen(str)-1;
  int start = 0;

  while( start<end )
  {
    str[start] ^= str[end];
    str[end] ^=   str[start];
    str[start]^= str[end];

    ++start;
    --end;
  }

  return str;
}

bool check_gim_sign(unsigned char* test)
{
    char sign[5] = {0x4D, 0x49, 0x47, 0x2E, 0x00};
    
    for(int i = 0; i < 5; i++)
    {
        if(sign[i] != test[i])
            return false;
    }

    return true;
}

void get_oef_part(Gim* image, FILE* gim)
{
    fseek(gim, 0x02, SEEK_CUR);
    
    fread(&image->eof.eof_addr, sizeof(char), 4, gim);
    fread(&image->eof.part_size, sizeof(char), 4, gim);
 
    fseek(gim, 0x04, SEEK_CUR);
    
    image->eof.eof_addr+= ftell(gim) - image->eof.part_size;
}

void get_fi_part(Gim* image, FILE* gim)
{
    fseek(gim, 0x02, SEEK_CUR);
    
    fread(&image->fi.fi_addr, sizeof(char), 4, gim);
    fread(&image->fi.part_size, sizeof(char), 4, gim);
    
    fseek(gim, 0x04, SEEK_CUR);
    
    image->fi.fi_addr+=ftell(gim)-image->fi.part_size;
}

void get_palette(Gim* image, FILE* gim)
{
    fseek(gim, 0x06, SEEK_CUR);

    fread(&image->palette.part_size, sizeof(char), 4, gim);
    
    fseek(gim, 0x04, SEEK_CUR);
    
    fread(&image->palette.data_offset, sizeof(char), 2, gim);
    
    fseek(gim, 0x02, SEEK_CUR);
    
    fread(&image->palette.palette_format, sizeof(char), 2, gim);
    
    fseek(gim, 0x02, SEEK_CUR);
    
    fread(&image->palette.palette_colors, sizeof(char), 2, gim);
    
    fseek(gim, 0x36, SEEK_CUR);
    
    image->palette.data = calloc(image->palette.palette_colors, sizeof(int));
    
    for(int i = 0; i < image->palette.palette_colors; i++)
    {
        fread(&image->palette.data[i], sizeof(char), 4, gim);
        image->palette.data[i] = end_swp(image->palette.data[i]);
    }
}

void get_image_data(Gim* image, FILE* gim)
{
    fseek(gim, 0x06, SEEK_CUR);
    
    fread(&image->image_data.part_size, sizeof(char), 4, gim);
    
    fseek(gim, 0x04, SEEK_CUR);
    
    fread(&image->image_data.data_offset, sizeof(char), 2, gim);
    
    fseek(gim, 0x02, SEEK_CUR);
    
    fread(&image->image_data.image_format, sizeof(char), 2, gim);
    fread(&image->image_data.pixel_order, sizeof(char), 2, gim);
    image->image_data.width = 0;
    image->image_data.height = 0;
    fread(&image->image_data.width, sizeof(char), 2, gim);
    fread(&image->image_data.height, sizeof(char), 2, gim);
    fread(&image->image_data.color_depth, sizeof(char), 2, gim);
    
    fseek(gim, 0x32, SEEK_CUR);
    
    if(image->image_data.image_format < 0x20)
    {
        image->image_data.data = calloc(image->image_data.part_size - 0x50, sizeof(int));
        
        for(int i = 0; i < image->image_data.part_size - 0x50; i++)
        {
            fread(&image->image_data.data[i], sizeof(char), 1, gim);
        }
    }
}
    

void get_gim_data(Gim* image, FILE* gim, int type)
{    
    switch(type)
    {
        case EOF_PART:
            get_oef_part(image, gim);
        break;
        
        case FI_PART:
            get_fi_part(image, gim);
        break;
        
        case PALETTE:
            get_palette(image, gim);
        break;
        
        case IMAGE_DATA:
            get_image_data(image, gim);
        break;
        
        case FI_DATA:
        break;
            
        default:
        break;
    }
}

void init_structs(Gim* s)
{
    s->eof.eof_addr = 0;
    s->eof.part_size = 0;
    
    s->fi.fi_addr = 0;
    s->fi.part_size = 0;
    
    s->palette.part_size = 0;
    s->palette.data_offset = 0;
    s->palette.palette_format = 0;
    s->palette.palette_colors = 0;
    s->palette.data = NULL;
    
    s->image_data.part_size = 0;
    s->image_data.data_offset = 0;
    s->image_data.image_format = 0;
    s->image_data.pixel_order = 0;
    s->image_data.width = 0;
    s->image_data.height = 0;
    s->image_data.color_depth = 0;
    s->image_data.data = 0;
}
    

void get_gim_header(Gim* image, FILE* gim)
{
    fread(image->sign, sizeof(char), 4, gim);
    image->sign[4] = '\0';
    if(!check_gim_sign(image->sign))
    {
        printf("Invalide gim signature\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Signature: %s\n", rev((char*)&image->sign));
    }
    
    fread(image->version, sizeof(char), 4, gim);
    image->version[4] = '\0';
    printf("Version: %s\n", rev((char*)&image->version));
    
    fread(image->plateforme, sizeof(char), 4, gim);
    printf("Plateforme: %s\n", rev((char*)&image->plateforme));
    fseek(gim, 0x04, SEEK_CUR);
}

void load_gim(Gim* image, FILE* gim)
{
    int a = 0;
    int type = 0;
    
    init_structs(image);
    
    get_gim_header(image, gim);
    while( a < 4)
    {
        fread(&type, sizeof(char), 2, gim);
        get_gim_data(image, gim, type);
        type = 0;
        a++;
    }
}

Uint32 get_pixel(SDL_Surface *surface, int x, int y)
{
    int nbOctetsParPixel = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * nbOctetsParPixel;

    switch(nbOctetsParPixel)
    {
        case 1:
            return *p;

        case 2:
            return *(Uint16 *)p;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;

        case 4:
            return *(Uint32 *)p;

        default:
            return 0; 
    }
}

void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int nbOctetsParPixel = surface->format->BytesPerPixel;
    
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * nbOctetsParPixel;

    switch(nbOctetsParPixel)
    {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else
            {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}

void blit_faster(SDL_Surface* ecran, Gim* image)
{
    SDL_Surface* surf = NULL;
    SDL_Rect pos;
    
    pos.w = 16;
    pos.h = 8;
    
    surf = SDL_CreateRGBSurface(SDL_HWSURFACE, 16, 8, 32, 0, 0, 0, 0);
    
    for(int i = 0; i < image->image_data.part_size/0x80; i++)
    {
        SDL_LockSurface(surf);
        for(int j = 0; j < 0x80; j++)
        {
            set_pixel(surf, j%16, j/16, image->palette.data[image->image_data.data[i*0x80+j]]);
        }
        SDL_UnlockSurface(surf);
        
        pos.x = 16*(i%(image->image_data.width/16));
        pos.y = 8*(i/(image->image_data.width/16));
        
        SDL_BlitSurface(surf, NULL, ecran, &pos);
    }
    
}

void blit_normal(SDL_Surface* ecran, Gim* image)
{
    (void)ecran;
    (void)image;
}

void convert_bmp(SDL_Surface* ecran, char* name)
{
    char* tab = NULL;
    
    tab = calloc(strlen(name)+5, sizeof(char));
    
    sprintf(tab, "%s.bmp", name);
    
    if(SDL_SaveBMP(ecran, tab) == -1)
    {
        printf("Impossible d'enregistrer : %s\n", tab);
    }
    else
    {
        printf("%s saved !\n", tab);
    }
    
    free(tab);
}

void blit_gim(SDL_Surface* ecran, Gim* image)
{
    switch(image->image_data.pixel_order)
    {
        case PIXEL_FASTER:
        blit_faster(ecran, image);
        break;
        
        case PIXEL_NORMAL:
        /*blit_normal(ecran, image);*/
        break;
    }
}
