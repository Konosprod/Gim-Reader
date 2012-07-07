#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include "gim.h"

void waitAndQuit()
{
    SDL_Event event;

    while ( event.type!= SDL_QUIT)
        SDL_WaitEvent( &event );

    SDL_Quit();
}


int main(int argc, char* argv[])
{
    SDL_Surface* ecran = NULL;
    SDL_Event event;
    
    Gim image;
    FILE* gim = fopen(argv[1], "rb+");
    
    load_gim(&image, gim);
    
    SDL_Init(SDL_INIT_VIDEO);
    
    printf("%d\n%d\n", image.image_data.width, image.image_data.height);
    
    ecran = SDL_SetVideoMode(image.image_data.width, image.image_data.height,
                             24, SDL_ANYFORMAT | SDL_HWSURFACE );
    blit_gim(ecran, &image);
    
    SDL_Flip(ecran);
    
    while(event.type != SDL_QUIT)
    {
        SDL_WaitEvent(&event);
        
        switch(event.type)
        {
            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_ESCAPE)
                    break;
                
                if(event.key.keysym.sym == SDLK_p)
                    convert_bmp(ecran, argv[1]);
           break;
           
           default:
           break;
       }
   }
    
    free(image.palette.data);
    free(image.image_data.data);
    
    fclose(gim);
    (void)argc;
    return 0;
}
