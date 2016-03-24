#ifndef KE_TIMESTEP
#define KE_TIMESTEP


#include <iostream>
#include <SDL/SDL.h>

/*
 * KE_Engine 
 *
 * @Nate Howard
 * nathanial.howard@uwrf.edu
 *
 * Created: 09/27/2010
 * Revised: 12/8/2010 //added support for clock_t or SDL
 * Revised: 1/10/2011 //added fix for spiral of death
 * 7/16/2011 no library.h
 */
 
class KE_Timestep
{
      private:
            //indicator
            bool ready; 
             
            //timestep
            unsigned long int frameTime;
            unsigned long int currentTime;
            unsigned long int accumulator;
            unsigned long int newTime;

      public:
            //functions
            KE_Timestep(unsigned int FPS);
            void Update();
            bool Check();
            
};

#endif
