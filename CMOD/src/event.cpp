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
//  event.ccp
//
//----------------------------------------------------------------------------//

#include "event.h"
#include "eventfactory.h"
#include "sieve.h"

extern map<string, EventFactory*> factory_lib;
extern ofstream * outputFile;

//----------------------------------------------------------------------------//

Event::Event(float stime, float dur, int type, string name, int level) {
  myStartTime = stime;
  myDuration = dur;
  myType = type;
  myName = name;
  printLevel = level;

  childEventDef = NULL;
  discreteMat = NULL;
}

//----------------------------------------------------------------------------//

Event::Event(const Event& origEvent) {
  myStartTime = origEvent.myStartTime;
  myDuration = origEvent.myDuration;
  myType = origEvent.myType;
  myName = origEvent.myName;
  printLevel = origEvent.printLevel;

  maxChildDur = origEvent.maxChildDur;
  unitsPerSecond = origEvent.unitsPerSecond;
  unitsPerBar = origEvent.unitsPerBar;

  layerVect = origEvent.layerVect;
  typeVect = origEvent.typeVect;

  numChildren = origEvent.numChildren;
  layerNumChildren = origEvent.layerNumChildren;
  layerRemainingChildren = origEvent.layerRemainingChildren;
  layerDensity =  origEvent.layerDensity;

  // make copies of all the subevents
  for (int i = 0; i < origEvent.childEvents.size(); i++) {
    childEvents.push_back( new Event(*origEvent.childEvents[i]) );
  }

  if (origEvent.childEventDef != NULL) {
    childEventDef = new FileValue( *origEvent.childEventDef );
  } else {
    childEventDef = NULL;
  }

  if (origEvent.discreteMat != NULL) {
    discreteMat = new Matrix(*(origEvent.discreteMat));
  } else {
    discreteMat = NULL;
  }

  //Note: All other vars are temporary things used in calculations
}

//----------------------------------------------------------------------------//

Event& Event::operator=(const Event& rhs) {
  myStartTime = rhs.myStartTime;
  myDuration = rhs.myDuration;
  myType = rhs.myType;
  myName = rhs.myName;
  printLevel = rhs.printLevel;

  maxChildDur = rhs.maxChildDur;
  unitsPerSecond = rhs.unitsPerSecond;
  unitsPerBar = rhs.unitsPerBar;

  layerVect = rhs.layerVect;
  typeVect = rhs.typeVect;

  numChildren = rhs.numChildren;
  layerNumChildren = rhs.layerNumChildren;
  layerRemainingChildren = rhs.layerRemainingChildren;
  layerDensity =  rhs.layerDensity;

  // make copies of all the subevents
  for (int i = 0; i < rhs.childEvents.size(); i++) {
    childEvents.push_back( new Event(*rhs.childEvents[i]) );
  }

  if (rhs.childEventDef != NULL) {
    childEventDef = new FileValue( *rhs.childEventDef );
  } else {
    childEventDef = NULL;
  }

  if (rhs.discreteMat != NULL) {
    discreteMat = new Matrix(*(rhs.discreteMat));
  } else {
    discreteMat = NULL;
  }

  //Note: All other vars are temporary things used in calculations

  return *this;
}

//----------------------------------------------------------------------------//

Event::~Event() {
  for (int i = 0; i < childEvents.size(); i++) {
    delete childEvents[i];
  }
  if (childEventDef != NULL) {
    delete childEventDef;
  }
  if (discreteMat != NULL) {
    delete discreteMat;
  }
}

//----------------------------------------------------------------------------//

void Event::initChildMaxDur( float aMaxDur ) {
  maxChildDur = aMaxDur;
}

//----------------------------------------------------------------------------//

void Event::initDiscreteInfo( int aUnitsPerSecond, int aUnitsPerBar ) {
  if (aUnitsPerSecond <= 0) {
    unitsPerSecond = 1;
  } else {
    unitsPerSecond = aUnitsPerSecond;
  }

  if (aUnitsPerBar <= 0) {
    unitsPerBar = 4;
  } else {
    unitsPerBar = aUnitsPerBar;
  }
}
//----------------------------------------------------------------------------//

