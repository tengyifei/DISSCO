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
#include "FunctionGenerator.h"
#include <dirent.h>




// these definitions are for calling yyparse(). They are copied from lex.yy.c
#ifndef YY_TYPEDEF_YY_SIZE_T
#define YY_TYPEDEF_YY_SIZE_T
typedef size_t yy_size_t;
#endif



#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

#ifndef YY_STRUCT_YY_BUFFER_STATE
#define YY_STRUCT_YY_BUFFER_STATE
struct yy_buffer_state
	{
	FILE *yy_input_file;

	char *yy_ch_buf;		/* input buffer */
	char *yy_buf_pos;		/* current position in input buffer */

	/* Size of input buffer in bytes, not including room for EOB
	 * characters.
	 */
	yy_size_t yy_buf_size;

	/* Number of characters read into yy_ch_buf, not including EOB
	 * characters.
	 */
	int yy_n_chars;

	/* Whether we "own" the buffer - i.e., we know we created it,
	 * and can realloc() it to grow it, and should free() it to
	 * delete it.
	 */
	int yy_is_our_buffer;

	/* Whether this is an "interactive" input source; if so, and
	 * if we're using stdio for input, then we want to use getc()
	 * instead of fread(), to make sure we stop fetching input after
	 * each newline.
	 */
	int yy_is_interactive;

	/* Whether we're considered to be at the beginning of a line.
	 * If so, '^' rules will be active on the next match, otherwise
	 * not.
	 */
	int yy_at_bol;

    int yy_bs_lineno; /**< The line count. */
    int yy_bs_column; /**< The column count. */
    
	/* Whether to try to fill the input buffer when we reach the
	 * end of it.
	 */
	int yy_fill_buffer;

	int yy_buffer_status;

#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
	/* When an EOF's been seen but there's still some text to process
	 * then we mark the buffer as YY_EOF_PENDING, to indicate that we
	 * shouldn't try reading from the input source any more.  We might
	 * still have a bunch of tokens to match, though, because of
	 * possible backing-up.
	 *
	 * When we actually see the EOF, we change the status to "new"
	 * (via yyrestart()), so that the user can continue scanning by
	 * just pointing yyin at a new input file.
	 */
#define YY_BUFFER_EOF_PENDING 2

	};
#endif /* !YY_STRUCT_YY_BUFFER_STATE */

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};



extern YY_BUFFER_STATE yy_scan_string( const char*);
extern int yyparse();
extern map<const char*, FileValue*, ltstr> file_data;


















/*! \brief The constructor of ProjectViewController
*
*******************************************************************************/
ProjectViewController::ProjectViewController(MainWindow* _mainWindow){
  emptyProject = true;

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
  duration      = "";
  numOfChannels = "2";
  sampleRate    = "44100";
  sampleSize    = "16";
  numOfThreads  = "1";
  topEvent      = "0";
  synthesis     = true;

  sharedPointers = new SharedPointers();
      sharedPointers->mainWindow = _mainWindow;
  sharedPointers->projectView = this;
  //create three new children widgets
  //eventAttributesView = new EventAttributesViewController(  sharedPointers );
  //paletteView = new PaletteViewController(  sharedPointers );


  //set the attributes of two Paned widgets and add children view in them
  //leftTwoPlusAttributes.set_position(200);
  //leftTwoPlusAttributes.pack1(*paletteView,true,false);
  //leftTwoPlusAttributes.pack2(*eventAttributesView,true,false);


  envelopeLibraryEntries = NULL;

  show_all_children();
}


