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
//   bottom.cpp
//
//----------------------------------------------------------------------------//

#include "bottom.h"
#include "random.h"
#include "eventfactory.h"

//----------------------------------------------------------------------------//

extern EnvelopeLibrary envlib_cmod;
extern Score score;
extern int numChan;
//extern int currChildNum;
extern map<string, EventFactory*> factory_lib;
extern ofstream * outputFile;
int Bottom::sndcount = 0;

//----------------------------------------------------------------------------//

Bottom::Bottom(float aStartTime, float aDuration, int aType, string aName, int level)
               :Event(aStartTime, aDuration, aType, aName, level) {
  frequencyFV = NULL;
  loudnessFV = NULL;
  spatializationFV = NULL;
  reverberationFV = NULL;
  modifiersFV = NULL;

  numPartialsFV = NULL;
  deviationFV = NULL;
  spectrumFV = NULL;

  notePitchClassFV = NULL;
  noteDynamicMarkFV = NULL;
  noteModifiersFV = NULL;

  currPartialNum = 0;
  wellTempPitch = -1;
}

//----------------------------------------------------------------------------//

Bottom::Bottom(const Bottom &orig) 
               :Event(orig) {
  frequencyFV = orig.frequencyFV;
  loudnessFV = orig.loudnessFV;
  spatializationFV = orig.spatializationFV;
  reverberationFV = orig.reverberationFV;
  modifiersFV = orig.modifiersFV;

  numPartialsFV = orig.numPartialsFV;
  deviationFV = orig.deviationFV;
  spectrumFV = orig.spectrumFV;

  notePitchClassFV = orig.notePitchClassFV;
  noteDynamicMarkFV = orig.noteDynamicMarkFV;
  noteModifiersFV = orig.noteModifiersFV;

  currPartialNum = orig.currPartialNum;
  mod_used = orig.mod_used;
  wellTempPitch = orig.wellTempPitch;

  for (int i = 0; i < orig.childSounds.size(); i++) {
    childSounds.push_back( new Sound(*orig.childSounds[i]) );
  }
  for (int i = 0; i < orig.childNotes.size(); i++) {
    childNotes.push_back( new Note(*orig.childNotes[i]) );
  }
}

//----------------------------------------------------------------------------//

Bottom& Bottom::operator=(const Bottom& rhs) {
  Event::operator=(rhs);

  frequencyFV = rhs.frequencyFV;
  loudnessFV = rhs.loudnessFV;
  spatializationFV = rhs.spatializationFV;
  reverberationFV = rhs.reverberationFV;
  modifiersFV = rhs.modifiersFV;

  numPartialsFV = rhs.numPartialsFV;
  deviationFV = rhs.deviationFV;
  spectrumFV = rhs.spectrumFV;

  notePitchClassFV = rhs.notePitchClassFV;
  noteDynamicMarkFV = rhs.noteDynamicMarkFV;
  noteModifiersFV = rhs.noteModifiersFV;

  currPartialNum = rhs.currPartialNum;
  mod_used = rhs.mod_used;
  wellTempPitch = rhs.wellTempPitch;

  for (int i = 0; i < rhs.childSounds.size(); i++) {
    childSounds.push_back( new Sound(*rhs.childSounds[i]) );
  }
  for (int i = 0; i < rhs.childNotes.size(); i++) {
    childNotes.push_back( new Note(*rhs.childNotes[i]) );
  }
}

//----------------------------------------------------------------------------//

Bottom::~Bottom() {
  for (int i = 0; i < childSounds.size(); i++) {
    delete childSounds[i];
  }
  for (int i = 0; i < childNotes.size(); i++) {
    delete childNotes[i];
  }
  // note: can't delete filevalues -- segfaults because something else is 
  //       holding onto them somewhere ---- (cavis)
  /*if (frequencyFV) { delete frequencyFV; cout << "deleted frequency" << endl; }
  if (loudnessFV) delete loudnessFV;
  if (spatializationFV) delete spatializationFV;
  if (reverberationFV) delete reverberationFV;
  if (modifiersFV != NULL) delete modifiersFV;
  if (numPartialsFV) delete numPartialsFV;
  if (deviationFV) delete deviationFV;
  if (spectrumFV) delete spectrumFV;
  if (notePitchClassFV) delete notePitchClassFV;
  if (noteDynamicMarkFV) delete noteDynamicMarkFV;
  if (noteModifiersFV) delete noteModifiersFV;*/
}

//----------------------------------------------------------------------------//

void Bottom::initBottomVars( FileValue* frequency, 
                             FileValue* loudness,
                             FileValue* spatialization, 
                             FileValue* reverberation,
                             FileValue* modifiers ) {
  frequencyFV = new FileValue(*frequency);
  loudnessFV = new FileValue(*loudness);
  spatializationFV = new FileValue(*spatialization);
  reverberationFV = new FileValue(*reverberation);
  modifiersFV = new FileValue(*modifiers);
}

