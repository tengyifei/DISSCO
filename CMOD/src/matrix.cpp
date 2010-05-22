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
//  matrix.cpp
//
//----------------------------------------------------------------------------//

#include "matrix.h"
#include "random.h"

using namespace std;
//----------------------------------------------------------------------------//

extern EnvelopeLibrary envlib;
extern int sever;

//----------------------------------------------------------------------------//

Matrix::Matrix(int numTypes, int numAttacks, int numDurations, 
               double init, vector<int> numTypesInLayers) {
  matr.clear();

  matr.resize(numTypes);
  for (int type = 0; type < numTypes; type++) {
    // do for each type

    matr[type].resize(numAttacks);
    for (int attNum = 0; attNum < numAttacks; attNum++) {
      // do for each attack

      matr[type][attNum].resize(numDurations);
      for (int durNum = 0; durNum < numDurations; durNum++) {
        // do for each dur

        matr[type][attNum][durNum].attdurprob = init;
        matr[type][attNum][durNum].normprob = 0;
        matr[type][attNum][durNum].type = type;
        matr[type][attNum][durNum].stime = 0;
        matr[type][attNum][durNum].dur = 0;
      }
    }
  }

  for (int i = 0; i < numTypesInLayers.size(); i++) {
    for (int j = 0; j < numTypesInLayers[i]; j++) {
      typeLayers.push_back( i );
    }
  }
}


//----------------------------------------------------------------------------//

Matrix::Matrix(const Matrix &orig) {
  matr = orig.matr;
  typeLayers = orig.typeLayers;
  typeProb = orig.typeProb;
}

//----------------------------------------------------------------------------//

Matrix& Matrix::operator=(const Matrix &orig) {
  matr = orig.matr;
  typeLayers = orig.typeLayers;
  typeProb = orig.typeProb;
}

//----------------------------------------------------------------------------//

Matrix::~Matrix() {
}

//----------------------------------------------------------------------------//

void Matrix::setAttacks(Sieve* attackSieve, vector<Envelope*> attackEnvs) {
  vector<int> attTimes;
  vector<double> attProbs;

  attackSieve->FillInVectors(attTimes, attProbs);

  // add the results into the matrix
  for (int type = 0; type < matr.size(); type++) {
    // do for each type

    for (int attNum = 0; attNum < matr[type].size(); attNum++) {
      // do for each attack
      double attackSieveValue = attProbs[attNum];
      if (attNum < 0 || attNum > matr[type].size()) {
        cerr << "Matrix::setAttacks -- error - getValue out of bounds;" << endl;
        cerr << "  attNum=" << attNum << ", matr[type].size()=" 
             << matr[type].size() << endl;
      }

      double attackEnvValue = attackEnvs[type]->getValue(attNum, matr[type].size());
      int attackStime = attTimes[attNum];

      for (int durNum = 0; durNum < matr[type][attNum].size(); durNum++) {
        matr[type][attNum][durNum].attdurprob += attackSieveValue;
        matr[type][attNum][durNum].attdurprob *= attackEnvValue;
        matr[type][attNum][durNum].stime = attackStime;
      }
    }
  }
}

//----------------------------------------------------------------------------//

