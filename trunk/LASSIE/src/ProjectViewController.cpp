/*******************************************************************************
*
*  File Name     : ProjectViewController.cpp
*  Date created  : Jan. 27 2010
*  Authors       : Ming-ching Chiu, Sever Tipei
*  Organization  : Music School, University of Illinois at Urbana Champaign
*  Description   : This file contains the implementation of class "Project".
*                  Each Project object holds the information of the project
*                  such as name, structure, etc.
*
*===============================================================================
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
*******************************************************************************/

#include "ProjectViewController.h"

//#include <stdio.h>
//#include <stdlib.h>

#include "IEvent.h"
#include "EventAttributesViewController.h"
#include "PaletteViewController.h"
#include "FileOperations.h"
#include "MainWindow.h"
#include "EnvelopeLibraryEntry.h"
#include "SharedPointers.h"

#include <dirent.h>

/*! \brief The constructor of ProjectViewController
*
*******************************************************************************/
ProjectViewController::ProjectViewController(MainWindow* _mainWindow){

  ///////////////////////////////////////////////drag and drop//////////////
  listTargets.push_back( Gtk::TargetEntry("STRING") );
  listTargets.push_back( Gtk::TargetEntry("text/plain") );

  //listTargets.push_back( Gtk::TargetEntry("EMPTY TARGET CONNECTING PALETTE AND PROJECT-TREE DRAG AND DROP") );
  //listTargets.push_back( Gtk::TargetEntry("text/plain") );
  //////////////////////////////////////////////////////////////////////////

  std::string topName = "";

  //add the Paned widget "leftTwoPlusAttributes" as a child
  add(leftTwoPlusAttributes);

  //no title when initializing the window
  pathAndName   = "";
  projectTitle  = "";
  fileFlag      = "";
  duration      = "0";
  numOfChannels = "2";
  sampleRate    = "44100";
  sampleSize    = "16";
  numOfThreads  = "1";
  topEvent      = "T0";
  synthesis     = true;

  sharedPointers = new SharedPointers();
      sharedPointers->mainWindow = _mainWindow;
  sharedPointers->projectView = this;
  //create three new children widgets


  eventAttributesView = new EventAttributesViewController(  sharedPointers );

  paletteView = new PaletteViewController(  sharedPointers );


  //set the attributes of two Paned widgets and add children view in them
  leftTwoPlusAttributes.set_position(300);
  leftTwoPlusAttributes.pack1(*paletteView,true,false);
  leftTwoPlusAttributes.pack2(*eventAttributesView,true,false);



  envelopeLibraryEntries = NULL;

  show_all_children();
}


/*! \brief The constructor of ProjectViewController
*
*******************************************************************************/
ProjectViewController::ProjectViewController(
  std::string _pathAndName, MainWindow* _mainWindow){
  

  
  
  ///////////////////////////////////////////////drag and drop//////////////
  listTargets.push_back( Gtk::TargetEntry("STRING") );
  //listTargets.push_back( Gtk::TargetEntry("text/plain") );
  //////////////////////////////////////////////////////////////////////////

  std::string topName = "T0";
  projectTitle  = FileOperations::stringToFileName(_pathAndName);
  fileFlag      = "THMLBsnv";
  duration      = "0";
  numOfChannels = "2";
  sampleRate    = "44100";
  sampleSize    = "16";
  numOfThreads  = "1";
  topEvent      = "T/T0";
  synthesis     = true;

  //add the Paned widget "leftTwoPlusAttributes" as a child
  add(leftTwoPlusAttributes);

  //no title when initializing the window
  pathAndName = _pathAndName;

  sharedPointers = new SharedPointers();
    sharedPointers->mainWindow = _mainWindow;
  sharedPointers->projectView = this;

  //create three new children widgets

  eventAttributesView = new EventAttributesViewController(sharedPointers);
  paletteView = new PaletteViewController(sharedPointers);


  //set the attributes of two Paned widgets and add children view in them
  leftTwoPlusAttributes.set_position(300);
  leftTwoPlusAttributes.pack1(*paletteView,true,false);
  leftTwoPlusAttributes.pack2(*eventAttributesView,true,false);


  showContents();
  
  
  
  
  //make a new shared pointers object so all object can find each other easily
  //without calling layers of layers of parents/ children

  


  sharedPointers->eventAttributesView = eventAttributesView;
  sharedPointers->paletteView = paletteView;
  
  
  envelopeLibraryEntries = NULL;
  
  
  
  
  
  show_all_children();
}


/*! \brief The destructor of ProjectViewController
*
*******************************************************************************/
ProjectViewController::~ProjectViewController(){
  //delete projectTreeView;
  delete eventAttributesView;
  delete paletteView;
  std::cout << "ProjectViewController destructor is called." << std::endl;
}