/*! \brief The constructor of ProjectViewController
*
*******************************************************************************/
ProjectViewController::ProjectViewController(
  std::string _pathAndName, MainWindow* _mainWindow){
  emptyProject = false;
  modifiedButNotSaved = true;
  

  
  
  ///////////////////////////////////////////////drag and drop//////////////
  listTargets.push_back( Gtk::TargetEntry("STRING") );
  //listTargets.push_back( Gtk::TargetEntry("text/plain") );
  //////////////////////////////////////////////////////////////////////////

  std::string topName = "0";
  projectTitle  = FileOperations::stringToFileName(_pathAndName);
  fileFlag      = "THMLBsnv";
  duration      = "";
  numOfChannels = "2";
  sampleRate    = "44100";
  sampleSize    = "16";
  numOfThreads  = "1";
  topEvent      = "0";
  synthesis     = true;
  
  
  
  // initialize default note modifiers
  defaultNoteModifiers.insert(pair<string,bool>("-8va",true));
  defaultNoteModifiers.insert(pair<string,bool>("+8va",true));
  defaultNoteModifiers.insert(pair<string,bool>("bend",true));
  defaultNoteModifiers.insert(pair<string,bool>("dry",true));
  defaultNoteModifiers.insert(pair<string,bool>("glissKeys",true));
  defaultNoteModifiers.insert(pair<string,bool>("glissStringRes",true));
  defaultNoteModifiers.insert(pair<string,bool>("graceTie",true));
  defaultNoteModifiers.insert(pair<string,bool>("letVibrate",true));
  defaultNoteModifiers.insert(pair<string,bool>("moltoVibrato",true));
  defaultNoteModifiers.insert(pair<string,bool>("mute",true));
  defaultNoteModifiers.insert(pair<string,bool>("pedal",true));
  defaultNoteModifiers.insert(pair<string,bool>("pluck",true));
  defaultNoteModifiers.insert(pair<string,bool>("pressSilently",true));
  defaultNoteModifiers.insert(pair<string,bool>("resonance",true));
  defaultNoteModifiers.insert(pair<string,bool>("resPedal",true));
  defaultNoteModifiers.insert(pair<string,bool>("sfz",true));
  defaultNoteModifiers.insert(pair<string,bool>("sffz",true));
  defaultNoteModifiers.insert(pair<string,bool>("tenuto",true));
  defaultNoteModifiers.insert(pair<string,bool>("tremolo",true));
  defaultNoteModifiers.insert(pair<string,bool>("vibrato",true));
  
  
  
  
  //add the Paned widget "leftTwoPlusAttributes" as a child
  add(leftTwoPlusAttributes);

  //no title when initializing the window
  pathAndName = _pathAndName;
  datPathAndName = pathAndName+ "/"+projectTitle+".dat";
  libPathAndName = pathAndName+ "/"+projectTitle+".lib";
  sharedPointers = new SharedPointers();
  sharedPointers->mainWindow = _mainWindow;
  sharedPointers->projectView = this;

  //create three new children widgets

  eventAttributesView = new EventAttributesViewController(sharedPointers);
  paletteView = new PaletteViewController(sharedPointers);


  //set the attributes of two Paned widgets and add children view in them
  leftTwoPlusAttributes.set_position(200);
  leftTwoPlusAttributes.pack1(*paletteView,true,false);
  leftTwoPlusAttributes.pack2(*eventAttributesView,true,false);


  showContents();
  
  
  
  
  //make a new shared pointers object so all object can find each other easily
  //without calling layers of layers of parents/ children

  


  sharedPointers->eventAttributesView = eventAttributesView;
  sharedPointers->paletteView = paletteView;
  
  

  IEvent* newEvent = new IEvent();
  newEvent->setEventName("0");
  newEvent->setEventType(eventTop);
  paletteView->insertEvent(newEvent,"Top");
  events.push_back(newEvent);



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
  //std::cout << "ProjectViewController destructor is called." << std::endl;
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
  
  
  //std::cout << "ProjectViewController cleanUpContents is called." << std::endl;
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
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("High");
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Mid");
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Low");
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Bottom");
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Spectrum");
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Note");
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Envelope");
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Sieve");
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Spatialization");
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Pattern");
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Reverb");
  paletteView->insertEvent(event);
  events.push_back(event);

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
    //std::cout << "the row selected is not a folder" << std::endl;
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
  else if (selectedPaletteFolder == "Spectrum"){
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
  else if (selectedPaletteFolder == "Note"){
    refBuilder->get_widget("buttonNote", typeButton);
    typeButton->set_active(true);
  }

  int result = newObjectDialog->run();

  while (result ==1){
    std::string name = nameEntry->get_text();
    std::string first = name.substr(0,1);
  
    if (name == ""){
    //prompt error (lack of name) and then loop
    //std::cout<<"User didn't name the object!"<<std::endl;
      Gtk::MessageDialog dialog(
        "Please Name the Object",
        false /* use_markup */,
        Gtk::MESSAGE_QUESTION,
        Gtk::BUTTONS_OK);

      dialog.run();
      dialog.hide();
      result = newObjectDialog->run();
    }
    else if (selectedPaletteFolder == "Bottom"&& first!="s"&&first!= "n"){
    
      Gtk::MessageDialog dialog(
        "The name of a Bottom event should start with 's' (sound) or 'n' (note).",
        false /* use_markup */,
        Gtk::MESSAGE_QUESTION,
        Gtk::BUTTONS_OK);

      dialog.run();
      dialog.hide();
      result = newObjectDialog->run();    
    
    
    
    }
    else {
      break;
    }
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
      flagForFolderMatching = "Spectrum";
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

    refBuilder->get_widget("buttonNote", typeButton);
    if (typeButton->get_active()) {
      type = eventNote;
      flagForFolderMatching = "Note";
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
      modified();
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
      modified();
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
      else if (selectedPaletteFolder =="Spectrum"){
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
      else if (selectedPaletteFolder =="Note"){
        newEvent->setEventType(eventNote);
      }
      paletteView->insertEvent(newEvent);
      events.push_back(newEvent);
    }
    else{ //decision == 2
      std::string name = nameEntry->get_text();

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
        modified();
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
  //if (_event->getEventTypeString() == "Folder"){
    //showAttributesView(false);
  //}
  
  //else {
    //showAttributesView(true);
    eventAttributesView->showAttributesOfEvent(_event);
  //}
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
  
  Gtk::Button* button3;
  refBuilder->get_widget("button3", button3);  
  button3->signal_clicked().connect(
      sigc::mem_fun(
        *this,&ProjectViewController::projectPropertiesDialogFunctionButtonClicked) );

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
  entry->grab_focus();

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
  //refreshProjectDotDat();
  modified();

}


void ProjectViewController::refreshProjectDotDat(){

  std::string stringbuffer;
  FILE* dat  = fopen(datPathAndName.c_str(), "w");

  stringbuffer = "title = \"" + projectTitle + "\";\n";
  
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse() == 0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal title value!"<<endl;
  }
  

  
  stringbuffer = "fileFlags = \"" + fileFlag + "\";\n";
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal fileFlag value!"<<endl;
  }

  stringbuffer = "fileList = \"T/" + topEvent + "\";\n";
  yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal fileList value!"<<endl;
  }

  stringbuffer = "pieceStartTime = 0;\n";
  fputs(stringbuffer.c_str(),dat);

  stringbuffer = "pieceDuration = " + duration + ";\n\n";
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal pieceDuration value!"<<endl;
  }

  if (synthesis){
    stringbuffer = "soundSynthesis = TRUE;\n";
  }
  else{
    stringbuffer = "soundSynthesis = FALSE;\n";
  }

  fputs(stringbuffer.c_str(),dat);

  stringbuffer = "numChannels = " + numOfChannels + ";\n";
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal numChannels value!"<<endl;
  }

  stringbuffer = "sampleRate = " + sampleRate + ";\n";
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal sampleRate value!"<<endl;
  }

  stringbuffer = "sampleSize = " + sampleSize + ";\n";
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal sampleSize value!"<<endl;
  }

  stringbuffer = "numThreads = " + numOfThreads + ";\n";
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal numThreads value!"<<endl;
  }
  
  
  
  /* metadata */
  
  //stringbuffer = "\n\n\n/*====================LASSIE METADATA===============*/\n\n\n";
  //fputs(stringbuffer.c_str(),dat);  
  
  std::string buffer2;
  buffer2 = (projectTitle == "")? "": projectTitle;
  stringbuffer = "LASSIETITLE = `" + buffer2 + "`;\n";
  fputs(stringbuffer.c_str(),dat);

  buffer2 = (fileFlag == "")? "": fileFlag;
  stringbuffer = "LASSIEFILEFLAGS = `" + buffer2 + "`;\n";
  fputs(stringbuffer.c_str(),dat);

  buffer2 = (topEvent == "")? "": topEvent;
  stringbuffer = "LASSIEFILELIST = `" + buffer2 + "`;\n";
  fputs(stringbuffer.c_str(),dat);

  stringbuffer = "LASSIEPIECESTARTTIME = `0`;\n";
  fputs(stringbuffer.c_str(),dat);

  buffer2 = (duration == "")? "": duration;
  stringbuffer = "LASSIEPIECEDURATION = `" + buffer2 + "`;\n\n";
  fputs(stringbuffer.c_str(),dat);

  if (synthesis){
    stringbuffer = "LASSIESOUNDSYNTHESIS = `TRUE`;\n";
  }
  else{
    stringbuffer = "LASSIESOUNDSYNTHESIS = `FALSE`;\n";
  }

  fputs(stringbuffer.c_str(),dat);

  buffer2 = (numOfChannels == "")? "": numOfChannels;
  stringbuffer = "LASSIENUMCHANNELS = `" + buffer2 + "`;\n";
  fputs(stringbuffer.c_str(),dat);

  buffer2 = (sampleRate == "")? "": sampleRate;
  stringbuffer = "LASSIESAMPLERITE = `" + buffer2 + "`;\n";
  fputs(stringbuffer.c_str(),dat);

  buffer2 = (sampleSize == "")? "": sampleSize;
  stringbuffer = "LASSIESAMPLESIZE = `" + buffer2 + "`;\n";
  fputs(stringbuffer.c_str(),dat);
  
  buffer2 = (numOfThreads == "")? "": numOfThreads;
  stringbuffer = "LASSIENUMTHREADS = `" + buffer2 + "`;\n";
  fputs(stringbuffer.c_str(),dat);
  


  fclose(dat);
}





