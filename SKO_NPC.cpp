#include "SKO_NPC.h"


SKO_NPC::SKO_NPC()
{

	facing_right = false;
	ground = false;
	current_frame = 0;
	sx = 0;
	sy = 0;
	x_speed = 0;
	y_speed = 0;
	animation_ticker = 0;
	x = 0;
	y = 0;
	sprite = 0;
	quest = 0;

	for (int page = 0; page < MAX_PAGES; page++)
	for (int i = 0; i < NUM_LINES; i++){
			line[page][i] = new OPI_Text();
			line[page][i]->SetText("");
			line[page][i]->pos_x = 150;
			line[page][i]->pos_y = 372 + i*12;
			line[page][i]->used = true;
			line[page][i]->R = 255;
			line[page][i]->G = 255;
			line[page][i]->B = 255;
		}
		x = 0;
		y = 0;
		num_pages = 0;
		final = "";

}

SKO_NPC::SKO_NPC(std::string npcSprite)
{
SKO_NPC();

      //open sprite config file
      std::string spriteFileLoc = "DAT/npcSprites.ini";
      printf("Reading sprite config from: %s\n", spriteFileLoc.c_str());

      INIReader configFile(spriteFileLoc);
      if (configFile.ParseError() < 0) {
      	 printf("error: Can't load '%s'\n", spriteFileLoc.c_str());
      	 return;
      }

      //load enemy sprite
      sprite = configFile.GetInteger(npcSprite, "id", 0);



      sx = configFile.GetInteger(npcSprite, "x", 0);
      sy = configFile.GetInteger(npcSprite, "y", 0);
      printf("sprite is: %i (%i,%i)\n", sprite, sx, sy);
      quest = configFile.GetInteger(npcSprite, "quest", 0);

     Respawn();

}

void SKO_NPC::Respawn()
{
	printf("SKO_NPC respawn....\n");
     //printf("Well, sx is %i and sy is %i and x1 is %i and x2 is %i\n", sx, sy, x1, x2);
     x = sx;
     y = sy;
     x_speed = 0;
     y_speed = 0;
     current_frame = 0;
     animation_ticker = 0;
}
