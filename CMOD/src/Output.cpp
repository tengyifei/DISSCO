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

#include "Output.h"

OutputNode* Output::top;
ofstream* Output::particelFile;
int Output::level;

void Output::writeLineToParticel(string line) {
  if(!particelFile) return;
  *particelFile << line << endl;
}

string Output::getLevelIndentation(bool isProperty, bool isEndLevel) {
  string indentation;
  for(int i = 0; i < level; i++)
    if(isProperty || isEndLevel || i < level - 1)
      indentation = indentation + "| ";
    else
      indentation = indentation + "+-";
  return indentation;
}
  
string Output::getPropertyIndentation(void) {
  return getLevelIndentation(true, false) + ". ";
} 

void Output::initialize(string particelFilename) {
  top = 0;
  particelFile = 0;
  level = -1;
  
  if(particelFilename != "") {
    particelFile = new ofstream();
    particelFile->open(particelFilename.c_str());
  }
}

void Output::free(void)
{
  delete top;
  delete particelFile;
}

OutputNode* Output::getCurrentLevelNode(void) {
  if(!top)
    return 0;
    
  OutputNode* currentNode = top;
  for(int i = 1; i <= level; i++)
    currentNode = currentNode->subNodes.back();
  return currentNode;
}

void Output::beginSubLevel(string name) {
  if(!top)
    top = new OutputNode(name);
  else
    getCurrentLevelNode()->subNodes.push_back(new OutputNode(name));
  level++;
  
  //Immediately write level to particel.
  writeLineToParticel(getLevelIndentation(false, false) + 
    getCurrentLevelNode()->nodeName);
}

void Output::addProperty(string name, string value, string units) {
  OutputNode* current = getCurrentLevelNode();
  if(!current)
    cerr << "Warning: Top level does not exist. Property can not be added."
      << endl;
  else
    current->addProperty(name, value, units);
  
  //Immediately write property to particel.
  string stringToWrite = getPropertyIndentation() + name + ": " + value;
  if(units != "")
    stringToWrite = stringToWrite + " " + units;
  writeLineToParticel(stringToWrite);
}

void Output::endSubLevel(void) {
  //Before closing level immediately write to particel.
  writeLineToParticel(getLevelIndentation(false, true) + "End " + 
    getCurrentLevelNode()->nodeName);
  level--;
}

void Output::exportToXML(string filename) {
  filename = "";
}

