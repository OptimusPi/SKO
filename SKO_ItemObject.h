#ifndef __SKO_ITEM_OBJECT_
#define __SKO_ITEM_OBJECT_


class SKO_ItemObject
{
      public:
      
      //is it used
      bool status;
      
      //which item is it
      char itemID;
      
      //physics
      float x, y, x_speed, y_speed;
      
      //constructor
      SKO_ItemObject();
      SKO_ItemObject(char ITEM_ID_in, float X, float Y, float X_speed, float Y_speed);
      
      //delete the object
      void remove();
       
};

#endif
