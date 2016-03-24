#ifndef __SKO_ENEMY_H_
#define __SKO_ENEMY_H_


#include <iostream>
#include "INIReader.h"
#include <cstdio>

class SKO_Enemy
{
public:
	  SKO_Enemy();
      SKO_Enemy(std::string enemySprite);
      void Respawn();
      
      //coords
      float x, y;
      float x_speed, y_speed;

      //which sprite to draw and interact with?
      int sprite;
       
      //cosmetic hp
      int hp_draw;
      int hp_ticker;
       
      bool facing_right; 
      bool attacking;
      bool ground;
      bool dead;
      bool hit;
      int current_frame;
      int animation_ticker;
      int attack_ticker;
      int hit_ticker;
};

#endif
