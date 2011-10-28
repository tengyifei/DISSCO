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
//   Bottom.cpp
//
//----------------------------------------------------------------------------//

#include "Bottom.h"
#include "Random.h"
#include "EventFactory.h"
#include "Output.h"

//----------------------------------------------------------------------------//

//Globals (eventually need to put these in a global class)
extern EnvelopeLibrary envlib_cmod;
extern Score score;
extern int numChan;
extern map<string, EventFactory*> factory_lib;

//----------------------------------------------------------------------------//

//Static sound count variable (maybe should be wrapped into global class)
int Bottom::sndcount = 0;

//----------------------------------------------------------------------------//

Bottom::Bottom(TimeSpan ts, int type, string name) :
  Event(ts, type, name),
  frequencyFV(0), loudnessFV(0), spatializationFV(0), reverberationFV(0),
  modifiersFV(0), numPartialsFV(0), deviationFV(0), spectrumFV(0),
  notePitchClassFV(0), noteDynamicMarkFV(0), noteModifiersFV(0),
  currPartialNum(0),
  wellTempPitch(-1) {}

//----------------------------------------------------------------------------//

Bottom::~Bottom() {
  for(int i = 0; i < childSounds.size(); i++)
    delete childSounds[i];
  for (int i = 0; i < childNotes.size(); i++)
    delete childNotes[i];
    
  /*Note: currently we can not delete FileValues. Deleting them causes a 
  segfaults because something else is holding onto them somewhere. (Cavis)*/
  
  /*We need to eventually investigate whether this causes a memory leak. We
  can ignore this for now since it is an end-of-program leak. (Andrew)*/
  
  #if 0
  if (frequencyFV) delete frequencyFV;
  if (loudnessFV) delete loudnessFV;
  if (spatializationFV) delete spatializationFV;
  if (reverberationFV) delete reverberationFV;
  if (modifiersFV != NULL) delete modifiersFV;
  if (numPartialsFV) delete numPartialsFV;
  if (deviationFV) delete deviationFV;
  if (spectrumFV) delete spectrumFV;
  if (notePitchClassFV) delete notePitchClassFV;
  if (noteDynamicMarkFV) delete noteDynamicMarkFV;
  if (noteModifiersFV) delete noteModifiersFV;
  #endif
}

//----------------------------------------------------------------------------//

void Bottom::initBottomVars(FileValue* frequency, 
                             FileValue* loudness,
                             FileValue* spatialization, 
                             FileValue* reverberation,
                             FileValue* modifiers) {
  frequencyFV = new FileValue(*frequency);
  loudnessFV = new FileValue(*loudness);
  spatializationFV = new FileValue(*spatialization);
  reverberationFV = new FileValue(*reverberation);
  modifiersFV = new FileValue(*modifiers);
}

//----------------------------------------------------------------------------//

void Bottom::initSoundVars(FileValue* numPartials, FileValue* deviation,
                            FileValue* spectrum) {
  numPartialsFV = new FileValue(*numPartials);
  deviationFV = new FileValue(*deviation);
  spectrumFV = new FileValue(*spectrum);
}

//----------------------------------------------------------------------------//

void Bottom::initNoteVars(FileValue* notePitchClass, FileValue* noteDynamicMark,
                           FileValue* noteModifiers) {
  notePitchClassFV = new FileValue(*notePitchClass);
  noteDynamicMarkFV = new FileValue(*noteDynamicMark);
  noteModifiersFV = new FileValue(*noteModifiers);
}

//----------------------------------------------------------------------------//

void Bottom::constructChild(TimeSpan tsChild, int type, string name,
  Tempo tempo, bool freqUsePattern, float patternFreqValue,bool loudnessUsePattern, float patternLoudnessValue) {
  /*First parse the child file. If a child factory does not exist, creating one
  will trigger the parser. If it already exists, 
  then the child is already
  parsed and available through the global factory library.*/
  EventFactory* childFactory = factory_lib[name];
  if(!childFactory)
    childFactory = new EventFactory(name);
  
  //Just to get the checkpoint. Not used any other time.
  checkPoint = (tsChild.start - ts.start) / ts.duration;
  
  //Needs to go to the Output class instead.
  //cout << "Name: " << myName << endl;
  
  /*Given the beginning of the filename we can determine whether or not this is
  a sound or a note. For example B/s000 is a sound, B/n001 is a note.*/
  if(name[2] == 's' || name[0] == 'S') {
    //Initialize the sound-related variables.
    initSoundVars(childFactory->getNumPartials(), childFactory->getDeviation(),
      childFactory->getSpectrum());
      
    //Build the sound.
    

    buildSound(tsChild, type, name, freqUsePattern, patternFreqValue, loudnessUsePattern, patternLoudnessValue);


    //Increment static sound counter.
    sndcount++;
    
  } else if(name[2] == 'n' || name[0] == 'N') {
    //Initialize the note-related variables.
    initNoteVars(childFactory->getNotePitchClass(),
      childFactory->getNoteDynamicMark(), childFactory->getNoteModifiers());
    
    //Build the note.
    buildNote(tsChild, type, name);
    
  } else {
    /*If the name does not begin with an 's' or an 'n', we do not know how to
    process it.*/
    cerr << "Bottom::constructChildren, invalid beginning character for " <<
      name << endl;
    exit(1);
  }
}

