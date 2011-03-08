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
//   Piece.cpp
//
//----------------------------------------------------------------------------//

#include "Piece.h"

#include "Define.h"
#include "EventFactory.h"
#include "EventParser.h"
#include "FileValue.h"
#include "Note.h"
#include "Output.h"
#include "Random.h"

//----------------------------------------------------------------------------//

/*These are the global variables in the program. Eventually this should be
reworked to be more C++ friendly.*/
map<string, EventFactory*> factory_lib;
EnvelopeLibrary envlib_cmod;
Score score;
int numChan;
Piece ThePiece;

//----------------------------------------------------------------------------//

void PieceHelper::createPiece(string path, string projectName, string seed,
  string soundFilename) {
  
  //Change working directory.
  chdir(path.c_str());
  
  //Convert seed string to seed number.
  int seedNumber = getSeedNumber(seed);
  
  EventFactory *mainFactory;
  Event *mainEvent;

  //Get main project file.
  string mainFile = projectName + ".dat";

  //Load library of envelopes
  string libraryFile = projectName + ".lib";
  char *tempString = strdup(libraryFile.c_str());
  envlib_cmod.loadLibrary(tempString);
  free(tempString);

  //Initialize the output class.
  string particelFilename = projectName + ".particel";
  Output::initialize(particelFilename);
  Output::beginSubLevel("Piece");
  
  //Seed the random number generator.
  Random::Seed((unsigned int)seedNumber);
  
  //Parse main file.
  cout << "\tMain - Parsing " << mainFile << endl;
  parseFile(mainFile, NULL, &ThePiece);

  //Print out initial description.
  ThePiece.Print();
  
  //Set global variables.
  numChan = ThePiece.numChannels;
  cout << "\t\tMain - Creating main event from " << ThePiece.fileList << endl;
  mainFactory = new EventFactory(ThePiece.fileList);

  //Create the main event.
  Tempo mainTempo; //Though we supply this, "Top" will provide its own tempo.
  TimeSpan pieceSpan;
  pieceSpan.start = ThePiece.pieceStartTime;
  pieceSpan.duration = ThePiece.pieceDuration;
  mainEvent = mainFactory->Build(pieceSpan, 0, mainTempo);
  mainEvent->buildChildEvents();
  
  //Finish particel output and free up the Output class members.
  Output::endSubLevel();
  
  cout << endl;
  cout << "-----------------------------------------------------------" <<
    endl;
  cout << "Build complete." << endl;
  cout << "-----------------------------------------------------------" <<
    endl << endl;
  cout.flush();
  
  //Write the XML output.
  string xmlFilename = projectName + ".xml";
  Output::exportToXML(xmlFilename);
  string fir = "firefox ";
  fir.append(xmlFilename);
  fir.append(" &");
  //system(fir.c_str());

  //Write the FOMUS output.
  string fomusFilename = "ScoreFiles/";
  fomusFilename += projectName;
  fomusFilename += "_";
  Output::exportToFOMUS(fomusFilename);
  system("rm -f ScoreFiles/*.ps ScoreFiles/*.ly");
  {
    cout << "Would you like to open up the score files in Firefox (y/n)? ";
    char response;
    cin >> response;
    if(response == 'y' || response == 'Y')
      system("firefox ScoreFiles/*.svg &");
  }
  cout << endl;
  Output::free();
  
  //Render sound.
  if(ThePiece.soundSynthesis) {
    //Set clipping mode.
    score.setClippingManagementMode(Score::CHANNEL_ANTICLIP);

    //Note: multithread isn't working quite right yet, so run in single thread.
    MultiTrack* renderedScore = 
      score.render(ThePiece.numChannels, ThePiece.sampleRate);

    //Write to file.
    AuWriter::write(*renderedScore, soundFilename);
    cout << endl;
    cout << "-----------------------------------------------------------" <<
      endl;
    cout << "Wrote: " << soundFilename << endl;
    cout << endl;
    cout << "-----------------------------------------------------------" <<
      endl;
    cout.flush();
    string aud = "audacity ";
    aud.append(soundFilename);
    aud.append(" &");
    cout << "Would you like to open up the soundfile in Audacity (y/n)? ";
    char response;
    cin >> response;
    if(response == 'y' || response == 'Y')
      system(aud.c_str());
        
    //Clean up.
    delete renderedScore;
  }

  //Clean up.
  delete mainEvent;
  delete mainFactory;
  
  cout << "Finished with CMOD." << endl;
  cout.flush();
}

//----------------------------------------------------------------------------//

int PieceHelper::getDirectoryList(string dir, vector<string> &files) {
  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(dir.c_str())) == NULL) {
    cout << "Error(" << errno << ") opening " << dir << endl;
    return errno;
  }

  while ((dirp = readdir(dp)) != NULL) {
    files.push_back(string(dirp->d_name));
  }
  closedir(dp);
  return 0;
}

//----------------------------------------------------------------------------//

string PieceHelper::getFixedPath(string path) {
  if(path == "")
    return "./";
  if(path.c_str()[path.length() - 1] != '/')
    path = path + "/";
  return path;
}

//----------------------------------------------------------------------------//

