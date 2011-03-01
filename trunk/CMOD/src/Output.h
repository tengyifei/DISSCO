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

#ifndef OUTPUT_H
#define OUTPUT_H

#include "Libraries.h"

struct OutputNode
{
  string nodeName;
  
  vector<string> propertyNames;
  vector<string> propertyValues;
  vector<string> propertyUnits;
  
  vector<OutputNode*> subNodes;
  
  OutputNode(string name);
  ~OutputNode();
  void addProperty(string name, string value, string units);
  static string findAndReplace(string in, string needle, string replace);
  static string sanitize(string name);
  string getXML(void);
};

///The static output class.
class Output
{
  static OutputNode* top;
  
  static ofstream *particelFile;
  static int level;
  
  static void writeLineToParticel(string line);
  
  protected:
  static string getLevelIndentation(bool isProperty, bool isEndLevel);
  static string getPropertyIndentation(void);
 
  public:
  static void initialize(string particelFilename);
  static void free(void);
  static OutputNode* getCurrentLevelNode(void);
  static void beginSubLevel(string name);
  static void addProperty(string name, string value, string units="");
  template <class T>
    static void addProperty(string name, T value, string units="") {
    stringstream oss; oss.setf(ios::fixed, ios::floatfield); oss.precision(2);
    oss << value; addProperty(name, string(oss.str()), units);
  }
  static void endSubLevel(void);
  static void exportToXML(string filename);
};

#endif
