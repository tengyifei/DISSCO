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
//   note.cpp
//
//----------------------------------------------------------------------------//

#include "note.h"

//----------------------------------------------------------------------------//

Note::Note(float gblStartTimeSec, float durationSec, int uPerSec, int uPerBar) {
  int sever;
  stimeSec = gblStartTimeSec;
  durSec = durationSec;

  unitsPerSecond = uPerSec;
  unitsPerBar = uPerBar;
/*
  cout << "Note::Note - stimeSec=" << stimeSec << " durSec=" << durSec 
	<< " uPerSec=" << uPerSec << " uPerBar=" << uPerBar << endl;
*/
  // convert seconds into units
  stimeUnits = (int) round(stimeSec * unitsPerSecond); // round to an int
//cout << "		stimeUnits = " << stimeUnits << endl;

  stimeBar = stimeUnits / unitsPerBar;
  stimeBeat = (stimeUnits % unitsPerBar) / unitsPerSecond;
  stimeUnitSubdiv = stimeUnits % unitsPerSecond;
/*
  cout << "	stimeBar=" << stimeBar << " stimeBeat=" << stimeBeat <<
	" stimeUnitSubdiv=" << stimeUnitSubdiv << endl;
*/
  //convert duration from seconds to units
  durUnits = (int) round(durSec * unitsPerSecond);  // round to an int
//cout << "		durUnits = " << durUnits << endl;

  //divide the duration so it can be notated
  durUnitSubBeg = (unitsPerSecond - stimeUnitSubdiv) % unitsPerSecond; //beginning subdivision
  if(durUnitSubBeg > durUnits) durUnitSubBeg = durUnits; //check for out of range 

  durBeat = (durUnits - durUnitSubBeg) / unitsPerSecond; //number of beats
  durUnitSubEnd = durUnits - (durBeat * unitsPerSecond) - durUnitSubBeg; //end subdiv.
/*
  cout << "	durUnitSubBeg=" << durUnitSubBeg << " durBeat=" << durBeat
	<< " durUnitSubEnd=" << durUnitSubEnd << endl;
  cin >> sever;
*/
  unitsPerBeat = unitsPerSecond; //Assuming quarter note = beat = 1 sec
  beatsPerBar = unitsPerBar / unitsPerBeat;
}

//----------------------------------------------------------------------------//

Note::Note(const Note& origNote) {
  unitsPerSecond = origNote.unitsPerSecond;
  unitsPerBar = origNote.unitsPerBar;
  unitsPerBeat = origNote.unitsPerBeat;
  beatsPerBar = origNote.beatsPerBar;

  stimeSec = origNote.stimeSec;
  stimeUnits = origNote.stimeUnits;
  stimeBar = origNote.stimeBar;
  stimeBeat = origNote.stimeBeat;
  stimeUnitSubdiv = origNote.stimeUnitSubdiv;

  durSec = origNote.durSec;
  durUnits = origNote.durUnits;
  durUnitSubBeg = origNote.durUnitSubBeg;

  durBeat = origNote.durBeat;
  durUnitSubEnd = origNote.durUnitSubEnd;

  pitchNum = origNote.pitchNum;
  octaveNum = origNote.octaveNum;
  octavePitch = origNote.octavePitch;
  pitchName = origNote.pitchName;

  loudnessNum = origNote.loudnessNum;
  loudnessMark = origNote.loudnessMark;

  modifiers = origNote.modifiers;
}

//----------------------------------------------------------------------------//

Note& Note::operator= (const Note& rhs) {
  unitsPerSecond = rhs.unitsPerSecond;
  unitsPerBar = rhs.unitsPerBar;
  unitsPerBeat = rhs.unitsPerBeat;
  beatsPerBar = rhs.beatsPerBar;

  stimeSec = rhs.stimeSec;
  stimeUnits = rhs.stimeUnits;
  stimeBar = rhs.stimeBar;
  stimeBeat = rhs.stimeBeat;
  stimeUnitSubdiv = rhs.stimeUnitSubdiv;

  durSec = rhs.durSec;
  durUnits = rhs.durUnits;
  durUnitSubBeg = rhs.durUnitSubBeg;

  durBeat = rhs.durBeat;
  durUnitSubEnd = rhs.durUnitSubEnd;

  pitchNum = rhs.pitchNum;
  octaveNum = rhs.octaveNum;
  octavePitch = rhs.octavePitch;
  pitchName = rhs.pitchName;

  loudnessNum = rhs.loudnessNum;
  loudnessMark = rhs.loudnessMark;

  modifiers = rhs.modifiers;

  return *this;
}

//----------------------------------------------------------------------------//

bool Note::operator< (const Note& rhs) {
  return (stimeSec < rhs.stimeSec);
}

//----------------------------------------------------------------------------//

Note::~Note() {
}

//----------------------------------------------------------------------------//

void Note::setPitchWellTempered( int absPitchNum, vector<string> pitchNames ) {
  pitchNum = absPitchNum;
  octaveNum = pitchNum / pitchNames.size();
  octavePitch = pitchNum % pitchNames.size();

  pitchName = pitchNames[octavePitch];
}

//----------------------------------------------------------------------------//

void Note::setPitchHertz( float freqHz ) {
  // not sure how to do this yet
}

//----------------------------------------------------------------------------//

void Note::setLoudnessMark( int dynamicNum, vector<string> dynamicNames ) {
  loudnessNum = dynamicNum;
  loudnessMark = dynamicNames[loudnessNum];
}

//----------------------------------------------------------------------------//