string PieceHelper::getProjectName(string path) {
  string dir = string(path);
  vector<string> files = vector<string>();
  getDirectoryList(dir, files);
  string g = "";
  for(unsigned int i = 0; i < files.size(); i++) {
    string f = files[i];
    if(f.length() >= 5 && f.substr(f.length() - 3, 3) == "dat") {
      g = f.erase(f.length() - 4);
      break;
    }
  }
  
  if(g == "") {
    cout << endl;
    cout << "=========================================================" << endl;
    cout << "Warning: there is no CMOD project file in this directory." << endl;
    cout << "=========================================================" << endl;
    cout << "Files in '" << path << "':" << endl;
    
    for(unsigned int i = 0; i < files.size(); i++)
      cout << files[i] << endl;
    cout << "=========================================================" << endl;
    cout << endl;
  }
  
  return g;
}

//----------------------------------------------------------------------------//

string PieceHelper::getSeedFile(string path) {
  string dir = string(path);
  vector<string> files = vector<string>();
  getDirectoryList(dir, files);
  string g = "";
  for(unsigned int i = 0; i < files.size(); i++) {
    string f = files[i];
    if(f.length() >= 6 && f.substr(f.length() - 4, 4) == "seed") {
      g = f.erase(f.length() - 5);
      break;
    }
  }
  return g;
}

//----------------------------------------------------------------------------//

string PieceHelper::getSeed(string path) {
  string preexistingSeed = getSeedFile(path);
  if(preexistingSeed != "")
    return preexistingSeed;
  cout << endl;
  cout << "-----------------------------------------------------------" << endl;
  cout << "Seed generation note: to bypass seed entry, create a file " << endl;
  cout << "in the project directory called '<seed>.seed' where <seed> " << endl;
  cout << "is your seed word or seed number." << endl;
  cout << "-----------------------------------------------------------" << endl;
  cout << endl;
  cout << "Enter a seed word or number: ";
  string s; cin >> s;
  return s;
}

//----------------------------------------------------------------------------//

int PieceHelper::getSeedNumber(string seed) {
  if(seed == "time") {
    int now = (int)time(0);
    cout << "Using current time " << now << " as seed." << endl;
    return now;
  }
  
  const char* seed_c = seed.c_str();
  int seedNumber = 0;
  int digits = 1;
  for(int i = 0; i < seed.length(); i++)
    digits *= 10;
  for(int i = 0; seed_c[i] != 0; i++) {
    digits /= 10;
    seedNumber += digits * (int)(seed_c[i] - '0');
  }
  if(seedNumber < 0) seedNumber = -seedNumber;
  return seedNumber;
}

//----------------------------------------------------------------------------//

void PieceHelper::createSoundFilesDirectory(string path) {
  string dir = string(path);
  vector<string> files = vector<string>();
  getDirectoryList(dir, files);
  string g = "";
  for(unsigned int i = 0; i < files.size(); i++) {
    if(files[i] == "SoundFiles")
      return;
  }
  
  string h = "mkdir " + path + "SoundFiles";
  system(h.c_str());
}

//----------------------------------------------------------------------------//

void PieceHelper::createScoreFilesDirectory(string path) {
  string dir = string(path);
  vector<string> files = vector<string>();
  getDirectoryList(dir, files);
  string g = "";
  bool dirExists = false;
  for(unsigned int i = 0; i < files.size(); i++) {
    if(files[i] == "ScoreFiles") {
      dirExists = true;
      break;
    }
  }
  
  string h = "mkdir " + path + "ScoreFiles";
  if(!dirExists)
    system(h.c_str());
  h = "rm -f " + path + "ScoreFiles/*.fms";
  system(h.c_str());
}

//----------------------------------------------------------------------------//

bool PieceHelper::doesFileExist(string path, string filename) {
  string dir = string(path);
  vector<string> files = vector<string>();
  getDirectoryList(dir, files);
  string g = "";
  for(unsigned int i = 0; i < files.size(); i++) {
    if(files[i] == filename)
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------//

string PieceHelper::getNextSoundFile(string path, string projectName) {
  path = path + "SoundFiles/";
  for(int i = 1; i < 10000; i++) {
    stringstream oss;
    oss << projectName << i << ".aiff";
    if(doesFileExist(path, oss.str()))
      continue;
    else
      return "SoundFiles/" + oss.str();
  }
  return "";
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
  Output::beginSubLevel("Information");
    Output::beginSubLevel("General");
      Output::addProperty("Title", title);
    Output::endSubLevel();
    
    Output::beginSubLevel("Audio");
      Output::addProperty("Using Synthesis", soundSynthesis ? "Yes" : "No");
      Output::addProperty("Channels", numChannels);
      Output::addProperty("Sample Rate", sampleRate, "Hz");
      Output::addProperty("Sample Resolution", sampleSize, "bits");
    Output::endSubLevel();
    
    Output::beginSubLevel("Execution");
      Output::addProperty("Seed", "(unspecified)");
      Output::addProperty("Threads", numThreads);
      Output::addProperty("File Flags", fileFlags);
      Output::addProperty("File List", fileList);
    Output::endSubLevel();
  Output::endSubLevel();
}

