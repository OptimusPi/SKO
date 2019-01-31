#include "KE_Timestep.h"
#include "OPI_Clock.h"

KE_Timestep::KE_Timestep(unsigned int FPS)
{
   frameTime = (unsigned long int)(1000000.0/FPS);
   currentTime = OPI_Clock::microseconds();
   newTime = 0;
   accumulator = 0;
   ready = false;
}

void KE_Timestep::Update()
{
     newTime = OPI_Clock::microseconds();
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
