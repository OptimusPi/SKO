#include "OPI_Text.h"

OPI_Text::OPI_Text()
{
    R = 255;
    G = 255;
    B = 255;
    length = 0;
    pos_x = 0;
    pos_y = 0;
    used = false;

    for (int i = 0; i < MAX_T_MESSAGE; i++){
    	letter_x[i] = 0;
    	letter_y[i] = 0;
    }
}

void OPI_Text::SetText(std::string index)
{

    //printf("%s %i\n", index, strlen(index));
    length = index.length();
    
    //max length
    if (length > MAX_T_MESSAGE)
       length = MAX_T_MESSAGE;

    //go thorugh the letters and place the coords
    for (int i = 0; i < length; i++)
    {
        switch (index[i])
        {
           //lowecase letters
           case 'a':
                letter_x[i] = 0;
                letter_y[i] = 22;
           break;
           case 'b':
                letter_x[i] = 8;
                letter_y[i] = 22;
           break;
           case 'c':
                letter_x[i] = 16;
                letter_y[i] = 22;
           break;
           case 'd':
                letter_x[i] = 24;
                letter_y[i] = 22;
           break;
           case 'e':
                letter_x[i] = 32;
                letter_y[i] = 22;
           break;
           case 'f':
                letter_x[i] = 40;
                letter_y[i] = 22;
           break;
           case 'g':
                letter_x[i] = 48;
                letter_y[i] = 22;
           break;
           case 'h':
                letter_x[i] = 56;
                letter_y[i] = 22;
           break;
           case 'i':
                letter_x[i] = 64;
                letter_y[i] = 22;
           break;
           case 'j':
                letter_x[i] = 72;
                letter_y[i] = 22;
           break;
           case 'k':
                letter_x[i] = 80;
                letter_y[i] = 22;
           break;
           case 'l':
                letter_x[i] = 88;
                letter_y[i] = 22;
           break;
           case 'm':
                letter_x[i] = 96;
                letter_y[i] = 22;
           break;
           case 'n':
                letter_x[i] = 0;
                letter_y[i] = 33;
           break;
           case 'o':
                letter_x[i] = 8;
                letter_y[i] = 33;
           break;
           case 'p':
                letter_x[i] = 16;
                letter_y[i] = 33;
           break;
           case 'q':
                letter_x[i] = 24;
                letter_y[i] = 33;
           break;
           case 'r':
                letter_x[i] = 32;
                letter_y[i] = 33;
           break;
           case 's':
                letter_x[i] = 40;
                letter_y[i] = 33;
           break;
           case 't':
                letter_x[i] = 48;
                letter_y[i] = 33;
           break;
           case 'u':
                letter_x[i] = 56;
                letter_y[i] = 33;
           break;
           case 'v':
                letter_x[i] = 64;
                letter_y[i] = 33;
           break;
           case 'w':
                letter_x[i] = 72;
                letter_y[i] = 33;
           break;
           case 'x':
                letter_x[i] = 80;
                letter_y[i] = 33;
           break;
           case 'y':
                letter_x[i] = 88;
                letter_y[i] = 33;
           break;
           case 'z':
                letter_x[i] = 96;
                letter_y[i] = 33;
           break;

           //capital letters
           //letters
           case 'A':
                letter_x[i] = 0;
                letter_y[i] = 0;
           break;
           case 'B':
                letter_x[i] = 8;
                letter_y[i] = 0;
           break;
           case 'C':
                letter_x[i] = 16;
                letter_y[i] = 0;
           break;
           case 'D':
                letter_x[i] = 24;
                letter_y[i] = 0;
           break;
           case 'E':
                letter_x[i] = 32;
                letter_y[i] = 0;
           break;
           case 'F':
                letter_x[i] = 40;
                letter_y[i] = 0;
           break;
           case 'G':
                letter_x[i] = 48;
                letter_y[i] = 0;
           break;
           case 'H':
                letter_x[i] = 56;
                letter_y[i] = 0;
           break;
           case 'I':
                letter_x[i] = 64;
                letter_y[i] = 0;
           break;
           case 'J':
                letter_x[i] = 72;
                letter_y[i] = 0;
           break;
           case 'K':
                letter_x[i] = 80;
                letter_y[i] = 0;
           break;
           case 'L':
                letter_x[i] = 88;
                letter_y[i] = 0;
           break;
           case 'M':
                letter_x[i] = 96;
                letter_y[i] = 0;
           break;
           case 'N':
                letter_x[i] = 0;
                letter_y[i] = 11;
           break;
           case 'O':
                letter_x[i] = 8;
                letter_y[i] = 11;
           break;
           case 'P':
                letter_x[i] = 16;
                letter_y[i] = 11;
           break;
           case 'Q':
                letter_x[i] = 24;
                letter_y[i] = 11;
           break;
           case 'R':
                letter_x[i] = 32;
                letter_y[i] = 11;
           break;
           case 'S':
                letter_x[i] = 40;
                letter_y[i] = 11;
           break;
           case 'T':
                letter_x[i] = 48;
                letter_y[i] = 11;
           break;
           case 'U':
                letter_x[i] = 56;
                letter_y[i] = 11;
           break;
           case 'V':
                letter_x[i] = 64;
                letter_y[i] = 11;
           break;
           case 'W':
                letter_x[i] = 72;
                letter_y[i] = 11;
           break;
           case 'X':
                letter_x[i] = 80;
                letter_y[i] = 11;
           break;
           case 'Y':
                letter_x[i] = 88;
                letter_y[i] = 11;
           break;
           case 'Z':
                letter_x[i] = 96;
                letter_y[i] = 11;
           break;

           //numbers
           case '1':
                letter_x[i] = 0;
                letter_y[i] = 44;
           break;
           case '2':
                letter_x[i] = 8;
                letter_y[i] = 44;
           break;
           case '3':
                letter_x[i] = 16;
                letter_y[i] = 44;
           break;
           case '4':
                letter_x[i] = 24;
                letter_y[i] = 44;
           break;
           case '5':
                letter_x[i] = 32;
                letter_y[i] = 44;
           break;
           case '6':
                letter_x[i] = 40;
                letter_y[i] = 44;
           break;
           case '7':
                letter_x[i] = 48;
                letter_y[i] = 44;
           break;
           case '8':
                letter_x[i] = 56;
                letter_y[i] = 44;
           break;
           case '9':
                letter_x[i] = 64;
                letter_y[i] = 44;
           break;
           case '0':
                letter_x[i] = 72;
                letter_y[i] = 44;
           break;

           //symbols
           case '.':
                letter_x[i] = 80;
                letter_y[i] = 44;
           break;
           case ',':
                letter_x[i] = 88;
                letter_y[i] = 44;
           break;
           case '/':
                letter_x[i] = 96;
                letter_y[i] = 44;
           break;
           case '\\':
                letter_x[i] = 0;
                letter_y[i] = 55;
           break;
           case '!':
                letter_x[i] = 8;
                letter_y[i] = 55;
           break;
           case '(':
                letter_x[i] = 16;
                letter_y[i] = 55;
           break;
           case ')':
                letter_x[i] = 24;
                letter_y[i] = 55;
           break;
           case '?':
                letter_x[i] = 32;
                letter_y[i] = 55;
           break;
           case ':':
                letter_x[i] = 40;
                letter_y[i] = 55;
           break;
           case ';':
                letter_x[i] = 48;
                letter_y[i] = 55;
           break;
           case '\"':
                letter_x[i] = 56;
                letter_y[i] = 55;
           break;
           case '\'':
                letter_x[i] = 64;
                letter_y[i] = 55;
           break;

           case '[':
                letter_x[i] = 72;
                letter_y[i] = 55;
           break;
           case ']':
                letter_x[i] = 80;
                letter_y[i] = 55;
           break;
           case '{':
                letter_x[i] = 88;
                letter_y[i] = 55;
           break;
           case '}':
                letter_x[i] = 96;
                letter_y[i] = 55;
           break;
           case '|':
                letter_x[i] = 0;
                letter_y[i] = 66;
           break;
           case '*':
                letter_x[i] = 8;
                letter_y[i] = 66;
           break;
           case '-':
                letter_x[i] = 16;
                letter_y[i] = 66;
           break;
           case '+':
                letter_x[i] = 24;
                letter_y[i] = 66;
           break;
           case '<':
                letter_x[i] = 32;
                letter_y[i] = 66;
           break;
           case '>':
                letter_x[i] = 40;
                letter_y[i] = 66;
           break;
           case '=':
                letter_x[i] = 48;
                letter_y[i] = 66;
           break;
           case '@':
                letter_x[i] = 56;
                letter_y[i] = 66;
           break;
           case '#':
                letter_x[i] = 64;
                letter_y[i] = 66;
           break;
           case '$':
                letter_x[i] = 72;
                letter_y[i] = 66;
           break;
           case '%':
                letter_x[i] = 80;
                letter_y[i] = 66;
           break;
           case '^':
                letter_x[i] = 88;
                letter_y[i] = 66;
           break;
           case '&':
                letter_x[i] = 96;
                letter_y[i] = 66;
           break;
           case '_':
                letter_x[i] = 0;
                letter_y[i] = 77;
           break;
           case '`':
                letter_x[i] = 8;
                letter_y[i] = 77;
           break;
           case '~':
                letter_x[i] = 16;
                letter_y[i] = 77;
           break;
           //space
           case ' ':
                letter_x[i] = 24;
                letter_y[i] = 77;
           break;

           //errors or undefined letters
           default:
                letter_x[i] = 24;
                letter_y[i] = 77;
           break;
        }//switch
    }//for
}//void
