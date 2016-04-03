#ifndef SKO_STALL_H
#define SKO_STALL_H


class SKO_Stall
{
      //to hold shops and banks...maybe more!
      public:
      
	  //what shop ID? 0 is bank
	  int shopId;

      //where is this stand at?
      int x, y;
      //how wide is it? how tall? I want a button for it man.
      int w, h;
      
      
      SKO_Stall(int x_in, int y_in, int w_in, int h_in);
      SKO_Stall();
};

#endif
