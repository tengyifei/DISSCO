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
  
  head = new EnvLibEntryNode(0,0);
  head->rightSeg = new EnvLibEntrySeg();
  head->rightSeg->leftNode = head;
  head->rightSeg->rightNode = new EnvLibEntryNode(1,0);
  head->rightSeg->rightNode->leftSeg = head->rightSeg;
  

}
EnvelopeLibraryEntry::~EnvelopeLibraryEntry(){//delete segments!}
}



void EnvelopeLibraryEntry::print(){

  EnvLibEntrySeg* currentSeg = head->rightSeg;
  EnvLibEntrySeg* prevSeg = NULL;
  
  while (currentSeg != NULL){
  	cout<<"x: "<<currentSeg->leftNode->x<<", y: "<<currentSeg->leftNode->y<<", ";
  	if (currentSeg->segmentType == envSegmentTypeLinear){
  		cout<<"LINEAR, ";
  	}
  	else if (currentSeg->segmentType == envSegmentTypeSpline){
  		cout<<"SPLINE, ";
  	}
  	else {
  		cout<<"EXPONENTIAL, ";
  	}
  	
  	if (currentSeg->segmentProperty ==envSegmentPropertyFlexible){
  		cout<<"FLEXIBLE"<<endl;
  	}
  	else{
  		cout<<"FIXED"<<endl;
  	}	
  
		prevSeg = currentSeg;
		currentSeg = currentSeg->rightNode->rightSeg;

  }
  	cout<<"x: "<<prevSeg->rightNode->x<<", y: "<<prevSeg->rightNode->y<<endl;
  


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

	number = _number;
	Collection<envelope_segment>* segments = _envelope->getSegments ();
	
	EnvLibEntryNode* currentNode = NULL;
	EnvLibEntryNode* prevNode = NULL;
	EnvLibEntrySeg* currentSeg = NULL;
	EnvLibEntrySeg* prevSeg = NULL;
	
	
	
	int i = 0;
	for (i ; i < segments->size()-1; i++){
	
		prevNode = currentNode;
		prevSeg = currentSeg;
		currentNode = new EnvLibEntryNode(segments->get(i).x, segments->get(i).y);
		currentSeg = new EnvLibEntrySeg();
		if (i ==0){
			head = currentNode;
		}
		else{
			prevSeg->rightNode = currentNode;
		}
		currentNode->leftSeg = prevSeg;
		currentNode->rightSeg = currentSeg;
		currentSeg->leftNode = currentNode;
		
		if (_envelope->getSegmentLengthType(i) == FIXED){
      

      currentSeg->segmentProperty = envSegmentPropertyFixed;
    }
    else {
      
      currentSeg->segmentProperty = envSegmentPropertyFlexible;
    }
    
        
    if (_envelope->getSegmentInterpolationType(i) ==EXPONENTIAL){
      currentSeg->segmentType = envSegmentTypeExponential;
    }
    else if(_envelope->getSegmentInterpolationType(i) == CUBIC_SPLINE){
      currentSeg->segmentType = envSegmentTypeSpline;
    }
    else {
      currentSeg->segmentType = envSegmentTypeLinear;
    }
    
    //thisLassSegment = &(lassSegments->get(1));
    //head->rightSeg->rightNode->y = thisLassSegment->y;   
  
  }// end of for loop

	currentSeg->rightNode = new EnvLibEntryNode(segments->get(i).x, segments->get(i).y);
	//cout<<"test1"<<endl;
	currentSeg->rightNode->leftSeg = currentSeg;
	//cout<<"test2"<<endl;
	//cout<<_envelope->getSegmentInterpolationType(i)<<", "<<_envelope->getSegmentLengthType(i)<<endl; //right things to call
		
	

}










