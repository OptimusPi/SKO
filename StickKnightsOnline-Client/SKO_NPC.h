#ifndef __SKO_NPC_H_
#define __SKO_NPC_H_


#include <iostream>
#include "INIReader.h"
#include <cstdio>
#include "OPI_Text.h"

class SKO_NPC
{

public:
	  SKO_NPC();
      SKO_NPC(std::string npcSprite);
      void Respawn();
      
      //coords
      float x, y;
      float x_speed, y_speed;

      //which sprite to draw and interact with?
      int sprite;
      int quest;

      //spawn
      int sx, sy;
       
       
      bool facing_right; 
      bool ground;
      unsigned int current_frame;
      unsigned long long int animation_ticker;
      std::string final;

  	static const int NUM_LINES = 10;
  	static const int MAX_PAGES = 10;
  	int num_pages;

  				//[page][line]
  	OPI_Text* line[MAX_PAGES][NUM_LINES];



};

#endif