void Event::initChildNames( FileValue* childNames ) {
  list<FileValue>* layersList = childNames->getListPtr(this);
  list<FileValue>::iterator iter = layersList->begin();

  while (iter != layersList->end()) {
    list<FileValue>* currLayer = iter->getListPtr(this);
    list<FileValue>::iterator currLayerIter = currLayer->begin();

    vector<string> currLayerVect;
    while (currLayerIter != currLayer->end()) {
      currLayerVect.push_back( currLayerIter->getString(this) );
      currLayerIter++;

      typeVect.push_back( currLayerVect.back() ); // keep track of all the types
    }
    layerVect.push_back( currLayerVect );
    iter++;
  }
}

//----------------------------------------------------------------------------//

void Event::initNumChildren( FileValue* numChildrenFV ) {
  if (layerVect.size() == 0) {
    cerr << "Error: ChildNames not initialized!" << endl;
    exit(1);
  }

  list<FileValue>* numChildrenArgs = numChildrenFV->getListPtr(this);
  list<FileValue>::iterator iter = numChildrenArgs->begin();

  string method = iter->getString(this);
  iter++;

  if (method == "FIXED") {
    // do for each layer
    if (iter == numChildrenArgs->end()) {
      // no additional args ... create same num children as types
      numChildren = typeVect.size();
    } else {
      // last arg is number of children to create
      numChildren = iter->getInt(this);
    }

  } else if (method == "BY_LAYER") {
    list<FileValue>* numPerLayer = iter->getListPtr(this);
    list<FileValue>::iterator numPerLayerIt = numPerLayer->begin();

    // check for errors
    if (numPerLayer->size() != layerVect.size()) {
      cerr << "Event::initNumChildren error in file " << myName << endl;
      cerr << "    BY_LAYER list isn't the same size as the layerVect" << endl;
      exit(1);
    }
    for (int i = 0; i < layerVect.size(); i++) {
      if (layerVect[i].size() != 1) {
        cerr << "Event::initNumChildren error in file " << myName << endl;
        cerr << "    BY_LAYER requires each layer to have only 1 file in it!!" << endl;
        exit(1);
      }
    }

    // re-initialize the type and layer vectors
    typeVect.clear();
    for (int i = 0; i < layerVect.size(); i++) {
      int numToAdd = numPerLayerIt->getInt(this);
      numPerLayerIt++;
      string currLayerStr = layerVect[i][0];
      layerVect[i].clear();
      for (int count = 0; count < numToAdd; count++) {
        layerVect[i].push_back(currLayerStr);
        typeVect.push_back(currLayerStr);
      }
    }

    numChildren = typeVect.size();

  } else if (method == "DENSITY") {
    FileValue densityFV = *iter;
    iter++;
    FileValue areaFV = *iter;
    iter++;
    FileValue underOneFV = *iter;

    // do for each layer
    for (int i = 0; i < layerVect.size(); i++) {
      float density = densityFV.getFloat(this);
      int area = areaFV.getInt(this);
      int underOne = underOneFV.getInt(this);

      float soundsPsec = pow(2, density * area - underOne);
      layerDensity.push_back( soundsPsec );
      layerNumChildren.push_back( (int)floor(layerDensity[i] * myDuration + 0.5) );
    }

    // initialize the remaining-children-to-create to be all of them
    layerRemainingChildren = layerNumChildren;
    // count the total number of children
    numChildren = 0;
    for (int i = 0; i < layerNumChildren.size(); i++) {
      numChildren += layerNumChildren[i];
    }
  } else {
    cerr << "Event::NumChildren - method=" << method << " doesn't exist" << endl;
    exit(1);
  }
}

//----------------------------------------------------------------------------//

void Event::initChildDef(FileValue* childrenDef) {
  childEventDef = new FileValue(*childrenDef);
}

//----------------------------------------------------------------------------//

