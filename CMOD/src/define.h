/*
CMOD (composition module)
Copyright (C) 2005  Sever Tipei (s-tipei@uiuc.edu)
                                                                                
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
                                                                                
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
                                                                                
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//----------------------------------------------------------------------------//
//
// define.h
//
// This file defines constants which are used throughout all of the
// classes.  
//
//----------------------------------------------------------------------------//


#ifndef DEFINE_H
#define DEFINE_H

#include "libraries.h"

//#define ENVLIB_DEFAULT "library000"

// Typecasting a BOOL to an integer 
typedef int BOOL;

// This is a nice feature that will turn all instances of DEBUG into a
// cout.  This makes very useful debugging information print out.
#define DEBUG cout

// Constants
//#define RAND_MAX 2147483647.0L
//#define HRAND_MAX 1073741824.0L
#define NYQUIST 22050.0L
#define CEILING 15000.0L
#define MINFREQ 20.0L
#define TRUE 1
#define FALSE 0
#define PI 4.0*atan(1.0)

//----------------------------------------------------------------------------//
//  The following are items that are common throughout 
//  the composition part of the program.
//----------------------------------------------------------------------------//

// name of the main (first) data file to be read
//static const char * INFILE = "daria";

// general
static const double GOLDENMEAN = 0.618;
static const float MIN_AUD_FREQ = 20.;
static const float MAX_AUD_FREQ = 20000.;
static const float FUNDAMENTAL = 13.75;
static const double C0 = 16.351597;
static const double WELL_TEMP_INCR = pow(2, 1./12.);
//static const double WELL_TEMP_INCR = 1.0594631;
static const float MAX_SONES = 256.;
static const float FIRST_CONST = -5.54;
static const float SECOND_CONST = -1.84;
static const int SAMPLING_RATE = 44100;

// constants for the Event class
static const int DEFAULT_START_TIME = 0;
static const int DEFAULT_DURATION = 1;

#endif /* DEFINE_H */
