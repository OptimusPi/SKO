#include "SKO_Item.h"

SKO_Item::SKO_Item(int w_in, int h_in, int price_in, std::string descr_in)
{
        w = w_in;
        h = h_in;
        price = price_in;
        descr = descr_in;
        hp = dp= sp = 0;
}

SKO_Item::SKO_Item(int w_in, int h_in, int price_in, std::string descr_in, int hp_in, int sp_in, int dp_in)
{
        w = w_in;
        h = h_in;
        price = price_in;
        descr = descr_in;
        hp = hp_in;
        sp = sp_in;
        dp = dp_in;
}

SKO_Item::SKO_Item()
{
  w = h = price = 
  hp = dp = sp = 0;
  descr = "";
}
