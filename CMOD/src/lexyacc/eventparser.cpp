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

#include "eventparser.h"
//#include "parser.y"
#include "parser.h"
#include "../filevalue.h"
#include "../eventfactory.h"

//----------------------------------------------------------------------------//

struct ltstr {
  bool operator()(const char* s1, const char* s2) const {
    return strcmp(s1,s2) < 0;
  }
};

//----------------------------------------------------------------------------//

std::map<const char*, FileValue*, ltstr> file_data;
//#define file_data (*file_data_ptr)

//----------------------------------------------------------------------------//

typedef struct EvKeyEval_s {
  const char *s;
  void (EventFactory::*fptr)(FileValue *arg);
} EvKeyEval;

//----------------------------------------------------------------------------//

typedef struct GbKeyEval_s {
  const char *s;
  void (CMODPiece::*fptr)(FileValue *arg);
} GbKeyEval;

//----------------------------------------------------------------------------//

      /**************************************************************
       *							    *
       * Modify this section to add new keywords.		    *
       *							    *
       * Store all key and function pairs in the 'xxkeys' array.    *
       * The {"",NULL} line MUST be the last.  Do NOT remove it!    *
       **************************************************************/

//global keys - referring to the entire piece (in main.cpp)
GbKeyEval gbkeys[] = { 
  {"title", &CMODPiece::setTitle},
  {"fileFlags", &CMODPiece::setFileFlags},
  {"fileList", &CMODPiece::setFileList},
  {"pieceStartTime", &CMODPiece::setPieceStartTime},
  {"pieceDuration", &CMODPiece::setPieceDuration},
  {"soundSynthesis", &CMODPiece::setSoundSynthesis},
  {"numChannels", &CMODPiece::setNumChannels},
  {"sampleRate", &CMODPiece::setSampleRate},
  {"sampleSize", &CMODPiece::setSampleSize},
  {"numThreads", &CMODPiece::setNumThreads},

  /* Insert more keyword/function pairs above this line. */
  {"", NULL}
};

//event keys - referring to various events
EvKeyEval evkeys[] = {
  // All Events
  {"maxChildDur", &EventFactory::setMaxChildDur},
  {"unitsPerSecond", &EventFactory::setUnitsPerSecond},
  {"unitsPerBar",  &EventFactory::setUnitsPerBar},
  {"childNames", &EventFactory::setChildNames},
  {"numChildren", &EventFactory::setNumChildren},
  {"childEventDef", &EventFactory::setChildEventDef},
  // for Bottom Events
  {"frequency", &EventFactory::setFrequency},
  {"loudness", &EventFactory::setLoudness},
  {"spatialization", &EventFactory::setSpatialization},
  {"reverberation", &EventFactory::setReverberation},
  {"modifiers", &EventFactory::setModifiers},
  // for sounds
  {"numPartials", &EventFactory::setNumPartials},
  {"deviation", &EventFactory::setDeviation},
  {"spectrum", &EventFactory::setSpectrum},
  // for notes
  {"notePitchClass", &EventFactory::setNotePitchClass},
  {"noteDynamicMark", &EventFactory::setNoteDynamicMark},
  {"noteModifiers", &EventFactory::setNoteModifiers},
  // Special cases (reading objs from external files)
  {"envelopeBuilder", &EventFactory::setEnvelopeBuilder},
  {"sieveBuilder", &EventFactory::setSieveBuilder},
  {"patternBuilder", &EventFactory::setPatternBuilder},

  /* Insert more keyword/function pairs above this line. */
  {"", NULL}
};

//----------------------------------------------------------------------------//

int parseFile(string filename, EventFactory* ef, CMODPiece *piece) {
  setFile(filename);
  //file_data_ptr = new std::map<char*, FileValue*>;
  yyparse();

  int index = 0;

  if (file_data["_global"] == NULL) {
    //taking care of parameters for particular events
    if (ef == NULL) return 0;

    while (evkeys[index].s != "" && evkeys[index].fptr != NULL) {

      if(!file_data[evkeys[index].s]) {
        FileValue *tmp = new FileValue();
        tmp->setOrigType(FVAL_NULL);
        (ef->*(evkeys[index].fptr))(tmp);
      }else {
        (ef->*(evkeys[index].fptr))(file_data[evkeys[index].s]);
      }
      index++;
    }
  } else {
    //taking care of the global parameters for the entire piece
    if (piece == NULL) return 0;

    while (gbkeys[index].s != "" && gbkeys[index].fptr != NULL) {

      if (gbkeys[index].fptr == NULL) {
        index++;
        continue;
      }

      if(!file_data[gbkeys[index].s]) {
        FileValue *tmp = new FileValue();
        tmp->setOrigType(FVAL_NULL);
        (piece->*(gbkeys[index].fptr))(tmp);
      }else {
        (piece->*(gbkeys[index].fptr))(file_data[gbkeys[index].s]);
      }
      index++;
    }
  }

  file_data.clear();
  return 1;
}


//----------------------------------------------------------------------------//

int setFile(string filename) {
  FILE *yytmp;
  extern FILE *yyin;
  yytmp = fopen(filename.c_str(), "r");

  if (yytmp == NULL) {
    cout << "ERROR: File " << filename << " does not exist!" << endl;
    exit(1);
  }

  yyin = yytmp;
  return 1;
}
