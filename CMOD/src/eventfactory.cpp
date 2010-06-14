/*
CMOD (composition module)
Copyright (C) 2007  Sever Tipei (s-tipei@uiuc.edu)

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
//   eventfactory.cpp
//
//----------------------------------------------------------------------------//

#include "eventfactory.h"
//#include "event.h"
#include "bottom.h"
#include "lexyacc/eventparser.h"
#include "cmodpiece.h"

extern map<string, EventFactory*> factory_lib;
extern CMODPiece Piece;

//---------------------------------------------------------------------------//

EventFactory::EventFactory() {
}

//---------------------------------------------------------------------------//

EventFactory::EventFactory(string filename) {
  cout << "Parsing file: " << filename << endl;
  name = filename;

  // parse filename (EventParser)
  parseFile(name, this, &Piece);

  // add event factories for subevents to library
  factory_lib[filename] = this;
}

//---------------------------------------------------------------------------//

EventFactory::~EventFactory() {

}

//----------------------------------------------------------------------------//

Event* EventFactory::Build(float startTime, float duration, int type, int level) {
  int i = 0;

  Event* newEvent;
  char c = name[0];
  if(c != 'B') {
    newEvent = new Event(startTime, duration, type, name, level);
  } else {
    Bottom* tmpEvent = new Bottom(startTime, duration, type, name, level);

    tmpEvent->initBottomVars( frequency, loudness, spatialization, 
                              reverberation, modifiers );
    newEvent = tmpEvent;
  }

  // initialize general event params
  //  (the theme here is to evaluate the filevalues here, or call a method
  //   in Event to evaluate the filevalues.  THERE ARE NO PRIVATE VARS IN
  //   Event TO HOLD ONTO FileValues!!
  newEvent->initChildMaxDur( maxChildDur->getFloat() );
  newEvent->initDiscreteInfo( unitsPerSecond->getInt(),
                              unitsPerBar->getInt() );
  newEvent->initChildNames( childNames );
  newEvent->initNumChildren( numChildren );

  // initialize child events, if there are any
  if (childEventDef != NULL) {
    newEvent->initChildDef( childEventDef );
  }

  return newEvent;
}



