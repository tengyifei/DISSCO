/*******************************************************************************
 *
 *  File Name     : EnvelopeLibraryEntry.cpp
 *  Date created  : May.17 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file contains the class "EnvelopeLibraryEntry" of 
 *                  LASSIE. Each EnvelopeLibraryEntry holds the information of
 *                  a premade envelope in the envelope library. It is also a 
 *                  doubly-linked list.                  
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
 
 #include "EnvelopeLibraryEntry.h"
//#include <stdlib.h>

EnvelopeLibraryEntry::EnvelopeLibraryEntry(int _number){
  number =_number;
  prev = NULL;
  next = NULL;
  yStart = 0;
  segments = new EnvLibEntrySegment();
  segments->prev = NULL;
  segments->next = NULL;
  segments->xStart = 0;
  segments->xDuration = 1;
  segments->y = 0; 
  segments->segmentType = envSegmentTypeLinear;
  segments->segmentProperty = envSegmentPropertyFixed;
  
}
EnvelopeLibraryEntry::~EnvelopeLibraryEntry(){//delete segments!}
}



void EnvelopeLibraryEntry::print(){
  //std::cout<<"number:"<< number<<std::endl;
  
  
  if (next != NULL) next->print();
  

}

int EnvelopeLibraryEntry::count(){
  if (next == NULL) return 1;
  else return next->count() + 1;
}


EnvelopeLibraryEntry* EnvelopeLibraryEntry::createNewEnvelope(){
  if (next != NULL) return next->createNewEnvelope();
  else {
    next = new EnvelopeLibraryEntry (number + 1);
    next->prev = this;
    return next;
  }
}


Glib::ustring EnvelopeLibraryEntry::getNumberString(){
  std::string temp;
  char tem1[20] ;
  sprintf(tem1,"%d",number); 
  
  temp = tem1;
  Glib::ustring ret = temp;
  return ret;


}




EnvelopeLibraryEntry::EnvelopeLibraryEntry(Envelope* _envelope,int _number){
  number =_number;
  prev = NULL;
  next = NULL;
  
  Collection<envelope_segment>* lassSegments = _envelope->getSegments();
  
  int size = lassSegments->size();
  
  
  //special case, if there are only two nodes in this envelope
  if (size ==2){
    envelope_segment thisLassSegment = lassSegments->get(0);
    yStart = thisLassSegment.y;
    segments = new EnvLibEntrySegment();
    segments->prev = NULL;
    segments->next = NULL;
    segments->xStart = 0;
       
    if (thisLassSegment.lengthType == FIXED){
      segments->segmentProperty = envSegmentPropertyFixed;
    }
    else {
      segments->segmentProperty = envSegmentPropertyFlexible;
    }
        
    if (thisLassSegment.interType ==EXPONENTIAL){
      segments->segmentType = envSegmentTypeExponential;
    }
    else if(thisLassSegment.interType == CUBIC_SPLINE){
      segments->segmentType = envSegmentTypeSpline;
    }
    else {
      segments->segmentType = envSegmentTypeLinear;
    }
    thisLassSegment = lassSegments->get(1);
    segments->xDuration = 1.0;
    segments->y = thisLassSegment.y;   
  
  }

  else{
    EnvLibEntrySegment* previousSegment = NULL;
    EnvLibEntrySegment* currentSegment = NULL;
  
  
  
    for (int i = 0; i < size -1; i ++){  //-1 because the last LASSseg has no segmentType and segProperty, treat it outside of the loop
      envelope_segment thisLassSegment = lassSegments->get(i);
      if (i ==0){ // the first segment
        yStart = thisLassSegment.y;
        segments = new EnvLibEntrySegment();
        segments->prev = NULL;
        segments->next = NULL;
        segments->xStart = 0;
        
        if (thisLassSegment.lengthType == FIXED){
          segments->segmentProperty = envSegmentPropertyFixed;
        }
        else {
          segments->segmentProperty = envSegmentPropertyFlexible;
        }
        
        if (thisLassSegment.interType ==EXPONENTIAL){
          segments->segmentType = envSegmentTypeExponential;
        }
        else if(thisLassSegment.interType == CUBIC_SPLINE){
          segments->segmentType = envSegmentTypeSpline;
        }
        else {
          segments->segmentType = envSegmentTypeLinear;
        }
        
        previousSegment = segments;
      }
      //not the first segment
      else{
        currentSegment = new EnvLibEntrySegment();
        currentSegment->prev = previousSegment;
        previousSegment->next = currentSegment;    
        currentSegment->xStart = thisLassSegment.x;
        previousSegment->y = thisLassSegment.y;
        previousSegment->xDuration = currentSegment->xStart - previousSegment->xStart;

        if (thisLassSegment.lengthType == FIXED){
          currentSegment->segmentProperty = envSegmentPropertyFixed;
        }
        else {
          currentSegment->segmentProperty = envSegmentPropertyFlexible;
        }
        
        if (thisLassSegment.interType ==EXPONENTIAL){
          currentSegment->segmentType = envSegmentTypeExponential;
        }
        else if(thisLassSegment.interType == CUBIC_SPLINE){
          currentSegment->segmentType = envSegmentTypeSpline;
        }
        else {
          currentSegment->segmentType = envSegmentTypeLinear;
        }  
        previousSegment = currentSegment;
      }
    
    }
    
    envelope_segment thisLassSegment = lassSegments->get(size -1);
    currentSegment->next = NULL;
    currentSegment->xDuration = 1.0 - currentSegment->xStart;
    currentSegment->y = thisLassSegment.y;   
  
  }
  
  
  delete lassSegments;
  
}











