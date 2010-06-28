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
//  note.h
//
//----------------------------------------------------------------------------//

#ifndef NOTE_H
#define NOTE_H

// CMOD includes
#include "libraries.h"

#include "define.h"

//----------------------------------------------------------------------------//

class Note {
  private:
    /*Careful: these are note units. Note units allow for the quantization of
    complex rhythms and have nothing to do with the global units (i.e. that 
    sounds events use). A note unit can be defined as the smallest possible
    mensural increment that will be used. The note unit will correspond exactly
    to a specific rhythmic duration which can be calculated using beatsPerBar
    and unitsPerBeat.

    Current assumptions: a beat is a quarter note.
    */
    
    //Conversion metrics
    int unitsPerSecond;   //note units per second--for this particular note
    int unitsPerBar;      //note units per bar
    int unitsPerBeat;     //note units per beat
    int beatsPerBar;      //number of beats per bar

    //Start time metrics
    float stimeSec;       //absolute start time in seconds
    int stimeUnits;       //absolute start time of the pitch (in note units)
    int stimeBar;         //index of bar (0 = measure 1, 1 = measure 2, etc.)
    int stimeBeat;        //index of beat within the bar (0 = beat 1)
    int stimeUnitSubdiv;  //start time within the beat (in note units)

    //Duration metrics
    float durSec;         //duration in seconds
    int durUnits;         //duration in note units
    int durUnitSubBeg;    /*how much of the duration is within the starting beat
                            in note units (or 0 if the note starts on a beat)*/
    int durBeat;          /*how many full beats are in the duration, after
                            completing 'durUnitSubBeg'*/
    int durUnitSubEnd;    //how much duration is left at the end (in units)

    //Pitch
    int pitchNum;          //absolute numeric value of the pitch
    int octaveNum;         //the octave the pitch is in
    int octavePitch;       //the number of the pitch within the octave
    std::string pitchName; //the string name of this pitch

    //Dynamic marking
    int loudnessNum;
    std::string loudnessMark;  //dynamic marking (i.e. "ff")

    //Modifiers
    std::vector<std::string> modifiers; //string names of the modifiers

  public:
   /**
    *  Default constructor for a Note.
    **/
    Note(){}

    /**
    *  Generic constructor.  Note_pitchClass, note_dynamicMark, and 
    *  note_modifiers are needed to translate notes into letter representation 
    *  and other score markings.
    *  \param gblStartTimeSec Start time of this note in seconds
    *  \param durSec Duration in seconds
    *  \param unitsPerSecond Units per second
    *  \param unitsPerBar Units per bar
    **/
    Note(float gblStartTimeSec, float durSec, int unitsPerSecond, int unitsPerBar);

    /**
     *  Note copy constructor.
     *	\param origNote Note object to make a copy of
     **/
    Note(const Note& origNote);

    /**
     *	Assignment operator
     *	\param rhs The Note to assign
     **/
    Note& operator= (const Note& rhs);

    /**
     *  Comparison operator (to sort in a list)
     *  \param rhs the object to compare to
     **/
    bool operator< (const Note& rhs);

    /**
     *	Note destructor.
     **/
    ~Note();

//-----------------------         ----------------------------------------//
    /**
     *  Assigns the pitch of a note
     *  \param absPitchNum Pitch on the well-tempered scale, starting with 0 = C0
     *  \param pitchNames The names of the pitches ( C, C#, D, Eb ... )
     **/
    void setPitchWellTempered( int absPitchNum, std::vector<std::string> pitchNames );

    /**
     *  Assigns the pitch of a note
     *  \param freqHz The frequency of the note in Hz
     *  \note this assumes a western, equal tempered scale
     **/
    void setPitchHertz( float freqHz );

    /**
     *  Assigns the loudness of a note
     *  \param dynamicNum The index into the noteDynamicMark array
     *  \param dynamicNames The names of the dynamics ( mf, f, ppp ...)
     **/
    void setLoudnessMark( int dynamicNum, std::vector<std::string> dynamicNames );

    /**
     *  Assigns the loudness of a note
     *  \param sones the loudness of the note in Sones (0-256)
     *  \note this translates sones into "ff", "mf" , "pp", etc.
     **/
    void setLoudnessSones( float sones );

    /**
     *  Assigns any modifiers to the sound
     *  \param modNums
     *  \param modNames
     **/
    void setModifiers(std::vector<std::string> modNames);

//----------------------------------------------------------------------------//
    /**
     *  Output the note start time as a string
     **/
    std::string toStringStartTime(int printLevel=2);

    /**
     *  Output the note start time as a string for outFile .particel
     **/
    std::string toStringStartTimeParticel();

    /**
     *  Output the note duration as a string
     **/
    std::string toStringDuration(int printLevel=2);

    /**
     *  Output the note duration as a string for outFile .particel
     **/
    std::string toStringDurationParticel();

    /**
     *  Output the other note attributes as a string
     **/
    std::string toStringOther(int printLevel=2);

    /**
     *  Output the other note attributes as a string for outFile .particel
     **/
    std::string toStringOtherParticel();

//----------------------------------------------------------------------------//


};

#endif /* NOTE_H */
