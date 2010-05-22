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
//   matrix.h
// 
//   Used to build a two-dimenssional matrix (representing probabilities
//   of occurrence) from input data and to modify it after each choice
//   (decision).
// 
//----------------------------------------------------------------------------//

#ifndef MATRIX_H
#define MATRIX_H

// CMOD includes
#include "define.h"
#include "sieve.h"
#include "../../LASS/src/lib.h"

#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>

struct MatPoint {
  double attdurprob; // probability of attack and duration
  double normprob; // normalized prob (with type taken into account)
  int type;
  int stime;
  int dur;
};

class Matrix {

  private:
    vector< vector< vector<MatPoint> > > matr;
    vector<int> typeLayers;
    vector<double> typeProb;

  public:

    /**
    *  Constructor.
    *  \param 
    **/
    Matrix(int numTypes, int numAttacks, int numDurations, 
           double init, vector<int> numTypesInLayers);

    /**
    *  Matrix Copy Constructor
    **/
    Matrix(const Matrix &orig);

    /**
     *   Matrix Assignment Operator
     **/
    Matrix& operator=(const Matrix& orig);

    /**
    *  Destructor.
    **/
    ~Matrix();

    /**
     *  
    **/
    void setAttacks(Sieve* attackSieve, vector<Envelope*> attackEnvs);

    /**
     *  
     **/
    void setDurations(Sieve* durSieve, vector<Envelope*> durEnvs);

    /**
     *  
     **/
    void setTypeProbs(vector<double> typeProbVect);

    /**
     *  
     **/
    MatPoint chooseM(int remain);

   /**
     *  Print.  Prints all the elements of the matrix.  
    **/
    void printMatrix(bool normalized);

  private:
    bool normalizeMatrix();
    void recomputeTypeProbs(int chosenType, int remaining);
    void removeConflicts(MatPoint chosenPt);

    /**
    *  Gets the value of the first subscript of the matrix
    *  \return x row number
    **/
//    int GetX();


    /**
    *  Gets the value of the second subscript of the matrix
    *  \return y column number
    **/
//    int GetY();

    /**
     *  Assign a vector - one value for each type or row of the matrix
     *  \param newVector read from a file somewhere else
     **/
//    void SetVector(vector<float> newVector);

    /**
    *  Loads e vector of nvelopes and finds the values corresponding to each 
    *  sieve location (attack point).
    *  NB. we deal here with sieve locations and not with actual time values.
    *    ====  first version does not need "probs" not used inside  ====
    *    ====  third version: all this should take place somewhere else  ====
    *    ====  no need to overload  ==== 
    *  \param envList list of envelopes to be used
    **/
//    void Envelopes(vector<Envelope*> envList);
/*
    void Envelopes(vector<float> probs, vector<Envelope*> envList)
    void Envelopes(vector<Collection<xy_point> > xyCollection, 
                   vector<vector<string> > segmentTypes, 
                   vector<vector<string> > segmentFixed);
*/

    /**
    *  Multiplies the elements of an existing matrix with those of a vector 
    *  of doubles.  Each matrix line is multiplied by the same vector elemet, 
    *  one vector element per line.
    **/ 
//    void IncludeVector();

    /**
    *  Multiplies the elements of an existing matrix with those of an 
    *  array of doubles.  Each matrix line between a lower limit (from)
    *  and an upper limit (to) is multiplied by the same array, each 
    *  element of the array with an element of the matrix line.
    *  \param array of probabilities for each matrix column
    *  \param from lower limit for rows
    *  \param to upper limit for rows
    **/
//    void IncludeArray(double array[], int from, int to);
//    void IncludeArray(vector<double> array, int from, int to);

    /**
    *  Takes a two-dimenssional array of doubles, divides
    *   each element by the sum of all elements, and adds the value to the
    *   preceding value (sum of values) so that the last element is always 1.
    **/
//    void Normalize();

    /**
    * Chooses an element out of the probability matrix, based on each
    * element's associated probability and a randomly generated number
    **/
//    void ChooseM();

    /**
    * Same as ChooseM but returns the values directly to the
    * caller through the reference params r and c.
    *  \param r row
    *  \param c column
    **/
//    void ChooseM(int &r, int &c);

