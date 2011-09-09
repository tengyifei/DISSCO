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
//  Event.ccp
//
//----------------------------------------------------------------------------//

#include "Event.h"
#include "EventFactory.h"
#include "Output.h"
#include "Sieve.h"

//Global -- eventually this should be moved into a class.
extern map<string, EventFactory*> factory_lib;

//----------------------------------------------------------------------------//
//Checked

Event::Event(TimeSpan ts, int type, string name) :
  name(name), type(type), ts(ts),
  childEventDef(0),
  maxChildDur(0), checkPoint(0),
  numChildren(0),
  restartsRemaining(0),
  currChildNum(0), childType(0),
  discreteMat(0) {}

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
  /*Only set the tempo to what was indicated in the file if the event does not
  already have a tempo that was derived from a parent. This is very important,
  so that tempos are not nested.*/
  Tempo fvTempo;
  fvTempo.setTempo(newTempo);
  fvTempo.setTimeSignature(newTimeSignature);
  fvTempo.setEDUPerTimeSignatureBeat((std::string)Ratio(newEDUPerBeat, 1));
  fvTempo.setStartTime(tempo.getStartTime());
  if(tempo.getStartTime() == 0)
    tempo = fvTempo;
  else if(!tempo.isTempoSameAs(fvTempo)) { //Warn if different tempi
    cout << endl << "WARNING: the tempo of this exact event differs from" << endl
      << "that of its exact parent." << endl;
    cout << "Parent: " << endl;
    cout << "  " << tempo.getTempoBeatsPerMinute() << endl;
    cout << "  " << tempo.getTempoBeat() << endl;
    cout << "  " << tempo.getTimeSignatureBeat() << endl;
    cout << "  " << tempo.getTimeSignatureBeatsPerBar() << endl;
    cout << "  " << tempo.getEDUPerTimeSignatureBeat() << endl;
    cout << "  " << tempo.getStartTime() << endl;
    cout << "File-Value Tempo: " << endl;
    cout << "  " << fvTempo.getTempoBeatsPerMinute() << endl;
    cout << "  " << fvTempo.getTempoBeat() << endl;
    cout << "  " << fvTempo.getTimeSignatureBeat() << endl;
    cout << "  " << fvTempo.getTimeSignatureBeatsPerBar() << endl;
    cout << "  " << fvTempo.getEDUPerTimeSignatureBeat() << endl;
    cout << "  " << fvTempo.getStartTime() << endl;
  }
  maxChildDur = newMaxChildDur;
}

//----------------------------------------------------------------------------//
//Checked

void Event::initChildNames(FileValue* childNames) {
  //Get iterator for the childNames list
  list<FileValue>* layersList = childNames->getListPtr(this);
  list<FileValue>::iterator iter = layersList->begin();

  //Initialize layerVect and typeVect
  while (iter != layersList->end()) {
    list<FileValue>* currLayer = iter++->getListPtr(this);
    list<FileValue>::iterator currLayerIter = currLayer->begin();

    vector<string> currLayerVect;
    while (currLayerIter != currLayer->end()) {
      currLayerVect.push_back(currLayerIter++->getString(this));

      typeVect.push_back(currLayerVect.back());
    }
    layerVect.push_back(currLayerVect);
  }
}

//----------------------------------------------------------------------------//

