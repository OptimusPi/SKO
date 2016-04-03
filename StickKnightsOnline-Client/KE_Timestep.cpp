#include "KE_Timestep.h"


KE_Timestep::KE_Timestep(unsigned int FPS)
{
   frameTime = (unsigned long int)(1000.0/FPS);
   currentTime = SDL_GetTicks();
   newTime = 0;
   accumulator = 0;
   ready = false;

}

void KE_Timestep::Update()
{
     newTime = SDL_GetTicks();
     accumulator += newTime - currentTime;
     currentTime = newTime;
}

bool KE_Timestep::Check()
{
     if ( accumulator >= frameTime )
     {
         accumulator -= frameTime;
         return true;
     }
     
     return false;
}
