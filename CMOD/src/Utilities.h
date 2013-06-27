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
//  Utilities.h
//  Created by Ming-ching Chiu 2012-2013
//  
//  The Utilities object is designed to evaluate the XML strings to their
//  proper format, whether an Event, a number, an Envelope or other objects such
//  as Pattern, Sieve, etc.
//
//  The Utilities is also the interface between the CMOD Event and LASS Score.
//  CMOD Events add the Sound/Note objects they produce to LASS Score through
//  The Utilities. 
//   
//
//----------------------------------------------------------------------------//


#ifndef UTILITIES_H
#define UTILITIES_H
#include "Libraries.h"
#include "Patter.h"

class Event;
class Piece;
class Patter;
class Sieve;
class Score;

class Utilities{

public:
  
  /**
   * Constructor
   **/
  Utilities(DOMElement* _root, 
            string _workingPath, 
            bool _soundSynthesis, 
            bool _outputParticel, 
            int _numThreads, 
            int _numChannels,
            int _samplingRate, 
            Piece* _piece);
  
  /**
   * Destructor
   **/          
  ~Utilities();
  
  
  //------------------------------Evaluation ---------------------------------//
  
  // Evaluate an XML string to a number.
  // An object (mostly an Event) must be provided to evaluate static functions 
  // such as current_child_num.
  double evaluate(string _input, void* _object);
  
  
  // Evaluate a function string to a CMOD object
  // The caller has to cast the returned pointer properly by itself.
  void* evaluateObject(string _input, void* _object, EventType _returnType); 
  
  // Evaluate a string to a DOMElement which represents a CMOD Event
  DOMElement* getEventElement(EventType _type, string _eventName);
  
  // Convert a DOMElement to a string
  static string XMLTranscode(DOMElement* _thisFunctionElement);
  
  // Getters
  bool getOutputParticel(){return outputParticel;}
  int getNumberOfChannels(){return numChannels;}
  int getSamplingRate(){return samplingRate;}
  
  
  //----------------------------- Other tasks---------------------------------//
  
  // Interface between CMOD Event and LASS Score
  void addSound(Sound* _sound);
  
  // return the final product
  MultiTrack* doneCMOD();
  
  /**
   * Clean up the unnecessary space in a string
   **/
  static string removeSpaces(string _originalString);
  
  
   
private:  
  
  
  
  // Helper for getting Envelopes
  Envelope* getEnvelope(string _input, void* _object);
  Envelope* envLib(DOMElement* _functionElement, void* _object);
  Envelope* readEnvFile (DOMElement* _functionElement, void* _object);
  Envelope* makeEnvelope (DOMElement* _functionElement, void* _object);

  // Helper for getting Pattern
  Patter* getPattern(string _functionString, void* _object);
  Patter* getPatternHelper(void* _object, DOMElement* _PATFunction);

  // Helper for getting Sieve
  Sieve* getSieve(string _functionString, void* _object);
  Sieve* getSieveHelper(void* _object, DOMElement* _SIVFunction);
  
  //Helper for getting SPA
  DOMElement* getSPAFunctionElement(void* _object);
  DOMElement* getSPAFunctionElementHelper(void* _object, DOMElement* _SPAFunction, bool _initialCall);
  
  // Helpers for getting REV   
  DOMElement* getREVFunctionElement(void* _object);
  DOMElement* getREVFunctionElementHelper(void* _object, DOMElement* _REVFunction, bool _initialCall);
  
  // Helpers for getting FIL
  DOMElement* getFILFunctionElement(void* _object);
  DOMElement* getFILFunctionElementHelper(void* _object, DOMElement* _FILFunction, bool _initialCall);
  
  
  /**
   * Helper function for Utilities::evaluate() to evaluate CMOD functions
   **/
  string evaluateFunction(string _functionString, void* _object);
  
  /**
   * Helper function for finding, in a string, the substring which represents
   * a CMOD function.
   **/ 
  size_t findTheEndOfFirstFunction(string _input);
  
  /**
   * Helper function for convert a string of in the format of "a, b, c, d"
   * to a vector containing 4 elements. 
   */
  std::vector<std::string> listElementToStringVector(DOMElement* _listElement);
  
  
  // CMOD Functions
  string function_RandomInt(DOMElement* _functionElement, void* _object);
  string function_Random(DOMElement* _functionElement, void* _object);
  string function_Select(DOMElement* _functionElement, void* _object);
  string function_SelectObject(DOMElement* _functionElement, void* _object);
  string function_GetPattern(DOMElement* _functionElement, void* _object);
  string function_Randomizer(DOMElement* _functionElement, void* _object);
  string function_Inverse(DOMElement* _functionElement, void* _object);
  string function_LN(DOMElement* _functionElement, void* _object);
  string function_Fibonacci(DOMElement* _functionElement, void* _object);
  string function_Decay(DOMElement* _functionElement, void* _object);
  string function_Stochos(DOMElement* _functionElement, void* _object);
  string function_ValuePick(DOMElement* _functionElement, void* _object);
  string function_ChooseL(DOMElement* _functionElement, void* _object);
  string function_MakeList(DOMElement* _functionElement, void* _object);
  
 
  // CMOD static functions
  string static_function_CURRENT_TYPE(void* _object);
  string static_function_CURRENT_CHILD_NUM(void* _object);
  string static_function_CURRENT_PARTIAL_NUM(void* _object);
  string static_function_CURRENT_DENSITY(void* _object);
  string static_function_CURRENT_SEGMENT(void* _object);
  string static_function_AVAILABLE_EDU(void* _object);
  string static_function_CURRENT_LAYER(void* _object);
  string static_function_PREVIOUS_CHILD_DURATION(void* _object);
  
  
  
  
  //-------------------------------Fields-------------------------------------//
    
  //Storage of DOMElement representations of CMOD Events
  std::map<string,DOMElement*> topEventElements;
  std::map<string,DOMElement*> highEventElements;
  std::map<string,DOMElement*> midEventElements;
  std::map<string,DOMElement*> lowEventElements;
  std::map<string,DOMElement*> bottomEventElements;
  std::map<string,DOMElement*> spectrumElements;
  std::map<string,DOMElement*> envelopeElements;
  std::map<string,DOMElement*> sieveElements;
  std::map<string,DOMElement*> spatializationElements;
  std::map<string,DOMElement*> patternElements;
  std::map<string,DOMElement*> reverbElements;
  std::map<string,DOMElement*> filterElements;
  std::map<string,DOMElement*> notesElements;
  
  // Storage of LASS Parsed/generated Envelopes
  EnvelopeLibrary* envelopeLibrary;
  
  // Piece Configurations
  bool soundSynthesis;
  bool outputParticel;
  int numThreads;
  int numChannels;
  int samplingRate;
  Piece* piece;
  Score* score;
  
  // A flag to indicate that the CMOD computation is done.
  bool doneCreatingSoundObjects;
  
};

#endif