//----------------------------------------------------------------------------//

void Bottom::initSoundVars( FileValue* numPartials, FileValue* deviation,
                            FileValue* spectrum ) {
  numPartialsFV = new FileValue(*numPartials);
  deviationFV = new FileValue(*deviation);
  spectrumFV = new FileValue(*spectrum);
}

//----------------------------------------------------------------------------//

void Bottom::initNoteVars( FileValue* notePitchClass, FileValue* noteDynamicMark,
                           FileValue* noteModifiers ) {
  notePitchClassFV = new FileValue(*notePitchClass);
  noteDynamicMarkFV = new FileValue(*noteDynamicMark);
  noteModifiersFV = new FileValue(*noteModifiers);
}

//----------------------------------------------------------------------------//

  void Bottom::constructChild(float stime, float dur, int type, string name, 
   int level) {

  // first, parse the child file
  EventFactory* childFactory = factory_lib[name];
  if (childFactory == NULL) {
    // Parse the file
    childFactory = new EventFactory(name);
  }
  
  // Att: MyStart Time and myDuration are the stime and dur of the parent !!
  // Just to get the checkPoint. Not used any other time.
  checkPoint = (double)(stime-myStartTime) / myDuration;

  // Bottom event - children will be either sounds, notes, or visuals 
  if (myName[2] == 's') {
    initSoundVars( childFactory->getNumPartials(),
                   childFactory->getDeviation(),
                   childFactory->getSpectrum() );

    buildSound(stime, dur, type, name);
    sndcount++;
    //childEvents.push_back((Bottom *) this);
  } else if (myName[2] == 'n'){
    initNoteVars( childFactory->getNotePitchClass(),
                  childFactory->getNoteDynamicMark(),
                  childFactory->getNoteModifiers() );
    buildNote(stime, dur, type, name);
    //childEvents.push_back((Bottom *) this);
  } else if (myName[2] == 'v'){
    cerr << "Bottom::constructChildren - visual output not yet implemented" << endl;
    exit(1);
  } else {
    cerr << "Bottom::constructChildren - invalid beginning character for " << myName << endl;
    exit(1);
  }
}

//----------------------------------------------------------------------------//

void Bottom::buildSound(float stime, float dur, int type, string name) {
  Sound* newSound = new Sound();

  newSound->setParam(START_TIME, stime);
  newSound->setParam(DURATION, dur);

  // frequency
  float baseFrequency = computeBaseFreq();

  // loudness
  float loudSones = computeLoudness();
  newSound->setParam(LOUDNESS, loudSones);

  // numpartials
  int numPartials = computeNumPartials( baseFrequency );

  // for each numPartial, create partial, and add to sound.
  for (int i = 0; i < numPartials; i++) {
    Partial part;
    currPartialNum = i;
    part.setParam(PARTIAL_NUM, i);

    // deviation
    double deviation = computeDeviation();

    // set the frequencies for each partial
    setPartialFreq( part, deviation, baseFrequency, currPartialNum );

    // spectrum for each partial
    setPartialSpectrum( part, currPartialNum);

    newSound->add(part);
  }

  // Apply modifiers here
  applyModifiers(newSound, numPartials);

  // Apply Spatialization
  applySpatialization(newSound, numPartials);

  // Apply Reverberation
  applyReverberation(newSound);

  printSound( stime, dur, type, name, numPartials, baseFrequency, loudSones );

  score.add(*newSound);
  childSounds.push_back(newSound);
}

//----------------------------------------------------------------------------//

void Bottom::buildNote(float stime, float dur, int type, string name) {
  Note* newNote = new Note(stime, dur, unitsPerSecond, unitsPerBar);

  // set the pitch
  float baseFrequency = computeBaseFreq();
  if (wellTempPitch <= 0) {
    // we didn't compute the well tempered pitch, so freq is only in hertz
    newNote->setPitchHertz( baseFrequency );
  } else {
    // get the pitch names, and turn into a vector
    vector<string> pitchNames;
    list<FileValue>* pitchList = notePitchClassFV->getListPtr(this);
    list<FileValue>::iterator iter = pitchList->begin();
    while (iter != pitchList->end()) {
      pitchNames.push_back( iter->getString(this) );
      iter++;
    }
    newNote->setPitchWellTempered( wellTempPitch, pitchNames );
  }

  // set the loudness
  float loudfloat = computeLoudness();
  int loudIndex = (int)loudfloat;
  list<FileValue>* loudList = noteDynamicMarkFV->getListPtr(this);
  if (loudIndex < loudList->size()) {
    // loudness mark is inbounds, so assume it's an index into this list
    vector<string> loudNames;
    list<FileValue>::iterator iter = loudList->begin();
    while (iter != loudList->end()) {
      loudNames.push_back( iter->getString(this) );
      iter++;
    }
    newNote->setLoudnessMark( loudIndex, loudNames );
  } else {
    newNote->setLoudnessSones( loudfloat );
  }

  if (modifiersFV != NULL) {
    vector<string> noteMods = applyNoteModifiers();
    newNote->setModifiers( noteMods );
  }

  printNote(*newNote, type, name);
  childNotes.push_back(newNote);
}

