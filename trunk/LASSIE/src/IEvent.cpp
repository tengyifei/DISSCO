/*******************************************************************************
 *
 *  File Name     : IEvent.cpp
 *  Date created  : Feb. 2 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This File contains the implementation of IEvent class and
 *                  its related subclasses.
 *
 *
 *
 *==============================================================================
 *
 *  This file is part of LASSIE.
 *  Copyright 2010 Ming-ching Chiu, Sever Tipei
 *
 *
 *  LASSIE is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  LASSIE is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with LASSIE.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include "IEvent.h"
#include "ProjectViewController.h"
#include "FunctionGenerator.h"

#include "../../CMOD/src/parser/lex.yy.c"





//#include <stdlib.h>

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};


IEvent::IEvent(){
  eventName = "New IEvent";
  oldEventName = "";
  eventOrderInPalette = 0;

  maxChildDur = "";

  unitsPerSecond = "6";
  timeSignatureEntry1 = "4";
  timeSignatureEntry2 = "4";
  
  tempoMethodFlag = 0; //0 = as note value, 1 = as fraction
  tempoPrefix = tempoPrefixNone;
  tempoNoteValue = tempoNoteValueQuarter;
  tempoFractionEntry1 = "";
  tempoFractionEntry2 = "";
  tempoValueEntry = "60";  
  
  
  flagNumChildren    = 0; // 0 = fixed, 1 = density
  numChildrenEntry1  = "";
  numChildrenEntry2  = "";
  numChildrenEntry3  = "";
  changedButNotSaved = false;
  
  childEventDefEntry1 = "";
  childEventDefEntry2 = "";
  childEventDefEntry3 = "";

  flagChildEventDef             = 0;
  flagChildEventDefStartType    = 2;
  flagChildEventDefDurationType = 2;
  
  
  childEventDefAttackSieve = "";
  childEventDefDurationSieve = "";
  
  
  extraInfo = NULL;
  
  
  

  EventLayer* newLayer = new EventLayer(this);
  layers.push_back(newLayer);
}


IEvent::IEvent(std::string _filePath, std::string _fileName, EventType _type){
		eventName = _fileName;
  eventType = _type;
  //std::cout<<" LASSIE parsing: "<< _filePath<<"/"<< _fileName<<"..."<<std::endl;

    IEventParseFile(_filePath+"/"+_fileName);
    
    eventType = _type;
  
  
  changedButNotSaved = false;           

}









IEvent::~IEvent(){
  if(extraInfo!=NULL){
    delete extraInfo;
  }
  //delete layers here
}


IEvent::SoundExtraInfo::SoundExtraInfo(){
  spectrumPartials = new SpectrumPartial();
  deviation = "";
  numPartials =1;
}


IEvent::SoundExtraInfo::~SoundExtraInfo(){
}


void IEvent::setEventName(std::string _eventName){
  eventName = _eventName;
  changedButNotSaved = true;
}

void IEvent::setEventType(EventType _type){
  eventType = _type;
  changedButNotSaved = true;
  if (extraInfo!= NULL){
    delete extraInfo;
  }
  if(_type == eventSound){
    //EventExtraInfo* SoundEventExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::SoundExtraInfo();
  }
  else if(_type == eventBottom){
    //EventExtraInfo* BottomEventExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::BottomEventExtraInfo();
    string firstChar = eventName.substr(0,1);
    if (firstChar =="s"){
    	extraInfo->setChildTypeFlag(0);
    }
    else if (firstChar =="n"){
    	extraInfo->setChildTypeFlag(1);
    }
    else {
    	extraInfo->setChildTypeFlag(2);
   	}
   
    
    
  }
  else if(_type == eventEnv){
    //EventExtraInfo* EnvelopeExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::EnvelopeExtraInfo();
  }  
  else if(_type == eventSiv){
    //EventExtraInfo* EnvelopeExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::SieveExtraInfo();
  }
  else if(_type == eventSpa){
    //EventExtraInfo* EnvelopeExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::SpatializationExtraInfo();
  }
  else if(_type == eventPat){
    //EventExtraInfo* EnvelopeExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::PatternExtraInfo();
  }
  else if(_type == eventRev){
    //EventExtraInfo* EnvelopeExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::ReverbExtraInfo();
  }      
  else if(_type == eventNote){
    //EventExtraInfo* EnvelopeExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::NoteExtraInfo();
  }   
  
}


std::string IEvent::getEventName(){
  return eventName;
}

EventType IEvent::getEventType(){
  return eventType;
}


std::string IEvent::getEventTypeString(){
  
  switch(eventType){
    case 0:
      return "Top";
    case 1:
      return "High";
    case 2:
      return "Mid";
    case 3:
      return "Low";
    case 4:
      return "Bottom";
    case 5:
      return "Spectrum";
    case 6:
      return "Env.";
    case 7:
      return "Sieve";
    case 8:
      return "Spat.";
    case 9:
      return "Pat.";
    case 10:
      return "Rev.";
    case 11:
      return "Folder";
    case 12:
      return "Note";
  }
}


void IEvent::setMaxChildDur(std::string _maxChildDur){
  maxChildDur = _maxChildDur;
  changedButNotSaved = true;
}


std::string IEvent::getMaxChildDur(){
  return maxChildDur;
}


void IEvent::setUnitsPerSecond(std::string _unitsPerSecond){
  unitsPerSecond = _unitsPerSecond;
  changedButNotSaved = true;
}


std::string IEvent::getUnitsPerSecond(){
  return unitsPerSecond;
}


void IEvent::setTimeSignatureEntry1(std::string _timeSignatureEntry1){
  timeSignatureEntry1 = _timeSignatureEntry1;
  changedButNotSaved = true;
}


std::string IEvent::getTimeSignatureEntry1(){
  return timeSignatureEntry1;
}


void IEvent::setTimeSignatureEntry2(std::string _timeSignatureEntry2){
  timeSignatureEntry2 = _timeSignatureEntry2;
  changedButNotSaved = true;
}


  
std::string IEvent::getTimeSignatureEntry2(){
  return timeSignatureEntry2;
}








int IEvent::getTempoMethodFlag(){
  return tempoMethodFlag;
} 
  
void IEvent::setTempoMethodFlag(int _flag){

  tempoMethodFlag = _flag;
  changedButNotSaved = true;
}  
TempoPrefix IEvent::getTempoPrefix(){
  return tempoPrefix;
}

void IEvent::setTempoPrefix(TempoPrefix _prefix){

  tempoPrefix = _prefix;
  changedButNotSaved = true;
}  

TempoNoteValue IEvent::getTempoNoteValue(){
  return tempoNoteValue;
}
 
void IEvent::setTempoNoteValue(TempoNoteValue _noteValue){

  tempoNoteValue = _noteValue;
  changedButNotSaved = true;
}  

  
std::string IEvent::getTempoFractionEntry1(){
  return tempoFractionEntry1;
}
void IEvent::setTempoFractionEntry1(std::string _entry1){
  tempoFractionEntry1 = _entry1;
  changedButNotSaved = true;
}  
std::string IEvent::getTempoFractionEntry2(){
  return tempoFractionEntry2;
}
void IEvent::setTempoFractionEntry2(std::string _entry2){
  tempoFractionEntry2 = _entry2;
  changedButNotSaved = true;
} 

std::string IEvent::getTempoValueEntry(){
  return tempoValueEntry;
}
void IEvent::setTempoValueEntry(std::string _entry){
  tempoValueEntry =_entry;
  changedButNotSaved = true;
}




void IEvent::setFlagNumChildren(int _flag){
  flagNumChildren = _flag;
  changedButNotSaved = true;
}


int IEvent::getFlagNumChildren(){
  return flagNumChildren;
}


void IEvent::setNumChildrenEntry1(std::string _string){
  numChildrenEntry1 = _string;
  changedButNotSaved = true;
}


std::string IEvent::getNumChildrenEntry1(){
  return numChildrenEntry1;
}


void IEvent::setNumChildrenEntry2(std::string _string){
  numChildrenEntry2 = _string;
  changedButNotSaved = true;
}


std::string IEvent::getNumChildrenEntry2(){
  return numChildrenEntry2;
}


void IEvent::setNumChildrenEntry3(std::string _string){
  numChildrenEntry3 = _string;
  changedButNotSaved = true;
}


std::string IEvent::getNumChildrenEntry3(){
  return numChildrenEntry3;
}


void IEvent::setChildEventDefEntry1(std::string _string){
  childEventDefEntry1 = _string;
  changedButNotSaved = true;
}


std::string IEvent::getChildEventDefEntry1(){
  return childEventDefEntry1;
}


void IEvent::setChildEventDefEntry2(std::string _string){
  childEventDefEntry2 = _string;
  changedButNotSaved = true;
}


std::string IEvent::getChildEventDefEntry2(){
  return childEventDefEntry2;
}


void IEvent::setChildEventDefEntry3(std::string _string){
  childEventDefEntry3 = _string;
  changedButNotSaved = true;
}


std::string IEvent::getChildEventDefEntry3(){
  return childEventDefEntry3;
}


void IEvent::setFlagChildEventDef(int _flag){
  // 0 = continuum, 1 = sweep, 2 = descrete
  flagChildEventDef = _flag;
}


int IEvent::getFlagChildEventDef(){
  return flagChildEventDef;
}


void IEvent::setFlagChildEventDefStartType(int _flag){

  // 0 = percentage, 1 = units, 2 = second.   -1 if childeventdef is discrete
  flagChildEventDefStartType = _flag;
}

int IEvent::getFlagChildEventDefStartType(){
  return flagChildEventDefStartType;
}



void IEvent::setFlagChildEventDefDurationType(int _flag){

  // 0 = percentage, 1 = units, 2 = second.   -1 if childeventdef is discrete
  flagChildEventDefDurationType = _flag;
}


int IEvent::getFlagChildEventDefDurationType(){
  return flagChildEventDefDurationType;
}


void IEvent::setChildEventDefAttackSieve(std::string _string){
  childEventDefAttackSieve = _string;
  
}


std::string IEvent::getChildEventDefAttackSieve(){
  return childEventDefAttackSieve;
}


void IEvent::setChildEventDefDurationSieve(std::string _string){
  childEventDefDurationSieve = _string;
}


std::string IEvent::getChildEventDefDurationSieve(){
  return childEventDefDurationSieve;
}


//deprecated. replaced by eventlayer::addchild
void IEvent::addChildOrObject(IEvent* _newChild){
  /*
  EventType type = _newChild->getEventType();

  if(type <= 4){
    // object to be added is a normal event

    //children.push_back(_newChild); //replaced by EventLayers
  }
  
  switch(type){
    case 5:
      sounds.push_back(_newChild);
      break;
    case 6:
      envelopes.push_back(_newChild);
      break;
    case 7:
      sieves.push_back(_newChild);
      break;
    case 8:
      spatializations.push_back(_newChild);
      break;
    case 9:
      patterns.push_back(_newChild);
      break;
    case 10:
      reverbs.push_back(_newChild);
      break;
  }
  
  
  _newChild->parents.push_back(this);
  _newChild->setModifiedButNotSaved();
  changedButNotSaved = true;
  */
}


void IEvent::removeChildOrObject(IEvent* _childToBeRemoved){
  //TODO: implement this function
}


void IEvent::setModifiedButNotSaved(){
  changedButNotSaved = true;
}

void IEvent::saveToDisk(std::string _pathOfProject){
	saveToDiskHelper(_pathOfProject, false);
}


void IEvent::saveAsToDisk(std::string _pathOfProject){ //save everything
	saveToDiskHelper(_pathOfProject, true);
}


void IEvent::saveToDiskHelper(std::string _pathOfProject, bool _forced){
  if(changedButNotSaved == false && _forced==false) return;
  changedButNotSaved = false;
  
  switch(eventType){
    case 0:
      saveAsTHMLB(_pathOfProject);
      break;
    case 1:
      saveAsTHMLB(_pathOfProject);
      break;
    case 2:
      saveAsTHMLB(_pathOfProject);
      break;
    case 3:
      saveAsTHMLB(_pathOfProject);
      break;
    case 4:
      saveAsTHMLB(_pathOfProject);
      break;
    case 5:
      saveAsSound(_pathOfProject);
      break;
    case 6:
      saveAsEnv(_pathOfProject);
      break;
    case 7:
      saveAsSiv(_pathOfProject);
      break;
    case 8:
      saveAsSpa(_pathOfProject);
      break;
    case 9:
      saveAsPat(_pathOfProject);
      break;
    case 10:
      saveAsRev(_pathOfProject);
      break;
    case 11: //no need to save folder
      break;
    case 12:
      saveAsNote(_pathOfProject);  
  }
}

