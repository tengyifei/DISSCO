/*
CMOD (Composition MODule)
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

//---------------------------------------------------------------------------//
//
//	Sieve.cpp
//
//---------------------------------------------------------------------------//

#include "sieve.h"
#include "random.h"

//---------------------------------------------------------------------------//

Sieve::Sieve() {
  skip = 0;
}

//---------------------------------------------------------------------------//

Sieve::Sieve(string aFile) {
  skip = 0;
  fileName = aFile;
}

//---------------------------------------------------------------------------//

Sieve::~Sieve() {
}

//---------------------------------------------------------------------------//

string Sieve::getFileName() {
  return fileName;
}

//---------------------------------------------------------------------------//

void Sieve::Build(int minVal, int maxVal, 
                  const char *eMethod, const char *wMethod, 
                  vector<int> eArgVect, vector<int> wArgVect) {
  Sieve::Elements(minVal, maxVal, eMethod, eArgVect);
  Sieve::Weights(wMethod, wArgVect);
}      


//---------------------------------------------------------------------------//

void Sieve::FillInVectors(vector<int>& intVect, vector<double>& doubleVect) {
  Sieve::CumulWeights();

  list<int>::iterator eIter = eList.begin();
  list<double>::iterator wIter = wList.begin();

  if ( eList.size() >= wList.size() ) {
    while (eIter != eList.end()) {
      intVect.push_back( *eIter );
      doubleVect.push_back( *wIter );
      eIter++;
      wIter++;
    }
  } else if ( wList.size() > eList.size() ) {
    while (wIter != wList.end()) {
      intVect.push_back( *eIter );
      doubleVect.push_back( *wIter );
      eIter++;
      wIter++;
    }
  }
}

int Sieve::GetNumItems() {
  int result = 0;
  if (eList.size() >= wList.size()) {
    result = eList.size();
  } else {
    result = wList.size();
  }
  return result;
}

//---------------------------------------------------------------------------//

int Sieve::Modify(Envelope *env, string method) {
  Sieve::AddEnvelope(env, method);
  Sieve::CumulWeights();

  return Sieve::ChooseL();
}


//---------------------------------------------------------------------------//

int Sieve::ChooseL() {
  double randomNumber = Random::Rand();

  list<int>::iterator eIter = eList.begin();
  list<double>::iterator wIter = wList.begin();

//  cout << "SIEVE::CHOOSEL  --- Random num = " << randomNumber << endl;
//  cout << "                --- eList = " << *eIter << ", wList = " << *wIter << endl;
  while (eIter != eList.end() && (randomNumber > *wIter)) {
    eIter++;
    wIter++;
//    cout << "                --- eList = " << *eIter << ", wList = " << *wIter << endl;
  }

//  cout << "SIEVE::CHOOSEL  --- picked " << *eIter << endl;
  return *eIter;
}


//---------------------------------------------------------------------------//

void Sieve::Elements(int minVal, int maxVal, 
                     const char *method, 
                     vector<int> eArgVect) {

  if(strcmp(method, "MEANINGFUL") == 0) {		//only meaningful elem.
    Sieve::Meaningful(minVal, maxVal, eArgVect);
  } else if(strcmp(method, "MODS") == 0) {		//uses moduli
    //cout << "  Sieve::Elements - MODS - minVal=" << minVal << "  maxVal="
    //     << maxVal << endl;
    Sieve::Multiples(minVal, maxVal, eArgVect);
  } else if(strcmp(method, "FAKE") == 0) {		//all elem, same weight
    Sieve::Fake(minVal, maxVal);
  } else if(strcmp(method, "FIBONACCI") == 0) {       	//Fibonacci sieve
    cerr << " see harmSieve" << endl;
    exit(1);
  } else if(strcmp(method, "OVERTONES") == 0) {       	//overtone series 
    cerr << "utility::SieveElements - overtones not available yet" << endl;
    exit(1);
  } else if(strcmp(method, "MULT_PARAMS") == 0) {     	//multiple parameters
    cerr << "utility::SieveElements - multiple params not available yet"
        << endl;
    exit(1);
  } else {
    cerr << "no method to build sieve: "<< method << endl;
    exit(1);
  }
}


//---------------------------------------------------------------------------//

void Sieve::Weights(const char *method, 
                    vector<int> wArgVect) {

  if(strcmp(method, "PERIODIC") == 0) {
    Sieve::PeriodicWeights(wArgVect);
  } else if(strcmp(method, "HIERARCHIC") == 0) {
    Sieve::HierarchicWeights(wArgVect);
  } else if(strcmp(method, "INCLUDE") == 0) {
    Sieve::IncludeWeights(wArgVect);
  } else {
    cerr << "Sieve::Weights - no method for asigning weights" << endl;
    exit(1);
  }
}


//---------------------------------------------------------------------------//

void Sieve::Meaningful(int minVal, int maxVal, vector<int> eArgVect) {
  skip = 0;

  for (int i = 0; i < eArgVect.size(); i++) {
    if(eArgVect[i] >= minVal && eArgVect[i] <= maxVal) {
      //if eList.Includes(eArgVect[i])
      eList.push_back( eArgVect[i] );
    }

    if(eArgVect[i] < minVal) {
      skip++;
    }
  }

  // sort the list, ascending
  eList.sort();
}

//---------------------------------------------------------------------------//

void Sieve::Multiples(int minVal, int maxVal, vector<int> numMods) {
  int element, modulo;

  eList.clear();

  skip = 0;

  if (minVal == 0) {
    eList.push_back(0);
  }

/*cout << "Sieve::Multiples - numMods=";
  for (int q = 0; q < numMods.size(); q++){
  cout << numMods[q] << ", ";
  }
  cout << endl;*/

  for (int i = 0; i < numMods.size(); i++) {
    int newElement = numMods[i];

    while (newElement <= maxVal) {
      if ( newElement >= minVal ) {
        // note: we don't have to check for duplicates before adding, because
        //     we'll sort and remove consecutive duplicates at the end
        eList.push_back(newElement);
      } else if (newElement < minVal) {
        skip++;
      }
      newElement += numMods[i]; // increment to the next multiple
    }
  }

  eList.sort(); // sort into ascending order
  eList.unique();  //remove consecutive duplicate values
  //cout << "             Sieve::Multiples - the eList sieve:" << endl;
  //eList.Print();
}