void ProjectViewController::save(){
  modifiedButNotSaved = false;
  sharedPointers->mainWindow->setSavedTitle();
  
  
  eventAttributesView->saveCurrentShownEventData();
  for (std::vector<IEvent*>::iterator iter = events.begin();
       iter != events.end();
       ++iter){
    (*iter)->saveToDisk(pathAndName);
  }
  saveEnvelopeLibrary();
  saveNoteModifierConfiguration();
  refreshProjectDotDat();
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
  FILE* file  = fopen(libPathAndName.c_str(), "w");
  
  if (envelopeLibraryEntries ==NULL){ //safety check
    //std::cout<<"There is no premade envelopes in the library"<<std::endl;
    fclose(file);
    return;
  }
  
  EnvelopeLibraryEntry* envLib = envelopeLibraryEntries;
  int count = envLib->count();
  
  //put the first number in to the file
  //std::cout<<count<<std::endl;
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
 	std::string _datPathAndName, 
 	std::string _libPathAndName){
  //std::string datFile =  _pathAndName + "/"+
    //                    FileOperations::stringToFileName(_pathAndName)
      //                  + ".dat"; 	
 	
  modifiedButNotSaved = false;
  ///////////////////////////////////////////////drag and drop//////////////
  listTargets.push_back( Gtk::TargetEntry("STRING") );
  //listTargets.push_back( Gtk::TargetEntry("text/plain") );
  //////////////////////////////////////////////////////////////////////////
  datPathAndName = _datPathAndName;
  libPathAndName = _libPathAndName;
  pathAndName = _pathAndName;
  

  //cout<<"the dat is this one: "<<datPathAndName<<endl;
  //using CMOD class to parse file and read necessary data for the .dat file
  //Piece* piece = new Piece();
  //parseFile(_datPathAndName, NULL, piece);
  //cout<<"LASSIE done parsing"<<endl;




  //YY_FLUSH_BUFFER;//flush the buffer make sure the buffer is clean
  FILE *yytmp;
  extern FILE *yyin;
  yytmp = fopen(_datPathAndName.c_str(), "r");

  if (yytmp == NULL) {
    cout << "ERROR: File " << _datPathAndName << " does not exist!" << endl;
    //exit(1);
  }
  else {
    yyin = yytmp; 
    //extern map<const char*, FileValue*, ltstr> file_data;
    yyparse(); 
    fclose(yyin);

    FileValue* value;
    std::string buffer2;

    //std::string topName = piece->fileList;
    value = file_data["LASSIETITLE"];
    buffer2 = value->getString();
    buffer2 = (buffer2 =="")?"":buffer2;
  
    //projectTitle = FileOperations::stringToFileName(_pathAndName);
    projectTitle = buffer2;
    //fileFlag      = piece->fileFlags;
    value = file_data["LASSIEFILEFLAGS"];
    buffer2 = value->getString();
    
    buffer2 = (buffer2 =="")?"":buffer2;
    fileFlag = buffer2;
  
  
    //sprintf(buffer,"%f", piece->pieceDuration); 
    //duration = buffer; 
    value = file_data["LASSIEPIECEDURATION"];
    buffer2 = value->getString();
    buffer2 = (buffer2 =="")?"":buffer2;
    duration = buffer2;  
    
    
    
    //sprintf(buffer,"%d", piece->numChannels); 
    //numOfChannels = buffer;
    value = file_data["LASSIENUMCHANNELS"];
    buffer2 = value->getString();
    buffer2 = (buffer2 =="")?"":buffer2;
    numOfChannels = buffer2;
  
  
    //sprintf(buffer,"%d", piece->sampleRate); 
    //sampleRate    = buffer;
    value = file_data["LASSIESAMPLERITE"];
    buffer2 = value->getString();
    buffer2 = (buffer2 =="")?"":buffer2;
    sampleRate = buffer2;
    
    
    
    //sprintf(buffer,"%d", piece->sampleSize); 
    //sampleSize    = buffer;
    value = file_data["LASSIESAMPLESIZE"];
    buffer2 = value->getString();
    buffer2 = (buffer2 =="")?"":buffer2;
    sampleSize = buffer2;
    
    
    
    
    //sprintf(buffer,"%d", piece->numThreads); 
    //numOfThreads  = buffer;
    
    value = file_data["LASSIENUMTHREADS"];
    buffer2 = value->getString();
    buffer2 = (buffer2 =="")?"":buffer2;
    numOfThreads = buffer2;
  
    //topEvent      = piece->fileList
    value = file_data["LASSIEFILELIST"];
    buffer2 = value->getString();
    buffer2 = (buffer2 =="")?"":buffer2;
    topEvent = buffer2;  
  
    value = file_data["LASSIESOUNDSYNTHESIS"];
    buffer2 = value->getString();
    synthesis = (buffer2 =="TRUE")?true:false;
  }

  //restore notemodifiers
  
  defaultNoteModifiers.insert(pair<string,bool>("-8va",true));
  defaultNoteModifiers.insert(pair<string,bool>("+8va",true));
  defaultNoteModifiers.insert(pair<string,bool>("bend",true));
  defaultNoteModifiers.insert(pair<string,bool>("dry",true));
  defaultNoteModifiers.insert(pair<string,bool>("glissKeys",true));
  defaultNoteModifiers.insert(pair<string,bool>("glissStringRes",true));
  defaultNoteModifiers.insert(pair<string,bool>("graceTie",true));
  defaultNoteModifiers.insert(pair<string,bool>("letVibrate",true));
  defaultNoteModifiers.insert(pair<string,bool>("moltoVibrato",true));
  defaultNoteModifiers.insert(pair<string,bool>("mute",true));
  defaultNoteModifiers.insert(pair<string,bool>("pedal",true));
  defaultNoteModifiers.insert(pair<string,bool>("pluck",true));
  defaultNoteModifiers.insert(pair<string,bool>("pressSilently",true));
  defaultNoteModifiers.insert(pair<string,bool>("resonance",true));
  defaultNoteModifiers.insert(pair<string,bool>("resPedal",true));
  defaultNoteModifiers.insert(pair<string,bool>("sfz",true));
  defaultNoteModifiers.insert(pair<string,bool>("sffz",true));
  defaultNoteModifiers.insert(pair<string,bool>("tenuto",true));
  defaultNoteModifiers.insert(pair<string,bool>("tremolo",true));
  defaultNoteModifiers.insert(pair<string,bool>("vibrato",true));

   //YY_FLUSH_BUFFER;//flush the buffer make sure the buffer is clean
  //FILE *yytmp;
  //extern FILE *yyin;
  std::string stringbuffer = pathAndName + "/.noteModifiersConfiguration";
  yytmp = fopen(stringbuffer.c_str(), "r");

  if (yytmp == NULL) {
    cout << "ERROR: File " << stringbuffer << " does not exist!" << endl;
    //exit(1);
  }

  else{
    yyin = yytmp; 
    //extern map<const char*, FileValue*, ltstr> file_data;
    yyparse(); 
    fclose (yyin);

    FileValue* value;
    //std::string buffer2;
  
    //std::string topName = piece->fileList;
    value = file_data["LASSIENOTEDEFAULTMODIFIER"];
    std::list<FileValue> valueList = value->getList();
    std::list<FileValue>::iterator valueListIter = valueList.begin();
    std::map<string, bool>::iterator modifierMapIter = defaultNoteModifiers.begin();
    
    while( valueListIter != valueList.end() && modifierMapIter != defaultNoteModifiers.end()){
      (*modifierMapIter).second = ((*valueListIter).getInt()==1)?true:false;
      modifierMapIter ++;
      valueListIter ++;
    }
     
    value = file_data["LASSIENOTECUSTOMMODIFIER"];
    if (value){
      valueList = value->getList();
      valueListIter = valueList.begin();
      for (valueListIter;valueListIter != valueList.end(); valueListIter++){
        customNoteModifiers.push_back((*valueListIter).getString());
      } 
    
    }
  }
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
  leftTwoPlusAttributes.set_position(200);
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
  //std::string libFile =  _pathAndName + "/"+
    //                    FileOperations::stringToFileName(_pathAndName)
      //                  + ".lib";
  std::string libFile = _libPathAndName;

  char libCharArray [libFile.length()+ 2];                      
  strcpy(libCharArray, libFile.c_str()); 
  //std::cout<<libCharArray<<std::endl;                         

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
        
        paletteView->insertEvent(newEvent, "Top");
        events.push_back(newEvent);
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

        paletteView->insertEvent(newEvent, "High");
        events.push_back(newEvent);
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

        paletteView->insertEvent(newEvent,"Mid");
        events.push_back(newEvent);
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

        paletteView->insertEvent(newEvent,"Low");
        events.push_back(newEvent);
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
        paletteView->insertEvent(newEvent,"Bottom");
        events.push_back(newEvent);
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
        paletteView->insertEvent(newEvent,"Spectrum");
				events.push_back(newEvent);
      }
    }
    closedir(dp);

  //make note event
 directory = _pathAndName+ "/N";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << directory <<", directory doesn't exist."<< std::endl;
     std::cout<<"Creating " <<directory <<endl;
     string temp = "mkdir "+ directory;
     system(temp.c_str());
  }
  else{
    while ((dirp = readdir(dp)) != NULL) {
      if (dirp->d_name[0]!= '.'){
        newEvent = new IEvent( directory ,string(dirp->d_name), eventNote);
        
        paletteView->insertEvent(newEvent,"Note");
        events.push_back(newEvent);
      }
    }  
    closedir(dp);
  }


  //  make Env Events
 directory = _pathAndName+ "/ENV";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << _pathAndName << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
      if (dirp->d_name[0]!= '.'){
        newEvent = new IEvent( directory ,string(dirp->d_name), eventEnv);
        
        paletteView->insertEvent(newEvent,"Envelope");
        events.push_back(newEvent);
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
        
        paletteView->insertEvent(newEvent,"Sieve");
        events.push_back(newEvent);
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
        
        paletteView->insertEvent(newEvent,"Pattern");
        events.push_back(newEvent);
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
        
        paletteView->insertEvent(newEvent,"Spatialization");
        events.push_back(newEvent);
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
        
        paletteView->insertEvent(newEvent,"Reverb");
        events.push_back(newEvent);
      }
    }
    closedir(dp);


  std::vector<IEvent*>::iterator eventsIter = events.begin();
  
  for (eventsIter; eventsIter != events.end(); eventsIter++){
    (*eventsIter)->link(this);
  }     


  
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

