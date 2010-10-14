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

#include "Event.h"
#include "EventFactory.h"
#include "Output.h"
#include "Sieve.h"

extern map<string, EventFactory*> factory_lib;
extern ofstream * outputFile;
extern ofstream * outFile;

//----------------------------------------------------------------------------//

Event::Event(float stime, float dur, int type, string name) {
  myStartTime = stime;
  myDuration = dur;
  myType = type;
  myName = name;

  childEventDef = NULL;
  discreteMat = NULL;
}

//----------------------------------------------------------------------------//

Event::Event(const Event& origEvent) {
  myStartTime = origEvent.myStartTime;
  myDuration = origEvent.myDuration;
  myType = origEvent.myType;
  myName = origEvent.myName;

  maxChildDur = origEvent.maxChildDur;
  tempo = origEvent.tempo;

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

  maxChildDur = rhs.maxChildDur;
  tempo = rhs.tempo;

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

//----------------------------------------------------------------------------//

void Event::initDiscreteInfo(
      std::string newTempo, std::string newTimeSignature, int newEDUPerBeat,
      float newMaxChildDur ) {
      
  tempo.setTempo(newTempo);
  tempo.setTimeSignature(newTimeSignature);
  cout << "EDUPERBEAT: " << newEDUPerBeat << endl;
  tempo.setEDUPerTimeSignatureBeat((std::string)Ratio(newEDUPerBeat, 1));
  maxChildDur = newMaxChildDur;
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

  Output::beginSubLevel(myName);
  outputProperties();
  
  string isExact;
  {
    float actualEDUDuration =
      (Ratio(getAvailableEDU(), 1) * tempo.getEDUDurationInSeconds()).To<float>();
    
    if(actualEDUDuration == myDuration)
      isExact = "Yes";
    else if(fabs(actualEDUDuration / myDuration - 1.0f) < 0.01f)
      isExact = "Almost";
    else
      isExact = "No";
  }
  Output::addProperty("Available EDU is Exact", isExact);
  Output::addProperty("EDU Duration", tempo.getEDUDurationInSeconds(), "sec.");
  
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
    
    if (buildResult)  {
      childStartTime += myStartTime;
      if(myExactStartTime.isDeterminate())
        exactChildStartTime += myExactStartTime;
      else {
        if(tempo.getStartTime() == 0) {
          tempo.setStartTime(myStartTime);
        }
        else if(tempo.getStartTime() != myStartTime)
        {
          cout << "Warning: children of parent are using " << 
            "multiple tempo start times. This should not be possible!" << endl;
        }
      }

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

      temporaryChildEvents.push_back(new Event(childStartTime, childDuration, 
        childType, childName));		

      // Now split between Event and Bottom (bottom will go to it's own method)
      //constructChild( childStartTime, childDuration, childType, childName);
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
    constructChild(t->myStartTime, t->myDuration, t->myType, t->myName);
    delete t;
  }
  temporaryChildEvents.clear();

  // all the stime/dur/types have been set for the children, so call the 
  // children (in-order) to build their own child events.
  for (int i = 0; i < childEvents.size(); i++) {
    childEvents[i]->buildChildEvents();
  }
  
  Output::endSubLevel();
}

//----------------------------------------------------------------------------//

void Event::constructChild(float stime, float dur, int type, string name) {
  EventFactory* childFactory = factory_lib[name];
  if (childFactory == NULL) {
    // Parse the file
    childFactory = new EventFactory(name);
  }

  // construct the child to this non-bottom event
  childEvents.push_back( childFactory->Build(stime, dur, type) );
}

//----------------------------------------------------------------------------//

bool Event::buildContinuum(list<FileValue>::iterator iter) {
  if (currChildNum == 0) {
    checkPoint = 0;
  }

  // get the start time
  float rawChildStartTime = iter++->getFloat(this);
  
  // how to process start time: EDU, SECONDS or PERCENTAGE
  string startType = iter++->getString(this);

  if (startType == "UNITS") //Deprecated 'UNITS' is now 'EDU'
    cout << "Warning: 'UNITS' type is now 'EDU'" << endl;

  if (startType == "EDU" || startType == "UNITS") {
    childStartTime = rawChildStartTime *
      tempo.getEDUDurationInSeconds().To<float>();
    exactChildStartTime = Ratio((int)rawChildStartTime, 1);
  } else if (startType == "SECONDS") {
    childStartTime = rawChildStartTime; // no conversion needed
    exactChildStartTime = Ratio(0, 0);  // floating point is not exact: NaN
  } else if (startType == "PERCENTAGE") {
    childStartTime = rawChildStartTime * myDuration; // convert to seconds
    exactChildStartTime = Ratio(0, 0);  // floating point is not exact: NaN
  } else {
    cerr << "Event::buildContinuum -- invalid or missing start type!" << endl;
    cerr << "      startType = " << startType << endl;
    cerr << "      in file " << myName << endl;
    exit(1);
  }
  checkPoint = (double)childStartTime / myDuration;

  // get the type
  childType = iter++->getInt(this);
  
  // get the duration
  float rawChildDuration = iter++->getFloat(this);
  
  // pre-quantize the duration in case "EDU" is used
  int rawChildDurationInt = (int)rawChildDuration;
  int maxChildDurInt = (int)maxChildDur;
  if(rawChildDurationInt > maxChildDurInt)
      rawChildDurationInt = maxChildDurInt;
  
  // how to process duration: EDU, SECONDS or PERCENTAGE
  string durType = iter++->getString(this);

  if (durType == "UNITS") //Deprecated 'UNITS' is now 'EDU'
    cout << "Warning: 'UNITS' type is now 'EDU'" << endl;

  if (durType == "EDU" || durType == "UNITS") {
    exactChildDuration = Ratio(rawChildDurationInt, 1);
    cout << "Test:" << endl;
    cout << tempo.getEDUDurationInSeconds() << endl;
    childDuration = // convert to seconds
      (float)rawChildDurationInt * tempo.getEDUDurationInSeconds().To<float>();
  } else if (durType == "SECONDS") {
    childDuration = rawChildDuration;
    if(childDuration > maxChildDur)
      childDuration = maxChildDur; // enforce limit
    exactChildDuration = Ratio(0, 0); // floating point is not exact: NaN
  } else if (durType == "PERCENTAGE") {
    childDuration = rawChildDuration * myDuration; // convert to seconds
    if(childDuration > maxChildDur)
      childDuration = maxChildDur; // enforce limit
    exactChildDuration = Ratio(0, 0); // floating point is not exact: NaN
  } else {
    cerr << "Event::buildContinuum -- invalid or missing duration type!" << endl;
    cerr << "      durtype = " << durType << endl;
    cerr << "      in file " << myName << endl;
    exit(1);
  }
  
  //DEBUG//
  cout << "***CONTINUUM***" << endl;
  cout << "startType: " << startType << endl;
  cout << "rawChildStartTime: " << rawChildStartTime << endl;
  cout << "childStartTime: " << childStartTime << endl;
  cout << "exactChildStartTime: " << exactChildStartTime << endl;
  cout << "checkPoint: " << checkPoint << endl;
  cout << "myDuration: " << myDuration << endl;
  cout << "durType: " << durType << endl;
  cout << "rawChildDuration: " << rawChildDuration << endl;
  cout << "rawChildDurationInt: " << rawChildDurationInt << endl;
  cout << "maxChildDur: " << maxChildDur << endl;
  cout << "maxChildDurInt: " << maxChildDurInt << endl;
  cout << "childDuration: " << childDuration << endl;
  cout << "exactChildDuration: " << exactChildDuration << endl;
  //-----//
  return true; //success!
}

//----------------------------------------------------------------------------//

bool Event::buildSweep(list<FileValue>::iterator iter) {
  // find start time and dur of last child
  if (currChildNum == 0) {
    lastTime = 0;
    exactLastTime = 0;
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

  // get the start time
  float rawChildStartTime = iter++->getFloat(this);
  
  // how to process start time: EDU, SECONDS or PERCENTAGE
  string startType = iter++->getString(this);

  if (startType == "UNITS") //Deprecated 'UNITS' is now 'EDU'
    cout << "Warning: 'UNITS' type is now 'EDU'" << endl;

  if (startType == "EDU" || startType == "UNITS") {
    childStartTime = rawChildStartTime * 
      tempo.getEDUDurationInSeconds().To<float>();
    exactChildStartTime = Ratio((int)rawChildStartTime, 1);
  } else if (startType == "SECONDS") {
    childStartTime = rawChildStartTime; // no conversion needed
    exactChildDuration = Ratio(0, 0); // floating point is not exact: NaN
  } else if (startType == "PERCENTAGE") {
    childStartTime = rawChildStartTime * myDuration; // convert to seconds
    exactChildDuration = Ratio(0, 0); // floating point is not exact: NaN
  }

  if (childStartTime < lastTime) {
    childStartTime = lastTime;
    exactChildStartTime = exactLastTime;
  }

  if (currChildNum == 0) {
    childStartTime = 0;
    exactChildStartTime = 0;
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

  // get the type
  childType = iter++->getInt(this);
  
  // get the duration
  float rawChildDuration = iter++->getFloat(this);
  
  // pre-quantize the duration in case "EDU" is used
  int rawChildDurationInt = (int)rawChildDuration;
  int maxChildDurInt = (int)maxChildDur;
  if(rawChildDurationInt > maxChildDurInt)
      rawChildDurationInt = maxChildDurInt;
  
  // how to process duration: EDU, SECONDS or PERCENTAGE
  string durType = iter++->getString(this);

  if (durType == "UNITS") //Deprecated 'UNITS' is now 'EDU'
    cout << "Warning: 'UNITS' type is now 'EDU'" << endl;

  if (durType == "EDU" || durType == "UNITS") {
    exactChildDuration = Ratio(rawChildDurationInt, 1);
    childDuration = // convert to seconds
      (float)rawChildDurationInt * tempo.getEDUDurationInSeconds().To<float>();
  } else if (durType == "SECONDS") {
    childDuration = rawChildDuration;
    if(childDuration > maxChildDur)
      childDuration = maxChildDur; // enforce limit
    exactChildDuration = Ratio(0, 0); // floating point is not exact: NaN
  } else if (durType == "PERCENTAGE") {
    childDuration = rawChildDuration * myDuration; // convert to seconds
    if(childDuration > maxChildDur)
      childDuration = maxChildDur; // enforce limit
    exactChildDuration = Ratio(0, 0); // floating point is not exact: NaN
  }
  lastTime = childStartTime + childDuration;
  exactLastTime = exactChildStartTime + exactChildDuration;

  //DEBUG//
  cout << "***SWEEP***" << endl;
  cout << "startType: " << startType << endl;
  cout << "rawChildStartTime: " << rawChildStartTime << endl;
  cout << "childStartTime: " << childStartTime << endl;
  cout << "exactChildStartTime: " << exactChildStartTime << endl;
  cout << "checkPoint: " << checkPoint << endl;
  cout << "myDuration: " << myDuration << endl;
  cout << "durType: " << durType << endl;
  cout << "rawChildDuration: " << rawChildDuration << endl;
  cout << "rawChildDurationInt: " << rawChildDurationInt << endl;
  cout << "maxChildDur: " << maxChildDur << endl;
  cout << "maxChildDurInt: " << maxChildDurInt << endl;
  cout << "childDuration: " << childDuration << endl;
  cout << "exactChildDuration: " << exactChildDuration << endl;
  cout << "lastTime: " << lastTime << endl;
  cout << "exactLastTime: " << exactLastTime << endl;
  //-----//

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

  // get something out of the matrix
  childPt  = discreteMat->chooseM(numChildren - currChildNum - 1);

  // check to see if we ran out of space in matrix --- (type=-1 is a flag)
  if (childPt.type == -1) {
    delete discreteMat; // delete the matrix so it gets recreated on a retry
    discreteMat = NULL;
    return false; // failure!
  }

  int stimeEDU = childPt.stime;
  int durEDU = childPt.dur;
  childType = childPt.type;

  if(durEDU > (int)maxChildDur)
    durEDU = maxChildDur;
  exactChildStartTime = stimeEDU;
  exactChildDuration = durEDU;
  
  childStartTime = (float)stimeEDU * 
    tempo.getEDUDurationInSeconds().To<float>();
  childDuration = (float)durEDU * 
    tempo.getEDUDurationInSeconds().To<float>();

  // using edu
  checkPoint = (double)childStartTime / myDuration;
  
  //DEBUG//
  cout << "***DISCRETE***" << endl;
  cout << "childStartTime: " << childStartTime << endl;
  cout << "exactChildStartTime: " << exactChildStartTime << endl;
  cout << "checkPoint: " << checkPoint << endl;
  cout << "myDuration: " << myDuration << endl;
  cout << "maxChildDur: " << maxChildDur << endl;
  cout << "childDuration: " << childDuration << endl;
  cout << "exactChildDuration: " << exactChildDuration << endl;
  //-----//

  return true; // success!
}

//----------------------------------------------------------------------------//

void Event::outputProperties() {
  Output::addProperty("Type", myType);
  Output::addProperty("Start Time", myStartTime, "sec.");
  Output::addProperty("Duration", myDuration, "sec.");
  Output::addProperty("Tempo",
    tempo.getTempoBeatsPerMinute().toPrettyString(), "BPM");
  
  Output::addProperty("Time Signature", tempo.getTimeSignature());
  Output::addProperty("Divisions",
    tempo.getEDUPerTimeSignatureBeat().toPrettyString(),
    "EDU/beat");
  Output::addProperty("Available EDU", getAvailableEDU());
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

//----------------------------------------------------------------------------//

int Event::getAvailableEDU()
{
  if(myExactDuration.isDeterminate() && myExactDuration != Ratio(0, 1))
    return myExactDuration.To<int>();
  else {
    //Don't have exact duration
    int myDurationInt = (int)myDuration;
    if(myDuration == (float)myDurationInt)
    {
      //But duration is an integer (might be able to have exact EDUs)
      Ratio EDUs = tempo.getEDUPerSecond() * Ratio(myDurationInt, 1);
      if(EDUs.Den() == 1)
      {
        //Must quantize...
        float approximateEDUs = EDUs.To<float>();
        int quantizedEDUs = (int)approximateEDUs;
        cout << "Warning: quantizing AVAILABLE_EDU from ";
        cout << approximateEDUs << " to " << quantizedEDUs << endl;
        return quantizedEDUs;
      }
      else if(EDUs.Den() == 0) //This shouldn't happen.
        return 0;
      else //We have exact EDUs
        return EDUs.To<int>();
    }
    else
    {
      Ratio EDUs = tempo.getEDUPerSecond();
      if(EDUs.Den() != 0)
      {
        //Must quantize...
        float approximateEDUs = EDUs.To<float>() * myDuration;
        int quantizedEDUs = (int)approximateEDUs;
        cout << "Warning: quantizing AVAILABLE_EDU from ";
        cout << approximateEDUs << " to " << quantizedEDUs << endl;
        return quantizedEDUs;
      }
      else
        return 0; //This shouldn't happen.
    }
  }
};

