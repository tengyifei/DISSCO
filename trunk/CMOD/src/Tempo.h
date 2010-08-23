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
//  Tempo.h
//
//----------------------------------------------------------------------------//

#ifndef TEMPO_H
#define TEMPO_H

// CMOD includes
#include "Libraries.h"
#include "Define.h"
#include "Rational.h"

/*
New tempo indications:
tempo
timeSignature
maxChildEDU
EDUPerBeat

//These tags will simply control the same variable.
Deprecating: maxChildDur --> maxChildEDU
Deprecating: unitsPerSecond --> EDUPerBeat

//This tag is being completely removed.
Deprecating and removing: unitsPerBar

EventFactory is created for each Child.
*/

//----------------------------------------------------------------------------//
class Tempo {
  Ratio tempoBeatsPerMinute;
  Ratio tempoBeat;
  Ratio timeSignatureBeat;
  Ratio timeSignatureBeatsPerBar;
  Ratio EDUPerTimeSignatureBeat;
  
  float tempoStartTime;
  
  /**When unitsPerSecond was used, it assumed this tempo (5/4 at quarter=60).*/
  void setBackwardsCompatibleTempo(void)
  {
    tempoBeatsPerMinute = 60;
    tempoBeat = Ratio(1,4);
    timeSignatureBeat = Ratio(1,4);
    timeSignatureBeatsPerBar = 5;
    EDUPerTimeSignatureBeat = 1;
    tempoStartTime = 0;
  }
  
  public:
  //Constructor initializes a backwards-compatible tempo of 5/4 quarter=60.
  Tempo()
  {
    //Default tempo should just be the backwards-compatible one.
    setBackwardsCompatibleTempo();
  }
  
  //Two tempos are identical if they have the same tempo and time signature.
  bool isTempoSameAs(Tempo& other) {
    return tempoBeatsPerMinute == other.tempoBeatsPerMinute &&
      tempoBeat == other.tempoBeat &&
      timeSignatureBeat == other.timeSignatureBeat &&
      timeSignatureBeatsPerBar == other.timeSignatureBeatsPerBar;
  }
  
  float getStartTime(void) {return tempoStartTime;}
  void setStartTime(float newStartTime) {tempoStartTime = newStartTime;}
  
  /*Sets the beats-per-minute using the following format:
  
  <beat-name>|<beat-ratio> = <beats-per-minute>
  
  Examples:
  tempo = 'quarter = 130'
  tempo = '1/4 = 130'
  tempo = 'dotted quarter = 130'
  tempo = '3/8 = 130'

  Tuple equivalents:
  triplet half = 1/3
  triplet quarter = 1/6
  triplet eighth = 1/12
  
  Also accepts decimal for the BPM: "quarter=60.5"  --> 1/4 = 121/2
  */
  void setTempo(string newTempo) {
    int length = (int)newTempo.length();
    string firstHalf, secondHalf;
    bool firstHalfIsNumber = false;
    bool inSecondHalf = false;
    bool countDigits = false;
    bool encounteredSecondHalfSlash = false;
    int digitCount = 0;
    for(int i = 0; i < length; i++) {
      char c = newTempo[i];
      if(c == ' ')
        continue;
      else if(c == '=')
        inSecondHalf = true;
      else if(!inSecondHalf) {
        if((c >= '0' && c <= '9') || c == '/')
          firstHalfIsNumber = true;
        firstHalf.append(1, c);
      }
      else {
        if(c == '.')
          countDigits = true;
        else if(c >= '0' && c <= '9') {
          secondHalf.append(1, c);
          if(countDigits)
            digitCount++;
        }
        else if(c == '/' && !countDigits && !encounteredSecondHalfSlash) {
          encounteredSecondHalfSlash = true;
          secondHalf.append("/");
        }
      }
    }
    
    //A really obscure way to add /10 /100 /1000 etc. depending on digitCount
    if(countDigits) {
      secondHalf.append("/1");
      secondHalf.append(digitCount, '0');
    }
    tempoBeatsPerMinute = secondHalf;
    
    if(firstHalfIsNumber) {
      tempoBeat = firstHalf;
      return;
    }
    
    if(firstHalf.find("thirt") != string::npos)
      tempoBeat = Ratio(1,32);
    else if(firstHalf.find("six") != string::npos)
      tempoBeat = Ratio(1,16);
    else if(firstHalf.find("eig") != string::npos)
      tempoBeat = Ratio(1,8);
    else if(firstHalf.find("quar") != string::npos)
      tempoBeat = Ratio(1,4);
    else if(firstHalf.find("hal") != string::npos)
      tempoBeat = Ratio(1,2);
    else if(firstHalf.find("who") != string::npos)
      tempoBeat = Ratio(1,1);
      
    if(firstHalf.find("doub") != string::npos)
      tempoBeat *= Ratio(7,4);
    else if(firstHalf.find("dot") != string::npos)
      tempoBeat *= Ratio(3,2);
      
    if(firstHalf.find("tripl") != string::npos)
      tempoBeat *= Ratio(2,3);
    else if(firstHalf.find("quin") != string::npos)
      tempoBeat *= Ratio(4,5);
    else if(firstHalf.find("sext") != string::npos)
      tempoBeat *= Ratio(5,6); 
    else if(firstHalf.find("sept") != string::npos)
      tempoBeat *= Ratio(6,7);
  }
  
