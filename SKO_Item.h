#include <string>

#ifndef __SKO_ITEM_H_
#define __SKO_ITEM_H_

class SKO_Item
{
      public:
             
      int w, h;
      int hp, sp, dp;
      int price;
      std::string descr;
      
      SKO_Item(int w_in, int h_in, int price_in, std::string descr_in, int hp_in, int sp_in, int dp_in);
      SKO_Item(int w_in, int h_in, int price_in, std::string descr_in);
      SKO_Item();
};

#endif
