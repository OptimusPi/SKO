#ifndef KE_TIMESTEP
#define KE_TIMESTEP

#include <iostream>

/*
 * KE_Engine 
 *
 * @Nate Howard
 * nathanial.howard@uwrf.edu
 *
 * Created: 09/27/2010
 * Revised: 12/8/2010 //added support for clock_t or SDL
 * Revised: 1/10/2011 //added fix for spiral of death
 * Revised: 7/16/2011 //no library.h
 * Revised: 1/30/2019 // using std::chrono instead of SDL
 * 
 */
 
class KE_Timestep
{
      private:
            //indicator
            bool ready; 
             
            //timestep
            unsigned long long int frameTime;
            unsigned long long int currentTime;
            unsigned long long int accumulator;
            unsigned long long int newTime;

      public:
            //functions
            KE_Timestep(unsigned int FPS);
            void Update();
            bool Check();
            
};

#endif