//----------------------------------------------------------------------------//

void Bottom::buildSound(TimeSpan tsChild, int type, string name, bool freqUsePattern, float patternFreqValue, bool loudnessUsePattern, float patternLoudnessValue) {
  //Create a new sound object.
  Sound* newSound = new Sound();
  
  //Output sound related properties.
  Output::beginSubLevel("Sound");
  Output::addProperty("Name", name);
  Output::addProperty("Type", type);
  Output::addProperty("Start Time", tsChild.start, "sec.");
  Output::addProperty("End Time", tsChild.start + tsChild.duration, "sec.");
  Output::addProperty("Duration", tsChild.duration, "sec.");
  
  //Set the start time and duration from the timespan.
  newSound->setParam(START_TIME, tsChild.start);
  newSound->setParam(DURATION, tsChild.duration);

  //Set the frequency.
  float baseFrequency = computeBaseFreq( freqUsePattern, patternFreqValue);
  Output::addProperty("Base Frequency", baseFrequency, "Hz");

  //Set the loudness.
  float loudSones = computeLoudness(loudnessUsePattern, patternLoudnessValue);
  newSound->setParam(LOUDNESS, loudSones);
  Output::addProperty("Loudness", loudSones, "sones");

  //Set the number of partials.
  int numPartials = computeNumPartials( baseFrequency );
  Output::beginSubLevel("Partials");
  Output::addProperty("Deviation", computeDeviation(), "normalized");
  
  //For each partial, create and add to sound.
  for (int i = 0; i < numPartials; i++) {
    //Create the next partial object.
    Partial partial;
    
    //Set the partial number of the partial based on the current index.
    partial.setParam(PARTIAL_NUM, i);

    //Compute the deviation for partials above the fundamental.
    double deviation = 0;
    if(i != 0)
      deviation = computeDeviation();
    
    //Set the frequencies for each partial.
    float actualFrequency = setPartialFreq(
      partial, deviation, baseFrequency, i);
    
    //Report the actual frequency.
    stringstream ss; if(i != 0) ss << "Partial " << i; else ss << "Fundamental";
    Output::addProperty(ss.str(), actualFrequency, "Hz");

    //Set the spectrum for this partial.
    setPartialSpectrum(partial, i);

    //Add the partial to the sound.
    newSound->add(partial);
  }
  Output::endSubLevel();

  //Apply the modifiers to the sound.
  applyModifiers(newSound, numPartials);

  //Apply the spatialization to the sound.
  applySpatialization(newSound, numPartials);

  //Apply the reverberation to the sound.
  applyReverberation(newSound);

  Output::endSubLevel();

  //Add the sound to the LASS score.
  score.add(*newSound);
  //The sound to the child sounds list.
  childSounds.push_back(newSound);
}

//----------------------------------------------------------------------------//

