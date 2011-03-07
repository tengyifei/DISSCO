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

#include "Piece.h"

int main(int parameterCount, char **parameterList) {
  
  //Determine settings.
  cout << endl;
  cout << "=========================SETTINGS==========================" << endl;
 
  //Determine the project path.
  string path;
  if(parameterCount >= 2)
    path = parameterList[1]; 
  if(path == "--help" || path == "-help" || path == "help") {
    cout << "Usage: cmod          Runs CMOD in the current directory." << endl;
    cout << "       cmod <path>   Runs CMOD in the <path> directory." << endl;
    cout << "       cmod help   Displays this help." << endl;
    return 0;
  }
  path = PieceHelper::getFixedPath(path);
  cout << "Working in path: " << path << endl;
  
  //Determine the project name.
  string projectName = PieceHelper::getProjectName(path);
  if(projectName == "")
    return 0;
  cout << "Project name: " << projectName << endl;
  
  //Determine the project seed.
  string seed = PieceHelper::getSeed(path);
  int seedNumber = PieceHelper::getSeedNumber(seed);
  cout << "Seed: " << seedNumber << endl;
  
  //Determine project sound file output.
  PieceHelper::createSoundFilesDirectory(path);
  PieceHelper::createScoreFilesDirectory(path);
  string soundFilename = PieceHelper::getNextSoundFile(path, projectName);
  if(soundFilename == "") {
    cout << "A new soundfile name could not be reserved." << endl;
    return 0;
  }
  cout << "New soundfile name: " << soundFilename << endl;
  cout << "===========================================================" << endl;
  cout << endl;  
  
  //Create the piece!
  PieceHelper::createPiece(path, projectName, seed, soundFilename);
  
  return 0;
}

