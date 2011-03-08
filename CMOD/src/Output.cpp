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

OutputNode::OutputNode(string name) : nodeName(name) {
}

OutputNode::~OutputNode()
{
  for(int i = 0; i < (int)subNodes.size(); i++)
    delete subNodes[i];
  propertyNames.clear();
  propertyValues.clear();
  propertyUnits.clear();
  subNodes.clear();
}

bool OutputNode::isBottom(void) {
  return (nodeName[0] == 'B' && nodeName[1] == '/');
}

bool OutputNode::isNote(void) {
  return (nodeName == "Note");
}

bool OutputNode::isBuildPhase(void) {
  string n = nodeName;
  return (n == "Sweep" || n == "Continuum" || n == "Discrete");
}

void OutputNode::addProperty(string name, string value, string units)
{
  propertyNames.push_back(name);
  propertyValues.push_back(value);
  propertyUnits.push_back(units);
}

string OutputNode::getProperty(string name) {
  for(unsigned int i = 0; i < propertyNames.size(); i++)
    if(sanitize(propertyNames[i]) == sanitize(name))
      return propertyValues[i];
  return "";
}

string OutputNode::getXML(void) {
  string s;
  s += "<";
  s += sanitize(nodeName);
  s += ">\n";
  
  for(int i = 0; i < propertyNames.size(); i++) {
    s += "<";
    s += sanitize(propertyNames[i]);
    s += ">";
    
    s += propertyValues[i];
    
    s += "</";
    s += sanitize(propertyNames[i]);
    s += ">\n";      
  }
  
  for(int i = 0; i < subNodes.size(); i++) {
    if(subNodes[i]->isBuildPhase())
      continue;
    s += subNodes[i]->getXML();     
  }
  
  s += "</";
  s += sanitize(nodeName);
  s += ">\n";
  return s;
}

void OutputNode::getFOMUS(vector<Tempo>& tempos, vector<string>& fomusdata) {  
  if(!isBottom()) {
    for(int i = 0; i < subNodes.size(); i++) {
      if(subNodes[i]->isBuildPhase())
        continue;
      subNodes[i]->getFOMUS(tempos, fomusdata);     
    }
    return;
  }
  else {
    Tempo t;
    t.setStartTime(atof(getProperty("TempoStartTime").c_str()));
    t.setTimeSignature(getProperty("TimeSignature"));
    t.setEDUPerTimeSignatureBeat(getProperty("Divisions"));
    string s;
    s.append(getProperty("TempoBeat"));
    s.append("=");
    s.append(getProperty("Tempo"));
    t.setTempo(s);

    string n;
    for(unsigned int i = 0; i < subNodes.size(); i++) {
      if(!subNodes[i]->isNote())
        continue;
      Ratio div = t.getEDUPerTimeSignatureBeat();
      Ratio start = subNodes[i]->getProperty("EDUStartTime");
      Ratio dur = subNodes[i]->getProperty("EDUDuration");
      Ratio pitch = subNodes[i]->getProperty("PitchNumber");
      pitch += Ratio(12, 1);
      n.append("time ");
      n.append((start / div).toString());
      n.append(" dur ");
      n.append((dur / div).toString());
      n.append(" pitch ");
      n.append(pitch.toPrettyString());
      n.append(";\n");
    }
    tempos.push_back(t);
    fomusdata.push_back(n);
  }
}

string OutputNode::findAndReplace(string in, string needle, string replace) {
  while(in.find(needle) != string::npos)
    in.replace(in.find(needle), needle.length(), replace);
  return in;
}

string OutputNode::sanitize(string name) {
  name = findAndReplace(name, " ", "");
  name = findAndReplace(name, "/", "_");
  return name;
}

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
  ofstream* xmlFile;
  if(filename == "")
    return;
  xmlFile = new ofstream();
  xmlFile->open(filename.c_str());
  *xmlFile << top->getXML();
}

void Output::exportToFOMUS(string filenamePrefix) {
  if(filenamePrefix == "")
    return;
  vector<Tempo> tempos;
  vector<string> fomusData;
  top->getFOMUS(tempos, fomusData);
  while(tempos.size() > 0) {
    stringstream ss_stem;
    float ttime = tempos[0].getStartTime();
    ss_stem << filenamePrefix << ttime << "s";
    string fmsFile = ss_stem.str() + ".fms";
    string lyFile = ss_stem.str() + ".ly";
    string pdfFile = ss_stem.str() + ".pdf";
    string svgFile = ss_stem.str() + ".svg";
    
    string currentFOMUSData = fomusData[0];
    /*For all of the FOMUS data that has equivalent tempo, merge them to the
    same FOMUS file.*/
    Tempo t = tempos[0];
    for(unsigned int i = 1; i < tempos.size(); i++) {
      if(tempos[0].isTempoSameAs(tempos[i])) {
        if(fomusData[i].size() > 0) {
          currentFOMUSData.append("/////////////////////////////////////\n");
          currentFOMUSData.append(fomusData[i]);
        }
        tempos.erase(tempos.begin() + i);
        fomusData.erase(fomusData.begin() + i);
        i--;
      }
    }
    tempos.erase(tempos.begin());
    fomusData.erase(fomusData.begin());
    
    if(currentFOMUSData.size() > 0) {
      string FOMUSHeader;
      stringstream tempoTime;
      int min = (int)(ttime / 60.f);
      float sec = (ttime - min * 60.f);
      
      tempoTime << t.getTempoBeat().toPrettyString() << "=" <<
        t.getTempoBeatsPerMinute().toPrettyString();
      tempoTime << " at " << min << ":";
      if(sec < 10.f)
        tempoTime << "0";
      tempoTime << sec;
      
      Ratio r = t.getTimeSignature();
      FOMUSHeader.append("//Header\n");
      FOMUSHeader.append("title \"");
      FOMUSHeader.append(tempoTime.str());
      FOMUSHeader.append("\"\n");
      FOMUSHeader.append("time 0 |timesig ( ");
      Ratio rn = r.Num();
      Ratio rd = r.Den();
      FOMUSHeader.append(rn.toPrettyString());
      FOMUSHeader.append(" ");
      FOMUSHeader.append(rd.toPrettyString());
      FOMUSHeader.append(" )|\n");
      FOMUSHeader.append("\n//Notes\n");
      
      {
      ofstream fomusFile;
      fomusFile.open(fmsFile.c_str());
      fomusFile << FOMUSHeader;
      fomusFile << currentFOMUSData;
      }
      
      {
      string fomusToLilypond = "fomus -i ";
      fomusToLilypond += fmsFile;
      fomusToLilypond += " -o ";
      fomusToLilypond += lyFile;
      system(fomusToLilypond.c_str());
      }
      
      {
      string pdfToSVG = "pdf2svg ";
      pdfToSVG += pdfFile;
      pdfToSVG += " ";
      pdfToSVG += svgFile;
      system(pdfToSVG.c_str());
      }
      /*
      {
        string firsvg = "firefox ";
        firsvg.append(svgFile);
        firsvg.append(" &");
        system(firsvg.c_str());
      }
      */
    }
  }
}