void Bottom::buildNote(TimeSpan tsChild, int type, string name) {
  //Create the note.
  Note* newNote = new Note(tsChild, tempo);
  
  //Output note-related properties.
  Output::beginSubLevel("Note");
  Output::addProperty("Name", name);
  Output::addProperty("Type", type);
  Output::addProperty("Start Time", tsChild.start, "sec.");
  Output::addProperty("End Time", tsChild.start + tsChild.duration, "sec.");
  Output::addProperty("Duration", tsChild.duration, "sec.");
  Output::addProperty("Tempo Start Time", tempo.getStartTime(), "sec.");
  Output::addProperty("EDU Start Time", tsChild.startEDU, "EDU");
  Output::addProperty("EDU Duration", tsChild.durationEDU, "EDU");

  //Set the pitch.
  float baseFrequency = computeBaseFreq(false, 0 ); //false because currently pattern doesn't suppart note 
  
  if(wellTempPitch <= 0) {
    //We did not compute the well tempered pitch, so frequency is only in Hertz.
    newNote->setPitchHertz(baseFrequency);
  } else {
    //Get the pitch names, and turn them into a vector.
    vector<string> pitchNames;
    list<FileValue>* pitchList = notePitchClassFV->getListPtr(this);
    list<FileValue>::iterator iter = pitchList->begin();
    
    while(iter != pitchList->end())
      pitchNames.push_back(iter++->getString(this));
      
    newNote->setPitchWellTempered(wellTempPitch, pitchNames);
  }

  //Set the loudness.
  float loudfloat = computeLoudness(false, 0); //false because current pattern doesn't support
  int loudIndex = (int)loudfloat;
  list<FileValue>* loudList = noteDynamicMarkFV->getListPtr(this);
  if(loudIndex < loudList->size()) {
    vector<string> loudNames;
    list<FileValue>::iterator iter = loudList->begin();
    
    while(iter != loudList->end())
      loudNames.push_back(iter++->getString(this));
      
    newNote->setLoudnessMark(loudIndex, loudNames);
    
  } else {
    newNote->setLoudnessSones(loudfloat);
  }

  if(modifiersFV) {
    vector<string> noteMods = applyNoteModifiers();
    newNote->setModifiers(noteMods);
  }
  Output::endSubLevel();
  childNotes.push_back(newNote);
}

//----------------------------------------------------------------------------//

list<Note> Bottom::getNotes() {
  list<Note> result;
  for(int i = 0; i < childNotes.size(); i++)
    result.push_back(*childNotes[i]);
  return result;
}

//----------------------------------------------------------------------------//

float Bottom::computeBaseFreq( bool usePattern, float patternFreqValue) {

  float baseFreqResult;

  list<FileValue>* defList = frequencyFV->getListPtr(this);
  list<FileValue>::iterator iter = defList->begin();

  /* 1st arg is method */
  string freq_method = iter->getString(this);
  iter++;

  if (freq_method == "CONTINUUM") {
    /* 2nd arg is a string (HERTZ or POW2) */
    string cont_method = iter->getString(this);
    iter++;

    if (cont_method == "HERTZ") {
      baseFreqResult = usePattern? patternFreqValue:iter->getFloat(this);        
      /* 3rd arg is a float (baseFreq in Hz) */
    }
      
    else if (cont_method == "POW2") {
      /* 3rd arg is a float (power of 2) */
      
      
      float step = usePattern ? patternFreqValue : iter->getFloat(this);
      double range = log10(CEILING / MINFREQ) / log10(2.); // change log base
      baseFreqResult = pow(2, step * range) * MINFREQ;  // equal chance for all 8vs
    }

  } else if (freq_method == "WELL_TEMPERED") {
    /* 2nd arg is an int */

    wellTempPitch = usePattern? (int) patternFreqValue: iter->getInt(this);
//  cout << "Bottom: computeBaseFreq - wellTempPitch=" << wellTempPitch << endl;
    baseFreqResult = C0 * pow(WELL_TEMP_INCR, wellTempPitch);

  } else if (freq_method == "FUNDAMENTAL") {
    /* 2nd arg is (float)fundamental_freq, 3rd arg is (int)overtone_num */
    float fund_freq = usePattern? patternFreqValue:iter->getFloat(this);
    iter++;
    int overtone_step = iter->getInt(this);
    baseFreqResult = fund_freq * overtone_step;
  }

  //cout <<"base Freq = "<< baseFreqResult<<endl;
  return baseFreqResult;
}

//----------------------------------------------------------------------------//

float Bottom::computeLoudness( bool usePattern, float patternLoudnessValue) {
  return usePattern?patternLoudnessValue:loudnessFV->getFloat(this);
}

//----------------------------------------------------------------------------//

int Bottom::computeNumPartials(float baseFreq) {
  int numPartsResult = numPartialsFV->getInt(this);

  // Decrease numPartials until p < CEILING
  // (CEILING is a global def from define.h)
  while(numPartsResult * baseFreq > CEILING) {
    numPartsResult--;
  }

  if(numPartsResult <= 0) {
    cerr << "Error: Bottom::computeNumPartials got 0, baseFrequency=" 
         << baseFreq << endl;
    exit(1);
  }

  return numPartsResult;
}

//----------------------------------------------------------------------------//

float Bottom::computeDeviation() {
  return deviationFV->getFloat(this);
}

//----------------------------------------------------------------------------//

float Bottom::setPartialFreq(Partial& part, float deviation, float baseFreq, int partNum) {

  // assign frequency to each partial
  float pDev = deviation * (Random::Rand() - 0.5) * 2;
  float pFreq = baseFreq * ((partNum + 1) + pDev);

  // if pFreq is out of range then set it to the closer of the max or min value
  if(pFreq < MINFREQ) {
    pFreq = MINFREQ;
  } else if(pFreq > CEILING) {
    pFreq = CEILING;
  }

  part.setParam(FREQUENCY, pFreq);
  return pFreq;
}

