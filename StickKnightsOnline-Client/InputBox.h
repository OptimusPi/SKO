#ifndef InputBox_H
#define InputBox_H

#include "OPI_Text.h"


class InputBox
{
      public:
             
      //constructor
      InputBox();
      //std::string test;
      
      //get values from input
      int getInt();
      //std::string InputBox::getString();
      
      
      //Text
      OPI_Text * inputText, * outputText, * okayText, * cancelText;
      
      bool numeric;
      
      

};

#endif
