/* INCLUDES */
#include <string>
#include "OPI_Text.h"

class SKO_Player
{
    public:
	/* FUNCTIONS */
        SKO_Player();
		void Respawn();

	/* VARIABLES */
		bool Status;

		// Name
		std::string	Nick, str_clantag;
		OPI_Text nametag, clantag;

		
    /* GAME STUFF */
    
    //position
        float x;
        float y;
        bool ground;
        int current_map;
        
    //physics
        float x_speed;
        float y_speed;
        
     //animation   
        bool facing_right; 
        bool attacking;
        int  current_frame;
        int animation_ticker;
        int attack_ticker;
        int arrow;
        
     //stats
        //health
        unsigned char hp;
        unsigned char max_hp;
        //experience
        unsigned char level;
        unsigned char stat_points;
        unsigned int xp;
        unsigned int max_xp;
        //strength and defence
        unsigned char strength;
        unsigned char defence;
        unsigned char regen;
        
        
   //items
        unsigned int inventory[24][2];
        unsigned int bank[256];
        char bank_index;
        
        //trading
        unsigned int localTrade[24][2];
        unsigned int remoteTrade[24][2];
        
        //parties
        int party, clan;
                
   //equip
        unsigned int equip[3];
        unsigned int equipI[3];
        
        bool hit;
        int hit_ticker;
};

        