void IEvent::saveAsTHMLB(std::string _pathOfProject){
  std::string fileName;
  std::string header;

  switch (eventType){
    case 0:
      fileName = _pathOfProject + "/T/" + eventName;
      header = "/*  Top Event: T/"+eventName+"  */\n\n";
      break;
    case 1:
      fileName = _pathOfProject + "/H/" + eventName;
      header = "/*  High Event: H/"+eventName+"  */\n\n";
      break;
    case 2:
      fileName = _pathOfProject + "/M/" + eventName;
      header = "/*  Mid Event: M/"+eventName+"  */\n\n";
      break;
    case 3:
      fileName = _pathOfProject + "/L/" + eventName;
      header = "/*  Low Event: L/"+eventName+"  */\n\n";
      break;
    case 4:
      fileName = _pathOfProject + "/B/" + eventName;
      header = "/*  Bottom Event: B/"+eventName+"  */\n\n";      
      break;      
  }
  
  std::cout<<"Saving "<<fileName<<" ..."<<std::endl;
  std::string stringbuffer;
  FILE* file  = fopen(fileName.c_str(), "w");
  
  fputs(header.c_str(),file);
  
  
  
  
  stringbuffer = "\n\nmaxChildDur = "+ maxChildDur +";\n";
  
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),file);
  }
  else {
    cout<<"illegal maxChildDur value!"<<endl;
  }
  
  
  
  stringbuffer = "timeSignature = \""
                 + timeSignatureEntry1
                 + "/" 
                 + timeSignatureEntry2
                 +"\";\n";   
  
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),file);
  }
    else {
    cout<<"illegal Time Signature value!"<<endl;
  }
  
  stringbuffer = "EDUPerBeat = "+ unitsPerSecond+";\n";
    yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),file);
  }
  else {

  }

  
  stringbuffer = "tempo = \"";
  if (tempoPrefix == tempoPrefixDotted){
      stringbuffer = stringbuffer + "dotted ";
    }
    else if (tempoPrefix == tempoPrefixDoubleDotted){
      stringbuffer = stringbuffer + "double dotted ";
    }     
    else if (tempoPrefix ==tempoPrefixTriple){
      stringbuffer = stringbuffer + "triple ";
    }          
    else if (tempoPrefix ==tempoPrefixQuintuple){
      stringbuffer = stringbuffer + "quintuple ";
    }     
    else if (tempoPrefix ==tempoPrefixSextuple){
      stringbuffer = stringbuffer + "sextuple ";
    }     
    else if (tempoPrefix ==tempoPrefixSeptuple){
      stringbuffer = stringbuffer + "septuple ";
    }     

    if (tempoNoteValue == tempoNoteValueWhole){
      stringbuffer = stringbuffer + "whole = ";
    }
    else if (tempoNoteValue == tempoNoteValueHalf){
      stringbuffer = stringbuffer + "half = ";
    }
    else if (tempoNoteValue == tempoNoteValueQuarter){
      stringbuffer = stringbuffer + "quarter = ";
    }
    else if (tempoNoteValue == tempoNoteValueEighth){
      stringbuffer = stringbuffer + "eighth = ";
    }
    else if (tempoNoteValue == tempoNoteValueSixteenth){
      stringbuffer = stringbuffer + "sixteenth = ";
    }
    else if (tempoNoteValue == tempoNoteValueThirtySecond){
      stringbuffer = stringbuffer + "thirtysecond = ";
    }
  
  
  
  if (tempoMethodFlag == 0) {// tempo as note value 
    
    
    stringbuffer = stringbuffer + tempoValueEntry+ "\";\n";
    yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }   
    else {
      cout<<"illegal tempo value!"<<endl;
    }

  
  }
   else { // tempo as fraction
    //enty1 notes in value seconds
    //entry : value = actual number : 60
    //entry1 * 60 / value = actual number
    
    
    int entry1 = atoi (tempoFractionEntry1.c_str()) * 60;
    int den = atoi (tempoValueEntry.c_str());
    
    char tempobuffer [20];
    sprintf(tempobuffer, "%d", entry1);
    string numString = string(tempobuffer);
    
    sprintf (tempobuffer,"%d", den);
    string denString = string(tempobuffer);
    
    string ratioNumber = numString + "/" + denString;
    Ratio ratio = Ratio(ratioNumber);
    
    
    
    sprintf(tempobuffer, "%d", ratio.Num());
    
    
    if (ratio.Den() ==1){
    
    	stringbuffer = stringbuffer + string(tempobuffer) + "\";\n";
    
    }
    else{
    
    	stringbuffer = stringbuffer + string(tempobuffer) + "/";
    
    	sprintf(tempobuffer, "%d", ratio.Den());
    
    	stringbuffer = stringbuffer + string(tempobuffer) + "\";\n";
    
    }
                 
    yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }
    else {
      cout<<"illegal tempo value!"<<endl;
    }
  
  
  }
  
  

  if (flagNumChildren ==0){ //fixed num children

      if (numChildrenEntry1 ==""|| numChildrenEntry1 ==""){  //num children not specified
        stringbuffer = "numChildren = <\"FIXED\">;\n\n";
      }else{  //num children is specified
         stringbuffer = "numChildren = <\"FIXED\", " +
                        numChildrenEntry1 +
                        ">;\n\n";
      }

  }
  else if (flagNumChildren ==1){  //density numChildren
     stringbuffer = "numChildren = <\"DENSITY\", " +
                    numChildrenEntry1 +
                       ", "+
                    numChildrenEntry2+
                       ", "+
                    numChildrenEntry3+
                    ">;\n\n";

  }
  else{    //by layer
    stringbuffer = "numChildren =<\"BY_LAYER\", <";
    std::string thisLayer;
    std::list<EventLayer*>::iterator i = layers.begin();
    bool firstNumberExisted = false;

    while(i != layers.end()){
      thisLayer = (*i)->outputChildrenNameString();

      if(thisLayer != ""){  //check if this layer has children
        stringbuffer = stringbuffer + (*i)->byLayer;
        firstNumberExisted = true;
      }

      i++;
      if(i == layers.end())break;

      thisLayer = (*i)->outputChildrenNameString();
      if(thisLayer != "" && firstNumberExisted){
        stringbuffer = stringbuffer + ", ";
      }
      
    }
    stringbuffer = stringbuffer + "> >;\n";
  }

  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),file);
  }
  else {
    cout<<"illegal numChildren value!"<<endl;
  }



  stringbuffer = "childNames = <\n";

  std::string thisLayer;
  std::list<EventLayer*>::iterator i = layers.begin();
  bool firstLayerExisted = false;

  while(i != layers.end()){
    thisLayer = (*i)->outputChildrenNameString();
    if (thisLayer != ""){  //check if this layer has children
      stringbuffer = stringbuffer + thisLayer;
      firstLayerExisted = true;
    }
    i++;
    if (i ==layers.end())break;

    thisLayer = (*i)->outputChildrenNameString();
    if (thisLayer != ""&&firstLayerExisted){
      stringbuffer = stringbuffer + ",\n";
    }
  }

  stringbuffer = stringbuffer + "\n             >;\n\n";
  
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),file);
  }
  
  stringbuffer = "childEventDef = < ";

  switch (flagChildEventDef){
    case 0:
      stringbuffer += "\"CONTINUUM\",\n\t\t  ";
      break;
    case 1:
      stringbuffer += "\"SWEEP\",\n\t\t  ";
      break;
    case 2:
      stringbuffer += "\"DISCRETE\",\n\t\t  ";
      
      break;
  }
  
  if(flagChildEventDef ==0 || flagChildEventDef ==1){  // contiuum or sweep
    stringbuffer += childEventDefEntry1;
    stringbuffer += ",\n\t\t  ";

    switch (flagChildEventDefStartType){
      case 0:
        stringbuffer += "\"PERCENTAGE\",\n\t\t  ";
        break;
      case 1:
        stringbuffer += "\"EDU\",\n\t\t  ";
        break;
      case 2:
        stringbuffer += "\"SECONDS\",\n\t\t  ";
        break;
    }

    stringbuffer += childEventDefEntry2;
    stringbuffer += ",\n\t\t  ";
    
    stringbuffer += childEventDefEntry3;
    stringbuffer += ",\n\t\t  ";
    
    switch (flagChildEventDefDurationType){
      case 0:
        stringbuffer += "\"PERCENTAGE\"\n\t\t>;\n\n";
        break;
      case 1:
        stringbuffer += "\"EDU\"\n\t\t>;\n\n";
        break;
      case 2:
        stringbuffer += "\"SECONDS\"\n\t\t>;\n\n";
        break;
    }
    
    yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }
    
    else {
              cout<<"illegal childEventDef value!"<<endl;
    }    
  }
  else {// discrete
    stringbuffer += childEventDefAttackSieve;
    stringbuffer += ",\n\t\t  ";
    stringbuffer += childEventDefDurationSieve;
    stringbuffer += ",\n\t\t  < ";   


    double weightSum = 0;
    
    std::list<EventLayer*>::iterator i =layers.begin();
    for (i ; i != layers.end(); i ++){
      //numTotalChildren += (*i)->size();
      weightSum += (*i)->getChildrenWeightSum();
      
    }
    
    //double unit = ((double) weightSum) / numTotalChildren; 
    double unit = 1.0 / weightSum; 

    
    i = layers.begin();
    
    
    // a temporary list makes it easy to print numbers out
    class llist {
    public: 
      llist(){ package =NULL; next = NULL;}
      ~llist(){}
      EventDiscretePackage* package;
      llist* next;
    };
    
    //the first node is a empty node
    llist* head =  new llist();
    llist* current = head;
    
    
    for (i ; i != layers.end(); i ++){
      std::list<EventDiscretePackage*>::iterator j = (*i)->children.begin();
        for ( j ; j !=(*i)->children.end(); j ++){
          current->next = new llist();
          current->next->package = *j;
          current = current->next;
        }
    }
    
    current = head;
    double temp;
    char charbuffer[10]; 
    
    if (current->next == NULL){ //no node at all 
      stringbuffer += ">,<>,<>\n\t\t>;\n\n";
    }
    else { //normal case
      current = head -> next;//cuz the first node is empty

      while (current != NULL){
      
        
        temp = ((double) atof (current->package->weight.c_str())) * unit;
        
        sprintf (charbuffer," %.3f", temp);
        
        stringbuffer += string (charbuffer);
        //fputs(charbuffer,file); save to string buffer instead
        
        if (current->next!=NULL){
          stringbuffer += ", ";
          //fputs(", ", file);
        }
        current = current ->next; 
      } //end while
      
      stringbuffer += ">,\n\t\t  <\n";
      //fputs(">,\n\t\t  <\n", file);
      current = head->next;

      while (current != NULL){
        stringbuffer += "\t\t    EnvLib("+ current->package->attackEnv+", " + current->package->attackEnvScale+")";

        if (current->next!=NULL){
          stringbuffer += ",\n";
        }
        current = current ->next; 
      } //end while
      
      stringbuffer +="\n\t\t  >,\n\t\t  <\n";      
      current = head->next;
     
      while (current != NULL){
        stringbuffer += "\t\t    EnvLib("+ current->package->durationEnv+", " + current->package->durationEnvScale+")";

        if (current->next!=NULL){
          stringbuffer+=",\n";
        }
        current = current ->next; 
      } //end while 
      stringbuffer+="\n\t\t  >\n\t\t>;\n\n";
      
    }// end normal case
    
    //clean up llist
 
    while (head!=NULL){
      current = head->next;
      delete head;
      head = current;
    }

    yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }
    
    else {
              cout<<"illegal childEventDef value!"<<endl;
    }
    
  }//end descrete
  
  
  if (eventType == eventBottom){ // save BottomExtra Info
    stringbuffer = "frequency = <\"";
    if (extraInfo->getFrequencyFlag() == 0){// well_tempered
      stringbuffer = stringbuffer + "WELL_TEMPERED\", " + extraInfo->getFrequencyEntry1()+ " >;\n\n";
    }
    else if (extraInfo->getFrequencyFlag() == 1){//fundamental
      stringbuffer = stringbuffer + "FUNDAMENTAL\", " + extraInfo->getFrequencyEntry1()+ ", "+ extraInfo->getFrequencyEntry2()+ " >;\n\n";    
    
    }
    else { //continuum
      stringbuffer = stringbuffer + "CONTINUUM\", \"" +
        ((extraInfo->getFrequencyContinuumFlag() == 0)?"HERTZ":"POW2") + "\", " + extraInfo->getFrequencyEntry1()+  " >;\n\n";    
    
    }
    
    yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }
    else {
      cout<<"illegal frequency value!"<<endl;
    }
    
    stringbuffer = "loudness = " + extraInfo->getLoudness() + ";\n\n" ;
    yy_scan_string( stringbuffer.c_str());
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }
    else {
      cout<<"illegal loudness value!"<<endl;
    }
    
    if (extraInfo->getChildTypeFlag() ==0){ //save spatialization and reverb only when childtype is sound
     	stringbuffer = "spatialization = " + extraInfo->getSpatialization() + ";\n\n" ;
      yy_scan_string( stringbuffer.c_str());
   		if (yyparse()==0){
      	fputs(stringbuffer.c_str(),file);
    	}
    	else {
      	cout<<"illegal spatialization value!"<<endl;
    	}
     
     	stringbuffer = "reverberation = " + extraInfo->getReverb() + ";\n\n";
    	yy_scan_string( stringbuffer.c_str());
    	if (yyparse()==0){
      	fputs(stringbuffer.c_str(),file);
    	}
    	else {
      	cout<<"illegal reverberation value!"<<endl;
    	}
		}

    //save modifier information
    
    EventBottomModifier* mod = extraInfo->getModifiers();
    if (mod!= NULL){
    
    stringbuffer = "modifiers = <\n";
    

    
      while (mod != NULL){
        stringbuffer = stringbuffer + mod->getSaveToDiskString();
      
        if (mod->next!=NULL){
          stringbuffer = stringbuffer + ",\n";
        }
        mod = mod->next;
      }

      stringbuffer = stringbuffer + "\n            >;\n\n";
    yy_scan_string( stringbuffer.c_str());  
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }
    else {
      cout<<"illegal modifiers value!"<<endl;
    }

    }
  }//end handling bottom info




  // LASSIE metadata
  char charBuffer[10];

  
  stringbuffer = "\n\n\n\n\n\n\n\n\n/*=============================LASSIE METADATA=========================*/\n\n\n";
  fputs (stringbuffer.c_str(), file);
  
  
  sprintf (charBuffer," %d", eventOrderInPalette);
  
  stringbuffer = "LASSIEeventName = <`" + eventName + "`," + charBuffer + ">;\n";
  fputs (stringbuffer.c_str(), file); 

  stringbuffer = "LASSIEmaxChildDur = `" + maxChildDur + "`;\n";
  fputs (stringbuffer.c_str(), file); 

  stringbuffer = "LASSIEEDUPerBeat = `" + unitsPerSecond + "`;\n";
  fputs (stringbuffer.c_str(), file); 

  stringbuffer = "LASSIEtimeSignatureEntry1 = `" + timeSignatureEntry1 + "`;\n";
  fputs (stringbuffer.c_str(), file); 

  stringbuffer = "LASSIEtimeSignatureEntry2 = `" + timeSignatureEntry2 + "`;\n";
  fputs (stringbuffer.c_str(), file); 

  sprintf(charBuffer, "%d", tempoMethodFlag);
  stringbuffer = "LASSIEtempoMethodFlag = " + string(charBuffer) + ";\n";
  fputs (stringbuffer.c_str(), file); 

  sprintf(charBuffer, "%d", tempoPrefix);
  stringbuffer = "LASSIEtempoPrefix = " + string(charBuffer) + ";\n";
  fputs (stringbuffer.c_str(), file); 

  sprintf(charBuffer, "%d", tempoNoteValue);  
  stringbuffer = "LASSIEtempoNoteValue = " + string(charBuffer) + ";\n";
  fputs (stringbuffer.c_str(), file); 

  stringbuffer = "LASSIEtempoFractionEntry1 = `" + tempoFractionEntry1 + "`;\n";
  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIEtempoFractionEntry2 = `" + tempoFractionEntry2 + "`;\n";
  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIEtempoValueEntry = `" + tempoValueEntry + "`;\n";
  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIEnumChildrenEntry1 = `" + numChildrenEntry1 + "`;\n";
  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIEnumChildrenEntry2 = `" + numChildrenEntry2 + "`;\n";
  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIEnumChildrenEntry3 = `" + numChildrenEntry3 + "`;\n";
  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIEchildEventDefEntry1 = `" + childEventDefEntry1 + "`;\n";
  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIEchildEventDefEntry2 = `" + childEventDefEntry2 + "`;\n";
  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIEchildEventDefEntry3 = `" + childEventDefEntry3 + "`;\n";
  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIEchildEventDefAttackSieve = `" + childEventDefAttackSieve + "`;\n";
  fputs (stringbuffer.c_str(), file);
   
  stringbuffer = "LASSIEchildEventDefDurationSieve = `" + childEventDefDurationSieve + "`;\n";
  fputs (stringbuffer.c_str(), file);
   

  sprintf(charBuffer, "%d", flagNumChildren);
  stringbuffer = "LASSIEflagNumChildren = " + string(charBuffer) + ";\n";
  fputs (stringbuffer.c_str(), file);

  sprintf(charBuffer, "%d", flagChildEventDef);  
  stringbuffer = "LASSIEflagChildEventDef = " + string(charBuffer) + ";\n";
  fputs (stringbuffer.c_str(), file);

  sprintf(charBuffer, "%d", flagChildEventDefStartType);  
  stringbuffer = "LASSIEflagChildEventDefStartType = " + string(charBuffer) + ";\n";
  fputs (stringbuffer.c_str(), file);

  sprintf(charBuffer, "%d",flagChildEventDefDurationType);  
  stringbuffer = "LASSIEflagChildEventDefDurationType = " + string(charBuffer) + ";\n";
  fputs (stringbuffer.c_str(), file);



  stringbuffer = "LASSIEeventLayers = <"; //

  std::list<EventLayer*>::iterator layersIter = layers.begin();

  while(layersIter != layers.end()){
    stringbuffer = stringbuffer + (*layersIter)->getLASSIEMetaDataString();
    
    layersIter++;
    
    if (layersIter != layers.end()){
      stringbuffer = stringbuffer + ",";
    }
  } 
  

  stringbuffer = stringbuffer + ">;\n";
  fputs (stringbuffer.c_str(),file);





  if (eventType == eventBottom){
    sprintf(charBuffer, "%d", extraInfo->getFrequencyFlag());
    stringbuffer = "LASSIEBOTTOMfrequencyFlag = " + string (charBuffer) + ";\n";
    fputs(stringbuffer.c_str(), file);
    
    sprintf(charBuffer, "%d", extraInfo->getFrequencyContinuumFlag());    
    stringbuffer = "LASSIEBOTTOMfrequencyContinuumFlag = " + string(charBuffer) + ";\n";

    fputs(stringbuffer.c_str(), file);
    
    
    
    stringbuffer = "LASSIEBOTTOMfrequencyEntry1 = `" + extraInfo->getFrequencyEntry1() + "`;\n" +
    "LASSIEBOTTOMfrequencyEntry2 = `" + extraInfo->getFrequencyEntry2() + "`;\n" +
    "LASSIEBOTTOMloudness = `" + extraInfo->getLoudness() + "`;\n" +
      "LASSIEBOTTOMspatialization = `" + extraInfo->getSpatialization() + "`;\n" +
      "LASSIEBOTTOMreverberation = `" + extraInfo->getReverb() + "`;\n";
    fputs (stringbuffer.c_str(), file);






    //save modifier information
    
    EventBottomModifier* mod = extraInfo->getModifiers();
    if (mod!= NULL){
    
    stringbuffer = "LASSIEBOTTOMmodifiers = <\n";
    

      while (mod != NULL){
        stringbuffer = stringbuffer + mod->getSaveLASSIEMetaDataString();
      
        if (mod->next!=NULL){
          stringbuffer = stringbuffer + ",\n";
        }
        mod = mod->next;
      }

      stringbuffer = stringbuffer + "\n            >;\n\n";
      fputs(stringbuffer.c_str(), file);

    }






    



  }



  fclose(file);
  

  



}