//----------------------------------------------------------------------------//

void Bottom::setPartialSpectrum(Partial& part, int partNum) {
  int envNum;
  float envScale;

  list<FileValue>* argList = spectrumFV->getListPtr(this);
  list<FileValue>::iterator iter = argList->begin();

  if (argList->size() == 1) {
    // 1 variable method ---- this must be an envelope, or a list of envs
    if (iter->getOrigType() == FVAL_LIST) {
      list<FileValue>* envList = iter->getListPtr(this);
      list<FileValue>::iterator envIter = envList->begin();
      if (partNum >= envList->size()) {
        cerr << "Bottom::setPartialSpectrum -- not enough ENVs in list. "
             << "List must be numPartials long." << endl;
        exit(1);
      }
      // get the partNum-th envelope
      for (int i = 0; i < partNum; i++) {
        envIter++;
      }
      part.setParam(WAVE_SHAPE, *envIter->getEnvelope(this)); 
    } else {
      // single envelope
      part.setParam(WAVE_SHAPE, *iter->getEnvelope(this));
    }

  } else if (argList->size() == 2) {
    // 2-var method --- separate envnum and scaling factor

    // Envelope number
    if (iter->getOrigType() == FVAL_LIST) {
      list<FileValue>* intList = iter->getListPtr(this);
      list<FileValue>::iterator intIter = intList->begin();

      // get the partNum-th envelope
      for (int i = 0; i < partNum; i++) {
        intIter++;
      }
      envNum = intIter->getInt(this);

    } else if (iter->getOrigType() == FVAL_FUNC) {
      // get an int from a function
      envNum = iter->getInt(this);

    } else if (iter->getOrigType() == FVAL_NUMBER) {
      // assign that env number to all partials
      envNum = iter->getInt(this) + partNum;
    } else {
      cerr << "Bottom::setPartialSpectrum - no method for partial envelopes!" << endl;
      exit(1);
    }
    iter++;

    // scaling factor
    if (iter->getOrigType() == FVAL_LIST) {
      list<FileValue> *floatList = iter->getListPtr(this);
      list<FileValue>::iterator floatIter = floatList->begin();

      // get the partNum-th scaling
      for (int i = 0; i < partNum; i++) {
        floatIter++;
      }
      envScale = floatIter->getFloat(this);

    } else if (iter->getOrigType() == FVAL_FUNC) {
      // get a float from a function
      envScale = iter->getFloat(this);

    } else if (iter->getOrigType() == FVAL_NUMBER) {
      // assign that scaling factor to all partials
      envScale = iter->getFloat(this);

    /*} else if( method == "RULES") {
      cout << "Bottom::SpectrumAssignEnvs: RULES not available yet" << endl;
      exit(1);*/
    } else {
      cerr << "Bottom::setPartialSpectrum - no method for partial amplitudes!" << endl;
      exit(1);
    }

    Envelope env = envlib_cmod.getEnvelopeRef( envNum );
    env.scale( envScale );
    part.setParam(WAVE_SHAPE, env);
  } else {
    cerr << "Bottom::setPartialSpectrum -- wrong number of args passed!" << endl;
    exit(1);
  }
}

//----------------------------------------------------------------------------//

void Bottom::applySpatialization(Sound* s, int numPartials) {

  list<FileValue>* defList = spatializationFV->getListPtr(this);
  list<FileValue>::iterator iter = defList->begin();

  /* 1st arg is method */
  string spat_method = iter->getString(this);
  iter++;

  /* 2nd arg is how to apply -- SOUND or PARTIAL */
  string applyHow = iter->getString(this);
  iter++;

  /* 3rd arg is a list of some sort */
  if (spat_method == "STEREO") {
    //will be a list of envs, of length 1 if applyhow == SOUND, or
    //  else if applyhow == PARTIAL, it will be numpartials in length
    list<FileValue>* envList = iter->getListPtr(this);
    iter++;
    spatializationStereo(s, envList, applyHow, numPartials);

  } else if (spat_method == "MULTI_PAN") {
    //will be a list of lists of envs ... number of items in 
    // outerlist = NumChannels, items in inner lists = NumPartials
    list<FileValue>* outerList = iter->getListPtr(this);
    iter++;
    spatializationMultiPan(s, outerList, applyHow, numPartials);

  } else if (spat_method == "POLAR") {
    //will be 2 lists of envelopes, both NumPartials in length
    list<FileValue>* thetaList = iter->getListPtr(this);
    iter++;

    list<FileValue>* radiusList = iter->getListPtr(this);
    iter++;

    spatializationPolar(s, thetaList, radiusList, applyHow, numPartials); 

  } else {
    cout << "spat_method = " << spat_method << endl;
    cout << "SOUND_SPATIALIZATION has invalid method!  Use STEREO, MULTI_PAN, or POLAR" << endl;
    exit(1);
  }

}

