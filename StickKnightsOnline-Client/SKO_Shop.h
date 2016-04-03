#include <string.h>

#ifndef __SKO_SHOP_H_
#define __SKO_SHOP_H_

//shop types
#define SHOP_BANK 0
#define SHOP_FOOD 1
#define SHOP_WEAPONS 2  

class SKO_Shop
{
      public:
             
      SKO_Shop();
          //shop type
          
      //simple shop def.
                       //x  y  (item[0], price[1])
      unsigned int item[6][4][2];
      
             
};

#endif