int IEvent::getNumberOfLayers(){
  return layers.size();
}


EventLayer* IEvent::addLayer(){
  EventLayer* newLayer = new EventLayer(this);
  layers.push_back(newLayer);
  return newLayer;
  
  
  
  
}





void IEvent::saveAsSound(std::string _pathOfProject){
  std::string fileName;
 
  fileName = _pathOfProject + "/S/"+ eventName;

  std::cout<<"Saving "<<fileName<<" ..."<<std::endl;
  std::string stringbuffer;
  FILE* file  = fopen(fileName.c_str(), "w");
  
  std::string header = "/*  Spectrum: S/"+eventName+"  */\n\n";  
  fputs(header.c_str(),file);
  
  stringbuffer = "numPartials = " + extraInfo->getNumPartials()+";\n";
    yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }
    else {
      cout<<"illegal numPartials value!"<<endl;
    }

  stringbuffer = "deviation = "+ extraInfo->getDeviation()+";\n";
    yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }
    else {
      cout<<"illegal deviation value!"<<endl;
    }

  
  stringbuffer = "spectrum = << " + extraInfo->getSoundSpectrumEnvelopesString()+"> >;\n";
    yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }
    else {
      cout<<"illegal spectrum value!"<<endl;
    }

  



  // LASSIE metadata
  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);
  
  stringbuffer = "LASSIEeventName = <`" + eventName + "`," + charBuffer + ">;\n";
  
  stringbuffer = "\n\n\n\n\n\n\n\n\n/*=============================LASSIE METADATA=========================*/\n\n\n";
  fputs (stringbuffer.c_str(), file);
  


  
  stringbuffer = "LASSIEeventName = <`" + eventName + "`," + charBuffer + ">;\n";
  

  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIESOUNDnumPartials = `" + extraInfo->getNumPartials() + "`;\n";
  fputs (stringbuffer.c_str(), file);

  stringbuffer = "LASSIESOUNDdeviation = `" + extraInfo->getDeviation() + "`;\n";
  fputs (stringbuffer.c_str(), file);

  
  stringbuffer = "LASSIESOUNDspectrum = <"+ extraInfo->getSpectrumMetaData() + ">;\n";
  fputs (stringbuffer.c_str(), file);
  



  fclose(file);
  
}


std::string IEvent::SoundExtraInfo::getSpectrumMetaData(){
  std::string returnString = "";
  SpectrumPartial* thisPartial = spectrumPartials;
  
  while (thisPartial!= NULL){
    std::string tempstring = thisPartial->envString;
    if (tempstring == ""){
      tempstring = "";
    }
  
  
  
    returnString = returnString +"`"+ tempstring + "`";
    thisPartial = thisPartial->next;
    if (thisPartial!= NULL){
      returnString = returnString + ",";
    }
  }
  
  return returnString;
  
}


std::string IEvent::SoundExtraInfo::getSoundSpectrumEnvelopesString(){
  std::string returnString = "";
  SpectrumPartial* thisPartial = spectrumPartials;
  
  while (thisPartial!= NULL){
    returnString = returnString + thisPartial->envString;
    thisPartial = thisPartial->next;
    if (thisPartial!= NULL){
      returnString = returnString + ",";
    }
  }
  
  return returnString;
}



void IEvent::saveAsEnv(std::string _pathOfProject){
  std::string fileName;
  fileName = _pathOfProject + "/ENV/"+ eventName;

  std::cout<<"Saving "<<fileName<<" ..."<<std::endl;
  std::string stringbuffer;
  FILE* file  = fopen(fileName.c_str(), "w");

  std::string header = "/*  Envelope: ENV/"+eventName+"  */\n\n";  
  fputs(header.c_str(),file);


  stringbuffer = "envelopeBuilder = " + extraInfo->getEnvelopeBuilder()+";\n";
    yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }
    else {
      cout<<"illegal builder value!"<<endl;
    }
  
  
  
   stringbuffer = "\n\n\n\n\n\n\n\n\n/*=============================LASSIE METADATA=========================*/\n\n\n";
  fputs (stringbuffer.c_str(), file);
  
  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);
  
  stringbuffer = "LASSIEeventName = <`" + eventName + "`," + charBuffer + ">;\n";
  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIEENV = `" + extraInfo->getEnvelopeBuilder() + "`;\n";
  fputs (stringbuffer.c_str(), file);
  
  

  fclose(file);
}


void IEvent::saveAsSiv(std::string _pathOfProject){
  std::string fileName;
  fileName = _pathOfProject + "/SIV/"+ eventName;



  std::cout<<"Saving "<<fileName<<" ..."<<std::endl;
  std::string stringbuffer;
  FILE* file  = fopen(fileName.c_str(), "w");

  std::string header = "/*  Sieve: SIV/"+eventName+"  */\n\n";  
  fputs(header.c_str(),file);

  stringbuffer = "sieveBuilder = " + extraInfo->getSieveBuilder()+";\n";
    yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }
    else {
      cout<<"illegal builder value!"<<endl;
    }

   stringbuffer = "\n\n\n\n\n\n\n\n\n/*=============================LASSIE METADATA=========================*/\n\n\n";
  fputs (stringbuffer.c_str(), file);
  
  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);
  
  stringbuffer = "LASSIEeventName = <`" + eventName + "`," + charBuffer + ">;\n";
  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIESIV = `" + extraInfo->getSieveBuilder() + "`;\n";
  fputs (stringbuffer.c_str(), file);
  fclose(file);
}