    /**
    *  Blocks the locations covered by the duration (durMatrix).  The edges 
    *  are smoothed following a user define slope.  Rows < from and 
    *  rows > to are assigned 0s.
    *  \param type of event corresponding to a row
    *  \param stimeMatrix column of the Matrix corresponding to the attack
    *  \param endLocM last Matrix location covered by this duration
    *  \param slope smoothing curve user defined
    *  \param from fist row to which this is applied
    *  \param to last row to which this is applied
    **/
//    void AdjustMatrix(int type, int stimeMatrix, int endLocM, int slope, 
//                      int from, int to);

    /**
    *  If remainO is -1, sets each element of the vector (a row number) in the 
    *  range specified by from and to to 0.  
    *  If remainO is 0, sets the vector element (a row number) specified by 
    *  type to 0.
    *  Otherwise, decreases the vector element specified by type by
    *  1/(remain0 + 1) for each element in the matrix row specified by type 
    *  that is greater than 0.  
    *  Finally, normalizes the vector element range specified by from and to.
    *  \param type a Matrix row corresponding to an (sub)object type
    *  \param numObjs the total number of children
    *  \param newObj the new objects number
    **/
//    void AdjustVector(int type, int numObjs, int newObj);

    /**
    *   Trims the Matrix by assigning 0s to locations outside the interval
    *   occupied by the assigned duration for the current type (row) and to
    *   all other rows (corresponding to all other types).
    *   NB. This function is used only for the duration matrix and performs
    *   similar tasks as performed on the s0 matrix for the type/attacks.
    *   \param type Matrix row corresponding to a type of event
    *   \param density density of this group of objects/events
    *   \param remain0 number of (sub)objects that still need to be created
    *   \param durLoc a location in the durArray (used for testing the end)
    **/
//    void TrimMatrix(int type, float density, int remain0, int durLoc);

    /**
     *  Finds the most desirable duration given the density and the number
     *  of unassigned yet objects: the number of all possible entry points is
     *  multiplied by the density to find out the total amount of activity or
     *  durMatrix of all events; this is then divided by the number of
     *  remaining, unassigned objects.
     *  Creates an array centered around this "peak".  The peak points to the
     *  most desirable durMatrix in this context.  An array is built through
     *  interpolation with values from 0 to 1 (at the peak) - weights for
     *  possible durMatrix.  It also makes sure that the endof the duration
     *  corresponds to a Matrix location.
     *  Finally, IncludeArray is called and  the current Matrix row of the
     *  existing duration matrix (which corresponds to the type already chosen)
     *  is multiplied by the array of weights corresponding to each column .
     *  \param remain0 number of (sub)objects not yet created by this event
     *  \param density of the current event
     *  \param type of the current event
     *  \param durLoc a location of the durration array
     *  \param durArray array of possible durations
     *  \param timeMatrix tart time (attack) in Matrix locations
     *  \param starTarray array of possible stimes.
     **/
//    void Mult(int remain0, float density, int type, int durLoc,
//              vector<int> durArray, int stimeMatrix, vector<int> startArray);

    /**
     *  PrintVectors.  Prints a vector/array on a 8 column format.
     **/
//    void PrintVector();

//----------------------------------------------------------------------------//

  private:

    /**
    *   Deletes the matrix and sets its dimensions to 0.
    **/
//    void Clear();

    /**
    *  Sets the dimensions of the matrix, vector, and the probability array.
    *  \param x number of rows
    *  \param y number of columns
    **/
//    void SetDim(int x, int y);

    /**
    *  Initialize the matrix with the same value (usually 0)
    *  \param init value to initialize the Matrix
    **/
//    void Init(double init);

    /**
    *  Given the values for a few points fills in an array by intrepolating.
    *  \param arraySize the size of the array
    *  \param array an array
    **/
//    void Interpol(int arraySize, double array[]);

    /**
    *  If flag = 1 smooth the the matrix locations leading up to
    *  or trailing away from a sound according to the user defined slope.
    *  \param line Matrix row corresponding to an object type
    *  \param slope defined by the user
    *  \param start beginning of the interval on which the slope is defined
    *  \parama finish end of the interval on which the slope is defined
    *  \param flag shows if the slope precedes or succedes the sound.
    **/
//    void Smooth(int line, int slope, int start, int finish, int flag);

//----------------------------------------------------------------------------//


};

#endif
