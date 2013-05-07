/*
CMOD (composition module)
Copyright (C) 2005  Sever Tipei (s-tipei@uiuc.edu)
   
   
   Update:
   This class is not yet implemented in the XML version of CMOD. 
   
                                            --Ming-ching Chiu May 06 2013
                                            
                                            

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
//  Note.h
//
//----------------------------------------------------------------------------//

#ifndef NOTE_H
#define NOTE_H

#include "Libraries.h"

#include "Rational.h"
#include "Tempo.h"
#include "TimeSpan.h"

//----------------------------------------------------------------------------//

class Note {

    //------//
    //Rhythm//
    //------//
    
    //The timespan of the note.
    TimeSpan ts;
    
    //The parent tempo.
    Tempo tempo;
    
    //-----//
    //Pitch//
    //-----//
    
    //Absolute numeric value of the pitch
    int pitchNum;
    
    //The octave the pitch is in
    int octaveNum;
    
    //The number of the pitches within the octave
    int octavePitch;
    
    //The string name of this pitch
    std::string pitchName;

    //Dynamic number
    int loudnessNum;
    
    //Dynamic marking (i.e. "ff")
    std::string loudnessMark;

    //Modifiers
    std::vector<std::string> modifiers; //string names of the modifiers

  public:

    //Constructor with timespan and tempo
    Note(TimeSpan ts, Tempo tempo);
    
    //Copy constructor
    Note(const Note& other);

    /**
     *  Comparison operator (to sort in a list)
     *  \param rhs the object to compare to
     **/
    bool operator < (const Note& rhs);

//----------------------------------------------------------------------------//

    /**
     *  Assigns the pitch of a note
     *  \param absPitchNum Pitch on the well-tempered scale, starting with 0=C0
     *  \param pitchNames The names of the pitches (C, C#, D, Eb, ...)
     **/
    void setPitchWellTempered(int absPitchNum,
      std::vector<std::string> pitchNames);

    /**
     *  Assigns the pitch of a note
     *  \param freqHz The frequency of the note in Hz
     *  \note this assumes a western, equal tempered scale
     **/
    void setPitchHertz(float freqHz);

    /**
     *  Assigns the loudness of a note
     *  \param dynamicNum The index into the noteDynamicMark array
     *  \param dynamicNames The names of the dynamics ( mf, f, ppp ...)
     **/
    void setLoudnessMark(int dynamicNum, std::vector<std::string> dynamicNames);

    /**
     *  Assigns the loudness of a note
     *  \param sones the loudness of the note in Sones (0-256)
     *  \note this translates sones into "ff", "mf" , "pp", etc.
     **/
    void setLoudnessSones(float sones);

    /**
     *  Assigns any modifiers to the sound
     *  \param modNums
     *  \param modNames
     **/
    void setModifiers(std::vector<std::string> modNames);
};
#endif /* NOTE_H */

