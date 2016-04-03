#include "SKO_Player.h"

SKO_Player::SKO_Player()
{
    arrow = 0;
    x = 400;
    y = 384;
    
    x_speed = 0;
    y_speed = 0;
    
    hp = 7;
    max_hp = 255;
    xp = 7;
    max_xp = 255;
    regen = 255;
    attack_ticker = 0;
    hit = 0;
    hit_ticker = 0;
    bank_index = 0;
    
    level = 255;
    stat_points = 255;
    strength = 255;
    defence = 255;
    
    facing_right = true;
    current_frame = 0;
    animation_ticker = 0;
    
    //holds all the worn items
    equip[0] = 0;
    equip[1] = 0;
    equip[2] = 0;
    equipI[0] = 0;
    equipI[1] = 0;
    equipI[2] = 0;
    
	Nick = "", str_clantag = "";
	
	Status = false;
	ground = false;
	attacking = false;
	
	for (int i = 0; i < 256; i++)
        bank[i] = 0;

    for (int i = 0; i < 24; i++)
    {
        inventory[i][0] = 0;
        inventory[i][1] = 0;
        localTrade[i][0] = 0;
        localTrade[i][1] = 0;
        remoteTrade[i][0] = 0;
        remoteTrade[i][1] = 0;
    }
    
    party = -1;
    clan = -1;
    current_map = 2;
}