//----------------------------------------------------------------------------//

void Bottom::spatializationStereo(Sound *s, 
                                  list<FileValue>* envList, 
                                  string applyHow,
                                  int numParts) {
  // check for valid input
  if (applyHow == "SOUND" && envList->size() != 1) {
    cerr << "Error: Spatialization applied to entire sound without "
         << "passing a list of exactly 1 envelope!" << endl;
    cerr << "envlist size was: " << envList->size() << endl;
    
    exit(1);
  } else if (applyHow == "PARTIAL" && envList->size() != numParts) {
    cerr << "Error: Spatialization applied to each partial and didn't "
         << "pass in list of envelopes the length of numPartials!" << endl;
    exit(1);
  }

  // create the spatialization
  list<FileValue>::iterator iter = envList->begin();
  if (applyHow == "SOUND") {
    iter->Evaluate(this);  // cavis +1
    Envelope* panning = iter->getEnvelope(this);
    Pan stereoPan(*panning); // cavis +1
    s->setSpatializer(stereoPan); // cavis +2?
  } else if (applyHow == "PARTIAL") {
    //this isn't implemented in LASS yet, so we can't do it here
    cerr << "Sorry, applying spatialization by PARTIAL not supported yet" << endl;
    exit(1);
  } else {
    cerr << "Error: " << applyHow << " is an invalid way to apply spatialization! "
         << "Use SOUND or PARTIAL" << endl;
    exit(1);
  }
}

//----------------------------------------------------------------------------//

void Bottom::spatializationMultiPan(Sound *s, 
                                    list<FileValue>* outerList, 
                                    string applyHow,
                                    int numParts) {
  // check for valid input ---- (size of the outer list)
  if (outerList->size() != numChan) {
    cerr << "Error: Spatialization outer list not equal to number of channels!" << endl;
    exit(1);
  }

  // split into applyHow cases --- since each will have different innerlist sizes
  if (applyHow == "SOUND") {
    vector<Envelope*> mult; // Vector to build multipan object
    list<FileValue>* innerList;

    list<FileValue>::iterator outerIter = outerList->begin();
    while (outerIter != outerList->end()) {
      outerIter->Evaluate(this);
      innerList = outerIter->getListPtr(this);

      // note: size of inner lists MUST be 1 since we apply to the whole sound
      if (innerList->size() != 1) {
        cerr << "Error: Spatialization applied to entire sound without "
             << "passing inner lists of exactly 1 envelope!" << endl;
        exit(1);
      }

      (innerList->front()).Evaluate(this);
      mult.push_back( (innerList->front()).getEnvelope(this) );

      outerIter++;
    }

    MultiPan multipan(numChan, mult);
    s->setSpatializer(multipan);

    for (int i = 0; i < mult.size(); i++) {
      cout << "MULTIPAN: env " << i << endl;
      mult[i]->print();
    }

  } else if (applyHow == "PARTIAL") {
    //this isn't implemented in LASS yet, so we can't do it here
    cerr << "Sorry, applying spatialization by PARTIAL not supported yet" << endl;
    exit(0);
  } else {
    cerr << "Error: " << applyHow << " is an invalid way to apply spatialization! "
         << "Use SOUND or PARTIAL" << endl;
    exit(1);
  }
}

//----------------------------------------------------------------------------//