void IEvent::saveAsSpa(std::string _pathOfProject){


  std::string fileName;
  fileName = _pathOfProject + "/SPA/"+ eventName;

  std::cout<<"Saving "<<fileName<<" ..."<<std::endl;
  std::string stringbuffer;
  FILE* file  = fopen(fileName.c_str(), "w");
  std::string header = "/*  Spatialization: SPA/"+eventName+"  */\n\n";  
  fputs(header.c_str(),file);

  stringbuffer = "spatialization = " + extraInfo->getSpatializationBuilder()+";\n";
    yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }
    else {
      cout<<"illegal builder value!"<<endl;
    }
  
   stringbuffer = "\n\n\n\n\n\n\n\n\n/*=============================LASSIE METADATA=========================*/\n\n\n";
  fputs (stringbuffer.c_str(), file);
  
  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);
  
  stringbuffer = "LASSIEeventName = <`" + eventName + "`," + charBuffer + ">;\n";
  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIESPA = `" + extraInfo->getSpatializationBuilder() + "`;\n";
  fputs (stringbuffer.c_str(), file);  

  fclose(file);
}


void IEvent::saveAsPat(std::string _pathOfProject){
  std::string fileName;
  fileName = _pathOfProject + "/PAT/"+ eventName;

  std::cout<<"Saving "<<fileName<<" ..."<<std::endl;
  std::string stringbuffer;
  FILE* file  = fopen(fileName.c_str(), "w");

  std::string header = "/*  Pattern: PAT/"+eventName+"  */\n\n";  
  fputs(header.c_str(),file);

  stringbuffer = "patternBuilder = " + extraInfo->getPatternBuilder()+";\n";
    yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }
    else {
      cout<<"illegal builder value!"<<endl;
    }
  
     stringbuffer = "\n\n\n\n\n\n\n\n\n/*=============================LASSIE METADATA=========================*/\n\n\n";
  fputs (stringbuffer.c_str(), file);
  
  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);
  
  stringbuffer = "LASSIEeventName = <`" + eventName + "`," + charBuffer + ">;\n";
  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIEPAT = `" + extraInfo->getPatternBuilder() + "`;\n";
  fputs (stringbuffer.c_str(), file);
  

  fclose(file);
}


void IEvent::saveAsRev(std::string _pathOfProject){
  std::string fileName;
  fileName = _pathOfProject + "/REV/"+ eventName;

  std::cout<<"Saving "<<fileName<<" ..."<<std::endl;
  std::string stringbuffer;
  FILE* file  = fopen(fileName.c_str(), "w");

  std::string header = "/*  Reverberation: REV/"+eventName+"  */\n\n";  
  fputs(header.c_str(),file);

  stringbuffer = "reverberation = " + extraInfo->getReverbBuilder()+";\n";
    yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
      fputs(stringbuffer.c_str(),file);
    }
    else {
      cout<<"illegal builder value!"<<endl;
    }

   stringbuffer = "\n\n\n\n\n\n\n\n\n/*=============================LASSIE METADATA=========================*/\n\n\n";
  fputs (stringbuffer.c_str(), file);
  
  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);
  
  stringbuffer = "LASSIEeventName = <`" + eventName + "`," + charBuffer + ">;\n";
  fputs (stringbuffer.c_str(), file); 
  
  stringbuffer = "LASSIEREV = `" + extraInfo->getReverbBuilder() + "`;\n";
  fputs (stringbuffer.c_str(), file);

  fclose(file);
}



void IEvent::saveAsNote(std::string _pathOfProject){


  std::string fileName;
  fileName = _pathOfProject + "/N/"+ eventName;

  std::cout<<"Saving "<<fileName<<" ..."<<std::endl;
  std::string stringbuffer;
  FILE* file  = fopen(fileName.c_str(), "w");
  std::string header = "/*  Note: N/"+eventName+"  */\n\n";  
  fputs(header.c_str(),file);

  stringbuffer ="notePitchClass = < \"C\", \"C#\", \"D\", \"Eb\", \"E\", \"F\", \"F#\", \"G\", \"Ab\", \"A\", \"Bb\", \"B\" >;\n\nnoteDynamicMark = < \"ppp\", \"pp\", \"p\", \"mp\", \"mf\", \"f\", \"ff\", \"fff\" >;\n\n";
  fputs(stringbuffer.c_str(),file);




  std::list<std::string> modifiers = extraInfo->getNoteModifiers();
  if (modifiers.size()!= 0){
    stringbuffer = "noteModifiers = < ";
    std::list<std::string>::iterator iter = modifiers.begin();
  
    while (iter!= modifiers.end()){
      stringbuffer = stringbuffer + "\""+*iter + "\"";
      iter ++;
      if (iter!= modifiers.end()){
        stringbuffer = stringbuffer + ", ";
      }
    }    
  
    stringbuffer = stringbuffer + " >;\n";
    fputs(stringbuffer.c_str(),file);
  }
  
  
  
  stringbuffer = "\n\n\n\n\n\n\n\n\n/*=============================LASSIE METADATA=========================*/\n\n\n";
  fputs (stringbuffer.c_str(), file);
  
  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);
  
  stringbuffer = "LASSIEeventName = <`" + eventName + "`," + charBuffer + ">;\n";
  fputs (stringbuffer.c_str(), file); 
  
  
  
  fclose(file);
}




void IEvent::showAllChildren(){
  std::list<EventLayer*>::iterator i = layers.begin();

  for (i ; i != layers.end(); ++i){
    std::cout << "===========================================";
    std::cout << std::endl;
    (*i)->showContents();
  }
}


std::string EventLayer::getByLayer(){
  return byLayer;
}




EventLayer::EventLayer(IEvent* _thisEvent){
  thisIEvent = _thisEvent;
  byLayer     = "";
}


EventLayer::~EventLayer(){
  std::list<EventDiscretePackage*>::iterator childIter = children.begin();
  for (childIter; childIter!= children.end(); childIter++){
    (*childIter)->event->removeParent(thisIEvent);
    delete (*childIter);
  }
  children.clear();
}

void IEvent::setChangedButNotSaved(bool value){
  changedButNotSaved = value;

}

EventDiscretePackage* EventLayer::addChild(IEvent* _child){
  thisIEvent->setChangedButNotSaved(true);
  EventDiscretePackage* newdyad = new EventDiscretePackage(_child);
  children.push_back(newdyad);
  
  _child->addParent(thisIEvent);
  
  return newdyad;
}



void EventLayer::showContents(){
  std::cout << "EventLayer begins showing contents" << std::endl;
  std::list<EventDiscretePackage*>::iterator i = children.begin();

  for (i; i != children.end(); ++i){
    std::cout << "!" << (*i)->event->getEventName() << std::endl;
  }

  std::cout << "EventLayer stop showing Contents" << std::endl;
}



int EventLayer::size(){
  return children.size();
}


std::string EventLayer::outputChildrenNameString(){
  if(children.size() == 0) return "";
  
  std::string temp = "                 <\n                    ";
  std::list<EventDiscretePackage*>::iterator i = children.begin();
  
  while(i != children.end()){
    IEvent* thisEvent = (*i)->event;

    if( thisEvent->getEventType() == eventTop){
      temp = temp + "\"T/" + thisEvent->getEventName() + "\"";

    }else if( thisEvent->getEventType() == eventHigh){
      temp = temp + "\"H/" + thisEvent->getEventName() + "\"";

    }else if( thisEvent->getEventType() == eventMid){
      temp = temp + "\"M/" + thisEvent->getEventName() + "\"";

    }else if( thisEvent->getEventType() == eventLow){
      temp = temp + "\"L/" + thisEvent->getEventName() + "\"";

    }else if( thisEvent->getEventType() == eventBottom){
      temp = temp + "\"B/" + thisEvent->getEventName() + "\"";
    
    }else if( thisEvent->getEventType() == eventSound){
      temp = temp + "\"S/" + thisEvent->getEventName() + "\"";
    }else if (thisEvent->getEventType() == eventNote){
    	temp = temp + "\"N/" + thisEvent->getEventName() + "\"";
   	}
    

    i++;
    if(i == children.end()){
      break;
    }
    temp = temp + ",\n                    ";
  }

  temp = temp + "\n                 >";
  return temp;
}


IEvent::EventExtraInfo* IEvent::getEventExtraInfo(){
  return extraInfo;
}


void IEvent::SoundExtraInfo::setDeviation(std::string _deviation){
  deviation = _deviation;
}


std::string IEvent::SoundExtraInfo::getDeviation(){
  return deviation;
}


//void IEvent::SoundExtraInfo::setNumPartials(std::string _numPartial){
  //numPartials = _numPartial;
//}


std::string IEvent::SoundExtraInfo::getNumPartials(){
  char charbuffer[20];            
  sprintf( charbuffer, "%d", numPartials);
  return string(charbuffer);
}


//void IEvent::SoundExtraInfo::setSpectrum(std::string _spectrum){
  //spectrum = _spectrum;
//}


SpectrumPartial* IEvent::SoundExtraInfo::getSpectrumPartials(){
  return spectrumPartials;
}

SpectrumPartial* IEvent::SoundExtraInfo::addPartial(){

  SpectrumPartial* newPartial = new SpectrumPartial();
  SpectrumPartial* end = spectrumPartials;
  numPartials ++;
  
  if (spectrumPartials ==NULL){
    spectrumPartials = newPartial;
    return newPartial;
  }
  else {
    while (end->next!= NULL){
      end = end->next;
    }
    end->next = newPartial;
    newPartial->prev = end;
    return newPartial;
  }
}



bool IEvent::SoundExtraInfo::deletePartial(SpectrumPartial* _partial){
  if (numPartials ==1 ){
    return false;
  }
  if (spectrumPartials == _partial){
    spectrumPartials = spectrumPartials ->next;
    if (spectrumPartials != NULL){
      spectrumPartials->prev = NULL;
    }
  }

  else {
    SpectrumPartial* search = spectrumPartials;
    while (search != _partial){
      search = search->next;
    }
    
    search->prev->next = search->next;
    if (search->next != NULL){
      search->next->prev = search->prev;
    }
  }
  numPartials --;
  delete _partial;
  return true;


}




double EventLayer::getChildrenWeightSum(){
  double sum = 0;
  
  std::list<EventDiscretePackage*>::iterator i = children.begin(); 
  for (i; i!= children.end(); i++){
     
     sum += atof((*i)->weight.c_str());
     
  }

  return sum;

}



IEvent::BottomEventExtraInfo::BottomEventExtraInfo(){
  frequencyFlag = 0;
  frequencyContinuumFlag = 0;
  frequencyEntry1 = "";
  frequencyEntry2 = "";
  loudness = "";
  spatialization = "";
  reverb = "";
  modifiers = NULL;
  childTypeFlag =-1;
  
}
IEvent::BottomEventExtraInfo::~BottomEventExtraInfo(){
}

int IEvent::BottomEventExtraInfo::getFrequencyFlag(){ // 0 = Well_tempered, 1 = Fundamental, 2 = Continuum
  return frequencyFlag;
}

void IEvent::BottomEventExtraInfo::setFrequencyFlag(int _flag){
  frequencyFlag = _flag;
}
int IEvent::BottomEventExtraInfo::getFrequencyContinuumFlag(){// 0 = hertz, 1 = power of two
  return frequencyContinuumFlag;
}

void IEvent::BottomEventExtraInfo::setFrequencyContinuumFlag(int _flag){
  frequencyContinuumFlag = _flag;
}
    
std::string  IEvent::BottomEventExtraInfo::getFrequencyEntry1(){
  return frequencyEntry1;
}

void  IEvent::BottomEventExtraInfo::setFrequencyEntry1(std::string _string){
  frequencyEntry1 = _string;
}
std::string  IEvent::BottomEventExtraInfo::getFrequencyEntry2(){
  return frequencyEntry2;
}

void  IEvent::BottomEventExtraInfo::setFrequencyEntry2(std::string _string){
  frequencyEntry2 = _string;
}
std::string  IEvent::BottomEventExtraInfo::getLoudness(){
  return loudness;
}
void  IEvent::BottomEventExtraInfo::setLoudness(std::string _string){
  loudness = _string;
}
std::string  IEvent::BottomEventExtraInfo::getSpatialization(){
  return spatialization;
}
void  IEvent::BottomEventExtraInfo::setSpatialization(std::string _string){
  spatialization = _string;
}
std::string  IEvent::BottomEventExtraInfo::getReverb(){
  return reverb;
}
void  IEvent::BottomEventExtraInfo::setReverb(std::string _string){
  reverb = _string;
}
    
void IEvent::BottomEventExtraInfo::setChildTypeFlag(int _type){
	childTypeFlag = _type;
};

int IEvent::BottomEventExtraInfo::getChildTypeFlag(){
	return childTypeFlag;
} 


EventBottomModifier* IEvent::BottomEventExtraInfo::getModifiers(){
  return modifiers;
}


EventBottomModifier* IEvent::BottomEventExtraInfo::addModifier(){
  EventBottomModifier* newModifier = new EventBottomModifier();
  if (modifiers == NULL){
    modifiers = newModifier;
    return modifiers;
  } //special case
  
  
  EventBottomModifier* tail = modifiers;
  while (tail->next!= NULL){
    tail = tail->next;
  }
  tail->next = newModifier;
  return newModifier;
  
}  

