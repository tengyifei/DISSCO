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
//  event.h
//
//    This class defines a generic event.  It is used to manage all the 
//    common things between types of events (top, high, low, bottom, etc.).
//
//----------------------------------------------------------------------------//

#ifndef EVENT_H
#define EVENT_H

// CMOD includes
#include "Libraries.h"

#include "Define.h"
#include "Matrix.h"
#include "FileValue.h"
#include "Note.h"
#include "Rational.h"
#include "Tempo.h"

//----------------  Constructors and Destructors  ---------------------------//

class Event {

  protected:
    // information about this event
    float myStartTime;
    Ratio myExactStartTime;
    float myDuration;
    Ratio myExactDuration;
    int myType;
    string myName;
    FileValue* childEventDef;

    // static information (doesn't change for each child)
    float maxChildDur;    // maximum allowed duration for a child
    
    ///The tempo that this event belongs to.
    Tempo tempo;
 
    double checkPoint;
    vector<Event*> childEvents; // children of this event

  private:
    // concerning the names of layers
    vector< vector<string> > layerVect; // < <"T/1">, <"T/1","T/3">, <"T/2"> >
    vector<string> typeVect;

    // concerning the number of child events to create
    int numChildren;  // total number of children (all layers)  (numObjs)
    vector<int> layerNumChildren; // number of children to create for each layer
    vector<int> layerRemainingChildren; // number of children yet to create (initially == layerNumChildren)
    vector<float> layerDensity; // density of each layer

    /* NOTE: the remaining vars are temporary --- they change for every child */
    // private vars used in various calculations
    float lastTime;
    Ratio exactLastTime;
    
    Matrix* discreteMat;


    // DON'T NEED??? -- used in creating children
    float childStartTime;
    float childDuration;
    Ratio exactChildStartTime;
    Ratio exactChildDuration;
    // need this one --- there's a getter for the static ftn CURRENT_CHILD_NUM
    int currChildNum; 
    int childType; // need this one too (static ftn)
    string childName;

  public:

    /**
    *  Default constructor for an Event.
    **/
    Event(){}

    /**
    *	Normal constructor for an Event.
    *	\param stime Start Time of the event
    *	\param dur Duration of the event
    *	\param type Type of the event
    *	\param name Name of the event
    *   \param level The number of parent events to this event
    **/
    Event(float stime, float dur, int type, string name);

    /**
    *  Event copy constructor.
    *	\param origEvent Event object to make a copy of
    **/
    Event(const Event& origEvent);

    /**
    *	Assignment operator
    *	\param origEvent The Event to assign
    **/
    Event& operator= (const Event& origEvent);

    /**
    *	Event destructor.
    **/
    virtual ~Event();

    //----------------- Initialization functions  --------------------//
    /**
     *	Initialize the "bar" division info to be used when generating discrete children
     **/
    void initDiscreteInfo(
      std::string newTempo, std::string newTimeSignature, int newEDUPerBeat,
      float newMaxChildDur );

    /**
     *	Initialize the list of files containing possible children (layers of the event)
     *  \param childNames A filevalue containing a list of lists-of-filenames, where each inner
     *                    list represents a layer and the number of types in it.
     **/
    void initChildNames( FileValue* childNames );

    /**
     *	Initialize the number of child objects to be created
     *  \param numChildren A filevalue containing a list to describe the number of child
     *                     objects to be created.
     **/
    void initNumChildren( FileValue* numChildrenFV );

    /**
     *	Initialize the child event definition
     *  \param childrenDef A filevalue containing definition of how to create child events
     **/
    void initChildDef( FileValue* childrenDef );

    //------------- Used by FileValue static functions  ------------//
    /**
     *	Returns the events name
     **/
    string getEventName() { return myName; };

    /**
    *	Returns the number of the child this event is currently building
    **/
    int getCurrentChild() { return currChildNum; };

    /**
     *	Returns layer of the child this event is currently building
     **/
    int getCurrentLayer();

    /**
    *   Returns the current childs type
    **/
    int getCurrentChildType() { return childType; };

    /**
     *  Returns the number of current partial -- will call to bottom in most cases
     **/
    virtual int getCurrPartialNum() { return 0; };

    /**
     *  Return this events duration in EDU
     **/
    int getAvailableEDU();
    
    /**
    *  Returns check point of the event
    *  \return check point of the event as a double
    **/
    double getCheckPoint() { return checkPoint; };

    /**
    *   BuildSubEvents. Builds sub-events from parsed information and 
    *   information already set for this event.
    *   Contains a loop whthin which objects arec created one at a time.  
    *   Each object has at least three parameters: start time, duration, 
    *   and type.  They are selected using one of the following methods: 
    *     CONTINUUM for continuous probability, non-sequential order
    *     DISCRETE  for discrete values, using a Matrix object, non-sequential
    *     SWEEP for reading values from a file provided by the user, 
    *   sequential order
    *   It also parses the subEvent file, creates a NextFactory, and 
    *   performs the same operations on related files (ENV, PATT, etc.)
    *   if they exist.  If the subEvent is a BOTTOM event it is treated in
    *   a different way.
    **/
    void buildChildEvents();

    /**
     *  Helper method to build a child event.  Overridden by bottom
     *   to allow the creation of note/sound/visual instead of the
     *   usual child events
     **/
    virtual void constructChild(float stime, float dur, int type, string name);

    /**
    *   Outputs information about the current subEvent.
    **/
    virtual void outputProperties();

    /**
     * Adds pointers to any notes in this Event (or any children) to a vector
     * \param noteVect a reference to a vector of notes
     **/
    virtual list<Note> getNotes();

  //------------- Private helper functions  ------------//
  private:
    /**
    *  Method for assigning float values for stimeSec and duration using
    *  continuos (stochastic) distributions and int value for type - 
    *  actually, a discrete value..
    *
    *   Follows aq slightly different procedure than Sweep and Discrete.  Why?
    *  \param iter FileValues to pass in for new objects
    **/
    bool buildContinuum(list<FileValue>::iterator iter);

    /**
     *  Method for assigning stimeSec and durSec values in sequential order - 
     *  "sweepeing" from left to right or beginning to end of the event.
     *  For stime and duration two different methods are used, one for integer 
     *  values the other for float values.  Type being a discrete value, the 
     *  integer values method is used for it.
     *  \param iter FileValues to pass in for new objects
     *
     *  ** NEEDS WORK **
     **/
    bool buildSweep(list<FileValue>::iterator iter);

    /**
    *   Wrapper for assigning values for stimeMatrix, type and durMatrix
    *   using a matrix.  Calls ObjCoordinates, Adjustments, and TimeConvert.
`   *   \param iter FileValues to pass in for new objects
    **/
    bool buildDiscrete(list<FileValue>::iterator iter);

//---------------------------------------------------------------------------//

};

#endif

