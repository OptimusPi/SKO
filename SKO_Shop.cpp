#include "SKO_Shop.h"

SKO_Shop::SKO_Shop()
{
     for (int x = 0; x < 6; x++)
         for (int y = 0; y < 4; y++)
         {
             //item 0
             item[x][y][0] = 0;
             
             //buy for 0 (dont have!!)
             item[x][y][1] = 0;                 
         }
}
