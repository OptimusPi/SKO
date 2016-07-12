#include "SKO_Map.h"


void SKO_Map::init(){
	num_enemies = 0;
	num_shops = 0;
	num_stalls = 0;
	num_signs = 0;
	num_targets = 0;
	number_of_fringe = 0;
	number_of_rects = 0;
	number_of_tiles = 0;
	num_npcs = 0;
}

SKO_Map::SKO_Map ()
{
	init();
}

SKO_Map::SKO_Map (std::string location, std::string mapName)
{
	 init();

	 //where is the map data and config?
	 std::string mapDataLoc;
	 std::string mapConfLoc;

	 //convert the number to string filenames
	 std::stringstream mapBinData, mapIniData;
	 mapBinData << location << "/" << mapName << "." << "map";
	 mapIniData << location << "/" << mapName << "." << "ini";
	 mapDataLoc = mapBinData.str();
	 mapConfLoc = mapIniData.str();

     printf("loading map data from: %s\n", mapDataLoc.c_str());
     std::ifstream MapFile(mapDataLoc.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
      
     if (MapFile.is_open())
     {
        //loading variables
        std::ifstream::pos_type size;
        char * memblock;

        //allocate memory
        size = MapFile.tellg();
        memblock = (char *)malloc(size);

        //load the file into memory
        MapFile.seekg (0, std::ios::beg);
        MapFile.read (memblock, size);
        //close file
        MapFile.close();

        //hold the result...
        unsigned int num;

        //build an int from 4 bytes
        ((char*)&num)[0] = memblock[0];
        ((char*)&num)[1] = memblock[1];
        ((char*)&num)[2] = memblock[2];
        ((char*)&num)[3] = memblock[3];


        //store the number into variables
        number_of_tiles = num;

        //check for too many tiles
        if (number_of_tiles > MAX_TILES) {
        	printf("ERROR: too many tiles! Please increase SKO_Map::MAX_TILES! [%i]\n", number_of_tiles);
        }
        //build an int from 4 bytes
        ((char*)&num)[0] = memblock[4];
        ((char*)&num)[1] = memblock[5];
        ((char*)&num)[2] = memblock[6];
        ((char*)&num)[3] = memblock[7];

        //store the number into variables
        number_of_fringe = num;
        //build an int from 4 bytes
        ((char*)&num)[0] = memblock[8];
        ((char*)&num)[1] = memblock[9];
        ((char*)&num)[2] = memblock[10];
        ((char*)&num)[3] = memblock[11];

        //store the number into variables
        number_of_rects = num;


        //check for too many tiles
        if (number_of_rects > MAX_TILES) {
        	printf("ERROR: too many rects! Please increase SKO_Map::MAX_TILES! [%i]\n", number_of_rects);
        }

        printf("tiles: %i\n", number_of_tiles);
        printf("fringe: %i\n", number_of_fringe);
        printf("rect: %i\n", number_of_rects);

        //
        //tiles
        //
        int last_i = 11;

        for (int i = 0; i < number_of_tiles; i++)
        {
            //9 bytes per tile silly ;)

            //build an int from 4 bytes
            ((char*)&num)[0] = memblock[last_i+1+i*9];
            ((char*)&num)[1] = memblock[last_i+2+i*9];
            ((char*)&num)[2] = memblock[last_i+3+i*9];
            ((char*)&num)[3] = memblock[last_i+4+i*9];

            //store the number into variables
            tile_x[i] = num;


            //build an int from 4 bytes
            ((char*)&num)[0] = memblock[last_i+5+i*9];
            ((char*)&num)[1] = memblock[last_i+6+i*9];
            ((char*)&num)[2] = memblock[last_i+7+i*9];
            ((char*)&num)[3] = memblock[last_i+8+i*9];

            //store the number into variables
            tile_y[i] = num;

            //store the number into variables
            tile[i] = memblock[last_i+9+i*9];

            if (tile[i] == 60)
            	tile[i] = 61;

//            printf("tile is %i\n", tile[i]);
        }

        last_i += number_of_tiles*9;
        //
        //fringe tiles
        //
        for (int i = 0; i < number_of_fringe; i++)
        {
            //9 bytes per tile silly ;)

            //build an int from 4 bytes
            ((char*)&num)[0] = memblock[last_i+1+i*9];
            ((char*)&num)[1] = memblock[last_i+2+i*9];
            ((char*)&num)[2] = memblock[last_i+3+i*9];
            ((char*)&num)[3] = memblock[last_i+4+i*9];

            //store the number into variables
            fringe_x[i] = num;


            //build an int from 4 bytes
            ((char*)&num)[0] = memblock[last_i+5+i*9];
            ((char*)&num)[1] = memblock[last_i+6+i*9];
            ((char*)&num)[2] = memblock[last_i+7+i*9];
            ((char*)&num)[3] = memblock[last_i+8+i*9];

            //store the number into variables
            fringe_y[i] = num;

            //store the number into variables
            fringe[i] = memblock[last_i+9+i*9];


            if (fringe[i] == 60)
                fringe[i] = 61;

//            printf("fringe tile is %i\n", fringe[i]);

        }
        last_i += number_of_fringe * 9;
        //
        //rects
        //
        for (int i = 0; i < number_of_rects; i++)
        {
            //read the map file
            //build an int from 4 bytes
            ((char*)&num)[0] = memblock[last_i+1+i*16];
            ((char*)&num)[1] = memblock[last_i+2+i*16];
            ((char*)&num)[2] = memblock[last_i+3+i*16];
            ((char*)&num)[3] = memblock[last_i+4+i*16];

            //store the number into variables
            collision_rect[i].x = num;

            if (i == 0)
            //printf("X Bytes: [%i][%i][%i][%i]\n", (int)((char*)&num)[0], (int)((char*)&num)[1], (int)((char*)&num)[2], (int)((char*)&num)[3]);
            printf("X is: %i\n", (short int)num);

            //build an int from 4 bytes
            ((char*)&num)[0] = memblock[last_i+5+i*16];
            ((char*)&num)[1] = memblock[last_i+6+i*16];
            ((char*)&num)[2] = memblock[last_i+7+i*16];
            ((char*)&num)[3] = memblock[last_i+8+i*16];

            //store the number into variables
            collision_rect[i].y = num;

            if (i == 0)
            //printf("Y Bytes: [%i][%i][%i][%i]\n", (int)((char*)&num)[0], (int)((char*)&num)[1], (int)((char*)&num)[2], (int)((char*)&num)[3]);
                printf("Y is: %i\n", (short int)num);

            //build an int from 4 bytes
            ((char*)&num)[0] = memblock[last_i+9+i*16];
            ((char*)&num)[1] = memblock[last_i+10+i*16];
            ((char*)&num)[2] = memblock[last_i+11+i*16];
            ((char*)&num)[3] = memblock[last_i+12+i*16];

            //store the number into variables
            collision_rect[i].w = num;

            if (i == 0)
            //printf("W Bytes: [%i][%i][%i][%i]\n", (int)((char*)&num)[0], (int)((char*)&num)[1], (int)((char*)&num)[2], (int)((char*)&num)[3]);
                printf("W is: %i\n", (short int)num);


            //build an int from 4 bytes
            ((char*)&num)[0] = memblock[last_i+13+i*16];
            ((char*)&num)[1] = memblock[last_i+14+i*16];
            ((char*)&num)[2] = memblock[last_i+15+i*16];
            ((char*)&num)[3] = memblock[last_i+16+i*16];


            //store the number into variables
            collision_rect[i].h = num;

            if (i == 0)
            //printf("H Bytes: [%i][%i][%i][%i]\n", (int)((char*)&num)[0], (int)((char*)&num)[1], (int)((char*)&num)[2], (int)((char*)&num)[3]);
                printf("H is: %i\n", (short int)num);
         }

         free(memblock);
     }else {
           printf("Can't load map data %s.\n", mapDataLoc.c_str());
           return;
     }

     //open map config file
     printf("Reading Map config from: %s\n", mapConfLoc.c_str());
	 INIReader configFile(mapConfLoc);
	 if (configFile.ParseError() < 0) {
		printf("error: Can't load '%s'\n", mapConfLoc.c_str());
		return;
	 }

	 //load portals
	 // note: portals are server-side
	 // 	  so they don't load on the client
	 //load enemies
	 num_enemies = configFile.GetInteger("count", "enemies", 0);
	 printf("num_enemies is %i\n", num_enemies);
	 for (int enemy = 0; enemy < num_enemies; enemy++){
		 std::stringstream ss;
		 ss << "enemy" << enemy;

		 //load sprite type from file
		 std::string spriteType  = configFile.Get(ss.str(), "sprite", "");
		 printf("spriteType is %s\n", spriteType.c_str());
		 Enemy[enemy] = SKO_Enemy(spriteType);
	 }

	 //load signs
	 num_signs = configFile.GetInteger("count", "signs", 0);
	 printf("num_signs is %i\n", num_signs);

	 for (int sign = 0; sign < num_signs; sign++){
		 std::stringstream ss;
		 ss << "sign" << sign;

		 Sign[sign] = SKO_Sign();
		 Sign[sign].x = configFile.GetInteger(ss.str(), "x", 0);
		 Sign[sign].y = configFile.GetInteger(ss.str(), "y", 0);
		 Sign[sign].w = configFile.GetInteger(ss.str(), "w", 0);
		 Sign[sign].h = configFile.GetInteger(ss.str(), "h", 0);

		 printf("Sign[sign].NUM_LINES is %i\n", Sign[sign].NUM_LINES);

		 //get all the lines of the text
		 for (int line = 0; line < Sign[sign].NUM_LINES; line++){
			 std::stringstream ss1;
			 ss1 << "line" << line;
			 std::string txt = configFile.Get(ss.str(), ss1.str(), "");
			 if (txt.length())
				 txt = txt.substr(1);
			 Sign[sign].line[line].SetText(txt.c_str());
			 printf("%s is: %s\n", ss1.str().c_str(), txt.c_str());
		 }
	 }

	 //load shops
	 num_shops = configFile.GetInteger("count", "shops", 0);
	 printf("num_shops is %i\n", num_shops);

	 //start at i= 1 ... why?
	 //  because ID 0 is reserved for the bank
	 for (int i = 0; i <= num_shops; i++){
		 Shop[i] = SKO_Shop();
		 std::stringstream ss1;
		 ss1 << "shop" << i;
		 std::string shopStr = ss1.str();
		 //loop all the X Y places
		 for (int x = 0; x < 6; x++){
			 for (int y = 0; y < 4; y++){
				 std::stringstream ss2, ss3;
				 ss2 << "item"  << "_" << "x" << x << "_" << "y" << y;
				 ss3 << "price" << "_" << "x" << x << "_" << "y" << y;

				 //x  y  (item[0], price[1])
				 std::string itemStr = ss2.str().c_str();
				 std::string priceStr = ss3.str().c_str();

				 //now load from config file the items and prices
				 Shop[i].item[x][y][0]
				        = configFile.GetInteger(shopStr.c_str(), itemStr.c_str(), 0);
				 Shop[i].item[x][y][1]
				        = configFile.GetInteger(shopStr.c_str(), priceStr.c_str(), 0);
			 }
		 }

		 //all done loading shops for now.
		 //in the future, maybe a shop title etc.
	 }

	 //load stalls
	 num_stalls = configFile.GetInteger("count", "stalls", 0);
	 printf("num_stalls is %i\n", num_stalls);

	 for (int i = 0; i < num_stalls; i++){
		 std::stringstream ss1;
		 ss1 << "stall" << i;
		 std::string stallStr = ss1.str();

		 Stall[i] = SKO_Stall();
		 Stall[i].shopId = configFile.GetInteger(stallStr, "shopId", 0);
		 Stall[i].x      = configFile.GetInteger(stallStr, "x", 0);
		 Stall[i].y      = configFile.GetInteger(stallStr, "y", 0);
		 Stall[i].w      = configFile.GetInteger(stallStr, "w", 0);
		 Stall[i].h      = configFile.GetInteger(stallStr, "h", 0);
	 }

	 //load targets
	 num_targets = configFile.GetInteger("count", "targets", 0);
	 printf("num_targets is %i\n", num_targets);

	 for (int i = 0; i < num_targets; i++){
		 std::stringstream ss1;
		 ss1 << "target" << i;
		 std::string targetStr = ss1.str();

		 Target[i] = SKO_Target();
		 Target[i].pic    = configFile.GetInteger(targetStr, "sprite", 0);
		 Target[i].x      = configFile.GetInteger(targetStr, "x", 0);
		 Target[i].y      = configFile.GetInteger(targetStr, "y", 0);
		 Target[i].w      = configFile.GetInteger(targetStr, "w", 0);
		 Target[i].h      = configFile.GetInteger(targetStr, "h", 0);
	 }

	 //load npcs
	 num_npcs = configFile.GetInteger("count", "npcs", 0);
	 printf("num_npcs is %i\n", num_npcs);

	 for (int i = 0; i < num_npcs; i++){
		 std::stringstream ss1;
		 ss1 << "npc" << i;
		 std::string targetStr = ss1.str();

		 NPC[i] = SKO_NPC();
		 NPC[i].sprite    = configFile.GetInteger(targetStr, "sprite", 0);
		 NPC[i].x = NPC[i].sx        = configFile.GetInteger(targetStr, "x", 0);
		 NPC[i].y = NPC[i].sy        = configFile.GetInteger(targetStr, "y", 0);
		 NPC[i].final     = configFile.GetInteger(targetStr, "final", 0);
		 NPC[i].num_pages = configFile.GetInteger(targetStr, "pages", 0);
		 NPC[i].quest = configFile.GetInteger(targetStr, "quest", -1); //-1 for non quest NPCs

		 //get all the lines of the text
		 for (int page = 0; page < NPC[i].num_pages; page++)
		 		 for (int line = 0; line < NPC[i].NUM_LINES; line++)
		 		 {
		 			 std::stringstream ss1;
		 			 ss1 << "page_" << page << "_";
		 			 ss1 << "line_" << line;
		 			 std::string txt = configFile.Get(targetStr, ss1.str(), "");
		 			 if (txt.length())
		 				 txt = txt.substr(1);
		 			 NPC[i].line[page][line]->SetText(txt.c_str());
		 			 printf("NPC %s is: %s\n", ss1.str().c_str(), txt.c_str());
		 		 }


	 }
}


