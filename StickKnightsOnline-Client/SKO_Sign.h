/*
 * SKO_Sign.h
 *
 *  Created on: Mar 13, 2013
 *      Author: midnite
 */

#ifndef SKO_SIGN_H_
#define SKO_SIGN_H_

#include "OPI_Text.h"



class SKO_Sign {
public:
	static const int NUM_LINES = 10;
	static const int NUM_PAGES = 2;
	SKO_Sign();
	OPI_Text line[NUM_LINES];

	int x, y, w, h;
	bool triggered, hasBeenClosed;
};


#endif /* SKO_SIGN_H_ */