IEvent* ProjectViewController::findIEvent(EventType _type, std::string _eventName){
  IEvent* toReturn = NULL;
  

  std::vector<IEvent*>::iterator eventsIter = events.begin();
  
  while (eventsIter != events.end()){
    if (  (_eventName.compare( (*eventsIter)->getEventName() ) == 0 ) && 
          ( _type == (*eventsIter)->getEventType() ) ){
      toReturn = (*eventsIter);
      break;
    } 
    eventsIter++;
  }    

  if (toReturn ==NULL){
    std::cout<<"Find event fail :-(    EventName:"<<_eventName<<"  type:" << (int) _type<< std::endl;
  }
  return toReturn;
}



void ProjectViewController::deleteKeyPressed(Gtk::Widget* _focus){
 	if (_focus ==NULL){
 		return;
 	}
 
  if (_focus->is_ancestor((Gtk::Widget&)*paletteView)){
    paletteView->deleteKeyPressed();
  }
  else {
    eventAttributesView->deleteKeyPressed(_focus);  
  }
}

void ProjectViewController::nKeyPressed(Gtk::Widget* _focus){
 	if (_focus ==NULL){
 		return;
 	}
  if (_focus->is_ancestor((Gtk::Widget&)*paletteView)){
    insertObject();
  }

}