void Event::buildChildEvents() {
  vector<Event*> temporaryChildEvents;

  print();
  cout << "Event::buildChildEvents - my name is: " << myName << endl;

  list<FileValue>* child_def_list = childEventDef->getListPtr(this);
  list<FileValue>::iterator iter = child_def_list->begin();

  string method = iter->getString(this);
  iter++;
  
  // Main loop for creating SubEvents
  bool buildResult;
  int restarts_remain = 6;

  for (currChildNum = 0; currChildNum < numChildren; currChildNum++) {

    if (method == "CONTINUUM") {
      buildResult = buildContinuum(iter);
    } else if (method == "SWEEP") {
      buildResult = buildSweep(iter);
    } else if (method == "DISCRETE") {
      buildResult = buildDiscrete(iter);
    }

    // check to see if the building succeeded
    if (buildResult) {
      childStartTime += myStartTime;

      if (childType >= typeVect.size()) {
        cerr << "Error: childType of '" << childType << "' specified" << endl;
        cerr << "    but " << myName << " only has " << typeVect.size() 
	     << " children!" << endl;
        exit(1);
      }
      string childName = typeVect[childType];
      if (childName == "") {
        cout << "Warning: childname not found in typevector!" << endl;
        for (int i = 0; i < typeVect.size(); i++) {
          cout << typeVect[i] << ", ";
        }
      }

      childPrintLevel = printLevel + 1;

      temporaryChildEvents.push_back(new Event(childStartTime, childDuration, 
        childType, childName, childPrintLevel));		

      // Now split between Event and Bottom (bottom will go to it's own method)
      //constructChild( childStartTime, childDuration, childType, childName, childPrintLevel );
    } else {
      // build failed!  Restart this loop up to 'restarts_remain' times
      if (restarts_remain > 0) {
        restarts_remain--;

        cout << "Event::Build -- failed to build in file " << myName << endl;
        cout << "  childnum=" << currChildNum << " out of " << numChildren 
             << " children" << endl;
        cout << "  method=" << method << "; " << restarts_remain << 
		" tries remaining." << endl;
        currChildNum = 0;
        for (int i = 0; i < childEvents.size(); i++) {
          delete childEvents[i];
          delete temporaryChildEvents[i];
        }
        childEvents.clear();
        temporaryChildEvents.clear();
      } else {
        string answer;
        cerr << "Event::Build -- failed to build a child too many times." << endl;
        cerr << "    Do you want to create fewer children? (y/n)" << endl;
        cin >> answer;
        if (answer == "y") {
          currChildNum = 0;
          for (int i = 0; i < childEvents.size(); i++) {
            delete childEvents[i];
            delete temporaryChildEvents[i];
          }
          childEvents.clear();
          temporaryChildEvents.clear();
          numChildren--;
          restarts_remain = 10;
          cerr << "   ...changed numChildren to " << numChildren << endl;
        } else {
          exit(1);
        }
      }
    }
  }

  int temp_ChildEventsSize = temporaryChildEvents.size();
  for (int i = 0; i < temp_ChildEventsSize; i++) {
    currChildNum = i;
    Event *t = temporaryChildEvents[i];
    constructChild(t->myStartTime, t->myDuration, t->myType, t->myName, 
     t->printLevel);
    delete t;
  }
  temporaryChildEvents.clear();

  // all the stime/dur/types have been set for the children, so call the 
  // children (in-order) to build their own child events.
  for (int i = 0; i < childEvents.size(); i++) {
    childEvents[i]->buildChildEvents();
  }
  
  // close the XML tag for this event
  indentPrint(printLevel);                                                    //
  *outputFile << "</event>" << endl;                                          //
}

//----------------------------------------------------------------------------//

void Event::constructChild(float stime, float dur, int type, string name, int level) {
  EventFactory* childFactory = factory_lib[name];
  if (childFactory == NULL) {
    // Parse the file
    childFactory = new EventFactory(name);
  }

  // construct the child to this non-bottom event
  childEvents.push_back( childFactory->Build(stime, dur, type, level) );
}

//----------------------------------------------------------------------------//

bool Event::buildContinuum(list<FileValue>::iterator iter) {
  if (currChildNum == 0) {
    checkPoint = 0;
  }

  // Get the start time
  childStartTime = iter->getFloat(this);
  iter++;
  // how to process start time: UNITS or SECONDS or PERCENTAGE
  string startType = iter->getString(this);
  iter++;

  if (startType == "UNITS") {
    childStartTime = childStartTime / unitsPerSecond;
  } else if (startType == "SECONDS") {
    // nothing to do
  } else if (startType == "PERCENTAGE") {
    childStartTime = childStartTime * myDuration;  //percentage of parent dur
  } else {
    cerr << "Event::buildContinuum -- invalid or missing start type!" << endl;
    cerr << "      startType = " << startType << endl;
    cerr << "      in file " << myName << endl;
    exit(1);
  }
  checkPoint = (double)childStartTime / myDuration;

  //Get the type
  childType = iter->getInt(this);
  iter++;
  // get the duration
  childDuration = iter->getFloat(this);
  iter++;
  // how to process duration: UNITS or SECONDS or PERCENTAGE
  string durType = iter->getString(this);
  iter++;

  if (durType == "UNITS") {
    childDuration = (float)childDuration / unitsPerSecond;
  } else if (durType == "SECONDS") {
    //nothing to do
  } else if (durType == "PERCENTAGE") {
    childDuration = childDuration * myDuration;
  } else {
    cerr << "Event::buildContinuum -- invalid or missing duration type!" << endl;
    cerr << "      durtype = " << durType << endl;
    cerr << "      in file " << myName << endl;
    exit(1);
  }

  float maxDurSec = maxChildDur / unitsPerSecond;
  if (childDuration > maxDurSec) {
    childDuration = maxDurSec;
  }

  return true; //success!
}