void Bottom::spatializationPolar(Sound *s, 
                                 list<FileValue>* thetaList,
                                 list<FileValue>* radiusList, 
                                 string applyHow,
                                 int numParts) {
  // check for valid input
  if (applyHow == "SOUND" && (thetaList->size() != 1 || radiusList->size() != 1)) {
    cerr << "Error: Spatialization applied to entire sound without "
         << "passing inner lists of exactly 1 envelope!" << endl;
    exit(1);
  } else if (applyHow == "PARTIAL" && (thetaList->size() != numParts || radiusList->size() != numParts)) {
    cerr << "Error: Spatialization applied to each partial and didn't "
         << "pass inner lists of envelopes the length of numPartials!" << endl;
    exit(1);
  }

  if (applyHow == "SOUND") {
    Envelope* thetaEnv;
    Envelope* radiusEnv;

    (thetaList->front()).Evaluate(this);
    thetaEnv = (thetaList->front()).getEnvelope(this);
    cout << "POLAR -- thetaEnv" << endl;
    thetaEnv->print();

    (radiusList->front()).Evaluate(this);
    radiusEnv = (radiusList->front()).getEnvelope(this);
    cout << "POLAR -- radiusEnv" << endl;
    radiusEnv->print();

    MultiPan multipan(numChan);
    float time, theta, radius;

    // take 100 samples of the envelopes and apply them to the sound
    // (this should be enough to catch the important parts of the env)
    int numPolarSamples = 100;
    //cout << "TIME    THETA   RADIUS" << endl;
    for (int i = 0; i <= numPolarSamples; i++) {
      time = (float)i / numPolarSamples;
      theta = PI * thetaEnv->getScaledValueNew(time, 1.0);
      radius = radiusEnv->getScaledValueNew(time, 1.0);

      multipan.addEntryLocation(time, theta, radius);
      //if (i % 5 == 0) {
      //  cout << setw(6) << time << "  ";
      //  cout << setw(6) << theta << " ";
      //  cout << setw(6) << radius << "  " << endl;
      //}
    }

    s->setSpatializer(multipan);

  } else if (applyHow == "PARTIAL") {
    //this isn't implemented in LASS yet, so we can't do it here
    cerr << "Sorry, applying spatialization by PARTIAL not supported yet" << endl;
    exit(0);
  } else {
    cerr << "Error: " << applyHow << " is an invalid way to apply spatialization! "
         << "Use SOUND or PARTIAL" << endl;
    exit(1);
  }
}

//----------------------------------------------------------------------------//

void Bottom::applyReverberation(Sound* s) {
  reverberationFV->Evaluate(this);
  if (reverberationFV->getReturnType() != FVAL_LIST) {
    cout << reverberationFV->TypeToString() << endl;
    cerr << "Syntax error for reverberation in file: " << name << endl;
    exit(1);
  }
  list<FileValue>* argList = reverberationFV->getListPtr(this);
  list<FileValue>::iterator iter = argList->begin();

  /* get the reverb method */
  string rev_method = iter->getString(this);
  iter++;

  if (rev_method == "SIMPLE") {
    //second input is roomsize
    float roomSize = iter->getFloat(this);
    iter++;

    Reverb* reverbObj = new Reverb(roomSize, SAMPLING_RATE);
    s->use_reverb(reverbObj);

  } else if (rev_method == "MEDIUM") {
    //second input is percent reverb envelope
    Envelope* percent_rev = iter->getEnvelope(this);
    iter++;

    //3 floats:  hi/low spread, gain all pass, delay
    float hi_low_spread = iter->getFloat(this);
    iter++;
    float gain_all_pass = iter->getFloat(this);
    iter++;
    float delay = iter->getFloat(this);
    iter++;

    Reverb* reverbObj = new Reverb(percent_rev, hi_low_spread, gain_all_pass, delay, SAMPLING_RATE);
    s->use_reverb(reverbObj);

  } else if (rev_method == "ADVANCED") {
    //second input is percent reverb envelope
    Envelope* percent_rev = iter->getEnvelope(this);
    iter++;

    //list of EXACTLY 6 comb gain filters
    float comb_gain_list[6];
    list<FileValue>* combList = iter->getListPtr(this);
    iter++;
    if (combList->size() != 6) {
      cerr << "Error: reverb comb gain list must contain 6 items!" << endl;
      exit(1);
    }
    list<FileValue>::iterator combIter = combList->begin();
    int combIndex = 0;
    while (combIter != combList->end()) {
      comb_gain_list[combIndex] = combIter->getFloat(this); 
      combIndex++;
      combIter++;
    }

    //list of EXACTLY 6 lp gain filters
    float lp_gain_list[6];
    list<FileValue>* lpList = iter->getListPtr(this);
    iter++;
    if (lpList->size() != 6) {
      cerr << "Error: reverb lp gain list must contain 6 items!" << endl;
      exit(1);
    }
    list<FileValue>::iterator lpIter = lpList->begin();
    int lpIndex = 0;
    while (lpIter != lpList->end()) {
      lp_gain_list[lpIndex] = lpIter->getFloat(this); 
      lpIndex++;
      lpIter++;
    }

    //2 floats:  gain all pass, delay
    float gain_all_pass = iter->getFloat(this);
    iter++;
    float delay = iter->getFloat(this);
    iter++;

    Reverb* reverbObj = new Reverb(percent_rev, &comb_gain_list[0], &lp_gain_list[0], gain_all_pass, delay, SAMPLING_RATE);
    s->use_reverb(reverbObj);
  } else {
    cerr << "Invalid method/syntax in reverb!" << endl;
    cerr << "   filename=" << name << endl;
    exit(1);
  }
}

