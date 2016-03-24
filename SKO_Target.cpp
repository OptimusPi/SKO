#include "SKO_Target.h"


SKO_Target::SKO_Target()
{
	x = 0;
	y = 0;
	w = 0;
	h = 0;
	active = false;
	pic = 0;
}

SKO_Target::SKO_Target(int x_in, int y_in, int w_in, int h_in, int pic_in)
{
	x = x_in;
	y = y_in;
	w = w_in;
	h = h_in;
	pic = pic_in;
	active = false;
}