void IEvent::BottomEventExtraInfo::removeModifier(EventBottomModifier* _modifier){
  if (modifiers == _modifier){ //if modifier to be removed is the head of list
    if (modifiers->next == NULL){ //the only modifier
      delete modifiers;
      modifiers = NULL;
      return;
    }
    else { //somethingelse in the back
      EventBottomModifier* temp = modifiers->next;
      delete modifiers;
      modifiers = temp;
      return;
    }
  }//end handling head of list
  
  else {
    EventBottomModifier* temp = modifiers->next;
    EventBottomModifier* temp2 = modifiers;
    while (temp != _modifier){
      temp2 = temp;
      temp = temp->next;
    }  
    
    temp2->next = temp->next;
    delete temp;
    return;
  }
}

EventBottomModifier::EventBottomModifier(){
  type = modifierTremolo;
  applyHowFlag = 0;
  probability = "";
  ampValue = "";
  rateValue = "";
  width = "";
  groupName = "";
  next = NULL;
}

EventBottomModifier::~EventBottomModifier(){
}

ModifierType EventBottomModifier::getModifierType(){
  return type;
}
void EventBottomModifier::setModifierType(ModifierType _type){
  type = _type;
}
  
int EventBottomModifier::getApplyHowFlag(){
  return applyHowFlag;
}
void EventBottomModifier::setApplyHowFlag(int _flag){
  applyHowFlag = _flag;
}
  
std::string  EventBottomModifier::getProbability(){
  return probability;
}
void  EventBottomModifier::setProbability(std::string _string){
  probability = _string;
}
std::string  EventBottomModifier::getAmpValue(){
  return ampValue;
}
void  EventBottomModifier::setAmpValue(std::string _string){
  ampValue = _string;
}
  
std::string  EventBottomModifier::getRateValue(){
  return rateValue;
}
void  EventBottomModifier::setRateValue(std::string _string){
  rateValue = _string;
}
  
std::string  EventBottomModifier::getWidth(){
  return width;
}
void  EventBottomModifier::setWidth(std::string _string){
  width = _string;
}
  
std::string  EventBottomModifier::getGroupName(){
  return groupName;
}
void  EventBottomModifier::setGroupName(std::string _string){
  groupName = _string;
}


int EventBottomModifier::getModifierTypeInt(){
  if (type ==   modifierTremolo){
    return 0;
  }
  else if (type ==  modifierVibrato){
    return 1;
  }
  else if (type == modifierGlissando){
    return 2;
  }
  else if (type ==  modifierBend){
    return 3;
  }
  else if (type ==  modifierDetune){
    return 4;
  }
  else if (type ==  modifierAmptrans){
    return 5;
  }
  else if (type ==  modifierFreqtrans){
    return 6;
  }
  else {
    return 7;
  }


}



std::string EventBottomModifier::getSaveToDiskString(){
  std::string stringbuffer = "";
  if (type == modifierTremolo){
    stringbuffer ="              <\n" 
                  "                \"TREMOLO\",\n"
                  "                "+ probability + ",\n"
                  "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
                  "                "+ ampValue +",\n" +
                  "                "+ rateValue + 
                  ((groupName ==""||groupName =="")?"\n              >":(
                  ",\n                <\"MUT_EX\", \"" + groupName + "\">\n              >"));
                  
  }
  else if (type == modifierVibrato){
    stringbuffer ="              <\n" 
                  "                \"VIBRATO\",\n"
                  "                "+ probability + ",\n"
                  "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
                  "                "+ ampValue +",\n" +
                  "                "+ rateValue + 
                  ((groupName ==""||groupName =="")?"\n              >":(
                  ",\n                <\"MUT_EX\", \"" + groupName + "\">\n              >"));
  
  }
  else if (type == modifierGlissando){
    stringbuffer ="              <\n" 
                  "                \"GLISSANDO\",\n"
                  "                "+ probability + ",\n"
                  "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
                  "                "+ ampValue +
                  ((groupName ==""||groupName =="")?"\n              >":(
                  ",\n                <\"MUT_EX\", \"" + groupName + "\">\n              >"));  
  }
  else if (type == modifierBend){
    stringbuffer ="              <\n" 
                  "                \"BEND\",\n"
                  "                "+ probability + ",\n"
                  "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
                  "                "+ ampValue +
                  ((groupName ==""||groupName =="")?"\n              >":(
                  ",\n                <\"MUT_EX\", \"" + groupName + "\">\n              >"));
  
  }
  else if (type == modifierDetune){
    stringbuffer ="              <\n" 
                  "                \"DETUNE\",\n"
                  "                "+ probability + ",\n"
                  "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
                  "                "+ ampValue +
                  ((groupName ==""||groupName =="")?"\n              >":(
                  ",\n                <\"MUT_EX\", \"" + groupName + "\">\n              >"));
  
  }
  else if (type == modifierAmptrans){
    stringbuffer ="              <\n" 
                  "                \"AMPTRANS\",\n"
                  "                "+ probability + ",\n"
                  "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
                  "                "+ ampValue +",\n" +
                  "                "+ rateValue +",\n" +
                  "                "+ width + 
                  ((groupName ==""||groupName =="")?"\n              >":(
                  ",\n                <\"MUT_EX\", \"" + groupName + "\">\n              >"));

  
  }
  else if (type == modifierFreqtrans){
    stringbuffer ="              <\n" 
                  "                \"FREQTRANS\",\n"
                  "                "+ probability + ",\n"
                  "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
                  "                "+ ampValue +",\n" +
                  "                "+ rateValue +",\n" +
                  "                "+ width + 
                  ((groupName ==""||groupName =="")?"\n              >":(
                  ",\n                <\"MUT_EX\", \"" + groupName + "\">\n              >"));


  
  }
  else{// type == modifierWave_type
    stringbuffer ="              <\n" 
                  "                \"WAVE_TYPE\",\n"
                  "                "+ probability + ",\n"
                  "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
                  "                "+ ampValue +
                  ((groupName ==""||groupName =="")?"\n              >":(
                  ",\n                <\"MUT_EX\", \"" + groupName + "\">\n              >"));
  
  }
  return stringbuffer;

}






std::string EventBottomModifier::getSaveLASSIEMetaDataString(){
  
  char temp1[10];
  char temp2[10];
  
  sprintf(temp1, "%d",(int) type);
  sprintf(temp2, "%d", applyHowFlag);  
  
  std::string stringbuffer = "<" + string(temp1) + "," + string(temp2) 
    + ",`" + probability 
    + "`,`" + ampValue
    + "`,`" + rateValue
    + "`,`" + width;
    
    
  if (groupName != "" && groupName != ""){  
    stringbuffer = stringbuffer +  "`,`" + groupName + "`>";
  }
  else {
    stringbuffer = stringbuffer + "`>";
  }
  return stringbuffer;

}













void IEvent::EnvelopeExtraInfo::setEnvelopeBuilder(std::string _string){
  envelopeBuilder = _string;
}

std::string IEvent::EnvelopeExtraInfo::getEnvelopeBuilder(){
  return envelopeBuilder;
}


void IEvent::SpatializationExtraInfo::setSpatializationBuilder(std::string _string){
  spatializationBuilder = _string;
}

std::string IEvent::SpatializationExtraInfo::getSpatializationBuilder(){
  return spatializationBuilder;
}


void IEvent::ReverbExtraInfo::setReverbBuilder(std::string _string){
  reverbBuilder = _string;
}

std::string IEvent::ReverbExtraInfo::getReverbBuilder(){
  return reverbBuilder;
}


void IEvent::SieveExtraInfo::setSieveBuilder(std::string _string){
  sieveBuilder = _string;
}

std::string IEvent::SieveExtraInfo::getSieveBuilder(){
  return sieveBuilder;
}

void IEvent::PatternExtraInfo::setPatternBuilder(std::string _string){
  patternBuilder = _string;
}

std::string IEvent::PatternExtraInfo::getPatternBuilder(){
  return patternBuilder;
}



IEvent::EventExtraInfo* IEvent::openExtraInfo(EventFactory* _event ,EventType _eventType){ //this function is not in used anymore


  char charBuffer[100];
  EventExtraInfo* newInfo;
  FileValue* value;
  if (_eventType == eventBottom){
    newInfo = (EventExtraInfo*) new BottomEventExtraInfo();
    value = _event->getFrequency();
    std::list<FileValue> frequencyList = value->getList();
    std::list<FileValue>::iterator frequencyListIter =frequencyList.begin();
    std::string frequencyType = frequencyListIter->getString();
    size_t foundWell_Tempered = frequencyType.find("WELL");
    size_t foundFundamental = frequencyType.find("FUN");
    if (foundWell_Tempered!=string::npos ){ // WellTempered
      newInfo->setFrequencyFlag(0);
      frequencyListIter++;
      int temp = frequencyListIter->getInt();
      sprintf(charBuffer,"%d", temp);
      newInfo->setFrequencyEntry1(string(charBuffer));
    }
    else if (foundFundamental != string::npos){ //fundamental
      newInfo->setFrequencyFlag(1);
      frequencyListIter++;
      float temp = frequencyListIter->getFloat();
      sprintf(charBuffer,"%.5f", temp);
      newInfo->setFrequencyEntry1(string(charBuffer));    

      frequencyListIter++;
      int temp2 = frequencyListIter->getInt();
      sprintf(charBuffer,"%d", temp2);
      newInfo->setFrequencyEntry2(string(charBuffer)); 
    }
    
    else { //continuum
      newInfo->setFrequencyFlag(2);
      frequencyListIter++;
      
      std:string freqMethod = frequencyListIter->getString();
      size_t foundPOW = freqMethod.find("POW");
      float temp;
      frequencyListIter++;
      if(foundPOW != string::npos){//pow2
        newInfo->setFrequencyContinuumFlag(1);
      }
      else{
        newInfo->setFrequencyContinuumFlag(0);
      }  
      temp = frequencyListIter->getFloat();
      sprintf(charBuffer,"%.5f", temp);
      newInfo->setFrequencyEntry1(string(charBuffer));      
    }
    value = _event->getLoudness();
    int temp = value->getInt();
    sprintf(charBuffer,"%d", temp);
    newInfo->setLoudness(string(charBuffer));     
    
    
    std::cout<<"BottomExtraInfo:\nFrequencyFlag: "<< newInfo->getFrequencyFlag()
             <<"\n    Frequency Entry1 :"<<newInfo->getFrequencyEntry1()
             <<"\n    Frequency Entry2 :"<<newInfo->getFrequencyEntry2()         
             <<"\n    loudness :"<<newInfo->getLoudness() //TODO implement spatialization and reverb               
             <<"\n\n\n\n"<<std::endl;
    

    value = _event->getModifiers();           
    std::list<FileValue> modifierList = value->getList();
    std::list<FileValue>::iterator modifierListIter =modifierList.begin();
    
    
    EventBottomModifier* currentModifier = NULL;
    EventBottomModifier* previousModifier = NULL;
    while (modifierListIter!= modifierList.end()){
      currentModifier = new EventBottomModifier();
      std::list<FileValue> thisModifierList = modifierListIter->getList(); 
      std::list<FileValue>::iterator modifierIter =thisModifierList.begin();
      std::string modifierType = modifierIter->getString();
      //TODO correctly parse object file names
      if (modifierType.find("TREM")!= string::npos){ //tremolo
        currentModifier->setModifierType(modifierTremolo);
      }
      else if (modifierType.find("VIB")!= string::npos){ //Vibrato
        currentModifier->setModifierType(modifierVibrato);
      }      
      else if (modifierType.find("GLIS")!= string::npos){ //Gliss
        currentModifier->setModifierType(modifierGlissando);
      }      
      else if (modifierType.find("BEND")!= string::npos){ //Bend
        currentModifier->setModifierType(modifierBend);
      }
      else if (modifierType.find("DETU")!= string::npos){ //Detune
        currentModifier->setModifierType(modifierDetune);
      }
      else if (modifierType.find("AMP")!= string::npos){ //amptran
        currentModifier->setModifierType(modifierAmptrans);
      }
      else if (modifierType.find("GLIS")!= string::npos){ //freq tran
        currentModifier->setModifierType(modifierFreqtrans);
      }      
      else { //waveype
        currentModifier->setModifierType(modifierWave_type);
      }
      
      if (previousModifier ==NULL){
        ((BottomEventExtraInfo*) newInfo)->modifiers = currentModifier;
        previousModifier = currentModifier;
      
      }
      else{
        previousModifier->next = currentModifier;
        previousModifier = currentModifier;
      }
      
      modifierListIter++;   
    
    }//end while
           
  }//end bottomextraevent
  
  
  
  
  else if (_eventType == eventSound){
 
    newInfo = (EventExtraInfo*) new SoundExtraInfo();
    value = _event->getNumPartials();

    int temp = value->getInt();

    sprintf(charBuffer,"%d", temp);  
    newInfo->setNumPartials(string(charBuffer));

    value = _event->getDeviation();
    float temp2 = value->getFloat();
    sprintf(charBuffer,"%.5f", temp2);  
    newInfo->setDeviation(string(charBuffer));
    

    //TODO  spectrum return a list. should parse it correctly
    /*
    value = _event->getSpectrum();
    list<FileValue> spectrumList= value->getList();
    
    bla bla bla 
 
    newInfo->setSpectrum(string(charBuffer));  
    */
   
  
  }
  else if (_eventType == eventEnv){//TODO  implement these craps
  
  }
  else if (_eventType == eventSiv){
  
  }
  else if (_eventType == eventSpa){
  
  }
  else if (_eventType == eventPat){
  
  }
  else if (_eventType == eventRev){
  
  }

  return newInfo;


}