void Event::initNumChildren(FileValue* numChildrenFV) {
  if(!layerVect.size()) {
    cerr << "Error: ChildNames not initialized!" << endl; exit(1);
  }

  list<FileValue>* numChildrenArgs = numChildrenFV->getListPtr(this);
  list<FileValue>::iterator iter = numChildrenArgs->begin();

  string method = iter++->getString(this);

  if(method == "FIXED") {
    //Do for each layer.
    if (iter == numChildrenArgs->end())
      //No additional arguments, so create same number of children as types.
      numChildren = typeVect.size();
    else
      //Last argument is number of children to create.
      numChildren = iter->getInt(this);
      
  } else if(method == "BY_LAYER") {
    list<FileValue>* numPerLayer = iter->getListPtr(this);
    list<FileValue>::iterator numPerLayerIt = numPerLayer->begin();

    //Check for errors.
    if(numPerLayer->size() != layerVect.size()) {
      cerr << "Event::initNumChildren error in file " << name << endl;
      cerr << "    BY_LAYER list isn't the same size as the layerVect" << endl;
      exit(1);
    }
    
    for(int i = 0; i < layerVect.size(); i++) {
      if (layerVect[i].size() != 1) {
        cerr << "Event::initNumChildren error in file " << name << endl;
        cerr << "    BY_LAYER requires each layer to have only 1 file in it!!"
          << endl;
        exit(1);
      }
    }

    //Re-initialize the type and layer vectors.
    typeVect.clear();
    for(int i = 0; i < layerVect.size(); i++) {
      int numToAdd = numPerLayerIt++->getInt(this);
      string currLayerStr = layerVect[i][0];
      layerVect[i].clear();
      for(int count = 0; count < numToAdd; count++) {
        layerVect[i].push_back(currLayerStr);
        typeVect.push_back(currLayerStr);
      }
    }
    
    //Get the number of children.
    numChildren = typeVect.size();

  } else if(method == "DENSITY") {
    FileValue densityFV = *iter; iter++;
    FileValue areaFV = *iter; iter++;
    FileValue underOneFV = *iter;

    //Do for each layer.
    for(int i = 0; i < layerVect.size(); i++) {
      float density = densityFV.getFloat(this);
      int area = areaFV.getInt(this);
      int underOne = underOneFV.getInt(this);
      float soundsPsec = pow(2, density * area - underOne);
      layerDensity.push_back(soundsPsec);
      layerNumChildren.push_back(
        (int)floor(layerDensity[i] * ts.duration + 0.5));
    }

    //Initialize the remaining-children-to-create to be all of them.
    layerRemainingChildren = layerNumChildren;
    
    //Count the total number of children.
    numChildren = 0;
    for(int i = 0; i < layerNumChildren.size(); i++)
      numChildren += layerNumChildren[i];
      
  } else {
    cerr << "Event::NumChildren method " << method << " doesn't exist" << endl;
    exit(1);
  }
}

//----------------------------------------------------------------------------//
//Checked

void Event::buildChildEvents() {
  
  //Begin this sub-level in the output and write out its properties.
  Output::beginSubLevel(name);
  outputProperties();

  //Build the event's children.
  cout << "Building event: " << name << endl;
  
  //Create the event definition iterator.
  list<FileValue>::iterator iter = childEventDef->getListPtr(this)->begin();
  string method = iter++->getString(this);
  
  //Set the number of possible restarts (for buildDiscrete)
  restartsRemaining = restartsNormallyAllowed;

  //Make sure that the temporary child events array is clear.
  if(temporaryChildEvents.size() > 0) {
    cerr << "WARNING: temporaryChildEvents should not contain data." << endl;
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
    constructChild(e->ts, e->type, e->name, e->tempo);

    //Delete the temporary child event.
    delete e;
  }
  //Clear the temporary event list.
  temporaryChildEvents.clear();

  //For each child that was created, build its children.
  for(int i = 0; i < childEvents.size(); i++)
    childEvents[i]->buildChildEvents();
  
  //End this output sublevel.
  Output::addProperty("Updated Tempo Start Time", tempo.getStartTime());
  Output::endSubLevel();
}

//----------------------------------------------------------------------------//
//Checked

