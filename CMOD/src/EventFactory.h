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
//   EventFactory class is the control center for the program flow.  It is the
//   class where all the new events are generated, their attributes set, and
//   subevents begin to be deal with.  The only real function, Build() is 
//   called and its job is to decide what type of file is to be looked at
//   (Top, High, Low, Bottom, etc.) and to set some parameters accordingly.
//   The eventfactory is the place where, after most of the calculations have
//   completed, the parameters get attributed to the respective event.
//
//----------------------------------------------------------------------------//

#ifndef EVENTFACTORY_H
#define EVENTFACTORY_H

//----------------------------------------------------------------------------//

#include "Libraries.h"

#include "Event.h"
#include "FileValue.h"
#include "Tempo.h"
#include "TimeSpan.h"

//----------------------------------------------------------------------------//

class EventFactory {
    //Name of the file to parse
    std::string fileToParse;

    //File values for basic information
    FileValue* childNames;
    FileValue* numChildren;
    FileValue* childEventDef;

    //File values for rhythmic information
    FileValue* tempo;
    FileValue* timeSignature;
    FileValue* EDUPerBeat;
    FileValue* maxChildDur;

    //File values for bottom events
    FileValue* frequency;
    FileValue* loudness;
    FileValue* spatialization;
    FileValue* reverberation;
    FileValue* modifiers;

    //File values for sounds
    FileValue* numPartials;
    FileValue* deviation;
    FileValue* spectrum;

    //File values for notes
    FileValue* notePitchClass;
    FileValue* noteDynamicMark;
    FileValue* noteModifiers;
    
    //File values for reading objects from external files
    FileValue *envelopeBuilder;
    FileValue *sieveBuilder;
    FileValue *patternBuilder;

  public:
  
    ///Constructor to parse the file and adds this event factory to the library.
    EventFactory(std::string fileToParse);

    /**
     *  Method to build an instance of an Event from a parsed file.
     *  \param startTime Floating point representation of start time.
     *  \param duration Floating point representation of duration.
     *  \param type Integer representation of type
     *  \param level the number of parents to the event (used for printing)
     *  \return Event object
     **/
    Event* Build(TimeSpan ts, int type, Tempo tempo);

    //------------------------------------------------------------------------//
    //               Getters and Setters for each File Value                  //
    //------------------------------------------------------------------------//
        
    FileValue* getTempo() {return tempo;}
    void setTempo(FileValue* fv) {tempo = fv;}
    
    FileValue* getTimeSignature() {return timeSignature;}
    void setTimeSignature(FileValue* fv) {timeSignature = fv;}
    
    FileValue* getEDUPerBeat() {return EDUPerBeat;}
    void setEDUPerBeat(FileValue* fv) {EDUPerBeat = fv;}
    
    FileValue* getMaxChildDur() {return maxChildDur;}
    void setMaxChildDur(FileValue* fv) {maxChildDur = fv;}
    
    FileValue* getChildNames() {return childNames;}
    void setChildNames(FileValue* fv) {childNames = fv;}
    
    FileValue* getNumChildren() {return numChildren;}
    void setNumChildren(FileValue* fv) {numChildren = fv;}
    
    FileValue* getChildEventDef() {return childEventDef;}
    void setChildEventDef(FileValue* fv) {childEventDef = fv;}
    
    FileValue* getFrequency() {return frequency;}
    void setFrequency(FileValue* fv) {frequency = fv;}
    
    FileValue* getLoudness() {return loudness;}
    void setLoudness(FileValue* fv) {loudness = fv;}
    
    FileValue* getSpatialization() {return spatialization;}
    void setSpatialization(FileValue* fv) {spatialization = fv;}
    
    FileValue* getReverberation() {return reverberation;}
    void setReverberation(FileValue* fv) {reverberation = fv;}
    
    FileValue* getModifiers() {return modifiers;}
    void setModifiers(FileValue* fv) {modifiers = fv;}
    
    FileValue* getNumPartials() {return numPartials;}
    void setNumPartials(FileValue* fv) {numPartials = fv;}
    
    FileValue* getDeviation() {return deviation;}
    void setDeviation(FileValue* fv) {deviation = fv;}
    
    FileValue* getSpectrum() {return spectrum;}
    void setSpectrum(FileValue* fv) {spectrum = fv;}
    
    FileValue* getNotePitchClass() {return notePitchClass;}
    void setNotePitchClass(FileValue* fv) {notePitchClass = fv;}
    
    FileValue* getNoteDynamicMark() {return noteDynamicMark;}
    void setNoteDynamicMark(FileValue* fv) {noteDynamicMark = fv;}
    
    FileValue* getNoteModifiers() {return noteModifiers;}
    void setNoteModifiers(FileValue* fv) {noteModifiers = fv;}
    
    FileValue* getEnvelopeBuilder() {return envelopeBuilder;}
    void setEnvelopeBuilder(FileValue* fv) {envelopeBuilder = fv;}
    
    FileValue* getSieveBuilder() {return sieveBuilder;}
    void setSieveBuilder(FileValue* fv) {sieveBuilder = fv;}
    
    FileValue* getPatternBuilder() {return patternBuilder;}
    void setPatternBuilder(FileValue* fv) {patternBuilder = fv;}

    //------------------------------------------------------------------------//
    //                Deprecated Setters for some File Values                 //
    //------------------------------------------------------------------------//
    
    void setUnitsPerSecond(FileValue* fv) { // --> deprecated
      //See if function was actually used, or if it was sent a dummy variable.
      if(fv->isNull())
        return;
      static bool warned = false;
      if(!warned) {
        warned = true;
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        cout << "Warning: using unitsPerSecond is deprecated. Instead" << endl;
        cout << "use EDUPerBeat. If no tempo and timeSignature are" << endl;
        cout << "indicated, then the default tempo is 'quarter=60'" << endl;
        cout << "and the default timeSignature is '5/4'" << endl;
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
      }
      
      //Forward directly to setEDUPerBeat.
      setEDUPerBeat(fv);
    }
    
    void setUnitsPerBar(FileValue* fv) { // --> deprecated
      //See if function was actually used, or if it was sent a dummy variable.
      if(fv->isNull())
        return;
      static bool warned = false;
      if(!warned) {
        warned = true;
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        cout << "Warning: unitsPerBar is deprecated and has no effect" << endl;
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
      }
    }
};
#endif /*EVENTFACTORY_H_*/

