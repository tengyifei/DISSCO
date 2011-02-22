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
//   Note.cpp
//
//----------------------------------------------------------------------------//

#include "Note.h"

#include "Event.h"
#include "Output.h"

//----------------------------------------------------------------------------//

Note::Note(TimeSpan ts, Tempo tempo) : ts(ts), tempo(tempo),
  pitchNum(0), octaveNum(0), octavePitch(0), loudnessNum(0) {
}

//----------------------------------------------------------------------------//

Note::Note(const Note& other) {
  ts = other.ts;
  tempo = other.tempo;
  pitchNum = other.pitchNum;
  octaveNum = other.octaveNum;
  octavePitch = other.octavePitch;
  pitchName = other.pitchName;
  loudnessNum = other.loudnessNum;
  loudnessMark = other.loudnessMark;
  modifiers = other.modifiers;
}

//----------------------------------------------------------------------------//

bool Note::operator < (const Note& rhs) {
  //Sort notes by their *global* start time.
  return (ts.start < rhs.ts.start);
}

//----------------------------------------------------------------------------//

void Note::setPitchWellTempered(int absPitchNum, vector<string> pitchNames) {
  pitchNum = absPitchNum;
  octaveNum = pitchNum / pitchNames.size();
  octavePitch = pitchNum % pitchNames.size();
  pitchName = pitchNames[octavePitch];
  Output::addProperty("Pitch Number", pitchNum, "semitones");
  Output::addProperty("Pitch Name", pitchName);
  Output::addProperty("Octave Number", octavePitch);
  Output::addProperty("Pitch In Octave", octavePitch);
}

//----------------------------------------------------------------------------//

void Note::setPitchHertz(float freqHz) {
  //Not sure how to implement this yet.
}

//----------------------------------------------------------------------------//

void Note::setLoudnessMark(int dynamicNum, vector<string> dynamicNames) {
  loudnessNum = dynamicNum;
  loudnessMark = dynamicNames[loudnessNum];
  Output::addProperty("Dynamic", loudnessMark);
  Output::addProperty("Dynamic Level", loudnessNum);
}

//----------------------------------------------------------------------------//

void Note::setLoudnessSones(float sones) {
  loudnessNum = -1;
  if(sones < 0 || sones > 256) {
    cerr << "Note received invalid value for sones!" << endl;
    exit(1);
  }
  else if(sones < 8)
    loudnessMark = "pp";
  else if(sones < 16)
    loudnessMark = "p";
  else if(sones < 32)
    loudnessMark = "mp";
  else if(sones < 64)
    loudnessMark = "mf";
  else if(sones < 128)
    loudnessMark = "f";
  else if(sones <= 256)
    loudnessMark = "ff";
}

//----------------------------------------------------------------------------//

void Note::setModifiers(vector<string> modNames) {
  for(int i = 0; i < modNames.size(); i++) {
    modifiers.push_back(modNames[i]);
    Output::addProperty("Modifier", modNames[i]);
  }
}

