#include "KE_Socket.h"
 
KE_Socket::KE_Socket()
{           
	Connected = false;
	len = 0;
	result = 0;
	sock_set = NULL;
	sock = NULL;
	time = 0;
	Data = "";
}
bool KE_Socket::Startup()
{
    	
    if(SDL_Init(0)==-1) {
        printf("SDL_Init: %s\n", SDL_GetError());
        return false;
    }
    if(SDLNet_Init()==-1) {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        return false;
    }
    
    return true;
}

bool KE_Socket::Connect(std::string Hostname, int port)
{
	Connected = false;
    IPaddress ip;

   /* Resolve the host we are connecting to */
	if (SDLNet_ResolveHost(&ip, Hostname.c_str(), port))
	{
		std::string error = SDLNet_GetError();
		fprintf(stderr, "SDLNet_ResolveHost: %s\n", error.c_str());
		return false;
	}
 
	/* Open a connection with the IP provided (listen on the host's port) */
	if (!(sock = SDLNet_TCP_Open(&ip)))
	{
		std::string error = SDLNet_GetError();
		fprintf(stderr, "SDLNet_TCP_Open: %s\n", error.c_str());
		return false;
	}
	
	//allocate the socket set
	sock_set = SDLNet_AllocSocketSet(1);
	
	//add the socket to the socket set
	SDLNet_TCP_AddSocket(sock_set, sock);

    Connected = true;
    return true;                
}

void KE_Socket::Cleanup()
{
     SDLNet_Quit();
}

void KE_Socket::Close()
{
	 printf("KE_Socket::Close() called\n");
     SDLNet_TCP_Close(sock);
     Connected = false;
}

void KE_Socket::Send(std::string packet)
{
     len = packet.length();
	 if (sock)
 		 if (SDLNet_TCP_Send(sock, packet.c_str(), len) < len)
  		 {
			fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			Close();
		 }
		 
		 else {
			 //TODO try to reconnect
		 }

}

int KE_Socket::Receive()
{
    
    result = 0;
    
    //only receive if there is data
    if (SDLNet_CheckSockets(sock_set, 0))
    {
         //time = clock();
    	// receive some text from sock
    	result = SDLNet_TCP_Recv(sock,msg,MAXLEN);
        //printf("\t* TIME IS: %i\n", SDL_GetTicks()); 
        if (result > 0) {
           //printf("\t* * * TIME IS: %i\n", SDL_GetTicks());        
           
           Data.append(msg, result);  
        } else if (result == 0){
        	Close();
        } else {
        	Close();
        }
        
    }
    
    return result;
}
int KE_Socket::BReceive()
{  
    // receive some text from sock
    result=SDLNet_TCP_Recv(sock,msg,MAXLEN);
    
    if (result > 0) {
           Data.append(msg, result);    
    }
        
    return result;
}




