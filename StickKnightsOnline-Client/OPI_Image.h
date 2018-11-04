#include <string>
#include <cstdlib>

#include "operating_system.h"

#ifdef WINDOWS_OS
	#include "SDL.h"
	#include "SDL_image.h"
	#include "SDL_opengl.h"
#else
	#include <SDL/SDL.h>
	#include <SDL/SDL_image.h>
#endif



#ifndef __OPI_IMAGE_H_
#define __OPI_IMAGE_H_





class OPI_Image
{
    public:

	OPI_Image();
    void setImage(std::string filename);
    void setImage(OPI_Image source);

    GLuint texture;
    int w;
    int h;
};

#endif