//---------------------------------------------------------------------------//

void Sieve::Fake(int minVal, int maxVal) {
  skip = 0;
  eList.clear();

  for (int i = minVal; i <= maxVal; i++) { 
    eList.push_back(i);
  } 

}

//---------------------------------------------------------------------------//

void Sieve::PeriodicWeights(vector<int> wArgVect) {
  for (int count = 0; count < wArgVect.size(); count++) {
    if (count < eList.size()) {
      wList.push_back(wArgVect[count % wArgVect.size()]);
    }
  }
}

//---------------------------------------------------------------------------//

void Sieve::HierarchicWeights(vector<int> wArgVect) {
  int level;
  double probability;

  for(int count = 0; count < wArgVect.size(); count++) {
    level = 0;
    probability = 0;

    while (level < wArgVect.size()) {

      if(count % wArgVect [level] == 0) {
         probability += wArgVect [level]; 
      }
      level++; 
    }

    if(count >= skip && count < eList.size() + skip) {
      wList.push_back(probability);
    }
  }
}

//---------------------------------------------------------------------------//

void Sieve::IncludeWeights(vector<int> wArgVect) {
  for(int i = 0; i < wArgVect.size(); i++) {
    if(i >= skip && i < eList.size()+ skip) {
      wList.push_back(wArgVect[i]);
    }
  }
}

//---------------------------------------------------------------------------//

void Sieve::AddEnvelope(Envelope *env, string method) {
  float value;
  double checkPoint;
  double probability;

  NormalizeWList();

  int index = 0;
  list<double>::iterator iter = wList.begin();
  while (iter != wList.end()) {
    if (wList.size() > 1) {
      checkPoint = (float)index / (float)(wList.size()-1);
      // check for valid checkpoint
      if (checkPoint < 0 || checkPoint > 1) {
        cerr << "Sieve::AddEnvelope -- checkpoint error:" << endl;
        cerr << "   checkPoint = " << checkPoint << endl;
      }
    } else {
      checkPoint = 0;
    }
    value = env->getValue(checkPoint, 1.);
    if (method == "VARIABLE") {
      probability = Random::PreferedValueDistribution(value, checkPoint);
    } else if (method == "CONSTANT") {
      probability = value;
    }

    //update this element
    *iter = *iter + probability;

    index++;
    iter++;
  }
}

//---------------------------------------------------------------------------//

void Sieve::CumulWeights() {
    double cumul = 0;

    NormalizeWList();

    list<double>::iterator iter = wList.begin();
    while (iter != wList.end()) {
      cumul += *iter;
      *iter = cumul;
      iter++;
    }
}

//---------------------------------------------------------------------------//

void Sieve::NormalizeWList() {
  double wListSum = 0;

  // get the sum of all the items
  list<double>::iterator iter = wList.begin();
  while (iter != wList.end()) {
    wListSum += *iter;
    iter++;
  }

  // go through the list again, normalizing all elements
  iter = wList.begin();
  while (iter != wList.end()) {
    *iter = *iter / wListSum;
    iter++;
  }
}