//----------------------------------------------------------------------------//

bool Event::buildSweep(list<FileValue>::iterator iter) {
  // find start time and dur of last child
  if (currChildNum == 0) {
    lastTime = 0;
  }

  // Set checkpoint to the endpoint of the last event
  checkPoint = lastTime / myDuration;

  if (checkPoint > 1) {
    cerr << "Event::Sweep -- Error1: childStartTime outside range of " 
        << "parent duration." << endl;
    cerr << "      childStime=" << childStartTime << ", parentDur=" 
        << myDuration << endl;
    cerr << "      in file: " << myName << ", childNum=" 
        << currChildNum << endl;
    cerr << "currChildNum=" << currChildNum << " lastTime=" << lastTime <<
	" checkPoint=" << checkPoint << endl;
    exit(1);
  }

  // Get the start time
  childStartTime = iter->getFloat(this);
  iter++;
  // how to process start time: UNITS or PERCENTAGE
  string startType = iter->getString(this);
  iter++;

  if (startType == "UNITS") {
    childStartTime = childStartTime / unitsPerSecond;
  } else if (startType == "SECONDS") {
    // nothing to do
  } else if (startType == "PERCENTAGE") {
    childStartTime = childStartTime * myDuration;  //percentage of parent dur
  }

  if (childStartTime < lastTime) {
    childStartTime = lastTime;
  }

  if (currChildNum == 0) {
    childStartTime = 0;
  }

  // set checkpoint to the start of this child event
  checkPoint = childStartTime / myDuration;

  if (checkPoint > 1) {
    cerr << "Event::Sweep -- Error2: childStartTime outside range of " 
        << "parent duration." << endl;
    cerr << "      childStime=" << childStartTime << ", parentDur=" 
        << myDuration << endl;
    cerr << "      in file: " << myName << ", childNum=" 
        << currChildNum << endl;
    exit(1);
  }

  //Get the type
  childType = iter->getInt(this);
  iter++;

  // get the duration
  childDuration = iter->getFloat(this);
  iter++;
  // how to process duration: UNITS or PERCENTAGE
  string durType = iter->getString(this);
  iter++;

  if (durType == "UNITS") {
    childDuration = childDuration / unitsPerSecond;
  } else if (durType == "SECONDS") {
    //nothing to do
  } else if (durType == "PERCENTAGE") {
    childDuration = childDuration * myDuration;
  }

  float maxDurSec = maxChildDur / unitsPerSecond;
  if (childDuration > maxDurSec) {
    childDuration = maxDurSec;
  }

  lastTime = childStartTime + childDuration;

  return true; // success!
}

//----------------------------------------------------------------------------//

