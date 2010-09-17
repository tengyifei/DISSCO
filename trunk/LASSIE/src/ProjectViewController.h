/*******************************************************************************
 *
 *  File Name     : ProjectViewController.h
 *  Date created  : Jan. 27 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file contains the class "Project" of LASSIE. Each 
 *                  Project object holds the information of the project such as 
 *                  name, structure, etc.
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

#ifndef PROJECT_VIEW_CONTROLLER_H
#define PROJECT_VIEW_CONTROLLER_H

#include "LASSIE.h"

#include "IEvent.h"

//Forward declarations

class EventAttributesViewController;
class PaletteViewController;
class EnvelopeLibraryEntry;
class MainWindow;
class SharedPointers;

class Envelope;



class ProjectViewController: public Gtk::VBox  {
public:
  
  //==========================================================================
  //============================Functions=====================================
  //==========================================================================
  
  /*! \brief The constructor of ProjectViewController
   *
   ***************************************************************************/
  ProjectViewController(MainWindow* _mainWindow);
  ProjectViewController(std::string _pathAndName, MainWindow* _mainWindow);
  ProjectViewController(std::string _pathAndName, MainWindow* _mainWindow,
  	int _calledByOpenProject);// the last arg is meaningless. openProject calls this function, add it just to make the signature different. 
  	
  
  /*! \brief The destructor of ProjectViewController
   *
   ***************************************************************************/
  ~ProjectViewController();
  
  
  EnvelopeLibraryEntry* createNewEnvelope();
  void showContents();
  void hideContents();
  void insertObject();
  void cleanUpContents();
  std::string getPathAndName();
  void showAttributes(IEvent* _event);
  void refreshObjectNameInPalette(IEvent* _event);
  void setProperties();
  void save();

  PaletteViewController* getPalette();
  EnvelopeLibraryEntry* getEnvelopeLibraryEntries();
  
  IEvent* getEventByTypeAndName( EventType type, std::string _name);
  
  
  ///////////////////////////////////drag and drop targets
  std::list<Gtk::TargetEntry> listTargets;
  
  //////////////////////////////////////////////////////////////////
  
private:
  
  
  //==========================================================================
  //=============================Fields=======================================
  //==========================================================================
  
  /*! \brief title of the project
   *
   ***************************************************************************/
  std::string pathAndName;
  
  
  /*! \brief pointer to eventAttributesView
   *
   ***************************************************************************/
  EventAttributesViewController* eventAttributesView;
  
  /*! \brief pointer to paletteView
   *
   ***************************************************************************/
  PaletteViewController* paletteView;
  
  /*! \brief pointer to leftTwoPlusAttributes
   *
   *          leftTwoPlusAttributes is a Paned widget containing another
   *          Paned widget (paletteAndTree) on the left and the attributes
   *          frame on it's right.
   *
   ***************************************************************************/
  Gtk::HPaned leftTwoPlusAttributes;
  

  
  IEvent* top;
  bool modifiedButNotSaved;
  std::vector <IEvent*> events;
  EnvelopeLibraryEntry* envelopeLibraryEntries; //this thing is a double-linked list
  Gtk::Dialog* newObjectDialog;
  void newObjectButtonClicked();
  
  Gtk::Dialog* projectPropertiesDialog;
  void projectPropertiesDialogButtonClicked();
  void saveEnvelopeLibrary();
  void refreshProjectDotDat();
  EnvelopeLibraryEntry* convertToLASSIEEnvLibEntry( Envelope* _envelope, int index);

  std::string topEvent;
  std::string projectTitle;
  std::string fileFlag;
  std::string duration;
  std::string numOfChannels;
  std::string sampleRate;
  std::string sampleSize;
  std::string numOfThreads;
  bool synthesis;
  
  SharedPointers* sharedPointers;
  
};

#endif //PROJECT_VIEW_CONTROLLER_H
