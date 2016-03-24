#ifndef __SKO_TARGET_H_
#define __SKO_TARGET_H_


#define TARGET_BULLSEYE 0
#define TARGET_CRATE    1
#define TARGET_KEG      2

class SKO_Target {
      public:
      
      //current position and movement
      int x, y, w, h;
      bool active;
      int pic;
      
      //constructor
      SKO_Target();
      SKO_Target(int x_in, int y_in, int w_in, int h_in, int pic_in);
      
      
      
};


#endif
