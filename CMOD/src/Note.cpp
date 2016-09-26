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
#include <string>

#include <iostream>
#include <fstream>
using namespace std;

ofstream * outputFile;
//extern ofstream * outputFile;

//----------------------------------------------------------------------------//

Note::Note(TimeSpan ts, Tempo tempo) : ts(ts), tempo(tempo),
  pitchNum(0), octaveNum(0), octavePitch(0), loudnessNum(0) {

//if ( &outputFile == " ") {
  outputFile = new std::ofstream;
  outputFile->open( "../SQ/NotationFile.txt", ios::app);
//}
}

//----------------------------------------------------------------------------//

Note::Note() {
}

//----------------------------------------------------------------------------//

Note:: ~Note() {
  delete outputFile;
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

void Note::setPitchWellTempered(int pitchNum) {

//octaveNum = pitchNum / pitchNames.size();
//octavePitch = pitchNum % pitchNames.size();
//pitchName = pitchNames[octavePitch];
  octaveNum = pitchNum / 12;
//*notaFile << "Octave Number " << octaveNum << endl;
  octavePitch = pitchNum % 12;

  if (octavePitch == 0) {
    pitchName = "C";
  } else if (octavePitch == 1) {
    pitchName = "C#";
  } else if (octavePitch == 2) {
    pitchName = "D";
  } else if (octavePitch == 3) {
    pitchName = "Eb";
  } else if (octavePitch == 4) {
    pitchName = "E";
  } else if (octavePitch == 5) {
    pitchName = "F";
  } else if (octavePitch == 6) {
    pitchName = "F#";
  } else if (octavePitch == 7) {
    pitchName = "G";
  } else if (octavePitch == 8) {
    pitchName = "G#";
  } else if (octavePitch == 9) {
    pitchName = "A";
  } else if (octavePitch == 10) {
    pitchName = "Bb";
  } else if (octavePitch == 11) {
    pitchName = "B";
  } else {
    cerr << "Invalid pitchNum or pitchName !" << endl;
  }

  Output::addProperty("Pitch Number", pitchNum, "semitones");
  Output::addProperty("Pitch Name", pitchName);
  Output::addProperty("Octave Number", octaveNum);
  Output::addProperty("Pitch In Octave", octavePitch);

  *outputFile<< setw(5) <<"Pitch " << pitchNum << setw(5) << octaveNum << setw(3) 
       << pitchName;

}

//----------------------------------------------------------------------------//

int Note::HertzToPitch(float freqHz) {

  int pitchNum;

  if ( freqHz >= CEILING || freqHz <= MINFREQ) {
    cerr << "WARNING: frequency out of range" << endl; 
  }

  pitchNum = rint(12 * log2(freqHz / C0));

  return pitchNum;
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
  } else if(sones <= 4) {
    loudnessMark = "ppp";
  } else if(sones <= 8) {
    loudnessMark = "pp";
  } else if(sones <= 16) {
    loudnessMark = "p";
  } else if(sones <= 32) {
    loudnessMark = "mp";
  } else if(sones <= 45) {
    loudnessMark = "mf";
  } else if(sones <= 64) {
    loudnessMark = "f";
  } else if(sones <= 128) {
    loudnessMark = "ff";
  } else if (sones <= 256) {
    loudnessMark = "fff";
  }

  Output::addProperty("Loudness", loudnessMark);

  *outputFile << setw(6) << loudnessMark;
}


//----------------------------------------------------------------------------//

void Note::setModifiers(vector<string> modNames) 
{
  for(int i = 0; i < modNames.size(); i++) {
    modifiers.push_back(modNames[i]);
    Output::addProperty("Playing Techniques:", modNames[i]);

    *outputFile << setw(8) << modNames[i];
  }
  
  *outputFile << endl; *outputFile << endl;
}


//----------------------------------------------------------------------------//

void Note::notateDurations( string aName, string startEDU, string durationEDU) 
{
  int stime, dur, endTime, bar, beat;
  std::vector<int> spellingBee;

  // Get needed parameters
  string attributeName = aName;
  std::stringstream ss(startEDU);
  ss >> stime; 
  std::stringstream ss1(durationEDU);
  ss1 >> dur; 
  endTime = stime + dur;

  int barEDUs = atoi(tempo.getEDUPerBar());
  int beatEDUs = atoi(tempo.getEDUPerTimeSignatureBeat());


  // Barlines, beats, subdivisions
  int numSbar = stime / barEDUs;		//start barline num
  int sBar = numSbar * barEDUs ;		//start barline EDUs
  int numSbeat = (stime - sBar) / beatEDUs;	//start beat num
  int sBeat = stime / beatEDUs * beatEDUs;	//start beat EDUs	
  int sDiv = stime - sBeat;			//start subdivisions EDUs

                                               //beginning subdiv of dur
  int eBegDiv = ((numSbeat + 1) * beatEDUs + sBar - stime) % beatEDUs; 
  int numEbar = endTime / barEDUs;		//end barline num
  int eBar = numEbar * barEDUs;			//end barline EDUs
  if ( eBar == endTime) eBar += 1;
  int numEbeat = (endTime - eBar) / beatEDUs;	//end beat num
  int eBeat = endTime / beatEDUs * beatEDUs;	//end beat RDUs
  int eDiv = endTime - eBeat;			//end subdivisions EDUs

  int durBeats = ((endTime - eDiv) - (stime + sDiv)) / beatEDUs;


  // Duration
  spellingBee.push_back(numSbar);
  spellingBee.push_back(numSbeat);
  spellingBee.push_back(sDiv);

  spellingBee.push_back(eBegDiv);		//3
  spellingBee.push_back(durBeats);
  spellingBee.push_back(eDiv);
  spellingBee.push_back(numEbar);

// Print
  *outputFile << "=========================== " << attributeName;
  *outputFile << " ==========================" << endl;
  *outputFile << "  stime=" << stime << " dur=" << dur << " endTime=" << endTime 
       << endl;
  *outputFile << "   barEDUs=" << barEDUs << " beatEDUs=" << beatEDUs <<  endl;

  *outputFile << "sBar=" << sBar << " numSbar=" << numSbar << endl;
  *outputFile << " sBeat=" << sBeat << " numSbeat=" << numSbeat << " sDiv="
       << sDiv << endl;
  *outputFile << "eBar=" << eBar << " numEbar=" << numEbar << endl;
  *outputFile << " eBeat=" << eBeat << " numEbeat=" << numEbeat << " eDiv="
       << eDiv << endl;

  *outputFile << " " << endl;
  *outputFile << "Bar " << spellingBee.at(0) << " [+" 
              << spellingBee.at(1) << "+" << spellingBee.at(2) << "/" 
              << beatEDUs << "]" << setw(5) << spellingBee.at(3) << "/" 
              << beatEDUs << setw(5) << spellingBee.at(4) << " Beats" 
              << setw(5)<< spellingBee.at(5) << "/" << beatEDUs << setw(13) 
              << "Bar " << spellingBee.at(6) << endl;

}  