//----------------------------------------------------------------------------//

void Bottom::applyModifiers(Sound *s, int numPartials) {

  vector<Modifier> modNoDep;  //mods without dependencies
  map<string, vector<Modifier> > modMutEx; // map mutex group names to the mods

  list<FileValue>* modList = modifiersFV->getListPtr(this);
  list<FileValue>::iterator modIter = modList->begin();
  while (modIter != modList->end()) {
    // create the modifier and add it to the proper group
    list<FileValue>::iterator currMod = modIter->getListPtr(this)->begin();
    // 1st arg is string, 2nd and 3rd always envs
    string modType = currMod->getString(this);
    currMod++;
    Envelope* probEnv = currMod->getEnvelope(this);
    currMod++;
    string applyHow = currMod->getString(this);
    currMod++;

    Modifier newMod(modType, probEnv, applyHow);
    string mutExGroup = "";
    if (applyHow == "SOUND") {
      // add all remaining envs
      while (mutExGroup == "" && currMod != modIter->getListPtr(this)->end()) {
        currMod->Evaluate(this);
        if (currMod->getReturnType() == FVAL_ENVELOPE) {
          newMod.addValueEnv( currMod->getEnvelope(this) );
          currMod++;
        } else {
          //not an envelope -- this must be MUT_EX
          FileValue mutExFV = currMod->getListPtr(this)->back();
          if (mutExFV.getReturnType() == FVAL_STRING) {
            mutExGroup = mutExFV.getString(this);
          } else {
            cerr << "Bottom::applyModifiers error: invalid syntax for MUT_EX group!" << endl;
            exit(1);
          }
        }
      }

    } else if (applyHow == "PARTIAL") {
      // get the filevalue for each mod value
      vector<FileValue> partVals;
      while (mutExGroup == "" && currMod != modIter->getListPtr(this)->end()) {
        currMod->Evaluate(this);
        if (currMod->getReturnType() == FVAL_ENVELOPE) {
          partVals.push_back( *currMod );
          currMod++;
        } else {
          //not an envelope -- this must be MUT_EX
          mutExGroup = currMod->getListPtr(this)->back().getString(this);
        }
      }
      // re-eval for every partial, and add them
      for (int i = 0; i < numPartials; i++) {
        for (int j = 0; j < partVals.size(); j++) {
          newMod.addValueEnv( partVals[j].getEnvelope(this) );
        }
      }
    }

    if (mutExGroup == "") {
      // not MUT_EX
      modNoDep.push_back(newMod);
    } else {
      // mutually exclusive
      modMutEx[mutExGroup].push_back(newMod);
    }

    modIter++; // go to the next MOD in the list
  }

  // go through the non-exclusive mods
  for (int i = 0; i < modNoDep.size(); i++) {
    if (modNoDep[i].willOccur(checkPoint)) {
      modNoDep[i].applyModifier(s, numPartials);
    }
  }

  // go through the exclusive mods
  map<string, vector<Modifier> >::iterator iter = modMutEx.begin();
  while (iter != modMutEx.end()) {
    vector<Modifier> modGroup = (*iter).second;

    //go through the group, and apply 1 at most
    bool appliedMod = false;
    for (int i = 0; i < modGroup.size() && !appliedMod; i++) {
      if (modGroup[i].willOccur(checkPoint)) {
        modGroup[i].applyModifier(s, numPartials);
        appliedMod = true;
      }
    }
    iter++;
  }

}

//----------------------------------------------------------------------------//

