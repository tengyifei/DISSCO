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


//---------------------------------------------------------------------------//
#include "Define.h"
#include "Rational.h"
#include "Tempo.h"

void developerMode(void)
{
  cout << "Working in developer mode. Currently only Andrew uses this mode."
    << endl;

  //Ratio test
  while(false)
  {
    cout << "Enter ratio: ";
    string entry;
    cin >> entry;
    Ratio r(entry);
    cout << "Ratio is expressed as: " << r << endl;
  }
  
  //Tempo parse test
  while(true)
  {
    Tempo t;
    
    string entry1, entry2, entry3;
    cout << "Enter tempo: "; cin >> entry1;
    cout << "Enter time signature: "; cin >> entry2;
    cout << "Enter EDU per time-sig beat: "; cin >> entry3;
    t.setTempo(entry1);
    t.setTimeSignature(entry2);
    t.setEDUPerTimeSignatureBeat(entry3);
    
    cout << "-------------------------------" << endl;
    cout << "getTimeSignatureBeatsPerBar: " << t.getTimeSignatureBeatsPerBar() << endl;
    cout << "getTimeSignatureBeat: " << t.getTimeSignatureBeat() << endl;
    cout << "getTempoBeatsPerMinute: " << t.getTempoBeatsPerMinute() << endl;
    cout << "getTempoBeat: " << t.getTempoBeat() << endl;
    cout << "getTempoBeatsPerBar: " << t.getTempoBeatsPerBar() << endl;
    cout << "getTimeSignatureBeatsPerMinute: " << t.getTimeSignatureBeatsPerMinute() << endl;
    cout << "getTempoBeatDurationInSeconds: " << t.getTempoBeatDurationInSeconds() << endl;
    cout << "getTimeSignatureBeatDurationInSeconds: " << t.getTimeSignatureBeatDurationInSeconds() << endl;
    cout << "getEDUPerTimeSignatureBeat: " << t.getEDUPerTimeSignatureBeat() << endl;
    cout << "getEDUPerBar: " << t.getEDUPerBar() << endl;
    cout << "getEDUPerTempoBeat: " << t.getEDUPerTempoBeat() << endl;
    cout << "getEDUPerSecond: " << t.getEDUPerSecond() << endl;
    cout << "getEDUDurationInSeconds: " << t.getEDUDurationInSeconds() << endl;
    cout << "-------------------------------" << endl;
  }
}



//---------------------------------------------------------------------------//







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

// CMOD includes
#include "Define.h"
#include "Random.h"
#include "Piece.h"
#include "EventFactory.h"
#include "Note.h"
#include "EventParser.h"

//---------------------------------------------------------------------------//
/* THESE ARE ALL GLOBAL VARS IN THIS PROGRAM!!! */
ofstream * outputFile; //
ofstream * outFile; //
map<string, EventFactory*> factory_lib;
EnvelopeLibrary envlib_cmod;
Score score;
int numChan;
int sever;
Piece Piece;
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

  cout << "Would you like to run developer mode (only for internal use)? ";
  char answer; cin >> answer;
  if(answer == 'y')
  {
    developerMode();
    return 0;
  }
  
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

  string part = pieceName + ".particel";
  outFile = new ofstream();
  outFile -> open(part.c_str());

  //time stamp
  long seedTime = time(NULL);
  time_t startJob = seedTime;
  * outFile << "  <random>" << startJob << endl;

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
    string noteOutput = pieceName + ".note";
    ofstream noteFile;
    ofstream notePrint;

    noteFile.open(noteOut.c_str());
    notePrint.open(noteOutput.c_str());

    // important --- sort the notes!
    notes.sort();

    noteFile << "<notes>" << endl;
    list<Note>::iterator iter = notes.begin();
    while (iter != notes.end()) {
      noteFile << "  <note>" << endl;
      noteFile << iter->toStringStartTime();
      noteFile << iter->toStringDuration();
      noteFile << iter->toStringOther();

      notePrint << "***************************************************" << endl;
      notePrint << "  " << iter->toStringStartTimeParticel() << endl;
      notePrint << "  " << iter->toStringDurationParticel() << endl;
      notePrint << "  " << iter->toStringOtherParticel() << endl;

      iter++;
    }
    noteFile << "  </note>" << endl;
    noteFile << "</notes>" << endl;
    noteFile.close();

    notePrint<< "***************************************************" << endl 
  	     << endl;
    notePrint.close();
  }
  
  *outputFile << "  </events>" << endl;                                       //
  *outputFile << "</particel>";
  
  //output the xml for the sounds
  score.xml_print();
  
  //create the score
  //generateScoreFromShell();

  //cleanup
  delete mainEvent;
  delete outputFile;
  delete outFile;

  time_t endJob = time(NULL);
  cout << "This job started " << startJob << " ended " << endJob << " it took "
       << endJob - startJob << "seconds.\n";
  //cin >> sever;

  delete mainFactory;
}

