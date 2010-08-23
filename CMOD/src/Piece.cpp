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
//   piece.cpp
//
//----------------------------------------------------------------------------//

#include "Piece.h"
#include "FileValue.h"

extern ofstream * outputFile;
//----------------------------------------------------------------------------//

Piece::Piece(const Piece& orig) {
  title = orig.title;
  fileFlags = orig.fileFlags;
  fileList = orig.fileList;
  soundSynthesis = orig.soundSynthesis;
  numChannels = orig.numChannels;
  sampleRate = orig.sampleRate;
  sampleSize = orig.sampleSize;
  numThreads = orig.numThreads;
  pieceStartTime = orig.pieceStartTime;
  pieceDuration = orig.pieceDuration;
}

//----------------------------------------------------------------------------//

Piece& Piece::operator= (const Piece& rhs) {
  title = rhs.title;
  fileFlags = rhs.fileFlags;
  fileList = rhs.fileList;
  soundSynthesis = rhs.soundSynthesis;
  numChannels = rhs.numChannels;
  sampleRate = rhs.sampleRate;
  sampleSize = rhs.sampleSize;
  numThreads = rhs.numThreads;
  pieceStartTime = rhs.pieceStartTime;
  pieceDuration = rhs.pieceDuration;
}

//----------------------------------------------------------------------------//

Piece::~Piece() {
}

//----------------------------------------------------------------------------//

void Piece::setTitle(FileValue *fv) {
  fv->Evaluate();
  if (!fv->isString()) cout << "Error: Type is " << fv->TypeToString() << 
        ".  setTitle expects String." << endl;
  title = fv->getString();
  cout << "Piece - setTitle: " << title << endl;
}

//----------------------------------------------------------------------------//

void Piece::setFileFlags(FileValue *fv) {
  fv->Evaluate();
  if (!fv->isString()) cout << "Error: Type is " << fv->TypeToString() << 
        ".  setFileFlags expects String." << endl;
  fileFlags = fv->getString(); 
}

//----------------------------------------------------------------------------//

void Piece::setFileList(FileValue *fv) { 
  fv->Evaluate();
  if (!fv->isString()) cout << "Error: Type is " << fv->TypeToString() << 
        ".  setFileList expects String." << endl;
  fileList = fv->getString();
}

//----------------------------------------------------------------------------//

void Piece::setSoundSynthesis(FileValue *fv) { 
  fv->Evaluate();
  if (!fv->isNumber()) cout << "Error: Type is " << fv->TypeToString() << 
        ".  setSoundSynthesis expects Number." << endl;
  soundSynthesis = (bool)fv->getFloat();
}

//----------------------------------------------------------------------------//

void Piece::setNumChannels(FileValue *fv) { 
  fv->Evaluate();
  if (!fv->isNumber()) cout << "Error: Type is " << fv->TypeToString() << 
        ".  setNumChannels expects Number." << endl;
  numChannels = fv->getInt();
}

//----------------------------------------------------------------------------//

void Piece::setSampleRate(FileValue *fv) { 
  fv->Evaluate();
  if (!fv->isNumber()) cout << "Error: Type is " << fv->TypeToString() << 
        ".  setSampleRate expects Number." << endl;
  sampleRate = fv->getInt();
}

//----------------------------------------------------------------------------//

void Piece::setSampleSize(FileValue *fv) { 
  fv->Evaluate();
  if (!fv->isNumber()) cout << "Error: Type is " << fv->TypeToString() << 
        ".  setSampleSize expects Number." << endl;
  sampleSize = fv->getInt();
}

//----------------------------------------------------------------------------//

void Piece::setNumThreads(FileValue *fv) {
  fv->Evaluate();
  if (!fv->isNumber()) cout << "Error: Type is " << fv->TypeToString() << 
        ".  setNumThreads expects Number." << endl;
  numThreads = fv->getInt();
}

//----------------------------------------------------------------------------//

void Piece::setPieceStartTime(FileValue *fv) {
  fv->Evaluate();
  if (!fv->isNumber()) cout << "Error: Type is " << fv->TypeToString() << 
        ".  setStartTime expects Number." << endl;
  pieceStartTime = fv->getInt();
}

//----------------------------------------------------------------------------//

void Piece::setPieceDuration(FileValue *fv) {
  fv->Evaluate();
  if (!fv->isNumber()) cout << "Error: Type is " << fv->TypeToString() << 
        ".  setDuration expects Number." << endl;
  pieceDuration = fv->getFloat();
}

//----------------------------------------------------------------------------//

void Piece::Print() {
  *outputFile << "  <title>" << title << "</title>" << endl <<                //
    "  <start-time-sec>" << pieceStartTime << "</start-time-sec>" << endl <<  //
    "  <duration-sec>" << pieceDuration << "</duration-sec>" << endl;         //
  if(soundSynthesis) {
    *outputFile << "  <channels>" << numChannels << "</channels>" << endl <<  //
      "  <sample-rate>" << sampleRate << "</sample-rate>" << endl <<          //
      "  <sample-bits>" << sampleSize << "</sample-bits>" << endl <<          //
      "  <threads>" << numThreads << "</threads>" << endl;                    //
  }

  cout << "*****************************" << endl;
  cout << "\t Piece -- " << title << endl
      << "fileFlags: " << fileFlags << endl
      << "fileList: " << fileList << endl
      << "startTime: " << pieceStartTime << endl
      << "duration: " << pieceDuration << endl
      << "soundSynthesis: " << soundSynthesis << endl
      << "numChannels: " << numChannels << endl
      << "sampleRate: " << sampleRate << endl
      << "sampleSize: " << sampleSize << endl
      << "numThreads: " << numThreads << endl;
  cout << "*****************************" << endl;		
}
