#include "OPI_Image.h"

OPI_Image::OPI_Image()
{
	 w = 0;
	 h = 0;
	 texture = 0;
}

int pot(unsigned int x)
{
    return ((x != 0) && !(x & (x - 1)));
}


void OPI_Image::setImage( std::string path )
{
    //printf("loading image...[%s]...\n", path.c_str());

    GLuint tex[1];			// This is a handle to our texture object
    SDL_Surface *surface;	// This surface will tell us the details of the OPI_Image
    GLenum texture_format = GL_RGBA;
    GLint  nOfColors;

    surface = IMG_Load(path.c_str());

    if ( surface )
    {
        w = surface->w;
        h = surface->h;

        if (!pot(w))
           printf("ERROR: OPI_Image width not power of two! [%s]\n", path.c_str());
        if (!pot(h))
           printf("ERROR: OPI_Image width not power of two! [%s]\n", path.c_str());


            // get the number of channels in the SDL surface
            nOfColors = surface->format->BytesPerPixel;
            if (nOfColors == 4)     // contains an alpha channel
            {

                    if (surface->format->Rmask == 0x000000ff)
                            texture_format = GL_RGBA;
                    else
                            texture_format = GL_BGRA;
            } else if (nOfColors == 3)     // no alpha channel
            {
                    if (surface->format->Rmask == 0x000000ff)
                            texture_format = GL_RGB;
                    else
                            texture_format = GL_BGR;
            } else {
                    printf("ERROR: not truecolor OPI_Image! [%s]\n", path.c_str());
                    // this error should not go unhandled
            }


        // Have OpenGL generate a texture object handle for us
        glGenTextures( 1, tex );

        // Bind the texture object
        glBindTexture( GL_TEXTURE_2D, tex[0] );


       // Set the texture's stretching properties
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        // Edit the texture object's OPI_Image data using the information SDL_Surface gives us
        glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
                          texture_format, GL_UNSIGNED_BYTE, surface->pixels );
    }

    // Free the SDL_Surface only if it was successfully created
    if ( surface )
        SDL_FreeSurface( surface );

    texture = tex[0];
}

void OPI_Image::setImage(OPI_Image source)
{
     texture = source.texture;
     w       = source.w;
     h       = source.h;
}

