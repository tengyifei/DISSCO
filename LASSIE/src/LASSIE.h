/*******************************************************************************
 *
 *  File Name     : LASSIE.h
 *  Date created  : Sept. 17 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This File contains all the dependency headers used by
                    LASSIE.
 *
 *
 *==============================================================================
 *
 *  This file is part of LASSIE.
 *  Copyright 2010 Ming-ching Chiu, Sever Tipei
 *
 *
 *  LASSIE is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  LASSIE is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with LASSIE.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#ifndef LASSIE_H
#define LASSIE_H

//GTKmm library
#include <gtkmm.h>
#include <sys/types.h>
#include <unistd.h>

//LASS library
#include "../../LASS/src/LASS.h"

/* Also includes
#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>
*/

//CMOD library
#include "../../CMOD/src/CMOD.h"

/* Also includes:
#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <ctime>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>
*/


//other
#include <sstream>



typedef enum {
  functionReturnInt,
  functionReturnFloat,
  functionReturnSPA,
  functionReturnREV,
  functionReturnENV,
  functionReturnSIV,
  functionReturnPAT,
  functionReturnIntList,
  functionReturnFloatList,
  functionReturnEnvelopeList, 
  functionReturnString, //used to parse FileValue
  functionReturnList,
  functionReturnMakeListFun
  
} FunctionReturnType;


#endif //LASSIE_H


