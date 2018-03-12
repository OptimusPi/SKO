#include "SKO_Sign.h"

SKO_Sign::SKO_Sign()
{
	for (int i = 0; i < NUM_LINES; i++){
		line[i] = OPI_Text();
		line[i].SetText("");
		line[i].pos_x = 150;
		line[i].pos_y = 372 + i*12;
		line[i].used = true;
		line[i].R = 255;
		line[i].G = 255;
		line[i].B = 255;
	}
	x = 0;
	y = 0;
	w = 0;
	h = 0;
	triggered = false;
}