  void setTimeSignature(string newTimeSignature) {
    int length = (int)newTimeSignature.length();
    string firstHalf, secondHalf;
    bool inSecondHalf = false;
    for(int i = 0; i < length; i++) {
      char c = newTimeSignature[i];
      if(c == ' ')
        continue;
      else if(c == '/')
        inSecondHalf = true;
      else if(c >= '0' && c <= '9') {
        if(!inSecondHalf) {
          firstHalf.append(1, c);
        }
        else {
          secondHalf.append(1, c);
        }
      }
    }
    timeSignatureBeatsPerBar = Ratio(firstHalf);
    timeSignatureBeat = Ratio(1) / Ratio(secondHalf);
  }
  
  void setEDUPerTimeSignatureBeat(string newEDUPerTimeSignatureBeat) {
    EDUPerTimeSignatureBeat = Ratio(newEDUPerTimeSignatureBeat);
  }
  
  Ratio getTimeSignatureBeatsPerBar(void) {
    return timeSignatureBeatsPerBar;
  }
  
  Ratio getTimeSignatureBeat(void) {
    return timeSignatureBeat;
  }
  
  Ratio getTempoBeatsPerMinute(void) {
    return tempoBeatsPerMinute;
  }
  
  Ratio getTempoBeat(void) {
    return tempoBeat;
  }
  
  Ratio getTempoBeatsPerBar(void) {
    return timeSignatureBeatsPerBar * (timeSignatureBeat / tempoBeat);
  }
  
  Ratio getTimeSignatureBeatsPerMinute(void) {
    return tempoBeatsPerMinute * (tempoBeat / timeSignatureBeat);
  }
  
  Ratio getTempoBeatDurationInSeconds(void) {
    return Ratio(60) / tempoBeatsPerMinute;
  }
  
  Ratio getTimeSignatureBeatDurationInSeconds(void) {
    return Ratio(60) / (tempoBeatsPerMinute * (tempoBeat / timeSignatureBeat));
  }
  
  Ratio getEDUPerTimeSignatureBeat(void) {
    return EDUPerTimeSignatureBeat;
  }
    
  Ratio getEDUPerBar(void) {
    return EDUPerTimeSignatureBeat * timeSignatureBeatsPerBar;
  }
  
  Ratio getEDUPerTempoBeat(void) {
    return EDUPerTimeSignatureBeat * (tempoBeat / timeSignatureBeat);
  }
  
  Ratio getEDUPerSecond(void) {
    return EDUPerTimeSignatureBeat *
      getTimeSignatureBeatsPerMinute() / Ratio(60);
  }
  
  Ratio getEDUDurationInSeconds(void) {
    return getTimeSignatureBeatDurationInSeconds() / EDUPerTimeSignatureBeat;
  }
};
#endif /* TEMPO_H */

