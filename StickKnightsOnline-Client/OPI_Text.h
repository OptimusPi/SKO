#ifndef __OPI_TEXT_H_
#define __OPI_TEXT_H_

#include <string>
#define MAX_T_MESSAGE 128

class OPI_Text
{
      public:
         OPI_Text();
         void SetText(std::string message);
         short int letter_x[MAX_T_MESSAGE];
         short int letter_y[MAX_T_MESSAGE];
         short int length;
         float R, G, B;
         int pos_x, pos_y;
         bool used;
             
};

#endif