vector<string> Bottom::applyNoteModifiers() {
  vector<string> result;

  vector<Modifier> modNoDep;  //mods without dependencies
  map<string, vector<Modifier> > modMutEx; // map mutex group names to the mods

  list<FileValue>* modList = modifiersFV->getListPtr(this);
  list<FileValue>::iterator modIter = modList->begin();

  while (modIter != modList->end()) {
    // create the modifier and add it to the proper group
    list<FileValue>::iterator currMod = modIter->getListPtr(this)->begin();
    // 1st arg is string, 2nd is env
    string modType = currMod->getString(this);
    currMod++;
    Envelope* probEnv = currMod->getEnvelope(this);
    currMod++;

    Modifier newMod(modType, probEnv, "SOUND");
    string mutExGroup = "";

    while (mutExGroup == "" && currMod != modIter->getListPtr(this)->end()) {
      FileValue mutExFV = currMod->getListPtr(this)->back();
      if (mutExFV.getReturnType() == FVAL_STRING) {
        mutExGroup = mutExFV.getString(this);
      } else {
        cerr << "Bottom::applyModifiers error: invalid syntax for MUT_EX group!" << endl;
        exit(1);
      }
    }

    if (mutExGroup == "") {
      // not MUT_EX
      modNoDep.push_back(newMod);
    } else {
      // mutually exclusive
      modMutEx[mutExGroup].push_back(newMod);
    }

    modIter++; // go to the next MOD in the list
  }

  // go through the non-exclusive mods
  for (int i = 0; i < modNoDep.size(); i++) {
    if (modNoDep[i].willOccur(checkPoint)) {
      result.push_back( modNoDep[i].getModName() );
    }
  }

  // go through the exclusive mods
  map<string, vector<Modifier> >::iterator iter = modMutEx.begin();
  if (iter != modMutEx.end()) {
    vector<Modifier> modGroup = (*iter).second;

    //go through the group, and apply 1 at most
    bool appliedMod = false;
    for (int i = 0; i < modGroup.size() && !appliedMod; i++) {
      if (modGroup[i].willOccur(checkPoint)) {
        result.push_back( modGroup[i].getModName() );
        appliedMod = true;
      }
    }
    iter++;
  }

  return result;
}


void Bottom::buildChildEvents() {
  
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


  list<FileValue>::iterator iter2 = iter;
  iter2 ++;
  iter2 ++;
  
  //check if child Type use pattern
  if (iter2->getFtnString() == "GetPattern"){
    cout<<"Use Pattern for Child Type"<<endl;
    childTypePattern = &(*iter2);
    childTypePattern->Evaluate();
  }
  
  //check if duration use pattern
  iter2 ++;

  if (iter2->getFtnString() == "GetPattern"){
    cout<<"Use Pattern for Child Duration"<<endl;
    childDurationPattern = &(*iter2);
    childDurationPattern->Evaluate();
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


  bool freqUsePattern = false;
  FileValue* freqPattern = NULL;
  
  list<FileValue>* defList = frequencyFV->getListPtr(this);
  iter = defList->begin();

  /* 1st arg is method we don't care about method now. just want to see if it's pattern*/
  string freq_method = iter->getString(this);
  iter++;

  if (freq_method == "CONTINUUM") {
  /* 2nd arg is a string (HERTZ or POW2) */
    iter++;
    if (iter->getFtnString() == "GetPattern"){
      freqUsePattern = true; 

    }
  }
  else if (freq_method == "WELL_TEMPERED"&&iter->getFtnString() =="GetPattern"){    
    freqUsePattern = true;

  } 
  else if (freq_method == "FUNDAMENTAL"&&iter->getFtnString() =="GetPattern") {
      freqUsePattern = true;
  
  }

  if (freqUsePattern){
    freqPattern = &(*iter);  
    freqPattern->Evaluate();
  }



  bool loudnessUsePattern = false;
  FileValue* loudnessPattern = NULL;
  
  //defList = loudnessFV->getListPtr(this);
  //defList = loudnessFV;
  //iter = defList->begin();

  /* 1st arg is method we don't care about method now. just want to see if it's pattern*/
  
  if (loudnessFV->getFtnString() == "GetPattern"){
    loudnessUsePattern = true;
    cout<<"loudness Use Pattern"<<endl;
  }
  

  if (loudnessUsePattern){
    loudnessPattern = &(*iter);  
    loudnessPattern->Evaluate();
  }



  //Using the temporary events that were created, construct the actual children.
  for (int i = 0; i < temporaryChildEvents.size(); i++) {
    //cout for implementing pattern 
    //cout<<"this for loop has run "<< i<< " times by "<<this<<endl; 
    
    //Increment the static current child number.
    currChildNum = i;
    
    //Get current event.
    Event *e = temporaryChildEvents[currChildNum];
    
    //Construct the child (overloaded in Bottom)
    if(freqUsePattern){
      float k = freqPattern->getFloat();      
      if (loudnessUsePattern){
        float loudness = loudnessPattern->getFloat();
        constructChild(e->ts, e->type, e->name, e->tempo, true, k, true, loudness);
      }
      else {
        //cout<< "use pattern, next pattern value: "<<k<<endl;
        constructChild(e->ts, e->type, e->name, e->tempo, true, k, false, 0);
      }
    }
    else { // frequency not pattern
      if (loudnessUsePattern){
        float loudness = loudnessPattern->getFloat();
        constructChild(e->ts, e->type, e->name, e->tempo, false, 0, true, loudness);
      }
      else {
        //cout<< "use pattern, next pattern value: "<<k<<endl;
        constructChild(e->ts, e->type, e->name, e->tempo, false, 0, false, 0);
      }   
     }
    
    
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