bool Event::buildDiscrete(list<FileValue>::iterator iter) {
  int sever;

  if (currChildNum == 0) {
    checkPoint = 0;
  }
  MatPoint childPt;

  if (discreteMat == NULL) {
    // first time called --- create the matrix!
    Sieve* attackSiv;
    Sieve* durSiv;
    vector<double> typeProbs;
    vector<Envelope*> attackEnvs;
    vector<Envelope*> durEnvs;
    vector<int> numTypesInLayers;

    attackSiv = iter->getSieve(this);
    iter++;

    durSiv = iter->getSieve(this);
    iter++;

    list<FileValue>* tmpList = iter->getListPtr(this);
    iter++;
    list<FileValue>::iterator tmpIter = tmpList->begin();
    while (tmpIter != tmpList->end()) {
      typeProbs.push_back(tmpIter->getFloat(this));
      tmpIter++;
    }

    tmpList = iter->getListPtr(this);
    iter++;
    tmpIter = tmpList->begin();
    while( tmpIter != tmpList->end()) {
      attackEnvs.push_back(tmpIter->getEnvelope(this));
      tmpIter++;
    }

    tmpList = iter->getListPtr(this);
    iter++;
    
    tmpIter = tmpList->begin();
    while( tmpIter != tmpList->end()) {
      durEnvs.push_back(tmpIter->getEnvelope(this));
      tmpIter++;
    }

    for (int i = 0; i < layerVect.size(); i++) {
      numTypesInLayers.push_back( layerVect[i].size() );
    }

    //FINALLY, create the matrix
    discreteMat = new Matrix(typeVect.size(), attackSiv->GetNumItems(), 
                           durSiv->GetNumItems(), 0.0, numTypesInLayers);

    discreteMat->setAttacks(attackSiv, attackEnvs);

    discreteMat->setDurations(durSiv, durEnvs);

    discreteMat->setTypeProbs(typeProbs);
  }
//discreteMat->printMatrix(1);
//cin >> sever;

  // get something out of the matrix
  childPt  = discreteMat->chooseM(numChildren - currChildNum - 1);

  // check to see if we ran out of space in matrix --- (type=-1 is a flag)
  if (childPt.type == -1) {
    delete discreteMat; // delete the matrix so it gets recreated on a retry
    discreteMat = NULL;
    return false; // failure!
  }

  //cout << "       ---- AFTER MATRIX CHOOSEM" << endl;
  //discreteMat->printMatrix();

  int stimeUnits = childPt.stime;
  int durUnits = childPt.dur;
  childType = childPt.type;

  childStartTime = (float)stimeUnits / (float)unitsPerSecond;
  childDuration = (float)durUnits / (float)unitsPerSecond;
/*
  cout << "Event::buildDiscrete - durUnits=" << durUnits << " unitsPerSecond="
	<< unitsPerSecond << " childDuration=" << childDuration << endl;
  cin >> sever;
*/
  float maxDurSec = maxChildDur / unitsPerSecond;
  if (childDuration > maxDurSec) {
    childDuration = maxDurSec;
  }

  // using units
  checkPoint = (double)childStartTime / myDuration;

  return true; // success!
}

//----------------------------------------------------------------------------//

void Event::print() {
  indentPrint(printLevel);                                                    //
  *outputFile << "<event>" << endl;                                           //

  indentPrint(printLevel + 1);                                                //
  *outputFile << "<name>" << myName << "</name>" << endl;                     //
  
  indentPrint(printLevel + 1);                                                //
  *outputFile << "<type>" << myType << "</type>" << endl;                     //
  
  indentPrint(printLevel + 1);                                                //
  *outputFile << "<start-time-sec>" << myStartTime <<                         //
    "</start-time-sec>" << endl;                                              //
    
  indentPrint(printLevel + 1);                                                //
  *outputFile << "<start-time-units>" << myStartTime * unitsPerSecond <<      //
    "</start-time-units>" << endl;                                            //
    
  indentPrint(printLevel + 1);                                                //
  *outputFile << "<duration-sec>" << myDuration << "</duration-sec>" << endl; //
    
  indentPrint(printLevel + 1);                                                //
  *outputFile << "<duration-units>" << myDuration * unitsPerSecond <<         //
    "</duration-units>" << endl;                                              //
}

void Event::indentPrint(int lvl) {
  // indent two spaces in the XML per level (plus two indentations to start)
  for (int i = 0; i < lvl + 2; i++) {
    *outputFile << "  ";                                                      //
  }
}

//----------------------------------------------------------------------------//

list<Note> Event::getNotes() {
  list<Note> result;
  for (int i = 0; i < childEvents.size(); i++) {
    list<Note> append = childEvents[i]->getNotes();
    list<Note>::iterator iter = append.begin();
    while (iter != append.end()) {
      result.push_back(*iter);
      iter++;
    }
    //result.insert(result.begin(), append.begin(), append.end());
  }
  return result;
}

//----------------------------------------------------------------------------//

int Event::getCurrentLayer() {
  int countInLayer = 0;
  
  for (int i = 0; i < layerVect.size(); i++) {
    countInLayer += layerVect[i].size();
    if (childType < countInLayer) {
/*cout << "                                                    type=" 
	<< childType << ", layer=" << i << endl;			*/
      return i;
    }
  }

  cerr << "Event::getCurrentLayer error in file " << myName << endl;
  cerr << "   Unable to get layer number" << endl;
  exit(1);
}



