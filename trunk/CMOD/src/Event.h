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
//  Event.h
//
//    This class defines a generic event.  It is used to manage all the 
//    common things between types of events (top, high, low, bottom, etc.).
//
//----------------------------------------------------------------------------//

#ifndef EVENT_H
#define EVENT_H

#include "Libraries.h"

#include "Define.h"
#include "Matrix.h"
#include "FileValue.h"
#include "Note.h"
#include "Rational.h"
#include "Tempo.h"

class Event {
  protected:
  
    //---------------------------- Information -------------------------------//
    
    //Name of the event
    string myName;
    
    //Type of the event
    int myType;
    
    //Start time of the event (in both float seconds and exact beats)
    float myStartTime;
    Ratio myExactStartTime;
    
    //Duration of the event (in both float seconds and exact beats)
    float myDuration;
    Ratio myExactDuration;
    
    //Tempo of the event
    Tempo tempo;
    
    //------------------------------ Children --------------------------------//
    
    //File value to the event definitions of children
    FileValue* childEventDef;

    //Children of the event
    vector<Event*> childEvents;
    
    //------------------------------ Building --------------------------------//
    
    //Maximum allowed duration for a child
    float maxChildDur;
    
    //Percentage of child start time divided by total parent duration
    double checkPoint;    
    
    //Previous start time
    float lastTime;
    Ratio exactLastTime;

  private:
  
    //-------------------------- Layers and Types ----------------------------//

    //Names of the layers (i.e. from < <"T/1">, <"T/1","T/3">, <"T/2"> > )
    vector< vector<string> > layerVect;
    
    //Names of the children by type.
    vector<string> typeVect;

    //Number of children to create (all layers)
    int numChildren;
    
    //Number of children to create for each layer
    vector<int> layerNumChildren;
    
    //Number of children yet to create (initially equal to layerNumChildren)
    vector<int> layerRemainingChildren;
    
    //Density of each layer.
    vector<float> layerDensity;
    
    //--------------------------- Restart Control ----------------------------//
    
    /*Putative child events are stored here. The children are not added
    immediately because the build process can fail and need to be restarted in
    the case of buildDiscrete.*/
    vector<Event*> temporaryChildEvents;
    
    //Number of restarts remaining.
    int restartsRemaining;
    static const int restartsNormallyAllowed = 6;
    static const int restartsAllowedWithFewerChildren = 10;
 
    //---------------------------- Static Values -----------------------------//
    
    //Stores value for the getter to the static function CURRENT_CHILD_NUM
    int currChildNum;
    
    //Stores value for the getter to the static function CURRENT_CHILD_TYPE
    int childType;
    
    //--------------------------- Discrete Matrix ----------------------------//
    
    Matrix* discreteMat;
       
    //----------------------------- Deprecated -------------------------------//
    
    //These will go away -- they are duplicated by myFoo for the child event.
    float childStartTime;
    float childDuration;
    Ratio exactChildStartTime;
    Ratio exactChildDuration;

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
    *  Method for determining the EDU-wise exactness of the duration. Returns
    *  "Yes", "No", or "Almost". "Almost" occurs when the EDU duration is
    *  within one-percent of the actual duration. This occurs when floating-
    *  point math approximates the result, but does not exactly produce it.
    *  "Yes" can only occur when the result is exactly divisible. "No" occurs
    *  for all other case (i.e., when there are 3.2 EDU for the total duration).
    **/
    string getEDUDurationExactness(void);
    /**
    *  Method for assigning float values for stimeSec and duration using
    *  continuos (stochastic) distributions and int value for type - 
    *  actually, a discrete value..
    *
    *   Follows aq slightly different procedure than Sweep and Discrete.  Why?
    *  \param iter FileValues to pass in for new objects
    **/
    bool buildContinuum(list<FileValue>::iterator iter, string childName);

    /**
     *  Method for assigning stimeSec and durSec values in sequential order - 
     *  "sweepeing" from left to right or beginning to end of the event.
     *  For stime and duration two different methods are used, one for integer 
     *  values the other for float values.  Type being a discrete value, the 
     *  integer values method is used for it.
     *  \param iter FileValues to pass in for new objects
     **/
    bool buildSweep(list<FileValue>::iterator iter, string childName);

    /**
    *   Wrapper for assigning values for stimeMatrix, type and durMatrix
    *   using a matrix.  Calls ObjCoordinates, Adjustments, and TimeConvert.
`   *   \param iter FileValues to pass in for new objects
    **/
    bool buildDiscrete(list<FileValue>::iterator iter, string childName);
    
    /**
     * Converts "SECONDS" to "sec.", "PERCENTAGE" to "%", etc.
     **/
    string unitTypeToUnits(string type);
    
    //Checks the event to see if it was built successfully.
    void checkEvent(bool buildResult);
    
    //Restarts the build process if necessary (for buildDiscrete).
    void tryToRestart(void);
};
#endif

