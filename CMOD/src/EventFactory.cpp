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
//   EventFactory.cpp
//
//----------------------------------------------------------------------------//

#include "EventFactory.h"

#include "Bottom.h"
#include "EventParser.h"
#include "Piece.h"

extern map<string, EventFactory*> factory_lib;
extern Piece ThePiece;

//---------------------------------------------------------------------------//

EventFactory::EventFactory(string fileToParse) : fileToParse(fileToParse) {
  cout << "Parsing file: " << fileToParse << endl;
  
  //Parse filename (EventParser)
  parseFile(fileToParse, this, &ThePiece);

  //Add event factories for subevents to library.
  factory_lib[fileToParse] = this;
}

//----------------------------------------------------------------------------//

Event* EventFactory::Build(TimeSpan ts, int type, Tempo tempo) {
  //Determine type of event to build (Bottom or Event).
  bool isBottomEvent = (fileToParse[0] == 'B');
  
  //Create a Bottom or an Event.
  Event* newEvent;
  if(isBottomEvent) {
    newEvent = new Bottom(ts, type, fileToParse);
    
    //Initialize Bottom specific parameters.
    dynamic_cast<Bottom*>(newEvent)->initBottomVars(frequency, loudness,
      spatialization, reverberation, modifiers);
  }
  else
    newEvent = new Event(ts, type, fileToParse);
    
  /*Send the tempo information given by the parent. This will be respected by
  initDiscreteInfo if it contains a non-zero tempo start time.*/
  newEvent->tempo = tempo;

  /*Initialize general event parameters. The theme here is to evaluate the
  file values here or call a method in Event to evaluate the file values. Events
  do not store file values as private variables.*/
  newEvent->initDiscreteInfo(EventFactory::tempo->getString(),
    timeSignature->getString(), EDUPerBeat->getInt(), maxChildDur->getFloat());
  newEvent->initChildNames(childNames);
  newEvent->initNumChildren(numChildren);

  //Initialize child events, if there are any.
  if(childEventDef)
    newEvent->initChildDef(childEventDef);

  return newEvent;
}