void Note::setLoudnessSones( float sones ) {
  loudnessNum = -1;

  if (sones < 0 || sones > 256) {
    cerr << "Note received invalid value for sones!" << endl;
    exit(1);
  } if (sones < 8) {
    loudnessMark = "pp";
  } else if (sones < 16) {
    loudnessMark = "p";
  } else if (sones < 32) {
    loudnessMark = "mp";
  } else if (sones < 64) {
    loudnessMark = "mf";
  } else if (sones < 128) {
    loudnessMark = "f";
  } else if (sones <= 256) {
    loudnessMark = "ff";
  }
}

//----------------------------------------------------------------------------//

void Note::setModifiers(vector<string> modNames) {
  for (int i = 0; i < modNames.size(); i++) {
    modifiers.push_back( modNames[i] );
  }
}

//----------------------------------------------------------------------------//

string Note::toStringStartTime(int printLevel) {
  ostringstream buffer;
  string level; for(int i = 0; i < printLevel; i++) level += "  ";
  
  buffer << level << "<units-per-second>" << unitsPerSecond <<
    "</units-per-second>" << endl;
  buffer << level << "<units-per-bar>" << unitsPerBar <<
    "</units-per-bar>" << endl;
  buffer << level << "<units-per-beat>" << unitsPerBeat <<
    "</units-per-beat>" << endl;
  buffer << level << "<beats-per-bar>" << beatsPerBar <<
    "</beats-per-bar>" << endl;
  buffer << level << "<start-time-sec>" << stimeSec <<
    "</start-time-sec>" << endl;
  buffer << level << "<start-time-units>" << stimeUnits <<
    "</start-time-units>" << endl;
  buffer << level << "<start-bar-index>" << stimeBar << 
    "</start-bar-index>" << endl;
  buffer << level << "<start-beat-index>" << stimeBeat <<
    "</start-beat-index>" << endl;
  buffer << level << "<start-subdivision-units>" << stimeUnitSubdiv <<
    "</start-subdivision-units>" << endl;

  return buffer.str();
}

//----------------------------------------------------------------------------//

string Note::toStringStartTimeParticel() {
  ostringstream buffer;

  buffer << "Start: " << setw(8) << stimeSec << "sec    ";
  buffer << "Bar:" << setw(7) << stimeBar << " Beat:" << setw(7) << stimeBeat;

  if (stimeUnitSubdiv > 0) {
    buffer << " + " << setw(5) << stimeUnitSubdiv << "/" << unitsPerSecond;
  }

  return buffer.str();
}

//----------------------------------------------------------------------------//

string Note::toStringDuration(int printLevel) {
  ostringstream buffer;
  string level; for(int i = 0; i < printLevel; i++) level += "  ";
  
  buffer << level << "<duration-sec>" << durSec <<
    "</duration-sec>" << endl;
  buffer << level << "<duration-units>" << durUnits << 
    "</duration-units>" << endl;
  buffer << level << "<duration-unit-sub-beg>" << durUnitSubBeg <<
    "</duration-unit-sub-beg>" << endl;
  buffer << level << "<duration-beat>" << durBeat <<
    "</duration-beat>" << endl;
  buffer << level << "<duration-unit-sub-end>" << durUnitSubEnd <<
    "</duration-unit-sub-end>" << endl;
  
  return buffer.str();
}

//----------------------------------------------------------------------------//

string Note::toStringDurationParticel() {
  ostringstream buffer;

  buffer << "Dur:   " << setw(8) << durSec << "sec    ";
  buffer << setw(4) << "units";

  if (durUnitSubBeg > 0 && durUnitSubBeg < unitsPerSecond) {
    buffer << setw(8) << durUnitSubBeg << "/" << unitsPerSecond << " ";
  } else {
    buffer << setw(13) << " ";
  }

  if (durBeat > 0) {
    buffer << durBeat;
  }
  if (durUnitSubEnd > 0) {
    buffer << " " << durUnitSubEnd << "/" << unitsPerSecond;
  }

  return buffer.str();
}

//----------------------------------------------------------------------------//

string Note::toStringOther(int printLevel) {
  ostringstream buffer;
  string level; for(int i = 0; i < printLevel; i++) level += "  ";

  //notate pitch
  buffer << level << "<pitch-name>" << pitchName << "</pitch-name>" << endl;
  buffer << level << "<octave-num>" << octaveNum << "</octave-num>" << endl;
  buffer << level << "<pitch-num>" << pitchNum << "</pitch-num>" << endl;
  buffer << level << "<pitch-in-octave>" << octavePitch << 
    "</pitch-in-octave>" << endl;
  
  //notate dynamic
  buffer << level << "<loudness>" << loudnessMark << "</loudness>" << endl;
  buffer << level << "<loudness-num>" << loudnessNum <<
    "</loudness-num>" << endl;

  //notate modifiers
  for (int i = 0; i < modifiers.size(); i++) {
    buffer << level << "<modifier>" << modifiers[i] << "</modifier>" << endl;
  }

  return buffer.str();
}

//----------------------------------------------------------------------------//

string Note::toStringOtherParticel() {
  ostringstream buffer;

  //notate pitch
  buffer << setw(9) << pitchName << " " << octaveNum;

  //notate dynamic
  buffer << "    " << loudnessMark;

  //notate modifiers
  buffer << "  ";
  for (int i = 0; i < modifiers.size(); i++) {
    buffer << "  " << modifiers[i];
  }

  return buffer.str();
}

//----------------------------------------------------------------------------//