//----------------------------------------------------------------------------//

float Bottom::computeBaseFreq() {
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
      baseFreqResult = iter->getFloat(this);
      /* 3rd arg is a float (baseFreq in Hz) */
    } else if (cont_method == "POW2") {
      /* 3rd arg is a float (power of 2) */
      float step = iter->getFloat(this);
      double range = log10(CEILING / MINFREQ) / log10(2.); // change log base
      baseFreqResult = pow(2, step * range) * MINFREQ;  // equal chance for all 8vs
    }

  } else if (freq_method == "WELL_TEMPERED") {
    /* 2nd arg is an int */
    wellTempPitch = iter->getInt(this);
    baseFreqResult = C0 * pow(WELL_TEMP_INCR, wellTempPitch);

  } else if (freq_method == "FUNDAMENTAL") {
    /* 2nd arg is (float)fundamental_freq, 3rd arg is (int)overtone_num */
    float fund_freq = iter->getFloat(this);
    iter++;
    int overtone_step = iter->getInt(this);
    baseFreqResult = fund_freq * overtone_step;
  }
  return baseFreqResult;
}

//----------------------------------------------------------------------------//

float Bottom::computeLoudness() {
  return loudnessFV->getFloat(this);
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

void Bottom::setPartialFreq(Partial& part, float deviation, float baseFreq, int partNum) {

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
    cerr << "Syntax error for reverberation in file: " << myName << endl;
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
    cerr << "   filename=" << myName << endl;
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

//----------------------------------------------------------------------------//

void Bottom::print() {
  // bottom prints no differently than regular events (for now)
  Event::print();
}

//----------------------------------------------------------------------------//

void Bottom::printSound(float stime, float dur, int type, string name, 
                        int numPartials, float freq, float sones) {
  indentPrint(printLevel + 1);                                                //
  *outputFile << "<sound>" << endl;                                           //
  
  indentPrint(printLevel + 1);                                                //
  *outputFile << "  <name>" << name << "</name>" << endl;                     //
  
  indentPrint(printLevel + 1);                                                //
  *outputFile << "  <type>" << type << "</type>" << endl;                     //
  
  indentPrint(printLevel + 1);                                                //
  *outputFile << "  <global-sound-count>" << sndcount <<                      //
    "</global-sound-count>" << endl;                                          //

  indentPrint(printLevel + 1);                                                //
  *outputFile << "  <start-time-sec>" << stime << "</start-time-sec>" << endl;//
  
  indentPrint(printLevel + 1);                                                //
  *outputFile << "  <start-time-units>" << stime * unitsPerSecond <<          //
    "</start-time-units>" << endl;                                            //

  indentPrint(printLevel + 1);                                                //
  *outputFile << "  <duration-sec>" << dur << "</duration-sec>" << endl;      //
  
  indentPrint(printLevel + 1);                                                //
  *outputFile << "  <duration-units>" << dur * unitsPerSecond <<              //
    "</duration-units>" << endl;                                              //

  indentPrint(printLevel + 1);                                                //
  *outputFile << "  <partials>" << numPartials << "</partials>" << endl;      //
  
  indentPrint(printLevel + 1);                                                //
  *outputFile << "  <sones>" << sones << "</sones>" << endl;                  //

  indentPrint(printLevel + 1);                                                //
  *outputFile << "  <frequency>" << freq << "</frequency>" << endl;           //

  indentPrint(printLevel + 1);                                                //
  *outputFile << "</sound>" << endl;                                          //
}

//----------------------------------------------------------------------------//

void Bottom::printNote(Note& n, int type, string name) {
  //borderPrint(printLevel + 1, borderchar);
  string level; for(int i = 0; i < printLevel + 3; i++) level += "  ";
  
  *outputFile << level << "<note>" << endl;
  
  *outputFile << n.toStringStartTime(printLevel + 4);

  *outputFile << n.toStringDuration(printLevel + 4);

  *outputFile << n.toStringOther(printLevel + 4);
  
  *outputFile << level << "</note>" << endl;

  //borderPrint(printLevel + 1, borderchar);
}

//----------------------------------------------------------------------------//

list<Note> Bottom::getNotes() {
  list<Note> result;
  for (int i = 0; i < childNotes.size(); i++) {
    result.push_back( *childNotes[i] );
  }
  return result;
}