void Matrix::setDurations(Sieve* durSieve, vector<Envelope*> durEnvs) {
  vector<int> durTimes;
  vector<double> durProbs;
  durSieve->FillInVectors(durTimes, durProbs);

  int allowed, start;
  float durEnd;

  // this marks the end-window of the parent event
  int maxStartTime = matr[0][matr[0].size()-1][0].stime;

  //int oldType = 0;

  // add the results into the matrix
  for (int type = 0; type < matr.size(); type++) {
    // do for each type

    for (int attNum = 0; attNum < matr[type].size(); attNum++) {
      // do for each attack

      for (int durNum = 0; durNum < matr[type][attNum].size(); durNum++) {
        double durSieveVal = durProbs[durNum];

        if (durNum < 0 || durNum > matr[type][attNum].size()) {
          cerr << "Matrix::setDurations -- error - getValue out of bounds;" << endl;
          cerr << "  durNum=" << durNum << ", matr[type][attNum].size()=" 
              << matr[type][attNum].size() << endl;
        }
/*
	if(type != oldType) {
	  cout << "Matrix::setDurations - envelope for type " << type << endl;
	  durEnvs[type] -> print();
	  cin >> sever;
	  oldType = type;
        }
*/
        double durEnvVal = durEnvs[type]->getValue(durNum, matr[type][attNum].size());
/*
  	cout << "Matrix::setDurations - envelope for type " << type << endl;
 	cout << "  attackNum=" << attNum << " durNum=" << durNum << " durEnvVal="
  	     << durEnvVal << endl;
*/
        matr[type][attNum][durNum].dur = durTimes[durNum];

        durEnd = matr[type][attNum][durNum].stime + matr[type][attNum][durNum].dur;
        allowed = 0;

        if (durEnd <= maxStartTime) {

          // set prob to 0 if it goes out of the window of the parent event
          start = attNum;
          while (matr[type][start][durNum].stime <= durEnd) {

            if ( durEnd == matr[type][start][durNum].stime ) allowed = 1;
  /*
            cout << "Matrix::setDurations - matr[" << type << "][" 
	     << attNum << "][" << durNum << "].stime=" << 
	     matr[type][attNum][durNum].stime << "  matr[" << type << "][" << 
             attNum  << "][" << durNum << "].dur=" << matr[type][attNum][durNum].dur
	     << endl;
	    cout << " durEnd=" << durEnd << " matr[" << type << "][" << start 
	     << "][" << durNum << "].stime="
	     << matr[type][start][durNum].stime << " allowed=" << allowed << endl;
*/
            start++;
            if (start >= matr[type].size() ) {
//            cout << "Matrix::setDurations - start= " << start << " >" << " matr["
//		<< type << "].size-1=" << matr[type].size() - 1 << endl;
              break;
            }
          }
        }

        if( allowed == 0 || durEnd > maxStartTime ) {
          matr[type][attNum][durNum].attdurprob = 0;
        } else {
          matr[type][attNum][durNum].attdurprob *= (durSieveVal * durEnvVal);
/*
          cout << "  stime=" << matr[type][attNum][durNum].stime << " dur=" <<
	   matr[type][attNum][durNum].dur << endl;
	  cout << "    durSieveVal=" << durSieveVal << " durEnvVal=" 
	   << durEnvVal << " matr[" << type << "][" << attNum << "][" << durNum 
	   << "].attdurprob=" << matr[type][attNum][durNum].attdurprob << endl;
*/
        }
      }
//    cout << " " << endl;
    }
  }
}

void Matrix::setTypeProbs(vector<double> typeProbVect) {
  typeProb = typeProbVect;
}


MatPoint Matrix::chooseM(int remain) {
  bool found = false;
  MatPoint chosenPt;
  MatPoint prevPt;
  double randNum = Random::Rand();

  // normalize the matrix (probs will add up to 1.0)
  bool success = normalizeMatrix();
  if (!success) {
    // failed to normalize -- out of space in the matrix
    MatPoint failed;
    failed.type = -1;
    return failed;
  }


  // find the nearest prob to that random in the matrix
  for (int type = 0; !found && type < matr.size(); type++) {
    // do for each type

    for (int attNum = 0; !found && attNum < matr[type].size(); attNum++) {
      // do for each attack

      for (int durNum = 0; !found && durNum < matr[type][attNum].size(); durNum++) {
        // do for each dur

        // look for the closest prob, greater than the rand num
        if (matr[type][attNum][durNum].normprob >= randNum) {
          found = true;
          chosenPt = matr[type][attNum][durNum];

          //printing stuff
/*
          cout << endl << "MATRIX ---- PICKED: matr[" << type << "][" 
              << attNum << "][" << durNum << "]  =  start:" 
              << chosenPt.stime << ", dur:" << chosenPt.dur << ", type:" 
              << chosenPt.type << ", %prob:" << (chosenPt.normprob-prevPt.normprob) << endl;
          printMatrix(true);
*/
        }
        prevPt = matr[type][attNum][durNum];
      }
    }
  }

  //printMatrix(false);
  //cout << endl << "removing conflicts!" << endl;
  // remove conflicts in the matrix (set probs to 0)
  removeConflicts(chosenPt);
  //printMatrix(false);

  // recompute the type prob vector
  recomputeTypeProbs(chosenPt.type, remain);

  return chosenPt;
}


