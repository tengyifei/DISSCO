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
//Should go away

Event::Event(float stime, float dur, int type, string name) {
  myStartTime = stime;
  myDuration = dur;
  myType = type;
  myName = name;

  childEventDef = NULL;
  discreteMat = NULL;
  childType = 0;
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

  childType = 0;
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
//Checked

Event::~Event() {
  delete childEventDef;
  delete discreteMat;
  for (int i = 0; i < childEvents.size(); i++)
    delete childEvents[i];
}

//----------------------------------------------------------------------------//
//Checked

void Event::initDiscreteInfo(std::string newTempo, std::string newTimeSignature,
  int newEDUPerBeat, float newMaxChildDur) {
  tempo.setTempo(newTempo);
  tempo.setTimeSignature(newTimeSignature);
  tempo.setEDUPerTimeSignatureBeat((std::string)Ratio(newEDUPerBeat, 1));
  maxChildDur = newMaxChildDur;
}

//----------------------------------------------------------------------------//
//Checked

void Event::initChildNames( FileValue* childNames ) {
  //Get iterator for the childNames list
  list<FileValue>* layersList = childNames->getListPtr(this);
  list<FileValue>::iterator iter = layersList->begin();

  //Initialize layerVect and typeVect
  while (iter != layersList->end()) {
    list<FileValue>* currLayer = iter->getListPtr(this);
    list<FileValue>::iterator currLayerIter = currLayer->begin();

    vector<string> currLayerVect;
    while (currLayerIter != currLayer->end()) {
      currLayerVect.push_back( currLayerIter->getString(this) );
      currLayerIter++;

      typeVect.push_back( currLayerVect.back() );
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
//Checked

void Event::initChildDef(FileValue* childrenDef) {
  childEventDef = new FileValue(*childrenDef);
}

//----------------------------------------------------------------------------//
//Checked

string Event::getEDUDurationExactness(void) {
  float actualEDUDuration =
    (Ratio(getAvailableEDU(), 1) * tempo.getEDUDurationInSeconds()).To<float>();
  
  if(actualEDUDuration == myDuration)
    return "Yes";
  else if(fabs(actualEDUDuration / myDuration - 1.0f) < 0.01f)
    return "Almost";
  else
    return "No";
}

//----------------------------------------------------------------------------//
//Checked

void Event::buildChildEvents() {
  
  //Begin this sub-level in the output and write out its properties.
  Output::beginSubLevel(myName);
  outputProperties();

  //Build the event's children.
  cout << "Building event: " << myName << endl;
  
  //Create the event definition iterator.
  list<FileValue>::iterator iter = childEventDef->getListPtr(this)->begin();
  string method = iter++->getString(this);
  
  //Set the number of possible restarts (for buildDiscrete)
  restartsRemaining = restartsNormallyAllowed;

  //Make sure that the temporary child events array is clear.
  if(temporaryChildEvents.size() > 0) {
    cerr << "Warning: temporaryChildEvents should not contain data." << endl;
    cerr << "There may be a bug in the code. Please report." << endl;
    exit(1);
  }
  
  //Make sure the childType indexes correctly.
  if (childType >= typeVect.size() || typeVect[childType] == "") {
    cerr << "There is a mismatch between childType and typeVect." << endl;
    exit(1);
  }

  //Create the child events.
  for (currChildNum = 0; currChildNum < numChildren; currChildNum++) {
    if (method == "CONTINUUM")
      checkEvent(buildContinuum(iter));
    else if (method == "SWEEP")
      checkEvent(buildSweep(iter));
    else if (method == "DISCRETE")
      checkEvent(buildDiscrete(iter));
    else {
      cerr << "Unknown build method: " << method << endl << "Aborting." << endl;
      exit(1);
    }
  }

  //Using the temporary events that were created, construct the actual children.
  for (int i = 0; i < temporaryChildEvents.size(); i++) {
    //Increment the static current child number.
    currChildNum = i;
    
    //Get current event.
    Event *e = temporaryChildEvents[currChildNum];
    
    //Construct the child (overloaded in Bottom)
    constructChild(e->myStartTime, e->myDuration, e->myType, e->myName);

    //Delete the temporary child event.
    delete e;
  }
  //Clear the temporary event list.
  temporaryChildEvents.clear();

  //For each child that was created, build its children.
  for (int i = 0; i < childEvents.size(); i++)
    childEvents[i]->buildChildEvents();
  
  //End this output sublevel.
  Output::endSubLevel();
}

//----------------------------------------------------------------------------//
//Checked

void Event::tryToRestart(void) {
  //Decrement restarts, or if there are none left, ask for fewer children.
  if (restartsRemaining > 0) {
    restartsRemaining--;
    cout << "Failed to build child " << currChildNum << " of " << numChildren
      << " in file " << myName << ". There are " << restartsRemaining 
      << " tries remaining." << endl;
  } else {
    cerr << "No tries remain. Try building with one less child? (y/n)" << endl;
    string answer; cin >> answer; if (answer != "y") exit(1);
    numChildren--; cerr << "Changed numChildren to " << numChildren << endl;
    restartsRemaining = restartsAllowedWithFewerChildren;
  }
  
  //Start over by clearing the event arrays and resetting the for-loop index.
  currChildNum = 0;    
  for (int i = 0; i < childEvents.size(); i++)
    delete temporaryChildEvents[i];
  temporaryChildEvents.clear();
}

//----------------------------------------------------------------------------//
//Checked

void Event::checkEvent(bool buildResult) {

  //If the build failed, restart if necessary.
  if (!buildResult) {
    tryToRestart();
    return;
  }
  
  //Handle start times according to exactness rules.
  childStartTime += myStartTime;
  if(myExactStartTime.isDeterminate())
    exactChildStartTime += myExactStartTime;
  else if(tempo.getStartTime() == 0)
    tempo.setStartTime(myStartTime);
  else if(tempo.getStartTime() != myStartTime)
    cout << "Warning: children of parent are using multiple tempo start " << 
      " times. There may be a bug. Please report." << endl;

  //Make sure the childType indexes correctly.
  if (childType >= typeVect.size() || typeVect[childType] == "") {
    cerr << "There is a mismatch between childType and typeVect." << endl;
    exit(1);
  }

  //Add the event to the temporary event list.
  temporaryChildEvents.push_back(new Event(childStartTime, childDuration, 
    childType, typeVect[childType]));
}

//----------------------------------------------------------------------------//
//Replace parameters with single event.

void Event::constructChild(float stime, float dur, int type, string name) {
  //Create the event factory.
  EventFactory* childFactory = factory_lib[name];
  if (!childFactory)
    childFactory = new EventFactory(name);

  //Construct the child for the event.
  childEvents.push_back(childFactory->Build(stime, dur, type));
}

//----------------------------------------------------------------------------//

string Event::unitTypeToUnits(string type) {
  if(type == "UNITS" || type == "EDU")
    return "EDU";
  else if(type == "SECONDS")
    return "sec.";
  else if(type == "PERCENTAGE")
    return "normalized";
  else
    return "";  
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
  string childName = typeVect[childType];
  
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

  //Output parameters in the different units available.
  Output::beginSubLevel("Continuum");
  Output::addProperty("Name", childName);
  Output::beginSubLevel("Parameters");
    Output::addProperty("Start", rawChildStartTime, unitTypeToUnits(startType));
    Output::addProperty("Duration", rawChildDuration, unitTypeToUnits(durType));
    if(unitTypeToUnits(startType) == "EDU")
      Output::addProperty("Max Duration", maxChildDur, "EDU");
    else
      Output::addProperty("Max Duration", maxChildDur, "sec.");
  Output::endSubLevel();
  Output::beginSubLevel("Seconds");
    Output::addProperty("Start", childStartTime, "sec.");
    Output::addProperty("Duration", childDuration, "sec.");
  Output::endSubLevel();
  Output::beginSubLevel("EDU");
    Output::addProperty("Start", exactChildStartTime, "EDU");
    Output::addProperty("Duration", exactChildDuration, "EDU");
  Output::endSubLevel();
  Output::addProperty("Checkpoint", checkPoint, "of parent");
  Output::endSubLevel();
  
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
  string childName = typeVect[childType];
  
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

  //Output parameters in the different units available.
  Output::beginSubLevel("Sweep");
  Output::addProperty("Name", childName);
  Output::beginSubLevel("Parameters");
    Output::addProperty("Start", rawChildStartTime, unitTypeToUnits(startType));
    Output::addProperty("Duration", rawChildDuration, unitTypeToUnits(durType));
    if(unitTypeToUnits(startType) == "EDU")
      Output::addProperty("Max Duration", maxChildDur, "EDU");
    else
      Output::addProperty("Max Duration", maxChildDur, "sec.");
  Output::endSubLevel();
  Output::beginSubLevel("Seconds");
    Output::addProperty("Start", childStartTime, "sec.");
    Output::addProperty("Duration", childDuration, "sec.");
    Output::addProperty("Previous", lastTime, "sec.");
  Output::endSubLevel();
  Output::beginSubLevel("EDU");
    Output::addProperty("Start", exactChildStartTime, "EDU");
    Output::addProperty("Duration", exactChildDuration, "EDU");
    Output::addProperty("Previous", exactLastTime, "EDU");
  Output::endSubLevel();
  Output::addProperty("Checkpoint", checkPoint, "of parent");
  Output::endSubLevel();

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
  string childName = typeVect[childType];

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
  
  
  //Output parameters in the different units available.
  Output::beginSubLevel("Sweep");
  Output::addProperty("Name", childName);
  Output::beginSubLevel("Parameters");
    Output::addProperty("Max Duration", maxChildDur, "EDU");
  Output::endSubLevel();
  Output::beginSubLevel("Seconds");
    Output::addProperty("Start", childStartTime, "sec.");
    Output::addProperty("Duration", childDuration, "sec.");
  Output::endSubLevel();
  Output::beginSubLevel("EDU");
    Output::addProperty("Start", exactChildStartTime, "EDU");
    Output::addProperty("Duration", exactChildDuration, "EDU");
  Output::endSubLevel();
  Output::endSubLevel();

  return true; // success!
}

//----------------------------------------------------------------------------//
//Checked

void Event::outputProperties() {
  Output::addProperty("Type", myType);
  Output::addProperty("Start Time", myStartTime, "sec.");
  Output::addProperty("Duration", myDuration, "sec.");
  Output::addProperty("Tempo",
    tempo.getTempoBeatsPerMinute().toPrettyString(), "BPM");
  Output::addProperty("Time Signature", tempo.getTimeSignature());
  Output::addProperty("Divisions",
    tempo.getEDUPerTimeSignatureBeat().toPrettyString(), "EDU/beat");
  Output::addProperty("Available EDU", getAvailableEDU());
  Output::addProperty("Available EDU is Exact", getEDUDurationExactness());
  Output::addProperty("EDU Duration", tempo.getEDUDurationInSeconds(), "sec.");
}

//----------------------------------------------------------------------------//
//Checked

list<Note> Event::getNotes() {
  list<Note> result;
  for (int i = 0; i < childEvents.size(); i++) {
    list<Note> append = childEvents[i]->getNotes();
    list<Note>::iterator iter = append.begin();
    while (iter != append.end()) {
      result.push_back(*iter);
      iter++;
    }
  }
  return result;
}

//----------------------------------------------------------------------------//
//Checked

int Event::getCurrentLayer() {
  int countInLayer = 0;
  for (int i = 0; i < layerVect.size(); i++) {
    countInLayer += layerVect[i].size();
    if (childType < countInLayer)
      return i;
  }
  cerr << "Unable to get layer number in file " << myName << endl; exit(1);
}

//----------------------------------------------------------------------------//
//Checked

int Event::getAvailableEDU()
{
  //Return exact duration if it is already apparent.
  if(myExactDuration.isDeterminate() && myExactDuration != Ratio(0, 1))
    return myExactDuration.To<int>();
 
  //The duration is not exact.
  int myDurationInt = (int)myDuration;
  Ratio EDUs;
  float durationScalar;
  if(myDuration == (float)myDurationInt)
  {
    //Since duration is an integer, it may still be possible to have exact EDUs.
    EDUs = tempo.getEDUPerSecond() * Ratio(myDurationInt, 1);
    if(EDUs.Den() == 0)//This shouldn't happen.
      return 0;
    else if(EDUs.Den() != 0 && EDUs.Den() != 1) //We have exact EDUs
      return EDUs.To<int>();
    else //Implied EDUs.Den() == 1
      durationScalar = 1;
  }
  else
  {
    EDUs = tempo.getEDUPerSecond();
    if(EDUs.Den() == 0)
      return 0; //This shouldn't happen.
    else //Implied EDUs.Den() != 0
      durationScalar = myDuration;
  }
  
  //The duration is not exact, so the available EDUs must be quantized.
  float approximateEDUs = EDUs.To<float>() * durationScalar;
  int quantizedEDUs = (int)(approximateEDUs + 0.001f);
  if(abs((float)quantizedEDUs - approximateEDUs) > 0.001f) {
    cout << "Warning: quantizing AVAILABLE_EDU from ";
    cout << approximateEDUs << " to " << quantizedEDUs << endl;
  }
  return quantizedEDUs;
};

