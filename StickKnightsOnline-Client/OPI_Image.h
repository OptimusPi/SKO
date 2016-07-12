#include <string>
#include <cstdlib>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_opengl.h"


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
