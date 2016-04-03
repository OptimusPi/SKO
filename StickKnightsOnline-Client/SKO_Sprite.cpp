#include "SKO_Sprite.h"

SKO_Sprite::SKO_Sprite(){
	x1 = x2 = y1 = y2 = weapon = hat = 0;
}

SKO_Sprite::SKO_Sprite(std::string spriteFileLoc, std::string sprite)
{
		printf("SKO_Sprite called\n");
		 //set the directory to DAT
	     std::string spriteFile = "DAT/" + spriteFileLoc + ".ini";

	     //read sprite file and load settings for this sprite
	     printf("Reading sprite configuration from %s:[%s]\n", spriteFile.c_str(), sprite.c_str());
		 INIReader configFile(spriteFile);
		 if (configFile.ParseError() < 0) {
			printf("SKO_Sprite::error: Can't load %s\n", spriteFile.c_str());
			return;
		 }

		 //set all the members of this sprite
		 x1 = configFile.GetInteger(sprite, "x1", 0);
		 x2 = configFile.GetInteger(sprite, "x2", 0);
		 y1 = configFile.GetInteger(sprite, "y1", 0);
		 y2 = configFile.GetInteger(sprite, "y2", 0);

		 //set spritesheet
		 std::string spriteImgLoc = "IMG/SPRITES/";
		 spriteImgLoc += configFile.Get(sprite, "spriteSheet", "");
		 spriteImgLoc += ".png";

		 printf("SKO_Sprite::loading spritesheet image from [%s]\n", spriteImgLoc.c_str());

		 spriteSheet.setImage(spriteImgLoc);

		 //set weapon
		 weapon = configFile.GetInteger(sprite, "weapon", -1);

		 //set hat
		 hat = configFile.GetInteger(sprite, "hat", -1);

}