void IEvent::IEventParseFile(std::string _fileName){
  YY_FLUSH_BUFFER;//flush the buffer make sure the buffer is clean
 FILE *yytmp;
  extern FILE *yyin;
  yytmp = fopen(_fileName.c_str(), "r");

  if (yytmp == NULL) {
    cout << "ERROR: File " << _fileName << " does not exist!" << endl;
    exit(1);
  }

  yyin = yytmp;
  


  
  
  extern map<const char*, FileValue*, ltstr> file_data;
  yyparse();  //after this step, extern map<const char*, FileValue*, ltstr> file_data will be filled;

  //map<const char*, FileValue*, ltstr>::iterator i = file_data.begin();
  
  //while (i != file_data.end()){
  //  std::cout<<"yaya!  "<<i->first<<"  "<<file_data[i->first]->getString()<<std::endl;
  //  i++;
 //}



  if (eventType >= 5){
    parseNonEvent();
    return;
  }



  
  FileValue* value;
  
  
  value = file_data["LASSIEeventName"];
  
  std::list<FileValue> nameAndOrderList = value->getList();
  
  if (nameAndOrderList.size()==0){ //old file format
    eventOrderInPalette = rand();
  }
  else {
    std::list<FileValue>::iterator iter = nameAndOrderList.begin();
  
    iter++;
    value = &(*iter);
    eventOrderInPalette = value->getInt(); //somehow this doesn't crash with the older version (no object order number) so i just keep it this way
  }
  
  
  
  value = file_data["LASSIEmaxChildDur"];
  if (value!= NULL){
    maxChildDur = value->getString();
  }
  else {
    value = file_data["maxChildDur"];
    maxChildDur = (value == NULL)?"": 
      FunctionGenerator::getFunctionString(value, functionReturnFloat);
  }
  
  
  value = file_data["LASSIEEDUPerBeat"];
  if (value!= NULL){
    unitsPerSecond = value->getString();
  }
  else {
    value = file_data["EDUPerBeat"];
    unitsPerSecond =(value == NULL)? "":  
      FunctionGenerator::getFunctionString(value, functionReturnFloat);
  }
  
  
  
  value = file_data["LASSIEtimeSignatureEntry1"];
  if (value!= NULL){
    timeSignatureEntry1 = value->getString();
    value = file_data["LASSIEtimeSignatureEntry2"];
    timeSignatureEntry2 =(value == NULL)? "": value ->getString();  
  }
  else {
    value = file_data["timeSignature"];
    if (value ==NULL){
      timeSignatureEntry1 =="";
      timeSignatureEntry2 =="";      
    }
    else {
      size_t whereIsSlash;
      size_t whereIsEqualSign;
      std::string stringbuffer = value->getString();
      whereIsSlash = stringbuffer.find("/");

      if (whereIsSlash==string::npos){
        timeSignatureEntry1 =="";
        timeSignatureEntry2 =="";
      }
      else {
        timeSignatureEntry1 = value->getString().substr(0, int(whereIsSlash));
        timeSignatureEntry2 = value->getString().substr(int(whereIsSlash)+1, value->getString().length()-1);  
      }
    }
  
  }
  

  
  value = file_data["LASSIEtempoMethodFlag"];
  if (value != NULL){
    tempoMethodFlag = value ->getInt(); //0 = as note value, 1 = as fraction
    value = file_data["LASSIEtempoPrefix"];
    tempoPrefix =(TempoPrefix) ((value == NULL)? 0: value ->getInt());    
    value = file_data["LASSIEtempoNoteValue"];
    tempoNoteValue =(TempoNoteValue)((value == NULL)? 0: value ->getInt());  
    value =file_data["LASSIEtempoFractionEntry1"]; 
    tempoFractionEntry1 = (value == NULL)? "": value->getString();
    value = file_data["LASSIEtempoFractionEntry2"];
    tempoFractionEntry2 = (value == NULL)? "": value->getString();
    value = file_data["LASSIEtempoValueEntry"];
    tempoValueEntry = (value == NULL)? "": value->getString();
  }
  else{
    size_t whereIsSlash;
    size_t whereIsEqualSign;
    value = file_data["tempo"];
    if (value == NULL){
        timeSignatureEntry1 =="";
        timeSignatureEntry2 =="";    
    
    
    
    }
    
    else {
      std::string stringbuffer = value->getString();
      whereIsSlash = stringbuffer.find("/");
      whereIsEqualSign = stringbuffer.find("=");

      if (whereIsSlash==string::npos){ // it's in "note value 
        tempoMethodFlag = 0;
        std::string firstHalf = value->getString().substr(0, int (whereIsSlash));
       
        if(firstHalf.find("thirt") != string::npos)
            tempoNoteValue = tempoNoteValueThirtySecond;
        else if(firstHalf.find("six") != string::npos)
          tempoNoteValue = tempoNoteValueSixteenth;
        else if(firstHalf.find("eig") != string::npos)
          tempoNoteValue = tempoNoteValueEighth;
        else if(firstHalf.find("quar") != string::npos)
          tempoNoteValue = tempoNoteValueQuarter;
        else if(firstHalf.find("hal") != string::npos)
          tempoNoteValue = tempoNoteValueHalf;
        else if(firstHalf.find("who") != string::npos)
          tempoNoteValue = tempoNoteValueWhole;
          
        if(firstHalf.find("doub") != string::npos)
              tempoPrefix = tempoPrefixDoubleDotted;
        else if(firstHalf.find("dot") != string::npos)
              tempoPrefix = tempoPrefixDotted;
        else if(firstHalf.find("tripl") != string::npos)
              tempoPrefix = tempoPrefixTriple;
        else if(firstHalf.find("quin") != string::npos)
          tempoPrefix = tempoPrefixQuintuple;
        else if(firstHalf.find("sext") != string::npos)
          tempoPrefix = tempoPrefixSextuple;
        else if(firstHalf.find("sept") != string::npos)
          tempoPrefix = tempoPrefixSeptuple;
        else tempoPrefix = tempoPrefixNone;  
    
        timeSignatureEntry1 =="";
        timeSignatureEntry2 =="";
      }
      else { //it's in fractional
        tempoMethodFlag = 1;
        tempoFractionEntry1 = value->getString().substr(0, int(whereIsSlash));
        tempoFractionEntry2 = value->getString().substr(int(whereIsSlash)+1, int(whereIsEqualSign) -1 - int(whereIsSlash)  );
      }
      tempoValueEntry =value->getString().substr(int(whereIsEqualSign)+1, value->getString().length()-1); 
    }
  
  }
  

  

  value = file_data["LASSIEnumChildrenEntry1"];
  if (value != NULL){
    numChildrenEntry1 = value->getString();  
    value = file_data["LASSIEnumChildrenEntry2"];
    numChildrenEntry2 = (value == NULL)? "": value->getString();
  
    value = file_data["LASSIEnumChildrenEntry3"];
    numChildrenEntry3 = (value == NULL)? "": value->getString();
   // 0 = fixed, 1 = density, 2 = By layer
  
    value = file_data["LASSIEflagNumChildren"];
    flagNumChildren = (value == NULL)? 0: value->getInt();  
  
  }
  else {
    //TODO implement this!
  }

  

  value = file_data["LASSIEchildEventDefEntry1"];
  childEventDefEntry1 = (value == NULL)? "": value->getString();
  
  value = file_data["LASSIEchildEventDefEntry2"];
  childEventDefEntry2 = (value == NULL)? "": value->getString();
  
  value = file_data["LASSIEchildEventDefEntry3"];
  childEventDefEntry3 = (value == NULL)? "": value->getString();
  
  value = file_data["LASSIEchildEventDefAttackSieve"];
  childEventDefAttackSieve = (value == NULL)? "": value->getString();
  
  value = file_data["LASSIEchildEventDefDurationSieve"];
  childEventDefDurationSieve = (value == NULL)? "": value->getString();



   // 0 = continuum, 1 = sweep, 2 = discrete
  
  value = file_data["LASSIEflagChildEventDef"];
  flagChildEventDef =(value == NULL)? 0: value ->getInt();

   // 0 = percentage, 1 = units, 2 = seconds
  
  value = file_data["LASSIEflagChildEventDefStartType"];
  flagChildEventDefStartType = (value == NULL)? 0: value->getInt();

   // 0 = percentage, 1 = units, 2 = seconds

  value = file_data["LASSIEflagChildEventDefDurationType"];
  flagChildEventDefDurationType = (value == NULL)? 0: value->getInt();



  value = file_data["LASSIEeventLayers"];
  
  std::list<FileValue> layersFileValue = value->getList();
  std::list<FileValue>::iterator i = layersFileValue.begin();
  
  for (i ; i != layersFileValue.end(); i ++){
    layers.push_back (new EventLayer(&(*i), this));
  }
  

  if (eventType == eventBottom){
  
  
    string firstChar = eventName.substr(0,1);
    int childTypeFlag = -1;
    if (firstChar =="s"){
    	childTypeFlag = 0;
    }
    else if (firstChar =="n"){
    	childTypeFlag = 1;
    }
    else {
    	childTypeFlag = 2;
   	}
    extraInfo = (EventExtraInfo*) new BottomEventExtraInfo(childTypeFlag); 
  
    
  }





  
  
  /*

 
    //numChildren
    value = event->getNumChildren();
  
    std::list<FileValue> numChildrenList = value->getList();
    std::list<FileValue>::iterator numChildrenIter =numChildrenList.begin();
    std::string numChildrenType = numChildrenIter->getString();
    size_t foundFixed = numChildrenType.find("FIXED");
    size_t foundDensity = numChildrenType.find("DENSITY");
    if (foundFixed!=string::npos){ // FIXED
      flagNumChildren = 0;
      if ( numChildrenList.size() ==1){ // numChildrenEntry1 was left blank
        numChildrenEntry1  = "";  
      }
      else {
        numChildrenIter ++;
        int temp = numChildrenIter->getInt();
        sprintf(charBuffer,"%d", temp);
        numChildrenEntry1 = string(charBuffer);
      }
      numChildrenEntry2  = "";
      numChildrenEntry3  = "";
    }
  
  
    else if (foundDensity != string::npos){ //Density
      flagNumChildren = 1;
      
      numChildrenIter ++;
      float temp = numChildrenIter->getFloat();
      sprintf(charBuffer,"%.5f", temp);
      numChildrenEntry1 = string(charBuffer);
      
      numChildrenIter ++;
      int temp2 = numChildrenIter->getInt();
      sprintf(charBuffer,"%d", temp2);
      numChildrenEntry2 = string(charBuffer);    
      
      numChildrenIter ++;
      temp2 = numChildrenIter->getInt();
      sprintf(charBuffer,"%d", temp2);
      numChildrenEntry3 = string(charBuffer);     

    }
    else { //by layer
      flagNumChildren =2;
      //constructing layers and connecting events will be done after all
      //events are created.
    } 



    //childEventDef
    value = event->getChildEventDef();
  
    std::list<FileValue> childEventDefList = value->getList();
    std::list<FileValue>::iterator childEventDefIter =childEventDefList.begin();
    std::string childEventDefType = childEventDefIter->getString();
    size_t foundConti = childEventDefType.find("CONTI");
    size_t foundSweep = childEventDefType.find("SWEEP");
    if   (foundConti!=string::npos|| foundSweep != string::npos ){ // Continuum/sweep
      size_t foundPercent;
      size_t foundUnit;
      flagChildEventDef = (foundConti==string::npos)?1:0;
      
      childEventDefIter ++;
      float temp = childEventDefIter->getFloat();
      sprintf(charBuffer,"%.5f", temp);
      childEventDefEntry1 = string(charBuffer);
      
      
      childEventDefIter ++;
      std::string temp2 = childEventDefIter->getString();
      foundPercent = temp2.find("PERCENT");
      foundUnit = temp2.find( "UNIT");
      
      if (foundPercent != string::npos){
        flagChildEventDefStartType = 0;
      }
    else if (foundUnit != string::npos){
          flagChildEventDefStartType = 1;
      }
      else {
        flagChildEventDefStartType =2;
      }
    
    
      childEventDefIter ++;
      temp = childEventDefIter->getFloat();
      sprintf(charBuffer,"%.5f", temp);
      childEventDefEntry2 = string(charBuffer);
      
      childEventDefIter ++;
      temp = childEventDefIter->getFloat();
      sprintf(charBuffer,"%.5f", temp);
      childEventDefEntry3 = string(charBuffer);    
    
   
      childEventDefIter ++;
      temp2 = childEventDefIter->getString();
      foundPercent = temp2.find("PERCENT");
      foundUnit = temp2.find( "UNIT");
    
      if (foundPercent != string::npos){
        flagChildEventDefDurationType = 0;
      }
     else if (foundUnit != string::npos){
       flagChildEventDefDurationType = 1;
     }
      else {
        flagChildEventDefDurationType =2;
      }
    

    }
  
    else{ //Discrete
      flagChildEventDef =2;
    //the rest will be handled when linking events 
  
    }

  


    EventLayer* newLayer = new EventLayer();
    layers.push_back(newLayer);
  }// end normal events
  
  */
  
  /*
  std::cout<<"Make new IEvent. Name: " << eventName
           <<"\n  maxChildDur = " <<maxChildDur
           <<"\n  timeSignatureEntry1 = "<<timeSignatureEntry1
           <<"\n  timeSignatureEntry2 = "<<timeSignatureEntry2
           <<"\n  EDUPerBeat = " <<unitsPerSecond
           <<"\n  TempoFractionEntry1 = "<< tempoFractionEntry1
           <<"\n  TempoFractionEntry2 = "<< tempoFractionEntry2
           <<"\n  tempoPrefix = "<< tempoPrefix
           <<"\n  tempoNoteValue = "<<tempoNoteValue
           <<"\n  tempoMethodFlag = "<<tempoMethodFlag
           <<"\n  tempoValueEntry = "<<tempoValueEntry
           <<"\n  flagNumChildren = "<<flagNumChildren
           <<"\n  numChildrenEntry1 = "<<numChildrenEntry1
           <<"\n  numChildrenEntry2 = "<<numChildrenEntry2
           <<"\n  numChildrenEntry3 = "<<numChildrenEntry3 
           <<"\n  flagchildEventDef = "<<flagChildEventDef
           <<"\n  ChildEventDefEntry1 = "<<childEventDefEntry1
           <<"\n  ChildEventDefEntry2 = "<<childEventDefEntry2           
           <<"\n  ChildEventDefEntry3 = "<<childEventDefEntry3
           <<"\n  flagCHildEventDefStartType = "<<flagChildEventDefStartType
           <<"\n  flagCHildEventDefDurationType = "<<flagChildEventDefDurationType
           <<"\n\n\n\n"<<std::endl;
           

  */






/*

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
  
  */
  
    file_data.clear();
    fclose (yyin);
    YY_FLUSH_BUFFER; //reset parser buffer so that next file would be parsed correctly even if the current one parse fail.

}




