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
//  main.ccp
// 
//  This is the main program for CMOD, the computer-assisted
//  composition software part of the EMCS.
//  It seeds the random number generator, loads the envelope
//  library and calls first the constructor, then the "build"
//  function for the class Top.
//---------------------------------------------------------------------------//


/**
*  This is the main program for CMOD, the computer-assisted composition 
*  software part of the EMCS.  It seeds the random number generator, loads 
*  the envelope library and calls first the constructor, then the "build"
*  function for the class Top.  
*  1/29/07: Justin King added doxygen commenting
**/

//---------------------------------------------------------------------------//

// CMOD includes
#include <stdio.h>
#include <stdlib.h>

#include "define.h"
#include "random.h"
#include "cmodpiece.h"
#include "eventfactory.h"
#include "note.h"
#include "lexyacc/eventparser.h"

//---------------------------------------------------------------------------//
/* THESE ARE ALL GLOBAL VARS IN THIS PROGRAM!!! */
ofstream * outputFile; //
map<string, EventFactory*> factory_lib;
EnvelopeLibrary envlib_cmod;
Score score;
int numChan;
int sever;
CMODPiece Piece;
//---------------------------------------------------------------------------//

/*Calls CMOD-BBS to generate score output.*/
void generateScoreFromShell(void)
{
  system("./cmodbbs");
  system("ls");
  system("evince output.pdf");
}

/**
*  This is the main program.
*  \param argc Count of arguments passed by shell/command line
*  \param argv Array of arguments set by shell/command line, as strings 
*   (char * 's)
*  \retval 0 On success
**/
int main(int argc, char **argv) {

  string pieceName;
  string mainFile;
  EventFactory *mainFactory;
  Event *mainEvent;

  // get mainfile from args or stdin	
  if (argc >= 2) {
    pieceName = argv[1];
  } else {
    cout << "enter the name of the top level: ";
    cin >> pieceName;
    cout << endl;
  }
  mainFile = pieceName + ".dat";

  //load library of envelopes
  string libraryFile = pieceName + ".lib";
  char *temp_str = strdup(libraryFile.c_str());
  envlib_cmod.loadLibrary(temp_str);
  free(temp_str);

  //create output/log file "particel" 
  string out = "cmod-output.xml";                                             //
  outputFile = new ofstream();                                                //
  outputFile->open(out.c_str());                                              //

  //time stamp
  time_t startJob = time(NULL);

  //start random number generator
  *outputFile << "<particel>" << endl;                                        //
  Random::Seed(startJob);
  //Random::Seed((unsigned int)1246381071);
  *outputFile << "  <random>" << startJob << "</random>" << endl;             //
  cout << "\tMain - Parsing " << mainFile << endl;

  // Parse main file
  parseFile(mainFile, NULL, &Piece);

  Piece.Print();
  *outputFile << "  <events>" << endl;                                        //
  
  // set global vars
  numChan = Piece.numChannels;

  cout << "\t\tMain - Creating main event from " << Piece.fileList << endl;
  mainFactory = new EventFactory(Piece.fileList);

  // create mainEvent
  mainEvent = mainFactory->Build(Piece.pieceStartTime, Piece.pieceDuration, 0, 0);
  mainEvent->buildChildEvents();

  // render mainEvent
  if(Piece.soundSynthesis) {
    // mainEvent->buildScore(&s); - We just need an empty score.  
    //Don't know what buildScore is for.
    score.setClippingManagementMode(Score::CHANNEL_ANTICLIP);

    // render in numChans channels, rate
    int sizeFlag;
    switch(Piece.sampleSize) {
    default:
      case 16:
        sizeFlag = _16_BIT_LINEAR;
        break;
      case 24:
        sizeFlag = _24_BIT_LINEAR;
        break;
      case 32:
        sizeFlag = _32_BIT_LINEAR;
        break;
    }

    MultiTrack* renderedScore;
    //Note: multithread isn't working quite right yet... (Cavis)
    if(Piece.numThreads > 1) {
      // Run with multiple threads
      renderedScore = score.render(Piece.numChannels, Piece.sampleRate,
                                  Piece.numThreads);
    } else {
      // Run non multithread version
      renderedScore = score.render(Piece.numChannels, Piece.sampleRate);
    }

    // write to file
    string outputFile1;
    cout << "how do you want to name this sound file ?" << endl;
    cin >> outputFile1;
    outputFile1 = "SoundFiles/" + outputFile1;
    AuWriter::write(*renderedScore, outputFile1);

    delete renderedScore;
  }

  //print any notes to a separate note file -- and print them IN ORDER!
  list<Note> notes = mainEvent->getNotes();
  if (notes.size() > 0) {
    string noteOut = "cmod-notes.xml";
    ofstream noteFile;
    noteFile.open(noteOut.c_str());

    // important --- sort the notes!
    notes.sort();

    noteFile << "<notes>" << endl;
    list<Note>::iterator iter = notes.begin();
    while (iter != notes.end()) {
      noteFile << "  <note>" << endl;
      noteFile << iter->toStringStartTime();
      noteFile << iter->toStringDuration();
      noteFile << iter->toStringOther();
      iter++;
    }
    noteFile << "  </note>" << endl;
    noteFile << "</notes>" << endl;
    noteFile.close();
  }
  
  *outputFile << "  </events>" << endl;                                       //
  *outputFile << "</particel>";
  
  //output the xml for the sounds
  score.xml_print();
  
  //create the score
  generateScoreFromShell();

  //cleanup
  delete mainEvent;
  delete outputFile;

  time_t endJob = time(NULL);
  cout << "This job took " << endJob - startJob << "seconds.\n";
  //cin >> sever;

  delete mainFactory;
}