bool Matrix::normalizeMatrix() {
  double matrSum = 0;
  double lastSum = 0;
  // get the sum of the matrix
  for (int type = 0; type < matr.size(); type++) {
    // do for each type

    for (int attNum = 0; attNum < matr[type].size(); attNum++) {
      // do for each attack

      for (int durNum = 0; durNum < matr[type][attNum].size(); durNum++) {
        // do for each dur

        matr[type][attNum][durNum].normprob = 
                matr[type][attNum][durNum].attdurprob * typeProb[type];
        matrSum += matr[type][attNum][durNum].normprob;
/*
	if(type == 0) {
	  cout << "Matrix::normalizeMatrix - typeProb[" << type << "]=" 
		<< typeProb[type] << endl;
	  cout << " matr[" << type << "][" << attNum << "]{" << durNum 
		<< "].normprob= " << matr[type][attNum][durNum].normprob << endl;
	  cout << "      matrSum=" << matrSum << endl;
	}
*/
      }
//cout << "     " << endl;
    }
  }

  if (matrSum == 0) {
    cerr << "MATRIX - ERROR: Sum of matrix is 0! (We're out of space)." << endl;
    return false; // indicate failure
  }


  // set the matrix probs to add up to 1
  for (int type = 0; type < matr.size(); type++) {
    // do for each type

    for (int attNum = 0; attNum < matr[type].size(); attNum++) {
      // do for each attack

      for (int durNum = 0; durNum < matr[type][attNum].size(); durNum++) {
        // do for each dur

        lastSum += (matr[type][attNum][durNum].normprob) / matrSum;
        matr[type][attNum][durNum].normprob = lastSum;
/*
	if(type == 0) {
	  cout << "		lastSum=" << lastSum << endl;
	}
*/
      }
//out << "    " << endl;
    }
//cin >> sever;
  }

  return true; // success!
}


void Matrix::recomputeTypeProbs(int chosenType, int remaining) {
  if (remaining != 0) {
    for (int i = 0; i < typeProb.size(); i++) {
      if (i == chosenType) {
        typeProb[i] = round(typeProb[i] * (remaining + 1) - 1) / remaining; 
      } else {
        typeProb[i] = (typeProb[i] * (remaining + 1)) / remaining;
      }
    }
  }
}


void Matrix::removeConflicts(MatPoint chosenPt) {
  int chosenStart = chosenPt.stime;
  int chosenEnd = chosenStart + chosenPt.dur;
  int chosenLayer = typeLayers[chosenPt.type];

  for (int type = 0; type < matr.size(); type++) {
    // only remove conflicts if this type is in the same layer
    //   as the chosen type
    if ( chosenLayer == typeLayers[type] ) {
  
      for (int attNum = 0; attNum < matr[type].size(); attNum++) {
        // do for each attack
  
        for (int durNum = 0; durNum < matr[type][attNum].size(); durNum++) {
          // do for each dur
  
          int currStart = matr[type][attNum][durNum].stime;
          int currEnd = currStart + matr[type][attNum][durNum].dur;
  
          if (chosenStart >= currStart && chosenStart < currEnd) {
            // start time is in this window --- set prob to 0
            matr[type][attNum][durNum].attdurprob = 0;
          } else if ( chosenEnd > currStart && chosenEnd <= currEnd) {
            // start time is in this window --- set prob to 0
            matr[type][attNum][durNum].attdurprob = 0;
          } else if ( chosenStart <= currStart && chosenEnd >= currEnd) {
            // start time is in this window --- set prob to 0
            matr[type][attNum][durNum].attdurprob = 0;
          }
        }
      }
    }
  }
}


//----------------------------------------------------------------------------//

void Matrix::printMatrix(bool normalized) {
  if (normalized) {
    cout << "$$$$$$$$$$$$$$$$$    NORMALIZED MATRIX      " 
         << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
  } else {
    cout << "&&&&&&&&&&&&&&&&&    UN-NORMALIZED MATRIX   " 
         << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
  }

  for (int type = 0; type < matr.size(); type++) {
    printf("Type %-2i", type);

    //print out a header row of possible start times
    for (int attNum = 0; attNum < matr[type].size(); attNum++) {
      printf("%7i  ", matr[type][attNum][0].stime);
    }
    cout << endl;

    // print the left column as a possible dur value, followed by probs
    for (int durNum = 0; durNum < matr[type][0].size(); durNum++) {
      printf("%5i  ", matr[type][0][durNum].dur);

      // print the probs
      for (int attNum = 0; attNum < matr[type].size(); attNum++) {
        if (normalized) {
          printf("%6.5f  ", matr[type][attNum][durNum].normprob);
        } else {
          printf("%6.5f  ", matr[type][attNum][durNum].attdurprob);
        }
      }
      cout << endl;
    }

  }

  if (normalized) {
    cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" 
         << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
  } else {
    cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" 
         << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
  }


}