IEvent::BottomEventExtraInfo::BottomEventExtraInfo(int _childTypeFlag){

	childTypeFlag = _childTypeFlag;
	
  extern map<const char*, FileValue*, ltstr> file_data;
  FileValue* value;
   
  
  value = file_data["LASSIEBOTTOMfrequencyFlag"]; 
  frequencyFlag = (value == NULL)? 0: value->getInt(); 
  
  value = file_data["LASSIEBOTTOMfrequencyContinuumFlag"];  
  frequencyContinuumFlag = (value == NULL)? 0: value->getInt(); //0 = hertz, 1 =] power of two

  value = file_data["LASSIEBOTTOMfrequencyEntry1"]; 
  frequencyEntry1 = (value == NULL)? "": value->getString();
 
  value = file_data["LASSIEBOTTOMfrequencyEntry2"]; 
  frequencyEntry2  = (value == NULL)? "": value->getString();

  value = file_data["LASSIEBOTTOMloudness"]; 
  loudness  = (value == NULL)? "": value->getString();

  value = file_data["LASSIEBOTTOMspatialization"]; 
  spatialization  = (value == NULL)? "": value->getString();

  value = file_data["LASSIEBOTTOMreverb"]; 
  reverb  = (value == NULL)? "": value->getString();  
  

  //read modifiers
  modifiers = NULL;
  
  
  value = file_data["LASSIEBOTTOMmodifiers"];
  
  if (value != NULL ){    
    std::list<FileValue> modifierList = value->getList();
   


    std::list<FileValue>::iterator modifierListIter =modifierList.begin();
    
    
    EventBottomModifier* currentModifier = NULL;
    EventBottomModifier* previousModifier = NULL;
    while (modifierListIter!= modifierList.end()){
      currentModifier = new EventBottomModifier();
      std::list<FileValue> thisModifierList = modifierListIter->getList(); 
      std::list<FileValue>::iterator modifierIter =thisModifierList.begin();


      currentModifier->setModifierType((ModifierType) modifierIter->getInt());
      
      modifierIter++;
      currentModifier->setApplyHowFlag(modifierIter->getInt());
      
      modifierIter++;
      currentModifier->setProbability(modifierIter->getString());      
      
      modifierIter++; 
      currentModifier->setAmpValue(modifierIter->getString()); 
      
      modifierIter++; 
      currentModifier->setRateValue(modifierIter->getString()); 
      
      modifierIter++; 
      currentModifier->setWidth(modifierIter->getString()); 
      
      modifierIter++; 
      
      if (modifierIter != thisModifierList.end()){
        currentModifier->setGroupName(modifierIter->getString()); 
      }
     
      if (previousModifier ==NULL){
        modifiers = currentModifier;
        previousModifier = currentModifier;
      
      }
      else{
        previousModifier->next = currentModifier;
        previousModifier = currentModifier;
      }
      
      modifierListIter++;   
    
    }//end while

  } //end if
  

}





EventLayer::EventLayer(FileValue* _thisLayerFileValue,IEvent* _thisEvent){
  thisIEvent = _thisEvent;


  std::list<FileValue> fileValueList = _thisLayerFileValue->getList();
  
  std::list<FileValue>::iterator fileValueListIter = fileValueList.begin();
  
  byLayer = fileValueListIter->getString(); //the first value in the list is teh name  
  
  fileValueListIter ++;
  std::list<FileValue> discretePackages = fileValueListIter->getList();

  std::list<FileValue>::iterator discretePackagesIter = discretePackages.begin();
  if (discretePackagesIter->getInt() == 666){//see dummy 
    return;
  }
  
  for (discretePackagesIter;discretePackagesIter!= discretePackages.end(); discretePackagesIter++){
    children.push_back(new EventDiscretePackage (&(*discretePackagesIter)))  ;
  } 


}


EventDiscretePackage::EventDiscretePackage( FileValue* _thisPackageFileValue){    
  event = NULL;
  
  std::list<FileValue> stringList = _thisPackageFileValue->getList();
  std::list<FileValue>::iterator i = stringList.begin();
 
  eventName = i->getString();
  i ++;
  eventType = (EventType) i->getInt();
  i ++;
  weight = i->getString();
  i ++;
  attackEnv = i->getString();
  i ++;
  attackEnvScale = i->getString();
  i ++;
  durationEnv = i->getString();
  i ++;
  durationEnvScale = i->getString();
}

std::string EventLayer::getLASSIEMetaDataString(){
  std::string a = "<`"+byLayer+ "`,<";
  
  std::list<EventDiscretePackage*>::iterator discretePackagesIter = children.begin();
  
  while (discretePackagesIter != children.end()){
    a = a + (*discretePackagesIter)->getLASSIEMetadataString();
  
    discretePackagesIter++;
    if (discretePackagesIter != children.end()){
      a = a + ",";
    }
  }
  
  if (children.size() ==0){
    a = a + "666"; //dummy
  }
  a = a + ">>";
  
  return a;
}



std::string EventDiscretePackage::getLASSIEMetadataString(){

  char charBuffer[5];
  sprintf (charBuffer, "%d", (int) event->getEventType());

  std::string a = "<`" + event->getEventName() + "`," 
              + string(charBuffer) +",`" //this is eventType
              + weight + "`,`" 
              + attackEnv + "`,`"
              + attackEnvScale + "`,`"
              + durationEnv + "`,`"
              + durationEnvScale + "`>" ;
              
  return a;
                
}





void IEvent::link(ProjectViewController* _projectView){  
  std::list<EventLayer*>::iterator i = layers.begin();
  for (i ; i != layers.end(); i ++){
    (*i)->link(_projectView, this); //link each layer
  }
  
  
  
  
 
}

void EventLayer::link(ProjectViewController* _projectView, IEvent* _thisEvent){
  std::list<EventDiscretePackage*>::iterator i = children.begin();
  for (i ; i != children.end(); i ++){
    (*i)->link(_projectView, _thisEvent); //link each layer
  }

}

void EventDiscretePackage::link(ProjectViewController* _projectView, IEvent* _thisEvent){
    event = _projectView->getEventByTypeAndName(eventType,eventName);
    event->addParent(_thisEvent);
}









void IEvent::parseNonEvent(){
  extern map<const char*, FileValue*, ltstr> file_data;
 
  
  FileValue* value;
  
  value = file_data["LASSIEeventName"];
  if (value!= NULL){
    std::list<FileValue> nameAndOrderList = value->getList();
    if (nameAndOrderList.size()==0){ //old file format
    eventOrderInPalette = rand();
    }
    
    else {
      std::list<FileValue>::iterator iter = nameAndOrderList.begin();
  
      iter++;
      value = &(*iter);
      eventOrderInPalette = value->getInt();
    }
  }
  else {
  
     //old file format
    eventOrderInPalette = rand();
  }
  
  if (eventType == eventSound){
    extraInfo = (EventExtraInfo*) new SoundExtraInfo();
    value = file_data["LASSIESOUNDnumPartials"];
    //extraInfo-> setNumPartials((value == NULL)? "": value->getString());

    value = file_data["LASSIESOUNDdeviation"];
    extraInfo-> setDeviation((value == NULL)? "": value->getString());
    
    value = file_data["LASSIESOUNDspectrum"];
    
    std::list<FileValue> fileValueList = value->getList();
    std::list<FileValue>::iterator fileValueListIter = fileValueList.begin();
    SpectrumPartial* thisPartial = NULL;
    if (fileValueList.size()!= 0){
    	thisPartial = extraInfo->getSpectrumPartials();
      thisPartial->envString = fileValueListIter->getString();    
      fileValueListIter++;
    }
    
    for (fileValueListIter; fileValueListIter!= fileValueList.end(); fileValueListIter++){
      thisPartial = extraInfo->addPartial();
      thisPartial->envString = fileValueListIter->getString();       
    }



    
    //extraInfo->setSpectrum((value == NULL)? "": value->getString());
  }
  else if (eventType == eventEnv){
    extraInfo = (EventExtraInfo*) new EnvelopeExtraInfo();
    value = file_data["LASSIEENV"];
    extraInfo->setEnvelopeBuilder((value == NULL)? "": value->getString());

  }
  else if (eventType == eventSiv){
    extraInfo = (EventExtraInfo*) new SieveExtraInfo();
    value = file_data["LASSIESIV"];
    extraInfo->setSieveBuilder((value == NULL)? "": value->getString());

  }
  else if (eventType == eventSpa){
    extraInfo = (EventExtraInfo*) new SpatializationExtraInfo();
    value = file_data["LASSIESPA"];
    extraInfo->setSpatializationBuilder((value == NULL)? "": value->getString());

  }  
  else if (eventType == eventPat){
    extraInfo = (EventExtraInfo*) new PatternExtraInfo();
    value = file_data["LASSIEPAT"];
    extraInfo->setPatternBuilder((value == NULL)? "": value->getString());

  }  
  else if (eventType == eventRev){
    extraInfo = (EventExtraInfo*) new ReverbExtraInfo();
    value = file_data["LASSIEREV"];
    extraInfo->setReverbBuilder((value == NULL)? "": value->getString());

  }
  else if (eventType == eventNote){
    extraInfo = (EventExtraInfo*) new NoteExtraInfo();
    value = file_data["noteModifiers"];
    if (value){
      std::list<FileValue> fileValueList = value->getList();
      std::list<FileValue>::iterator fileValueListIter = fileValueList.begin();
    
      for (fileValueListIter; fileValueListIter!= fileValueList.end(); fileValueListIter++){
        extraInfo->addNoteModifiers(fileValueListIter->getString());     
      }
    }
  }  

}  



void IEvent::addParent(IEvent* _event){
  parents.push_back(_event);
}


bool IEvent::removeParent(IEvent* _event){
  bool success = false;
  std::list<IEvent*>::iterator iter =  parents.begin();
  
  while (iter != parents.end()){
    if (*iter == _event){
      success = true;
      break;
    }  
    iter++;
  } //end while
  
  if (success){
    parents.erase(iter);
  }

  return success;
  
}



