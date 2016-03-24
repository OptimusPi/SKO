/*
 * SKO_Sprite.h
 *
 *  Created on: Mar 24, 2013
 *      Author: midnite
 */

#ifndef SKO_SPRITE_H_
#define SKO_SPRITE_H_

#include "INIReader.h"
#include "OPI_Image.h"
#include <string>
#include <iostream>
#include <cstdio>

class SKO_Sprite{
public:
    SKO_Sprite();
    SKO_Sprite(std::string spriteFileLoc, std::string sprite);

    //collision rect inside the sprite
    int x1,x2,y1,y2;

    //spritesheet
    OPI_Image spriteSheet;

    //weapon
    int weapon;

    //hat or helmet
    int hat;
};



#endif /* SKO_SPRITE_H_ */