void Event::tryToRestart(void) {
  //Decrement restarts, or if there are none left, ask for fewer children.
  if(restartsRemaining > 0) {
    restartsRemaining--;
    cout << "Failed to build child " << currChildNum << " of " << numChildren
      << " in file " << name << ". There are " << restartsRemaining 
      << " tries remaining." << endl;
  } else {
    //Ask for permission to build with less children.
    cerr << "No tries remain. Try building with one less child? (y/n)" << endl;
    string answer; cin >> answer; if (answer != "y") exit(1);
    
    //Build with one less child.
    numChildren--; cerr << "Changed numChildren to " << numChildren << endl;
    
    //Reset the restart count.
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
  
  /*Up to now the child start time is an *offset*, that is, it has no context
  yet within the piece. The following section uses the start time/tempo rules to
  determine the correct exact and inexact start times, in some cases leading to
  a new tempo.*/
  
  /*Inexact start time is global. That is, it *always* refers to the position in
  time relative to the beginning of the piece. Thus the child start time is 
  merely the child offset added of the parent start time.*/
  tsChild.start += ts.start;
  
  /*The next part of the code deals with exactness issues since inexact and
  exact events may be nested inside each other.*/
  
  /*The following graphic attempts to show the many possibilities of nested
  exact and inexact offsets.
  
  i = inexact start offset
  e = exact start offset
  T = tempo start time
  
  0=============================================================================
  |            EVENT 1-----------------------------------------------------. . .
  |            |           EVENT 2-----------------------------------------. . .
  |            |           |          EVENT 3------------------------------. . .
  |            |           |          |         EVENT 4--------------------. . .
  |            |           |          |         |         EVENT 5----------. . .
  |            |           |          |         |         |       EVENT 6--. . .
  |            |           |          |         |         |       |
  |      +     i1    +     e2    +    e3   +    i4   +    i5  +   e6
  |            |           |          |         |         |       |
  .            .           .          .         .         .       .
  .            .           .          .         .         .       .
  .            .           .          .         .         .       .
  |    TEMPO   |   START   |  TIMES   |         |         |       |
               \\                                         \\
               T1         (T1)       (T1)                 T5     (T5)
  
  Inexact Start Times (~ means the exact value is truncated to floating point):
  Event 1 = i1
  Event 2 = i1 + ~e2
  Event 3 = i1 + ~e2 + ~e3
  Event 4 = i1 + ~e2 + ~e3 + i4
  Event 5 = i1 + ~e2 + ~e3 + i4 + i5
  Event 6 = i1 + ~e2 + ~e3 + i4 + i5 + ~e6
  
  Exact Start Times:
  Event 1 = (not applicable)
  Event 2 = T1 + e2
  Event 3 = T1 + (e2 + e3)
  Event 4 = (not applicable)
  Event 5 = (not applicable)
  Event 6 = T5 + e6
  
  Note that Event 4 ignores tempo information altogether since its child is
  inexact.
  
  Possible combinations:
  1) Parent inexact, child inexact (Events 4-5)
  Since both are inexact, nothing further is to be done. They will both only
  have global inexact time offsets.*/
  if(!ts.startEDU.isDeterminate() && !tsChild.startEDU.isDeterminate()) {
    //Nothing to do here.
  }
  
  /*2) Parent exact, child inexact (Events 3-4)
  Since the child is inexact, nothing further is to be done. The child will
  simply have a global inexact time offset. The parent will already have
  calculated its tempo start time.*/
  if(ts.startEDU.isDeterminate() && !tsChild.startEDU.isDeterminate()) {
    //Nothing to do here.
  }
  
  /*3) Parent exact, child exact (Events 2-3)
  Since the both are exact, the child inherits the tempo of the parent. Its
  exact offset is calculated by adding the exact parent start time offset.
  
  Important Note:
  If the child attempts to override the parent tempo, it will be ignored and the
  above calculation. This is to prevent implicitly nested tempos, which are
  better handled explicitly at the moment. For example it would be very
  difficult to properly render "4/4 for 3 1/4 beats, then change to 5/8 for
  3 beats as a child tempo." If this nesting were allowed, it would be very
  ambiguous as to how to return back to 4/4. Even if the 5/8 were to trigger a
  new tempo start time, in the score this would be misleading making it appear
  that the two sections were not rhythmically related, even though they
  inherently are by virtue of them both being exact.*/
  if(ts.startEDU.isDeterminate() && tsChild.startEDU.isDeterminate()) {
    tsChild.startEDU += ts.startEDU;
    /*We need to force child to have the same tempo, so that weird things do not
    happen. This is done below by explictly setting the tempo of the child. This
    will in turn be honored by initDiscreteInfo which will not override the
    given parent tempo. Note in order for this to be done, the tempo is passed
    to buildChildEvents, to constructChild, to EventFactory::Build, and finally
    to initDiscreteInfo.*/
  }
  
  /*4) Parent inexact, child exact (Events 1-2, 5-6) 
  In this case, since the parent did not have an exact offset from the
  grandparent, the exact child needs a new reference point. This triggers the
  creation of a new tempo start time *for the parent*. Since the child is
  offset an exact amount from the parent, the parent is the new tempo reference.
  
  This could easily be the source of confusion: when a parent offset is inexact, 
  and a child offset is exact, it is the parent which takes on the new tempo.
  Note that this implies that the child's siblings will refer to the same new
  tempo start time.*/
  if(!ts.startEDU.isDeterminate() && tsChild.startEDU.isDeterminate()) {
    /*The offset is the new start time, so nothing needs to be done to
    tsChild.startEDU. Instead we need to trigger a new tempo start for the
    parent. If this is the second exact child of a parent, then it will merely
    set the start time to the same thing.*/
    tempo.setStartTime(ts.start);
    //We need to force child to have the same tempo. See statement for 3).
  }
 
  //Make sure the childType indexes correctly.
  if (childType >= typeVect.size() || typeVect[childType] == "") {
    cerr << "There is a mismatch between childType and typeVect." << endl;
    exit(1);
  }

  //Create new event.
  Event* e = new Event(tsChild, childType, typeVect[childType]);
  
  //Force the child to have the parent tempo if the child is exact.
  if(tsChild.startEDU.isDeterminate())
    e->tempo = tempo;
  
  //Add the event to the temporary event list.
  temporaryChildEvents.push_back(e);
}

//----------------------------------------------------------------------------//
//Checked

void Event::constructChild(TimeSpan ts, int type, string name, Tempo tempo) {
  //Create the event factory.
  EventFactory* childFactory = factory_lib[name];
  if(!childFactory)
    childFactory = new EventFactory(name);

  //Construct the child for the event.
  childEvents.push_back(childFactory->Build(ts, type, tempo));
}

//----------------------------------------------------------------------------//
//Checked

void Event::outputProperties() {
  Output::addProperty("Type", type);
  Output::addProperty("Start Time", ts.start, "sec.");
  Output::addProperty("Duration", ts.duration, "sec.");
  Output::addProperty("Tempo Start Time", tempo.getStartTime());
  Output::addProperty("Tempo",
    tempo.getTempoBeatsPerMinute().toPrettyString(), "BPM");
  Output::addProperty("Tempo Beat", tempo.getTempoBeat(), "of whole note");
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
  for(int i = 0; i < layerVect.size(); i++) {
    countInLayer += layerVect[i].size();
    if(childType < countInLayer)
      return i;
  }
  cerr << "Unable to get layer number in file " << name << endl; exit(1);
}

//----------------------------------------------------------------------------//
//Checked

int Event::getAvailableEDU()
{
  //Return exact duration if it is already apparent.
  if(ts.startEDU.isDeterminate() && ts.startEDU != Ratio(0, 1))
    return ts.startEDU.To<int>();
 
  //The duration is not exact.
  int myDurationInt = (int)ts.duration;
  Ratio EDUs;
  float durationScalar;
  if(ts.duration == (float)myDurationInt)
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
      durationScalar = ts.duration;
  }
  
  //The duration is not exact, so the available EDUs must be quantized.
  float approximateEDUs = EDUs.To<float>() * durationScalar;
  int quantizedEDUs = (int)(approximateEDUs + 0.001f);
  if(abs((float)quantizedEDUs - approximateEDUs) > 0.001f) {
    cout << "WARNING: quantizing AVAILABLE_EDU from ";
    cout << approximateEDUs << " to " << quantizedEDUs << endl;
  }
  return quantizedEDUs;
};

