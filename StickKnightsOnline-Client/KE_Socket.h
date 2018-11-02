#include "SDL_net.h"
#if WINDOWS_OS == MY_OS
	#include "SDL.h"
#else
	#include "sdl/SDL.h"
#endif

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>


#ifndef KE_SOCKET_H_
#define KE_SOCKET_H_

#define MAXLEN 100000

class KE_Socket
{
    private:
      TCPsocket sock;
      SDLNet_SocketSet sock_set;
      
      int result;
      int len;
      char msg[MAXLEN];


    public:  
        std::string Data;
        long int time;
        KE_Socket();
        bool Connected;           
        bool Startup();
        bool Connect(std::string Hostname, int port);
        void Send(std::string packet);
        int  Receive();
        int  BReceive();
        void Cleanup();  
        void Close(); 
};

#endif