bool ProjectViewController::getEmptyProject(){
  return emptyProject;
}


void ProjectViewController::projectPropertiesDialogFunctionButtonClicked (){
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat, duration);
    generator->run();

  if (generator->getResultString() !=""){
    duration = generator->getResultString();
  }

  delete generator;
  
  projectPropertiesDialog->hide();
  delete projectPropertiesDialog;
  setProperties();

}


void ProjectViewController::modified(){
  if (modifiedButNotSaved == false){
    modifiedButNotSaved = true;
    sharedPointers->mainWindow->setUnsavedTitle();
  
  }
  
  
}

bool ProjectViewController::getSaved(){
  return !modifiedButNotSaved;
}


void ProjectViewController::showAttributesView(bool _show){
  if (_show && leftTwoPlusAttributes.get_child2() ==NULL ){
    
    leftTwoPlusAttributes.pack2(*eventAttributesView,true,false);
    show_all_children();  
  }  
  else if (!_show &&leftTwoPlusAttributes.get_child2() != NULL){  
    leftTwoPlusAttributes.remove(*(leftTwoPlusAttributes.get_child2()));
    show_all_children();  
    
  }
 
}




void ProjectViewController::configureNoteModifiers(){
  //Load the GtkBuilder file and instantiate its widgets:
  Glib::RefPtr<Gtk::Builder> noteModifiersConfigurationDialogRefBuilder = Gtk::Builder::create();

  #ifdef GLIBMM_EXCEPTIONS_ENABLED
    try{
      noteModifiersConfigurationDialogRefBuilder->add_from_file("./LASSIE/src/UI/NoteModifiersConfiguration.ui");
    }
    catch(const Glib::FileError& ex){
      std::cerr << "FileError: " << ex.what() << std::endl;
    }
    catch(const Gtk::BuilderError& ex){
      std::cerr << "BuilderError: " << ex.what() << std::endl;
    }
  #else
    std::auto_ptr<Glib::Error> error;

    if (!noteModifiersConfigurationDialogRefBuilder->add_from_file("./LASSIE/src/UI/NoteModifiersConfiguration.ui", error)){
      std::cerr << error->what() << std::endl;
    }
  #endif /* !GLIBMM_EXCEPTIONS_ENABLED */

	
  //Get the GtkBuilder-instantiated Dialog:
  noteModifiersConfigurationDialogRefBuilder->get_widget(
    "NoteConfigurationDialog", noteModifiersConfigurationDialog);
	noteModifiersConfigurationDialog->set_title("Note Modifiers Configuration");
	
  noteModifiersConfigurationDialogRefBuilder->get_widget(
    "CustomModifiersVBox",noteModifiersConfigurationCustomVBox );
	
	
	CustomNoteModifierHBox* box;
	vector<string>::iterator iter = customNoteModifiers.begin();
	for (iter; iter!= customNoteModifiers.end(); iter++){
	  box = new CustomNoteModifierHBox(this, *iter);
	  
    customNotModifierHBoxes.push_back(box);
    noteModifiersConfigurationCustomVBox->pack_start(*box,Gtk::PACK_SHRINK);
	}
	
	Gtk::CheckButton* checkButton;
	if (defaultNoteModifiers["-8va"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "Minus8vaButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["+8va"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "Plus8vaButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["bend"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "BendButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["dry"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "DryButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["glissKeys"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "GlissKeysButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["glissStringRes"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "GlissStringResButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["graceTie"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "GraceTieButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["letVibrate"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "LetVibrateButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["moltoVibrato"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "MoltoVibratoButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["mute"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "MuteButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["pedal"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "PedalButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["pluck"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "PluckButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["pressSilently"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "PressSilentlyButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["resonance"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "ResonanceButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["resPedal"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "ResPedalButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["sfz"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "SfzButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["sffz"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "SffzButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["tenuto"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "TenutoButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["tremolo"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "TremoloButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	if (defaultNoteModifiers["vibrato"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "VibratoButton", checkButton);	  
	  checkButton->set_active(true);
	}
	
	Gtk::Button* button;
  noteModifiersConfigurationDialogRefBuilder->get_widget(
     "AddModifierButton", button);		
	
	button->signal_clicked().connect(sigc::mem_fun
    (*this, &ProjectViewController::ConfigureNoteModifiersAddButtonClicked));  
	

	noteModifiersConfigurationDialog->show_all_children();
	
  int result = noteModifiersConfigurationDialog->run();
  
  noteModifiersConfigurationDialog->hide();
  
  if (result == 1){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "Minus8vaButton", checkButton);	  
	  defaultNoteModifiers["-8va"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "Plus8vaButton", checkButton);	  
	  defaultNoteModifiers["+8va"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "BendButton", checkButton);	  
	  defaultNoteModifiers["bend"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "DryButton", checkButton);	  
	  defaultNoteModifiers["dry"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "GlissKeysButton", checkButton);	  
	  defaultNoteModifiers["glissKeys"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "GlissStringResButton", checkButton);	  
	  defaultNoteModifiers["glissStringRes"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "GraceTieButton", checkButton);	  
	  defaultNoteModifiers["graceTie"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "LetVibrateButton", checkButton);	  
	  defaultNoteModifiers["letVibrate"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "MoltoVibratoButton", checkButton);	  
	  defaultNoteModifiers["moltoVibrato"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "MuteButton", checkButton);	  
	  defaultNoteModifiers["mute"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "PedalButton", checkButton);	  
	  defaultNoteModifiers["pedal"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "PluckButton", checkButton);	  
	  defaultNoteModifiers["pluck"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "PressSilentlyButton", checkButton);	  
	  defaultNoteModifiers["pressSilently"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "ResonanceButton", checkButton);	  
	  defaultNoteModifiers["resonance"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "ResPedalButton", checkButton);	  
	  defaultNoteModifiers["resPedal"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "SfzButton", checkButton);	  
	  defaultNoteModifiers["sfz"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "SffzButton", checkButton);	  
	  defaultNoteModifiers["sffz"] = checkButton->get_active();   
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "TenutoButton", checkButton);	  
	  defaultNoteModifiers["tenuto"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "TremoloButton", checkButton);	  
	  defaultNoteModifiers["tremolo"] = checkButton->get_active();  
  
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "VibratoButton", checkButton);	  
	  defaultNoteModifiers["vibrato"] = checkButton->get_active();      


    std::vector<CustomNoteModifierHBox*>::iterator iter =customNotModifierHBoxes.begin();
    customNoteModifiers.clear();
    string modifierText;
    for (iter; iter!= customNotModifierHBoxes.end(); iter++){
      modifierText = (*iter)->getText();
      if (modifierText != ""){
        customNoteModifiers.push_back( (*iter)->getText());
      }
    }
    
    modified();
    eventAttributesView->buildNoteModifiersList();
    
  }
   
  noteModifiersConfigurationDialog = NULL;
  noteModifiersConfigurationCustomVBox = NULL;

  std::vector<CustomNoteModifierHBox*>::iterator iter2 =customNotModifierHBoxes.begin();
  for (iter2; iter2!= customNotModifierHBoxes.end(); iter2++){
    delete (*iter2);
  }  
  customNotModifierHBoxes.clear();
}

void ProjectViewController::ConfigureNoteModifiersAddButtonClicked(){
  CustomNoteModifierHBox* newHBox = new CustomNoteModifierHBox(this);
  customNotModifierHBoxes.push_back(newHBox);
  noteModifiersConfigurationCustomVBox->pack_start(*newHBox,Gtk::PACK_SHRINK);
  noteModifiersConfigurationDialog->show_all_children();

}


CustomNoteModifierHBox::CustomNoteModifierHBox(ProjectViewController* _projectView){
  projectView = _projectView;
  label.set_text("Modifier Name: ");
  removeButton.set_label("Remove Modifier");
  pack_start(label,Gtk::PACK_SHRINK);
  pack_start(entry,Gtk::PACK_EXPAND_WIDGET);
  pack_start(removeButton,Gtk::PACK_SHRINK);

  removeButton.signal_clicked().connect(sigc::mem_fun
    (*this, &CustomNoteModifierHBox::removeButtonClicked));  
}

CustomNoteModifierHBox::CustomNoteModifierHBox(ProjectViewController* _projectView, string _string){
  entry.set_text(_string);
  
  projectView = _projectView;
  label.set_text("Modifier Name: ");
  removeButton.set_label("Remove Modifier");
  pack_start(label,Gtk::PACK_SHRINK);
  pack_start(entry,Gtk::PACK_EXPAND_WIDGET);
  pack_start(removeButton,Gtk::PACK_SHRINK);

  removeButton.signal_clicked().connect(sigc::mem_fun
    (*this, &CustomNoteModifierHBox::removeButtonClicked));  

  
}


CustomNoteModifierHBox::~CustomNoteModifierHBox(){
}

std::string CustomNoteModifierHBox::getText(){
  return entry.get_text();
}

void CustomNoteModifierHBox::removeButtonClicked(){
  projectView->removeCustomNoteModifier(this);
}

void ProjectViewController::removeCustomNoteModifier(CustomNoteModifierHBox* _hbox){
  std::vector<CustomNoteModifierHBox*>::iterator iter = customNotModifierHBoxes.begin();
  while(*iter != _hbox && iter != customNotModifierHBoxes.end()){
    iter++;
  } 
   
  
  customNotModifierHBoxes.erase(iter);
  noteModifiersConfigurationCustomVBox->remove(*_hbox);
  delete _hbox;
  noteModifiersConfigurationDialog->show_all_children();

}



void ProjectViewController::saveNoteModifierConfiguration(){
  std::string stringbuffer = pathAndName + "/.noteModifiersConfiguration";
  //char charbuffer[60];
    
  FILE* file  = fopen(stringbuffer.c_str(), "w");
  
  stringbuffer = "LASSIENOTEDEFAULTMODIFIER = <";
  
  std::map<string, bool>::iterator iter = defaultNoteModifiers.begin();
  
  while (iter != defaultNoteModifiers.end()){
    //cout <<(*iter).second;
    string buffer2 = ((*iter).second)? "1":"0";
    stringbuffer = stringbuffer + buffer2;
    iter++;
    if (iter!= defaultNoteModifiers.end()){
      stringbuffer = stringbuffer + ", ";
    }
  }
  
  stringbuffer = stringbuffer + ">;\n"  ; 
  fputs(stringbuffer.c_str(),file);
  
  
  if (customNoteModifiers.size() !=0){
  
    stringbuffer = "LASSIENOTECUSTOMMODIFIER = <";
    
    std::vector<string>::iterator iter2 = customNoteModifiers.begin();
    
    while (iter2 != customNoteModifiers.end()){
      stringbuffer = stringbuffer + "`" + *iter2 + "`";
      iter2 ++;
      if (iter2!= customNoteModifiers.end()){
        stringbuffer = stringbuffer + ", ";
      }
    }
    stringbuffer = stringbuffer + ">;\n";
    fputs(stringbuffer.c_str(),file);
  }
 
  fclose(file);

  
}


std::map<std::string, bool> ProjectViewController::getDefaultNoteModifiers(){
  return defaultNoteModifiers;

}
std::vector<std::string> ProjectViewController::getCustomNoteModifiers(){
  return customNoteModifiers;
}



void ProjectViewController::saveAs(std::string _newPathAndName){
	pathAndName = _newPathAndName;
	projectTitle  = FileOperations::stringToFileName(pathAndName);
	datPathAndName = pathAndName+ "/"+projectTitle+".dat";
  libPathAndName = pathAndName+ "/"+projectTitle+".lib";
	
	saveEnvelopeLibrary();
  saveNoteModifierConfiguration();
  refreshProjectDotDat();
  
  eventAttributesView->saveCurrentShownEventData();
  for (std::vector<IEvent*>::iterator iter = events.begin();
       iter != events.end();
       ++iter){
    (*iter)->saveAsToDisk(pathAndName);
  }
	

}


bool ProjectViewController::checkNameExists(string _name, EventType _type){
	vector<IEvent*>::iterator iter = events.begin();
	bool returnValue = false;
	while (!returnValue && iter != events.end()){

		if ((*iter)->getEventName() == _name && (*iter)->getEventType() == _type){
			returnValue = true;
		}

		iter++;
	} 

	return returnValue;

}