//----------------------------------------------------------------------------//
//Checked

string Event::getEDUDurationExactness(void) {
  float actualEDUDuration =
    (Ratio(getAvailableEDU(), 1) * tempo.getEDUDurationInSeconds()).To<float>();
  
  if(actualEDUDuration == ts.duration)
    return "Yes";
  else if(fabs(actualEDUDuration / ts.duration - 1.0f) < 0.01f)
    return "Almost";
  else
    return "No";
}

//----------------------------------------------------------------------------//
//Checked

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
    cout << "WARNING: 'UNITS' type is now 'EDU'" << endl;

  if (startType == "EDU" || startType == "UNITS") {
    tsChild.start = rawChildStartTime *
      tempo.getEDUDurationInSeconds().To<float>();
    tsChild.startEDU = Ratio((int)rawChildStartTime, 1);
  } else if (startType == "SECONDS") {
    tsChild.start = rawChildStartTime; // no conversion needed
    tsChild.startEDU = Ratio(0, 0);  // floating point is not exact: NaN
  } else if (startType == "PERCENTAGE") {
    tsChild.start = rawChildStartTime * ts.duration; // convert to seconds
    tsChild.startEDU = Ratio(0, 0);  // floating point is not exact: NaN
  } else {
    cerr << "Event::buildContinuum -- invalid or missing start type!" << endl;
    cerr << "      startType = " << startType << endl;
    cerr << "      in file " << name << endl;
    exit(1);
  }
  checkPoint = (double)tsChild.start / ts.duration;

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
    cout << "WARNING: 'UNITS' type is now 'EDU'" << endl;

  if (durType == "EDU" || durType == "UNITS") {
    tsChild.durationEDU = Ratio(rawChildDurationInt, 1);
    tsChild.duration = // convert to seconds
      (float)rawChildDurationInt * tempo.getEDUDurationInSeconds().To<float>();
  } else if (durType == "SECONDS") {
    tsChild.duration = rawChildDuration;
    if(tsChild.duration > maxChildDur)
      tsChild.duration = maxChildDur; // enforce limit
    tsChild.durationEDU = Ratio(0, 0); // floating point is not exact: NaN
  } else if (durType == "PERCENTAGE") {
    tsChild.duration = rawChildDuration * ts.duration; // convert to seconds
    if(tsChild.duration > maxChildDur)
      tsChild.duration = maxChildDur; // enforce limit
    tsChild.durationEDU = Ratio(0, 0); // floating point is not exact: NaN
  } else {
    cerr << "Event::buildContinuum -- invalid or missing duration type!" << endl;
    cerr << "      durtype = " << durType << endl;
    cerr << "      in file " << name << endl;
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
    Output::addProperty("Start", tsChild.start, "sec.");
    Output::addProperty("Duration", tsChild.duration, "sec.");
  Output::endSubLevel();
  Output::beginSubLevel("EDU");
    Output::addProperty("Start", tsChild.startEDU, "EDU");
    Output::addProperty("Duration", tsChild.durationEDU, "EDU");
  Output::endSubLevel();
  Output::addProperty("Checkpoint", checkPoint, "of parent");
  Output::endSubLevel();
  
  return true; //success!
}