bool EventLayer::removeChild(EventDiscretePackage* _child){
  bool success = false;
  std::list<EventDiscretePackage*>::iterator iter =  children.begin();
  
  while (iter != children.end()){
    if (*iter == _child){
      success = true;
      break;
    }  
    iter++;
  } //end while
  
  if (success){
    children.erase(iter);
  }

  delete _child;
  return success;


}




void EventLayer::deleteLayer(){
  thisIEvent->deleteLayer (this);
}

void IEvent::deleteLayer (EventLayer* _deleteLayer){
  std::list<EventLayer*>::iterator i = layers.begin();
  
  while (*i != _deleteLayer){
    i++;
  }
  layers.erase(i);
  
  delete (*i);
}



IEvent::NoteExtraInfo::NoteExtraInfo(){
}

IEvent::NoteExtraInfo::~NoteExtraInfo(){
}

std::list<std::string> IEvent::NoteExtraInfo::getNoteModifiers(){
  return modifiers;
}
  
void IEvent::NoteExtraInfo::addNoteModifiers(std::string _modifier){
  modifiers.push_back(_modifier);
}    


void IEvent::NoteExtraInfo::clearNoteModifiers(){
  modifiers.clear();
}


IEvent::IEvent(IEvent* _original, string _newName){
	eventName = _newName;
  oldEventName = "";
  eventOrderInPalette = 0;
	eventType = _original->eventType;
  maxChildDur = _original->maxChildDur;

  unitsPerSecond = _original->unitsPerSecond;
  timeSignatureEntry1 = _original->timeSignatureEntry1;
  timeSignatureEntry2 = _original->timeSignatureEntry2;
  
  tempoMethodFlag = _original->tempoMethodFlag; //0 = as note value, 1 = as fraction
  tempoPrefix = _original-> tempoPrefix;
  tempoNoteValue = _original-> tempoNoteValue;
  tempoFractionEntry1 = _original->tempoFractionEntry1;
  tempoFractionEntry2 = _original->tempoFractionEntry2;
  tempoValueEntry = _original->tempoValueEntry;  
  
  
  flagNumChildren    = _original->flagNumChildren; // 0 = fixed, 1 = density
  numChildrenEntry1  = _original->numChildrenEntry1;
  numChildrenEntry2  = _original->numChildrenEntry2;
  numChildrenEntry3  = _original->numChildrenEntry3;
  changedButNotSaved = true;
  
  childEventDefEntry1 = _original->childEventDefEntry1;
  childEventDefEntry2 = _original->childEventDefEntry2;
  childEventDefEntry3 = _original->childEventDefEntry3;

  flagChildEventDef             = _original->flagChildEventDef;
  flagChildEventDefStartType    = _original->flagChildEventDefStartType;
  flagChildEventDefDurationType = _original->flagChildEventDefDurationType;
  
  
  childEventDefAttackSieve = _original->childEventDefAttackSieve;
  childEventDefDurationSieve = _original->childEventDefDurationSieve;
  
  

  if (eventType ==eventBottom){
  	extraInfo = (EventExtraInfo*) new BottomEventExtraInfo((BottomEventExtraInfo*) _original->extraInfo);

  }
  else if (eventType== eventRev){
  	extraInfo = (EventExtraInfo*) new ReverbExtraInfo((ReverbExtraInfo*) _original->extraInfo);
  }
  else if (eventType == eventSiv){
  	extraInfo = (EventExtraInfo*) new SieveExtraInfo((SieveExtraInfo*) _original->extraInfo);
  }
  else if (eventType == eventEnv){
  	extraInfo = (EventExtraInfo*) new EnvelopeExtraInfo((EnvelopeExtraInfo*) _original->extraInfo);
  }
  else if (eventType == eventSpa){
  	extraInfo = (EventExtraInfo*) new SpatializationExtraInfo((SpatializationExtraInfo*) _original->extraInfo);
  }
  else if (eventType == eventPat){
  	extraInfo = (EventExtraInfo*) new PatternExtraInfo((PatternExtraInfo*) _original->extraInfo);
  }
  else if (eventType == eventSound){
  	extraInfo = (EventExtraInfo*) new SoundExtraInfo((SoundExtraInfo*) _original->extraInfo);
  }
  else if (eventType == eventNote){
  	extraInfo = (EventExtraInfo*) new NoteExtraInfo((NoteExtraInfo*) _original->extraInfo);
  }
  
  //layers go here
  
  list<EventLayer*>::iterator iter = _original->layers.begin();
  
  while (iter!= _original->layers.end()){
  
  	EventLayer* newLayer = new EventLayer(this, *iter);
  
  	layers.push_back(newLayer);
  	iter++;
  }
  
  
}



IEvent::ReverbExtraInfo::ReverbExtraInfo(ReverbExtraInfo* _original){
	reverbBuilder = _original->reverbBuilder;
}

IEvent::NoteExtraInfo::NoteExtraInfo(NoteExtraInfo* _original){
	 modifiers = _original->modifiers;
}




IEvent::SoundExtraInfo::SoundExtraInfo(SoundExtraInfo* _original){
	    
    numPartials = _original->numPartials;
    deviation = _original->deviation;
    
    SpectrumPartial* currentOriginalPartial = _original->spectrumPartials;
    SpectrumPartial* currentPartial = NULL;
    SpectrumPartial* previousPartial = NULL;
    spectrumPartials = new SpectrumPartial();
    currentPartial = spectrumPartials;
    spectrumPartials->envString = currentOriginalPartial->envString;
    
    while (currentOriginalPartial->next != NULL){
    	currentOriginalPartial = currentOriginalPartial->next;
    
    	previousPartial = currentPartial;
    	currentPartial = new SpectrumPartial();
    	currentPartial->envString = currentOriginalPartial->envString;
    	currentPartial->prev = previousPartial;
    	previousPartial->next = currentPartial;
    
   
    }
    
     
}


IEvent::BottomEventExtraInfo::BottomEventExtraInfo(BottomEventExtraInfo* _original){

  
	frequencyFlag = _original->frequencyFlag; // 0 = Well_tempered, 1 = Fundamental, 2 = Continuum
  frequencyContinuumFlag = _original->frequencyContinuumFlag; //0 = hertz, 1 =] power of two
  childTypeFlag = _original->childTypeFlag; // 0 = sound, 1 = note, 2 = visual
  frequencyEntry1 = _original->frequencyEntry1;
  frequencyEntry2 = _original->frequencyEntry2;
  loudness = _original->loudness;
  spatialization = _original->spatialization;
  reverb = _original->reverb;
    
  
  if (_original->modifiers == NULL){    
  	modifiers = NULL;

  }
  else{
  
  	modifiers = new EventBottomModifier(_original->modifiers);
  
  	EventBottomModifier* currentOriginalModifier = _original->modifiers;
  	EventBottomModifier* prevModifier = NULL;
  	EventBottomModifier* currentModifier = modifiers;
  
  	while (currentOriginalModifier->next!= NULL){
  		currentOriginalModifier = currentOriginalModifier->next;
  		prevModifier= currentModifier;
  		currentModifier = new EventBottomModifier(currentOriginalModifier);
  		prevModifier->next = currentModifier;
  
  	}
  }  	    
}


EventBottomModifier::EventBottomModifier(EventBottomModifier* _original){
	next=NULL;
  type = _original->type;
  applyHowFlag = _original->applyHowFlag;
  probability = _original->probability;
  ampValue = _original->ampValue;
  rateValue = _original->rateValue;
  width = _original->width;
  groupName = _original->groupName;

}




EventLayer::EventLayer(IEvent* _thisEvent, EventLayer* _originalLayer){

	byLayer = _originalLayer->byLayer;
 	thisIEvent = _thisEvent;
    
  list<EventDiscretePackage*>::iterator iter = _originalLayer->children.begin(); 
  while (iter != _originalLayer->children.end()){
   
    EventDiscretePackage* newPackage = new EventDiscretePackage(*iter);
    children.push_back(newPackage);
     
    iter++;
     
   }

}

EventDiscretePackage::EventDiscretePackage(EventDiscretePackage* _originalPackage){
    event = _originalPackage->event;
    eventType  = _originalPackage->eventType; // this one and eventName is used to store info to in order to link
    eventName = _originalPackage->eventName;
    weight = _originalPackage->weight;
    attackEnv = _originalPackage->attackEnv;
    attackEnvScale = _originalPackage->attackEnvScale;
    durationEnv = _originalPackage->durationEnv;
    durationEnvScale = _originalPackage->durationEnvScale;
}







void IEvent::setEventOrderInPalette(int _number){
  eventOrderInPalette = _number;
  changedButNotSaved = true;
}
  
int IEvent::getEventOrderInPalette(){
  return eventOrderInPalette;
}




string IEvent::getEventFolderName(){
    switch(eventType){
    case 0:
      return "Top";
    case 1:
      return "High";
    case 2:
      return "Mid";
    case 3:
      return "Low";
    case 4:
      return "Bottom";
    case 5:
      return "Spectrum";
    case 6:
      return "Envelope";
    case 7:
      return "Sieve";
    case 8:
      return "Spatialization";
    case 9:
      return "Pattern";
    case 10:
      return "Reverb";
    case 11:
      return "Folder";
    case 12:
      return "Note";
  }
}


bool IEvent::haveString(string _string){
  size_t position = maxChildDur.find (_string, 0);
  if (position != -1){
    return true;
  }
  
  position = timeSignatureEntry1.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = timeSignatureEntry2.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = unitsPerSecond.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = tempoValueEntry.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = tempoFractionEntry1.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = tempoFractionEntry2.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = numChildrenEntry1.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = numChildrenEntry2.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = numChildrenEntry3.find (_string, 0);
  if (position != -1){
    return true;
  }

  std::string thisLayer;
  std::list<EventLayer*>::iterator i = layers.begin();
  bool firstNumberExisted = false;
  while(i != layers.end()){
    thisLayer = (*i)->outputChildrenNameString();
    position = thisLayer.find (_string, 0);
    if (position != -1){
       return true;
    }   
    i++;     
  }

  position = childEventDefEntry1.find (_string, 0);
  if (position != -1){
    return true;
  }
  
  position = childEventDefEntry2.find (_string, 0);
  if (position != -1){
    return true;
  }
  
  position = childEventDefEntry3.find (_string, 0);
  if (position != -1){
    return true;
  }
  
  position = numChildrenEntry3.find (_string, 0);
  if (position != -1){
    return true;
  }
  
  position = numChildrenEntry3.find (_string, 0);
  if (position != -1){
    return true;
  }
  
  position = childEventDefAttackSieve.find (_string, 0);
  if (position != -1){
    return true;
  }
  
    position = childEventDefDurationSieve.find (_string, 0);
  if (position != -1){
    return true;
  }    

  if (extraInfo!= NULL){
    return extraInfo->haveString(_string);  
  }   
  return false;

}


bool IEvent::EnvelopeExtraInfo::haveString(string _string){
  size_t position = envelopeBuilder.find (_string, 0);
  if (position != -1){
    return true;
  }
  return false;
}

bool IEvent::PatternExtraInfo::haveString(string _string){
  size_t position = patternBuilder.find (_string, 0);
  if (position != -1){
    return true;
  }
  return false;
}
bool IEvent::SpatializationExtraInfo::haveString(string _string){
  size_t position = spatializationBuilder.find (_string, 0);
  if (position != -1){
    return true;
  }
  return false;
}
bool IEvent::SieveExtraInfo::haveString(string _string){
  size_t position = sieveBuilder.find (_string, 0);
  if (position != -1){
    return true;
  }
  return false;
}
bool IEvent::ReverbExtraInfo::haveString(string _string){
  size_t position = reverbBuilder.find (_string, 0);
  if (position != -1){
    return true;
  }
  return false;
}

bool IEvent::NoteExtraInfo::haveString(string _string){
  return false;

}


bool IEvent::SoundExtraInfo::haveString(string _string){
  size_t position = deviation.find (_string, 0);
  if (position != -1){
    return true;
  }

  position = getSoundSpectrumEnvelopesString().find (_string, 0);
  if (position != -1){
    return true;
  }  

  return false;

}




bool IEvent::BottomEventExtraInfo::haveString(string _string){
  size_t position = frequencyEntry1.find (_string, 0);
  if (position != -1){
    return true;
  } 
  position = frequencyEntry2.find (_string, 0);
  if (position != -1){
    return true;
  }   
  position = loudness.find (_string, 0);
  if (position != -1){
    return true;
  }   
  position = spatialization.find (_string, 0);
  if (position != -1){
    return true;
  }   
  position = reverb.find (_string, 0);
  if (position != -1){
    return true;
  }   


    
  EventBottomModifier* mod = getModifiers();
  if (mod!= NULL){
    
    string stringbuffer = "";
    
 
    while (mod != NULL){
      stringbuffer = stringbuffer + mod->getSaveToDiskString();
    
      if (mod->next!=NULL){
        stringbuffer = stringbuffer + ",\n";
      }
      mod = mod->next;
    }
    position = stringbuffer.find (_string, 0);
    if (position != -1){
      return true;
    }     
  }
  return false;
}

