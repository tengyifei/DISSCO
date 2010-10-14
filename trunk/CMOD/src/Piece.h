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
//  Piece.h
//
//----------------------------------------------------------------------------//

#ifndef PIECE_H
#define PIECE_H

#include "Libraries.h"
#include "Define.h"

//---------------------------------------------------------------------------//

struct PieceHelper {

  static int getDirectoryList(string dir, vector<string> &files);
  
  static string getFixedPath(string path);
  
  static string getProjectName(string path);
  
  static string getSeedFile(string path);
  
  static string getSeed(string path);
  
  static int getSeedNumber(string seed);
  
  static void createSoundFilesDirectory(string path);
  
  static bool doesFileExist(string path, string filename);
  
  static string getNextSoundFile(string path, string projectName);

  static void createPiece(string path, string projectName, string seed,
    string soundFilename);
};

//---------------------------------------------------------------------------//
class FileValue;

class Piece {
  public:
    std::string title;
    std::string fileFlags;
    std::string fileList;
    bool soundSynthesis;
    int numChannels;
    int sampleRate;
    int sampleSize;
    int numThreads;
    double pieceStartTime;
    double pieceDuration;

//---------------------------------------------------------------------------//
    /**
     *  Default constructor for a Piece.
     **/
    Piece(){}

    /**
     *  Piece copy constructor.
     *	\param orig Piece object to make a copy of
     **/
    Piece(const Piece& orig);

    /**
     *	Assignment operator
     *	\param rhs The Piece to assign
     **/
    Piece& operator= (const Piece& rhs);

    /**
     *	Piece destructor.
     **/
    ~Piece();

//---------------------------------------------------------------------------//
    /**
     *  Sets the title of the piece and prints it
     **/
    void setTitle(FileValue *fv);

    /**
     *  sets the fileFlags
     **/
    void setFileFlags(FileValue *fv);

    /**
     *  Sets the list of files used by current event
     **/
    void setFileList(FileValue *fv);

    /**
     *  Sets a flag identifying if sound synthesis takes place
     **/
    void setSoundSynthesis(FileValue *fv);

    /**
     *  Sets the number of channels for sound synthesis
     **/
    void setNumChannels(FileValue *fv);

    /**
     *  Sets the sample rate for sound synthesis
     **/
    void setSampleRate(FileValue *fv);

    /**
     *  Sets the sample size (in bits) for sound synthesis
     **/
    void setSampleSize(FileValue *fv);

    /**
     *  Sets the number of threads to run
     **/
    void setNumThreads(FileValue *fv);

    /**
     *  Sets the start time for the piece (default is 0)
     **/
    void setPieceStartTime(FileValue *fv);

    /**
     *  Sets the duration of the piece in secounds
     **/
    void setPieceDuration(FileValue *fv);

    /**
     *  Prints information about the piece
     **/
    void Print();

};

//---------------------------------------------------------------------------//

#endif