void ProjectViewController::cleanUpContents(){

  MainWindow* _mainWindow = sharedPointers->mainWindow;
  //delete projectTreeView;
  delete eventAttributesView;
  delete paletteView;
  delete sharedPointers;
  sharedPointers = new SharedPointers;
    sharedPointers->mainWindow = _mainWindow;
  sharedPointers->projectView = this;
  
  
  std::cout << "ProjectViewController cleanUpContents is called." << std::endl;
  //projectTreeView = new ProjectTreeViewController(sharedPointers);
  eventAttributesView = new EventAttributesViewController(sharedPointers);
  paletteView = new PaletteViewController(sharedPointers);
  
  

  //sharedPointers->projectTreeView = projectTreeView;
  sharedPointers->eventAttributesView = eventAttributesView;
  sharedPointers->paletteView = paletteView;
  
  

  leftTwoPlusAttributes.pack1(*paletteView,true,false);
  leftTwoPlusAttributes.pack2(*eventAttributesView,true,false);
  //paletteAndTree.pack1(*paletteView,true,false);
  //paletteAndTree.pack2(*projectTreeView,true,false);

  show_all_children();
}


void ProjectViewController::showContents(){
  IEvent* event;
  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Top");
  events.push_back(event);
  paletteView->insertEvent(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("High");
  events.push_back(event);
  paletteView->insertEvent(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Mid");
  events.push_back(event);
  paletteView->insertEvent(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Low");
  events.push_back(event);
  paletteView->insertEvent(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Bottom");
  events.push_back(event);
  paletteView->insertEvent(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Sound");
  events.push_back(event);
  paletteView->insertEvent(event);


  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Envelope");
  events.push_back(event);
  paletteView->insertEvent(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Sieve");
  events.push_back(event);
  paletteView->insertEvent(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Spatialization");
  events.push_back(event);
  paletteView->insertEvent(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Pattern");
  events.push_back(event);
  paletteView->insertEvent(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Reverb");
  events.push_back(event);
  paletteView->insertEvent(event);

  //event = new IEvent();
  //event->setEventType(eventTop);
  //event->setEventName("T0");
  //events.push_back(event);
  //paletteView->insertEvent(event,"Top");

  //should skip this part because one project can possibly have more than 1 top event. so let user decide.
  //projectTreeView->insertEvent(event);
}


void ProjectViewController::hideContents(){
  cleanUpContents();
}


void ProjectViewController::insertObject(){
  Glib::ustring selectedPaletteFolder = paletteView->folderSelected();
  //make sure a folder is selected in palette.
  if (selectedPaletteFolder == "None"){
    //prompt error (no row selected in the palette)
    std::cout << "the row selected is not a folder" << std::endl;
    Gtk::MessageDialog dialog(
      "Please select a folder to store the new object.",
      false /* use_markup */,
      Gtk::MESSAGE_QUESTION,
      Gtk::BUTTONS_OK);

    dialog.run();
    return;
	}

  //Load the GtkBuilder file and instantiate its widgets:
  Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();

  #ifdef GLIBMM_EXCEPTIONS_ENABLED
    try{
      refBuilder->add_from_file("./LASSIE/src/UI/FileNewObject.ui");
    }
    catch(const Glib::FileError& ex){
      std::cerr << "FileError: " << ex.what() << std::endl;
    }
    catch(const Gtk::BuilderError& ex){
      std::cerr << "BuilderError: " << ex.what() << std::endl;
    }
  #else
    std::auto_ptr<Glib::Error> error;

    if (!refBuilder->add_from_file("./LASSIE/src/UI/FileNewObject.ui", error)){
      std::cerr << error->what() << std::endl;
    }
  #endif /* !GLIBMM_EXCEPTIONS_ENABLED */

  //Get the GtkBuilder-instantiated Dialog:
  refBuilder->get_widget("newObjectDialog", newObjectDialog);

  Gtk::Button* okButton;
  refBuilder->get_widget("newObjectDialogOKButton", okButton);

  Gtk::Button* cancelButton;
  refBuilder->get_widget("newObjectDialogCancelButton", cancelButton);

  if(okButton){
    okButton->signal_clicked().connect(
      sigc::mem_fun(*this,&ProjectViewController::newObjectButtonClicked) );
    }

    if(cancelButton){
      cancelButton->signal_clicked().connect(
        sigc::mem_fun(*this,&ProjectViewController::newObjectButtonClicked) );
    }

    Gtk::Entry* nameEntry;
    refBuilder->get_widget("newObjectNameEntry", nameEntry);

  EventType type;
  std::string flagForFolderMatching;
  Gtk::RadioButton* typeButton;

  //set the defult radio button of type
  if (selectedPaletteFolder =="Top"){
    refBuilder->get_widget("buttonTop", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "High"){
    refBuilder->get_widget("buttonHigh", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Mid"){
    refBuilder->get_widget("buttonMid", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Low"){
    refBuilder->get_widget("buttonLow", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Bottom"){
    refBuilder->get_widget("buttonBottom", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Sound"){
    refBuilder->get_widget("buttonSound", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Envelope"){
    refBuilder->get_widget("buttonEnv", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Sieve"){
    refBuilder->get_widget("buttonSiv", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Spatialization"){
    refBuilder->get_widget("buttonSpa", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Pattern"){
    refBuilder->get_widget("buttonPat", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Reverb"){
    refBuilder->get_widget("buttonRev", typeButton);
    typeButton->set_active(true);
  }

  int result = newObjectDialog->run();

  while (result ==1 && nameEntry->get_text() == ""){
    //prompt error (lack of name) and then loop
    std::cout<<"User didn't name the object!"<<std::endl;
    Gtk::MessageDialog dialog(
      "Please Name the Object",
      false /* use_markup */,
      Gtk::MESSAGE_QUESTION,
      Gtk::BUTTONS_OK);

    dialog.run();
    dialog.hide();

    result = newObjectDialog->run();
  }

  if(result ==1){

    refBuilder->get_widget("buttonTop", typeButton);
    if (typeButton->get_active()) {
      type = eventTop;
      flagForFolderMatching = "Top";
    }

    refBuilder->get_widget("buttonHigh", typeButton);
    if (typeButton->get_active()) {
      type = eventHigh;
      flagForFolderMatching = "High";
    }

    refBuilder->get_widget("buttonMid", typeButton);
    if (typeButton->get_active()) {
      type = eventMid;
      flagForFolderMatching = "Mid";
    }

    refBuilder->get_widget("buttonLow", typeButton);
    if (typeButton->get_active()) {
      type = eventLow;
      flagForFolderMatching = "Low";
    }

    refBuilder->get_widget("buttonBottom", typeButton);
    if (typeButton->get_active()) {
      type = eventBottom;
      flagForFolderMatching = "Bottom";
    }

    refBuilder->get_widget("buttonSound", typeButton);
    if (typeButton->get_active()) {
      type = eventSound;
      flagForFolderMatching = "Sound";
    }

    refBuilder->get_widget("buttonEnv", typeButton);
    if (typeButton->get_active()) {
      type = eventEnv;
      flagForFolderMatching = "Envelope";
    }

    refBuilder->get_widget("buttonSiv", typeButton);
    if (typeButton->get_active()) {
      type = eventSiv;
      flagForFolderMatching = "Sieve";
    }

    refBuilder->get_widget("buttonSpa", typeButton);
    if (typeButton->get_active()) {
      type = eventSpa;
      flagForFolderMatching = "Spatialization";
    }

    refBuilder->get_widget("buttonPat", typeButton);
    if (typeButton->get_active()) {
      type = eventPat;
      flagForFolderMatching = "Pattern";
    }

    refBuilder->get_widget("buttonRev", typeButton);
    if (typeButton->get_active()) {
      type = eventRev;
      flagForFolderMatching = "Reverb";
    }

    refBuilder->get_widget("buttonFolder", typeButton);
    if (typeButton->get_active()) {
      type = eventFolder;
      flagForFolderMatching = "Folder";
    }

    int decision =2; //if mismatch, what does the user want. 0 = change folder. 1 = change type 2 = keep this way


    if (selectedPaletteFolder != flagForFolderMatching){

      //Load the GtkBuilder file and instantiate its widgets:
      Glib::RefPtr<Gtk::Builder> refBuilder1 = Gtk::Builder::create();
      #ifdef GLIBMM_EXCEPTIONS_ENABLED
        try{
          refBuilder1->add_from_file("./LASSIE/src/UI/PaletteInsertTypeMismatch.ui");
        }
        catch(const Glib::FileError& ex){
          std::cerr << "FileError: " << ex.what() << std::endl;
        }
        catch(const Gtk::BuilderError& ex){
          std::cerr << "BuilderError: " << ex.what() << std::endl;
        }
      #else
        std::auto_ptr<Glib::Error> error;
        if (!refBuilder1->add_from_file("./LASSIE/src/UI/FileNewObject.ui", error)){
          std::cerr << error->what() << std::endl;
        }
      #endif /* !GLIBMM_EXCEPTIONS_ENABLED */

      //Get the GtkBuilder-instantiated Dialog:
      Gtk::Dialog* mismatchDialog;
      std::string stringBuffer;
      refBuilder1->get_widget("MismatchDialog", mismatchDialog);

      Gtk::Label* folderName;
      refBuilder1->get_widget("folderName", folderName);
      stringBuffer =   "The folder you selected to store the object is \""
                     + selectedPaletteFolder
                     + "\".";

      folderName->set_text(stringBuffer);

      Gtk::Label* objectType;
      refBuilder1->get_widget("objectType", objectType);
      stringBuffer =   "The object you want to create has the type \""
                     + flagForFolderMatching
                     + "\".";

      objectType->set_text(stringBuffer);

      Gtk::Button* okButton;
      refBuilder1->get_widget("OKButton", okButton);

      Gtk::Button* cancelButton;
      refBuilder1->get_widget("CancelButton", cancelButton);

      Gtk::RadioButton* changeFolderButton;
      refBuilder1->get_widget("changeFolderButton", changeFolderButton);
      stringBuffer =   "Put this object in the \""
                     + flagForFolderMatching
                     + "\" folder.";

      changeFolderButton->set_label(stringBuffer);

      Gtk::RadioButton* changeTypeButton;
      refBuilder1->get_widget("changeTypeButton", changeTypeButton);
      stringBuffer =   "Change the type of the object to \""
                     + selectedPaletteFolder
                     + "\".";

      changeTypeButton->set_label(stringBuffer);

      Gtk::RadioButton* justDoItButton;
      refBuilder1->get_widget("justDoItButton", justDoItButton);

      int result = mismatchDialog->run();
      mismatchDialog->hide();

      if (changeFolderButton->get_active()){
        decision = 0;
      }
      else if (changeTypeButton->get_active()){
        decision = 1;
      }
      else{
        decision = 2;
      }
    } //end handle type mismatch

    if (decision == 0){
      //change folder
      std::cout << " mismatched object and folder is not properly implemented.";
      std::cout << " don't do this! might cause CMOD not functioning properly.";
      std::cout << " also duplicated file names are not checked here.";
      std::cout << std::endl;

      IEvent* newEvent = new IEvent();
      newEvent->setEventName(nameEntry->get_text());
      newEvent->setEventType(type);
      paletteView->insertEvent(newEvent,flagForFolderMatching);
      events.push_back(newEvent);
    }
    else if(decision ==1){
      //change type

      std::cout << " mismatched object and folder is not properly implemented.";
      std::cout << " don't do this! might cause CMOD not functioning properly.";
      std::cout << " also duplicated file names are not checked here.";
      std::cout << std::endl;
      IEvent* newEvent = new IEvent();
      newEvent->setEventName(nameEntry->get_text());

      if (selectedPaletteFolder =="Top"){
        newEvent->setEventType(eventTop);
      }
      else if (selectedPaletteFolder =="High"){
        newEvent->setEventType(eventHigh);
      }
      else if (selectedPaletteFolder =="Mid"){
        newEvent->setEventType(eventMid);
      }
      else if (selectedPaletteFolder =="Low"){
        newEvent->setEventType(eventLow);
      }
      else if (selectedPaletteFolder =="Bottom"){
        newEvent->setEventType(eventBottom);
      }
      else if (selectedPaletteFolder =="Sound"){
        newEvent->setEventType(eventSound);
      }
      else if (selectedPaletteFolder =="Envelope"){
        newEvent->setEventType(eventEnv);
      }
      else if (selectedPaletteFolder =="Sieve"){
        newEvent->setEventType(eventSiv);
      }
      else if (selectedPaletteFolder =="High"){
        newEvent->setEventType(eventHigh);
      }
      else if (selectedPaletteFolder =="Spatialization"){
        newEvent->setEventType(eventSpa);
      }
      else if (selectedPaletteFolder =="Pattern"){
        newEvent->setEventType(eventPat);
      }
      else if (selectedPaletteFolder =="Reverb"){
        newEvent->setEventType(eventRev);
      }
      paletteView->insertEvent(newEvent);
      events.push_back(newEvent);
    }
    else{
      std::string name = nameEntry->get_text();

      if (selectedPaletteFolder =="Bottom"){ // do first letter check here if bottom (must be "snv"
        std::cout<<"Should do Bottom Name checking"<<std::endl;
        if (name[0]!='s' && name[0]!='n' && name[0]!= 'v'){
          std::cout<<"complain!"<<std::endl;
        }
      }
      std::vector<IEvent*>::iterator it;
      int renameDialogFlag;
      //check object name exist.
      for(it = events.begin(); it != events.end(); it ++){
        if(   (*it)->getEventName() == nameEntry->get_text()
           && (*it)->getEventType() == type
           || nameEntry->get_text() == ""
           && renameDialogFlag == 1){

          Glib::RefPtr<Gtk::Builder> refBuilder2
          = Gtk::Builder::create();
          Gtk::Dialog* renameDialog;
          Gtk::Entry* newNameEntry;
          Gtk::Button* okButton;
          Gtk::Button* cancelButton;

          refBuilder2->add_from_file(
          "./LASSIE/src/UI/CheckObjectNameExists.ui");
          refBuilder2->get_widget("renameDialog", renameDialog);
          refBuilder2->get_widget("okButton", okButton);
          refBuilder2->get_widget("cancelButton", cancelButton);

          renameDialogFlag = renameDialog->run();
          renameDialog->hide();

          refBuilder2->get_widget("newNameEntry", newNameEntry);
          nameEntry = newNameEntry;

          it = events.begin();
        }
      }

      if(   nameEntry->get_text() != ""
         && renameDialogFlag != 0){

        IEvent* newEvent = new IEvent();
        newEvent->setEventName(nameEntry->get_text());
        newEvent->setEventType(type);
        paletteView->insertEvent(newEvent);
        events.push_back(newEvent);
      }
    }
  }

  delete newObjectDialog;
  newObjectDialog = NULL;
}


std::string ProjectViewController::getPathAndName(){
  return pathAndName;
}


void ProjectViewController::newObjectButtonClicked(){
  newObjectDialog->hide();
}


void ProjectViewController::projectPropertiesDialogButtonClicked(){
  projectPropertiesDialog->hide();
}


//called by palette View to show the attributes of selected event
void ProjectViewController::showAttributes(IEvent* _event){
  eventAttributesView->showAttributesOfEvent(_event);
}


void ProjectViewController::refreshObjectNameInPalette(IEvent* _event){
  paletteView->refreshObjectName(_event);
  //projectTreeView->refreshObjectName(_event);
}


void ProjectViewController::setProperties (){
  //Load the GtkBuilder file and instantiate its widgets:
  Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();

  #ifdef GLIBMM_EXCEPTIONS_ENABLED
    try{
      refBuilder->add_from_file("./LASSIE/src/UI/ProjectPropertiesDialog.ui");
    }
    catch(const Glib::FileError& ex){
      std::cerr << "FileError: " << ex.what() << std::endl;
    }
    catch(const Gtk::BuilderError& ex){
      std::cerr << "BuilderError: " << ex.what() << std::endl;
    }
  #else
    std::auto_ptr<Glib::Error> error;

    if (!refBuilder->add_from_file(
           "./LASSIE/src/UI/ProjectPropertiesDialog.ui", error)){

      std::cerr << error->what() << std::endl;
    }
  #endif /* !GLIBMM_EXCEPTIONS_ENABLED */

  //Get the GtkBuilder-instantiated Dialog:
  refBuilder->get_widget("projectPropertiesDialog", projectPropertiesDialog);
  //newObjectDialog->set_parent(*this);

  //Get the GtkBuilder-instantiated Button, and connect a signal handler:
  Gtk::Button* okButton;
  refBuilder->get_widget("OK", okButton);

  Gtk::Button* cancelButton;
  refBuilder->get_widget("Cancel", cancelButton);

  if (okButton){
    okButton->signal_clicked().connect(
      sigc::mem_fun(
        *this,&ProjectViewController::projectPropertiesDialogButtonClicked) );
  }

  if (cancelButton){
    cancelButton->signal_clicked().connect(
    sigc::mem_fun(
      *this,&ProjectViewController::projectPropertiesDialogButtonClicked) );
  }

  Gtk::Entry* entry;
  refBuilder->get_widget("titleEntry", entry);
  entry->set_text(projectTitle);

  refBuilder->get_widget("flagEntry", entry);
  entry->set_text(fileFlag);

  refBuilder->get_widget("durationEntry", entry);
  entry->set_text(duration);

  refBuilder->get_widget("channelEntry", entry);
  entry->set_text(numOfChannels);

  refBuilder->get_widget("sampleRateEntry", entry);
  entry->set_text(sampleRate);

  refBuilder->get_widget("sampleSizeEntry", entry);
  entry->set_text(sampleSize);

  refBuilder->get_widget("threadsEntry", entry);
  entry->set_text(numOfThreads);

  refBuilder->get_widget("topEventEntry", entry);
  entry->set_text(topEvent);

  Gtk::CheckButton* button;
  refBuilder->get_widget("synthesisCheckBox", button);

  button->set_active(synthesis);

  int result = projectPropertiesDialog->run();

  if (result ==1){
    refBuilder->get_widget("titleEntry", entry);
    projectTitle = entry->get_text();

    refBuilder->get_widget("flagEntry", entry);
    fileFlag = entry->get_text();

    refBuilder->get_widget("durationEntry", entry);
    duration = entry->get_text();

    refBuilder->get_widget("channelEntry", entry);
    numOfChannels = entry->get_text();

    refBuilder->get_widget("sampleRateEntry", entry);
    sampleRate = entry->get_text();

    refBuilder->get_widget("sampleSizeEntry", entry);
    sampleSize = entry->get_text();

    refBuilder->get_widget("threadsEntry", entry);
    numOfThreads = entry->get_text();

    refBuilder->get_widget("topEventEntry", entry);
    topEvent = entry->get_text();

    synthesis = button->get_active();
  }

  delete projectPropertiesDialog;
  projectPropertiesDialog = NULL;
  refreshProjectDotDat();

}


void ProjectViewController::refreshProjectDotDat(){
  std::string fileName =  FileOperations::stringToFileName(pathAndName);
  std::string datFileName = pathAndName
                            + "/"
                            + fileName
                            + ".dat";


  std::cout<<datFileName<<std::endl;
  std::string stringbuffer;
  FILE* dat  = fopen(datFileName.c_str(), "w");

  stringbuffer = "title = \"" + projectTitle + "\";\n";
  fputs(stringbuffer.c_str(),dat);

  stringbuffer = "fileFlags = \"" + fileFlag + "\";\n";
  fputs(stringbuffer.c_str(),dat);

  stringbuffer = "fileList = \"" + topEvent + "\";\n";
  fputs(stringbuffer.c_str(),dat);

  stringbuffer = "pieceStartTime = 0;\n";
  fputs(stringbuffer.c_str(),dat);

  stringbuffer = "pieceDuration = " + duration + ";\n\n";
  fputs(stringbuffer.c_str(),dat);

  if (synthesis){
    stringbuffer = "soundSynthesis = TRUE;\n";
  }
  else{
    stringbuffer = "soundSynthesis = FALSE;\n";
  }

  fputs(stringbuffer.c_str(),dat);

  stringbuffer = "numChannels = " + numOfChannels + ";\n";
  fputs(stringbuffer.c_str(),dat);

  stringbuffer = "sampleRate = " + sampleRate + ";\n";
  fputs(stringbuffer.c_str(),dat);

  stringbuffer = "sampleSize = " + sampleSize + ";\n";
  fputs(stringbuffer.c_str(),dat);

  stringbuffer = "numThreads = " + numOfThreads + ";\n";
  fputs(stringbuffer.c_str(),dat);

  fclose(dat);
}





void ProjectViewController::save(){
  eventAttributesView->saveCurrentShownEventData();
  for (std::vector<IEvent*>::iterator iter = events.begin();
       iter != events.end();
       ++iter){
    (*iter)->saveToDisk(pathAndName);
  }
  saveEnvelopeLibrary();
}


PaletteViewController* ProjectViewController::getPalette(){
  return paletteView;
}


IEvent* ProjectViewController::getEventByTypeAndName(
  EventType type,std::string _name){

  std::vector<IEvent*>::iterator i = events.begin();
  bool found = false;
  IEvent* beReturn = NULL;

  while( i != events.end()&&!found){
    if (    (*i)->getEventName()
         == _name&& (*i)->getEventType()
         == type){

      found == true;
      beReturn = *i;
    }
    i ++;
  }
  return beReturn;
}


EnvelopeLibraryEntry* ProjectViewController::createNewEnvelope(){
  if (envelopeLibraryEntries == NULL) {
    envelopeLibraryEntries = new EnvelopeLibraryEntry(1); 
    return envelopeLibraryEntries;
  }
  else {
    return envelopeLibraryEntries->createNewEnvelope();
  }


}


void ProjectViewController::saveEnvelopeLibrary(){
  std::string stringbuffer;
  char charbuffer[60];
  std::string fileName = pathAndName + "/"+projectTitle+".lib";
  FILE* file  = fopen(fileName.c_str(), "w");
  
  if (envelopeLibraryEntries ==NULL){ //safety check
    std::cout<<"There is no premade envelopes in the library"<<std::endl;
    fclose(file);
    return;
  }
  
  EnvelopeLibraryEntry* envLib = envelopeLibraryEntries;
  int count = envLib->count();
  
  //put the first number in to the file
  std::cout<<count<<std::endl;
  sprintf(charbuffer,"%d\n",count);
  fputs(charbuffer,file);
  
  
  
  count = 1;//use count as a index counter
  while (envLib!=NULL){
    sprintf (charbuffer, "Envelope %d\n", count);
        fputs(charbuffer,file);  
 
 
 
  
    int lineNumber = envLib->segments->count();
    sprintf(charbuffer,"%d\n",lineNumber);
    fputs(charbuffer,file); 
    EnvLibEntrySegment* libSeg = envLib-> segments;
    while (libSeg!=NULL){
      sprintf(charbuffer,"%.3f", libSeg->xStart);   
      stringbuffer = charbuffer;
      stringbuffer = stringbuffer + "\t";
      //stringbuffer =  charbuffer+ "\t";
           
      if (libSeg->prev ==NULL){
        sprintf(charbuffer,"%.3f", envLib->yStart);
        stringbuffer = stringbuffer + charbuffer +"\t";
      }// handle first segment
      else{
        sprintf(charbuffer,"%.3f", libSeg->prev->y);
        
        stringbuffer = stringbuffer + charbuffer +"\t";
      
      } //normal segment
        
       
      if (libSeg->segmentType == envSegmentTypeLinear){
        stringbuffer = stringbuffer + "LINEAR\t\t";
      }
      else if (libSeg->segmentType ==envSegmentTypeExponential){
        stringbuffer = stringbuffer + "EXPONENTIAL\t";
      }
      else {
        stringbuffer = stringbuffer + "SPLINE\t\t";
      }         
      
        
      if (libSeg->segmentProperty == envSegmentPropertyFlexible){
        stringbuffer = stringbuffer + "FLEXIBLE\t";
      }
      else {
              stringbuffer = stringbuffer + "FIXED\t\t";
      } 
      
      sprintf (charbuffer,"%.3f\n", libSeg->xDuration);
      stringbuffer = stringbuffer+charbuffer;
      
      if (libSeg->next ==NULL){

        sprintf (charbuffer,"%.3f\n", libSeg->y);
        stringbuffer = stringbuffer + "1.00\t"+ charbuffer;      
      
      }//handle last segment

      
      
      fputs(stringbuffer.c_str(),file);        
      libSeg = libSeg->next;
    } //end printing one envelope
  
 
 
    count++;
    envLib = envLib->next;
  }
  
    fclose(file);
}




ProjectViewController::ProjectViewController(
	std::string _pathAndName, 
	MainWindow* _mainWindow,
 	int _calledByOpenProject){
  std::string datFile =  _pathAndName + "/"+
                        FileOperations::stringToFileName(_pathAndName)
                        + ".dat"; 	
 	

  ///////////////////////////////////////////////drag and drop//////////////
  listTargets.push_back( Gtk::TargetEntry("STRING") );
  //listTargets.push_back( Gtk::TargetEntry("text/plain") );
  //////////////////////////////////////////////////////////////////////////



  //using CMOD class to parse file and read necessary data for the .dat file
  Piece* piece = new Piece();
  parseFile(datFile, NULL, piece);
  

	char buffer [256];
  std::string topName = piece->fileList;
  projectTitle = FileOperations::stringToFileName(_pathAndName);
  fileFlag      = piece->fileFlags;
  sprintf(buffer,"%f", piece->pieceDuration); 
  duration = buffer; 
  sprintf(buffer,"%d", piece->numChannels); 
  numOfChannels = buffer;
  sprintf(buffer,"%d", piece->sampleRate); 
  sampleRate    = buffer;
  sprintf(buffer,"%d", piece->sampleSize); 
  sampleSize    = buffer;
  sprintf(buffer,"%d", piece->numThreads); 
  numOfThreads  = buffer;
  topEvent      = piece->fileList;
  synthesis     = piece->soundSynthesis;

  delete piece; //since the data is no longer needed. delete piece to prevent
  //memory leak
	



  //add the Paned widget "leftTwoPlusAttributes" as a child
  add(leftTwoPlusAttributes);


  pathAndName = _pathAndName;

  sharedPointers = new SharedPointers();
  sharedPointers->mainWindow = _mainWindow;
  sharedPointers->projectView = this;

  //create three new children widgets
  //projectTreeView = new ProjectTreeViewController(sharedPointers);
  eventAttributesView = new EventAttributesViewController(sharedPointers);
  paletteView = new PaletteViewController(sharedPointers);


  //set the attributes of two Paned widgets and add children view in them
  leftTwoPlusAttributes.set_position(300);
  leftTwoPlusAttributes.pack1(*paletteView,true,false);
  leftTwoPlusAttributes.pack2(*eventAttributesView,true,false);

  //paletteAndTree.set_position(150);
  //paletteAndTree.pack1(*paletteView,true,false);
  //paletteAndTree.pack2(*projectTreeView,true,false);

  showContents();
    
  //make a new shared pointers object so all object can find each other easily
  //without calling layers of layers of parents/ children

  //sharedPointers->projectTreeView = projectTreeView;
  sharedPointers->eventAttributesView = eventAttributesView;
  sharedPointers->paletteView = paletteView;  
  
  
  
  
  //Reconstruct Envelope Entries
  envelopeLibraryEntries = NULL;
  std::string libFile =  _pathAndName + "/"+
                        FileOperations::stringToFileName(_pathAndName)
                        + ".lib";

  char libCharArray [libFile.length()+ 2];                      
  strcpy(libCharArray, libFile.c_str()); 
  std::cout<<libCharArray<<std::endl;                         




  //read envelope out one by one and convert to LASSIE::envelopelibraryentry
  EnvelopeLibrary* envelopeLibrary = new EnvelopeLibrary();  
  envelopeLibrary->loadLibraryNewFormat(libCharArray);
  EnvelopeLibraryEntry* previousEntry = NULL;
  Envelope* thisEnvelope;
  for (int i = 1; i <= envelopeLibrary->size(); i ++){
    thisEnvelope = envelopeLibrary->getEnvelope(i);
    EnvelopeLibraryEntry* thisEntry =convertToLASSIEEnvLibEntry(thisEnvelope, i);
    delete thisEnvelope; 
    
    if (previousEntry ==NULL){
      envelopeLibraryEntries = thisEntry;
      previousEntry = thisEntry;
      thisEntry->prev = NULL;
    }
    else{
      previousEntry->next = thisEntry;
      thisEntry->prev = previousEntry;
    } 
    previousEntry = thisEntry;
  }
  

  //construct all the IEvents by reading all the files under the dir


  /*   This is the template of iterating all files
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(_pathAndName.c_str())) == NULL) {
        std::cout << "Error(" << ") opening " << _pathAndName << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
      //std::cout<<(string(dirp->d_name));
      std::cout<<"====="<<dirp->d_name<<std::endl;
    }
    closedir(dp);

  */

  

  
  
  //  constructs all events
  std::string directory;
  DIR *dp;
  struct dirent *dirp;
  IEvent* newEvent;
  

  // make Top Events
  directory = _pathAndName+ "/T";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << _pathAndName << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
      if (dirp->d_name[0]!= '.'){
        newEvent = new IEvent( directory ,string(dirp->d_name), eventTop);
        events.push_back(newEvent);
        paletteView->insertEvent(newEvent, "Top");
      }
    }
    
    

    
    closedir(dp);


  //  make High Events
 directory = _pathAndName+ "/H";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << _pathAndName << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
      if (dirp->d_name[0]!= '.'){
        newEvent = new IEvent( directory ,string(dirp->d_name), eventHigh);
        events.push_back(newEvent);
        paletteView->insertEvent(newEvent, "High");
      }
    }
    closedir(dp);
  
    
  //  make Mid Events
 directory = _pathAndName+ "/M";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << _pathAndName << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
      if (dirp->d_name[0]!= '.'){
        newEvent = new IEvent( directory ,string(dirp->d_name), eventMid);
        events.push_back(newEvent);
        paletteView->insertEvent(newEvent,"Mid");
      }
    }
    closedir(dp);
  //  make Low Events
 directory = _pathAndName+ "/L";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << _pathAndName << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
      if (dirp->d_name[0]!= '.'){
        newEvent = new IEvent( directory ,string(dirp->d_name), eventLow);
        events.push_back(newEvent);
        paletteView->insertEvent(newEvent,"Low");
      }
    }
    closedir(dp);
  //  make Bottom Events
 directory = _pathAndName+ "/B";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << _pathAndName << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
      if (dirp->d_name[0]!= '.'){
        newEvent = new IEvent( directory ,string(dirp->d_name), eventBottom);
        events.push_back(newEvent);
        paletteView->insertEvent(newEvent,"Bottom");
      }
    }
    closedir(dp);

  //  make Sound Events
  

 directory = _pathAndName+ "/S";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << _pathAndName << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
      if (dirp->d_name[0]!= '.'){
        newEvent = new IEvent( directory ,string(dirp->d_name), eventSound);
        events.push_back(newEvent);
        paletteView->insertEvent(newEvent,"Sound");
      }
    }
    closedir(dp);

  //  make Env Events
 directory = _pathAndName+ "/ENV";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << _pathAndName << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
      if (dirp->d_name[0]!= '.'){
        newEvent = new IEvent( directory ,string(dirp->d_name), eventEnv);
        events.push_back(newEvent);
        paletteView->insertEvent(newEvent,"Envelope");
      }
    }
    closedir(dp);
  //  make Siv Events
 directory = _pathAndName+ "/SIV";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << _pathAndName << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
      if (dirp->d_name[0]!= '.'){
        newEvent = new IEvent( directory ,string(dirp->d_name), eventSiv);
        events.push_back(newEvent);
        paletteView->insertEvent(newEvent,"Sieve");
      }
    }
    closedir(dp);
  //  make Pat Events
 directory = _pathAndName+ "/PAT";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << _pathAndName << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
      if (dirp->d_name[0]!= '.'){
        newEvent = new IEvent( directory ,string(dirp->d_name), eventPat);
        events.push_back(newEvent);
        paletteView->insertEvent(newEvent,"Pattern");
      }
    }
    closedir(dp);

  //  make Spa Events
 directory = _pathAndName+ "/SPA";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << _pathAndName << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
      if (dirp->d_name[0]!= '.'){
        newEvent = new IEvent( directory ,string(dirp->d_name), eventSpa);
        events.push_back(newEvent);
        paletteView->insertEvent(newEvent,"Spatialization");
      }
    }
    closedir(dp);
  //  make Rev Events
 directory = _pathAndName+ "/REV";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << _pathAndName << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
      if (dirp->d_name[0]!= '.'){
        newEvent = new IEvent( directory ,string(dirp->d_name), eventRev);
        events.push_back(newEvent);
        paletteView->insertEvent(newEvent,"Reverb");
      }
    }
    closedir(dp);


    



  //TODO:link all IEvents to each other properly.

  
  delete envelopeLibrary;



show_all_children(); 	
}

EnvelopeLibraryEntry* ProjectViewController::getEnvelopeLibraryEntries(){
	return envelopeLibraryEntries;
}


  
EnvelopeLibraryEntry* ProjectViewController::
  convertToLASSIEEnvLibEntry( Envelope* _envelope, int index){
  return new EnvelopeLibraryEntry (_envelope, index);  
}




