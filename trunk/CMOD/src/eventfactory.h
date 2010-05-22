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

// CMOD includes
#include "define.h"
#include "filevalue.h"
#include "event.h"

#include <string>
#include <map>
#include <iostream>

// forward declare
//class FileValue;
//class Event;
//----------------------------------------------------------------------------//

class EventFactory {
  private:
    std::string name;

    // Common to all Events
    FileValue* maxChildDur;
    FileValue* unitsPerSecond;
    FileValue* unitsPerBar;
    FileValue* childNames;
    FileValue* numChildren;

    FileValue* childEventDef;

    // for Bottom Events
    FileValue* frequency;
    FileValue* loudness;
    FileValue* spatialization;
    FileValue* reverberation;
    FileValue* modifiers;

    // for sounds
    FileValue* numPartials;
    FileValue* deviation;
    FileValue* spectrum;

    // for notes
    FileValue* notePitchClass;
    FileValue* noteDynamicMark;
    FileValue* noteModifiers;

    // for visuals
    // nothing yet

    // Special cases (reading objs from external files)
    FileValue *envelopeBuilder;
    FileValue *sieveBuilder;
    FileValue *patternBuilder;

//----------------------------------------------------------------------------//

  public:

    /**
    *  Constructor; creates an empty object
    **/
    EventFactory();

    /**
    *  Constructor; parses the file and adds this eventfactory to the library
    **/
    EventFactory(std::string filename);

    /**
    *  Destructor
    **/
    virtual ~EventFactory();

    /**
     *  Method to build an instance of an Event from a parsed file.
     *  \param startTime Floating point representation of start time.
     *  \param duration Floating point representation of duration.
     *  \param type Integer representation of type
     *  \param level the number of parents to the event (used for printing)
     *  \return Event object
     **/
    Event* Build(float startTime, float duration, int type, int level);

    //Getters and Setters for all the FileValues
    FileValue* getMaxChildDur() {
      return maxChildDur;
    }
    void setMaxChildDur(FileValue* fv) {
      maxChildDur = fv;
    }
    /* --------------------------- */
    FileValue* getUnitsPerSecond() {
      return unitsPerSecond;
    }
    void setUnitsPerSecond(FileValue* fv) {
      unitsPerSecond = fv;
    }
    /* --------------------------- */
    FileValue* getUnitsPerBar() {
      return unitsPerBar;
    }
    void setUnitsPerBar(FileValue* fv) {
      unitsPerBar = fv;
    }
    /* --------------------------- */
    FileValue* getChildNames() {
      return childNames;
    }
    void setChildNames(FileValue* fv) {
      childNames = fv;
    }
    /* --------------------------- */
    FileValue* getNumChildren() {
      return numChildren;
    }
    void setNumChildren(FileValue* fv) {
      numChildren = fv;
    }
    /* --------------------------- */
    FileValue* getChildEventDef() {
      return childEventDef;
    }
    void setChildEventDef(FileValue* fv) {
      childEventDef = fv;
    }
    /* --------------------------- */
    FileValue* getFrequency() {
      return frequency;
    }
    void setFrequency(FileValue* fv) {
      frequency = fv;
    }
    /* --------------------------- */
    FileValue* getLoudness() {
      return loudness;
    }
    void setLoudness(FileValue* fv) {
      loudness = fv;
    }
    /* --------------------------- */
    FileValue* getSpatialization() {
      return spatialization;
    }
    void setSpatialization(FileValue* fv) {
      spatialization = fv;
    }
    /* --------------------------- */
    FileValue* getReverberation() {
      return reverberation;
    }
    void setReverberation(FileValue* fv) {
      reverberation = fv;
    }
    /* --------------------------- */
    FileValue* getModifiers() {
      return modifiers;
    }
    void setModifiers(FileValue* fv) {
      modifiers = fv;
    }
    /* --------------------------- */
    FileValue* getNumPartials() {
      return numPartials;
    }
    void setNumPartials(FileValue* fv) {
      numPartials = fv;
    }
    /* --------------------------- */
    FileValue* getDeviation() {
      return deviation;
    }
    void setDeviation(FileValue* fv) {
      deviation = fv;
    }
    /* --------------------------- */
    FileValue* getSpectrum() {
      return spectrum;
    }
    void setSpectrum(FileValue* fv) {
      spectrum = fv;
    }
    /* --------------------------- */
    FileValue* getNotePitchClass() {
      return notePitchClass;
    }
    void setNotePitchClass(FileValue* fv) {
      notePitchClass = fv;
    }
    /* --------------------------- */
    FileValue* getNoteDynamicMark() {
      return noteDynamicMark;
    }
    void setNoteDynamicMark(FileValue* fv) {
      noteDynamicMark = fv;
    }
    /* --------------------------- */
    FileValue* getNoteModifiers() {
      return noteModifiers;
    }
    void setNoteModifiers(FileValue* fv) {
      noteModifiers = fv;
    }
    /* --------------------------- */
    FileValue* getEnvelopeBuilder() {
      return envelopeBuilder;
    }
    void setEnvelopeBuilder(FileValue* fv) {
      envelopeBuilder = fv;
    }
    /* --------------------------- */
    FileValue* getSieveBuilder() {
      return sieveBuilder;
    }
    void setSieveBuilder(FileValue* fv) {
      sieveBuilder = fv;
    }
    /* --------------------------- */
    FileValue* getPatternBuilder() {
      return patternBuilder;
    }
    void setPatternBuilder(FileValue* fv) {
      patternBuilder = fv;
    }

//----------------------------------------------------------------------------//

};

#endif /*EVENTFACTORY_H_*/
