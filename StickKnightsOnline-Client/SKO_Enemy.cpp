#include "SKO_Enemy.h"


SKO_Enemy::SKO_Enemy()
{
	attacking = false;
	dead = false;
	facing_right = false;
	ground = false;
	hit = false;
	hp_draw = 0;
	x_speed = 0;
	y_speed = 0;
	hit_ticker = 0;
	attack_ticker = 0;
	animation_ticker = 0;
	hp_ticker = 0;
	x = 0;
	y = 0;
	sprite = 0;

}

SKO_Enemy::SKO_Enemy(std::string enemySprite)
{

      facing_right = true;
      attacking = false;
      current_frame = 0;
      animation_ticker = 0;
      attack_ticker = 0;
      ground = true;
      hp_draw = 1;
      hp_ticker = 0;

      //open sprite config file
      std::string spriteFileLoc = "DAT/enemySprites.ini";
      //printf("Reading sprite config from: %s\n", spriteFileLoc.c_str());

      INIReader configFile(spriteFileLoc);
      if (configFile.ParseError() < 0) {
      	 printf("error: Can't load '%s'\n", spriteFileLoc.c_str());
      	 return;
      }

      //load enemy sprite
      sprite = configFile.GetInteger(enemySprite, "id", 0);

      printf("sprite is: %i\n", sprite);

      //coords
      x = 0, y = 0;
      x_speed = 0, y_speed = 0;

}