//----------------------------------------------------------------------------//

bool Event::buildSweep(list<FileValue>::iterator iter) {
  // find start time and dur of last child
  if (currChildNum == 0) {
    tsPrevious.start = 0;
    tsPrevious.startEDU = 0;
  }

  // Set checkpoint to the endpoint of the last event
  checkPoint = tsPrevious.start / ts.duration;

  if (checkPoint > 1) {
    cerr << "Event::Sweep -- Error1: tsChild.start outside range of " 
        << "parent duration." << endl;
    cerr << "      childStime=" << tsChild.start << ", parentDur=" 
        << ts.duration << endl;
    cerr << "      in file: " << name << ", childNum=" 
        << currChildNum << endl;
    cerr << "currChildNum=" << currChildNum << " tsPrevious.start=" << tsPrevious.start <<
	" checkPoint=" << checkPoint << endl;
    exit(1);
  }

  // get the start time
  float rawChildStartTime = iter++->getFloat(this);
  
  // how to process start time: EDU, SECONDS or PERCENTAGE
  string startType = iter++->getString(this);

  if (startType == "UNITS") //Deprecated 'UNITS' is now 'EDU'
    cout << "WARNING: 'UNITS' type is now 'EDU'" << endl;

  if (startType == "EDU" || startType == "UNITS") {
    tsChild.start = rawChildStartTime * 
      tempo.getEDUDurationInSeconds().To<float>();
    tsChild.startEDU = Ratio((int)rawChildStartTime, 1);
  } else if (startType == "SECONDS") {
    tsChild.start = rawChildStartTime; // no conversion needed
    tsChild.durationEDU = Ratio(0, 0); // floating point is not exact: NaN
  } else if (startType == "PERCENTAGE") {
    tsChild.start = rawChildStartTime * ts.duration; // convert to seconds
    tsChild.durationEDU = Ratio(0, 0); // floating point is not exact: NaN
  }

  if (tsChild.start < tsPrevious.start) {
    tsChild.start = tsPrevious.start;
    tsChild.startEDU = tsPrevious.startEDU;
  }

  if (currChildNum == 0) {
    tsChild.start = 0;
    tsChild.startEDU = 0;
  }

  // set checkpoint to the start of this child event
  checkPoint = tsChild.start / ts.duration;

  if (checkPoint > 1) {
    cerr << "Event::Sweep -- Error2: tsChild.start outside range of " 
        << "parent duration." << endl;
    cerr << "      childStime=" << tsChild.start << ", parentDur=" 
        << ts.duration << endl;
    cerr << "      in file: " << name << ", childNum=" 
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
    cout << "WARNING: 'UNITS' type is now 'EDU'" << endl;

  if (durType == "EDU" || durType == "UNITS") {
    tsChild.durationEDU = Ratio(rawChildDurationInt, 1);
    tsChild.duration = // convert to seconds
      (float)rawChildDurationInt * tempo.getEDUDurationInSeconds().To<float>();
  } else if (durType == "SECONDS") {
    tsChild.duration = rawChildDuration;
    if(tsChild.duration > maxChildDur)
      tsChild.duration = maxChildDur; // enforce limit
    tsChild.durationEDU = Ratio(0, 0); // floating point is not exact: NaN
  } else if (durType == "PERCENTAGE") {
    tsChild.duration = rawChildDuration * ts.duration; // convert to seconds
    if(tsChild.duration > maxChildDur)
      tsChild.duration = maxChildDur; // enforce limit
    tsChild.durationEDU = Ratio(0, 0); // floating point is not exact: NaN
  }
  tsPrevious.start = tsChild.start + tsChild.duration;
  tsPrevious.startEDU = tsChild.startEDU + tsChild.durationEDU;

  //Output parameters in the different units available.
  Output::beginSubLevel("Discrete");
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
    Output::addProperty("Start", tsChild.start, "sec.");
    Output::addProperty("Duration", tsChild.duration, "sec.");
    Output::addProperty("Previous", tsPrevious.start, "sec.");
  Output::endSubLevel();
  Output::beginSubLevel("EDU");
    Output::addProperty("Start", tsChild.startEDU, "EDU");
    Output::addProperty("Duration", tsChild.durationEDU, "EDU");
    Output::addProperty("Previous", tsPrevious.startEDU, "EDU");
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

    attackSiv = iter++->getSieve(this);

    durSiv = iter++->getSieve(this);

    list<FileValue>* tmpList = iter++->getListPtr(this);
    list<FileValue>::iterator tmpIter = tmpList->begin();
    while (tmpIter != tmpList->end()) {
      typeProbs.push_back(tmpIter++->getFloat(this));
    }

    tmpList = iter++->getListPtr(this);
    tmpIter = tmpList->begin();
    while( tmpIter != tmpList->end()) {
      attackEnvs.push_back(tmpIter++->getEnvelope(this));
    }

    tmpList = iter++->getListPtr(this);
    
    tmpIter = tmpList->begin();
    while( tmpIter != tmpList->end()) {
      durEnvs.push_back(tmpIter++->getEnvelope(this));
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
  tsChild.startEDU = stimeEDU;
  tsChild.durationEDU = durEDU;
  
  tsChild.start = (float)stimeEDU * 
    tempo.getEDUDurationInSeconds().To<float>();
  tsChild.duration = (float)durEDU * 
    tempo.getEDUDurationInSeconds().To<float>();

  // using edu
  checkPoint = (double)tsChild.start / ts.duration;
  
  
  //Output parameters in the different units available.
  Output::beginSubLevel("Discrete");
  Output::addProperty("Name", childName);
  Output::beginSubLevel("Parameters");
    Output::addProperty("Max Duration", maxChildDur, "EDU");
  Output::endSubLevel();
  Output::beginSubLevel("Seconds");
    Output::addProperty("Start", tsChild.start, "sec.");
    Output::addProperty("Duration", tsChild.duration, "sec.");
  Output::endSubLevel();
  Output::beginSubLevel("EDU");
    Output::addProperty("Start", tsChild.startEDU, "EDU");
    Output::addProperty("Duration", tsChild.durationEDU, "EDU");
  Output::endSubLevel();
  Output::endSubLevel();

  //Return success.
  return true;
}


