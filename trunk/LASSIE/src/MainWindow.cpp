/*******************************************************************************
*
*  File Name     : MainWindow.cpp
*  Date created  : Jan. 24 2010
*  Authors       : Ming-ching Chiu, Sever Tipei
*  Organization  : Music School, University of Illinois at Urbana Champaign
*  Description   : This file contains the class of main window of LASSIE
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

#include "MainWindow.h"

#include "ProjectViewController.h"
#include "FileOperations.h"
#include "EditOperations.h"
#include "HelpOperations.h"
#include "EnvelopeLibraryWindow.h"

/*! \brief The constructor of MainWindow
*
*******************************************************************************/
MainWindow::MainWindow(){

  set_title("LASSIE");
  set_default_size(800, 600);

  add(mainBox); // put the main box into the window


  // Create actions for menus and toolbars:
  menuRefActionGroup = Gtk::ActionGroup::create();

  // File|New sub menu:
  menuRefActionGroup->add(
    Gtk::Action::create("Project","Project","Create a new project"),
    sigc::mem_fun(*this, &MainWindow::menuFileNewProject));

  menuRefActionGroup->add(
    Gtk::Action::create("Object",Gtk::Stock::NEW,"_Object"),
    sigc::mem_fun(*this, &MainWindow::menuFileNewObject));


  // File and its sub-menu
  menuRefActionGroup->add(Gtk::Action::create("FileMenu", "File"));

  menuRefActionGroup->add(Gtk::Action::create("FileNew", "New"));

  menuRefActionGroup->add(
    Gtk::Action::create("FileOpen",Gtk::Stock::OPEN),
    sigc::mem_fun(*this, &MainWindow::menuFileOpen));

  menuRefActionGroup->add(
    Gtk::Action::create("FileSave",Gtk::Stock::SAVE),
    sigc::mem_fun(*this, &MainWindow::menuFileSave));

  menuRefActionGroup->add(
    Gtk::Action::create("FileSaveAs",Gtk::Stock::SAVE_AS),
    sigc::mem_fun(*this, &MainWindow::menuFileSaveAs));

  menuRefActionGroup->add(
    Gtk::Action::create("FileClose",Gtk::Stock::CLOSE),
    sigc::mem_fun(*this, &MainWindow::menuFileClose));

  menuRefActionGroup->add(
    Gtk::Action::create("FilePrint",Gtk::Stock::PRINT),
    sigc::mem_fun(*this, &MainWindow::menuFilePrint));

  menuRefActionGroup->add(
    Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
    sigc::mem_fun(*this, &MainWindow::menuFileQuit));


  // Edit and its sub-menu
  menuRefActionGroup->add(Gtk::Action::create("EditMenu", "Edit"));

  menuRefActionGroup->add(
    Gtk::Action::create("EditUndo", Gtk::Stock::UNDO),
    sigc::mem_fun(*this, &MainWindow::menuEditUndo));

  menuRefActionGroup->add(
    Gtk::Action::create("EditRedo", Gtk::Stock::REDO),
    sigc::mem_fun(*this, &MainWindow::menuEditUndo));

  menuRefActionGroup->add(
    Gtk::Action::create("EditCut", Gtk::Stock::CUT),
    sigc::mem_fun(*this, &MainWindow::menuEditCut));

  menuRefActionGroup->add(
    Gtk::Action::create("EditCopy", Gtk::Stock::COPY),
    sigc::mem_fun(*this, &MainWindow::menuEditCopy));

  menuRefActionGroup->add(
    Gtk::Action::create("EditPaste", Gtk::Stock::PASTE),
    sigc::mem_fun(*this, &MainWindow::menuEditPaste));

  menuRefActionGroup->add(
    Gtk::Action::create("EditSelectAll", Gtk::Stock::SELECT_ALL),
    sigc::mem_fun(*this, &MainWindow::menuEditSelectAll));

  menuRefActionGroup->add(
    Gtk::Action::create("EditPreferences", Gtk::Stock::PREFERENCES),
    sigc::mem_fun(*this, &MainWindow::menuEditSelectAll));


  // Project Menu
  menuRefActionGroup->add(Gtk::Action::create("ProjectMenu", "Project"));

  menuRefActionGroup->add(
    Gtk::Action::create("ProjectProperties", Gtk::Stock::PROPERTIES),
    sigc::mem_fun(*this, &MainWindow::menuProjectProperties));

  menuRefActionGroup->add(
    Gtk::Action::create("Synthesize", "Synthesize"),
    sigc::mem_fun(*this, &MainWindow::menuProjectSynthesize));


  // Help and its sub-menu
  menuRefActionGroup->add( Gtk::Action::create("HelpMenu", "Help"));

  menuRefActionGroup->add( 
    Gtk::Action::create("HelpContents",Gtk::Stock::HELP, "Contents"),
    sigc::mem_fun(*this, &MainWindow::menuHelpContents));

  menuRefActionGroup->add(
    Gtk::Action::create("HelpAbout", Gtk::Stock::ABOUT),
    sigc::mem_fun(*this, &MainWindow::menuHelpAbout));



  // UIManager reads the XML below to layout the menu.
  // add_accel_group activates using hotkeys within this window.
  menuRefUIManager = Gtk::UIManager::create();
  menuRefUIManager->insert_action_group(menuRefActionGroup);
  add_accel_group(menuRefUIManager->get_accel_group());



  // Layout the actions in a menubar and toolbar:
  ui_info_befor = 
        "<ui>"
        "  <menubar name='menuBar'>"
        "    <menu action='FileMenu'>"
        "      <menu action='FileNew'>"
        "        <menuitem action='Project'/>"
        "        <menuitem action='Object'/>"
        "      </menu>"
        "      <separator/>"
        "      <menuitem action='FileOpen'/>"
        "      <menuitem action='FileSave'/>"
        "      <menuitem action='FileSaveAs'/>"
        "      <menuitem action='FileClose'/>"
        "      <menuitem action='FilePrint'/>"
        "      <separator/>";

  ui_info_after = 
        "      <separator/>"
        "      <menuitem action='FileQuit'/>"
        "    </menu>"
        "    <menu action='EditMenu'>"
        "      <menuitem action='EditUndo'/>"
        "      <menuitem action='EditRedo'/>"
        "      <separator/>"
        "      <menuitem action='EditCut'/>"
        "      <menuitem action='EditCopy'/>"
        "      <menuitem action='EditPaste'/>"
        "      <menuitem action='EditSelectAll'/>"
        "      <separator/>"
        "      <menuitem action='EditPreferences'/>"
        "    </menu>"
        "     <menu action='ProjectMenu'>"
        "      <menuitem action='ProjectProperties'/>"
        "      <menuitem action = 'Synthesize'/>"
        "    </menu>"
        "    <menu action='HelpMenu'>"
        "      <menuitem action='HelpContents'/>"
        "      <menuitem action='HelpAbout'/>"
        "    </menu>"
        "  </menubar>"
        "  <toolbar  name='ToolBar'>"
        "    <toolitem action='Object'/>"
        "    <toolitem action='FileOpen'/>"
        "    <toolitem action='FileSave'/>"
        "    <toolitem action='FileClose'/>"
        "    <toolitem action='FilePrint'/>"
        "    <separator/>"
        "    <toolitem action='EditCut'/>"
        "    <toolitem action='EditCopy'/>"
        "    <toolitem action='EditPaste'/>"
        "    <separator/>"
        "    <toolitem action='HelpContents'/>"
        "  </toolbar>"
        "</ui>";

  ui_info_action = "";

  ui_info = ui_info_befor + ui_info_after;

  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    menuRefUIManager->add_ui_from_string(ui_info);
  }
  catch(const Glib::Error& ex){
    std::cerr << "building menus failed: " <<  ex.what();
  }
  #else
  std::auto_ptr<Glib::Error> ex;
  menuRefUIManager->add_ui_from_string(ui_info, ex);
  if(ex.get()){
    std::cerr << "building menus failed: " <<  ex->what();
  }
  #endif // GLIBMM_EXCEPTIONS_ENABLED


  // Get the menubar and toolbar widgets, and add them to mainBox container.

  pointerToMenubarWidget = 
    menuRefUIManager->get_widget("/menuBar");
  if(pointerToMenubarWidget){ // add menu 
    mainBox.pack_start(*pointerToMenubarWidget, Gtk::PACK_SHRINK);
  }	
  pointerToToolbarWidget = 
    menuRefUIManager->get_widget("/ToolBar");
  if(pointerToToolbarWidget){ // add tool
    mainBox.pack_start(*pointerToToolbarWidget, Gtk::PACK_SHRINK);
  }


  // make an empty project view to the mainbox. 
  // notice that when adding projectView, Pack_EXPAND_WIDGET is used,
  // not PACK_SHRINK,
  // because we want the projectView to fill in the space when 
  // the main window expand.  
  projectView = new ProjectViewController(this);  
  mainBox.pack_start(*projectView, Gtk::PACK_EXPAND_WIDGET); 
  // put the pointer of the current projectView into <vector> projects
  projects.push_back(projectView);
  

  
    // create an instance of EnvelopeLibraryWindow
  //envelopeLibraryWindow = (EnvelopeLibraryWindow*) new Gtk::Window();
  envelopeLibraryWindow = new EnvelopeLibraryWindow();
  envelopeLibraryWindow->setActiveProject(projectView);
  

  

  Gtk::Main::signal_key_snooper().connect(sigc::mem_fun(*this,&MainWindow::captureKeyStroke));
  
 
  show_all_children();
}


/*! \brief The destructor of MainWindow
*
*******************************************************************************/
MainWindow::~MainWindow(){
  for(std::vector<ProjectViewController*>::
        iterator it = projects.begin(); 
      it!=projects.end();
      it++){

    delete *it;
  }
}

// here is the "File menu" action functions block
////////////////////////////////////////////////////////////////////////////////
void MainWindow::menuFileNewProject(){
  ProjectViewController* newProject = FileOperations::newProject(this);

  if(newProject!= NULL){
    projects.push_back(newProject);
    MainWindow::includeUi_info(newProject->getPathAndName(),"add");
    changeCurrentProjectViewTo(newProject);

    std::string title = "LASSIE - ";
    title += newProject->getPathAndName();
    set_title(title);
    newProject->setProperties();
  }
}

void MainWindow::menuFileNewObject(){
  projectView->insertObject();
}

void MainWindow::menuFileOpen(){
  ProjectViewController* openProject = FileOperations::openProject(this);

  if(openProject!= NULL){
    projects.push_back(openProject);
    MainWindow::includeUi_info(openProject->getPathAndName(),"add");
    changeCurrentProjectViewTo(openProject);
    std::string title = "LASSIE - ";
    title += openProject->getPathAndName();
    set_title(title);
    openProject->setProperties();
    
    envelopeLibraryWindow->setActiveProject(openProject);
    
    
  }
  
}

void MainWindow::menuFileSave(){
  if (projectView->getEmptyProject()== false){
    projectView->save();
  }
}

void MainWindow::menuFileSaveAs(){}//TODO

void MainWindow::menuFileClose(){
  FileOperations::close(this);
}

void MainWindow::menuFilePrint(){}//TODO

void MainWindow::menuFileQuit(){	
  hide(); //Closes the main window to stop the Gtk::Main::run().
}
////////////////////////////////////////////////////////////////////////////////

// here is the "Edit menu" action functions block
////////////////////////////////////////////////////////////////////////////////
void MainWindow::menuEditUndo(){}//TODO
void MainWindow::menuEditRedo(){}//TODO
void MainWindow::menuEditCut(){}//TODO
void MainWindow::menuEditCopy(){}//TODO
void MainWindow::menuEditPaste(){}//TODO
void MainWindow::menuEditSelectAll(){}//TODO
void MainWindow::menuEditPreferences(){}//TODO
////////////////////////////////////////////////////////////////////////////////

// here is the "Help menu" action functions block
////////////////////////////////////////////////////////////////////////////////
void MainWindow::menuHelpContents(){
  HelpOperations::showContents();
}

void MainWindow::menuHelpAbout(){
  HelpOperations::showAbout();
}
////////////////////////////////////////////////////////////////////////////////

void MainWindow::changeCurrentProjectViewTo(
                  ProjectViewController* _newProject)
{
  mainBox.remove(*projectView);
  mainBox.pack_start(*_newProject, Gtk::PACK_EXPAND_WIDGET); 
  projectView = _newProject;
  envelopeLibraryWindow->setActiveProject(_newProject);
  
  show_all_children();
}


void MainWindow::menuPathAndName(){}//TODO


void MainWindow::includeUi_info(Glib::ustring pathAndName,
                                Glib::ustring ctrl)
{
  if(ctrl == "add"){
    mainBox.remove(*pointerToMenubarWidget);
    mainBox.remove(*pointerToToolbarWidget);

    ui_info_action = ui_info_action +
                     "<menuitem action='" +
                     pathAndName +
                        "'/>";

    ui_info = ui_info_befor + ui_info_action + ui_info_after;

    menuRefActionGroup->add(
      Gtk::Action::create(pathAndName,pathAndName),
      sigc::mem_fun(*this, &MainWindow::menuPathAndName)
    );

    menuRefUIManager = Gtk::UIManager::create();
    menuRefUIManager->insert_action_group(menuRefActionGroup);
    add_accel_group(menuRefUIManager->get_accel_group());

    menuRefUIManager->add_ui_from_string(ui_info);

    pointerToMenubarWidget = menuRefUIManager->get_widget("/menuBar");
    if(pointerToMenubarWidget){ // add menu 
      mainBox.pack_start(*pointerToMenubarWidget, Gtk::PACK_SHRINK);
    }

    pointerToToolbarWidget = menuRefUIManager->get_widget("/ToolBar");
    if(pointerToToolbarWidget){ // add tool
      mainBox.pack_start(*pointerToToolbarWidget, Gtk::PACK_SHRINK);
    }
  }else if(ctrl == "delete"){}//TODO
}



void MainWindow::menuProjectProperties(){
	projectView->setProperties();
}

/* to do:
*    implements functions and function descriptions
*/


void MainWindow::showEnvelopeLibraryWindow(){
  envelopeLibraryWindow->show();

}


int   MainWindow::captureKeyStroke(Gtk::Widget* _widget,GdkEventKey* _gdkEventKey){

  if (_gdkEventKey->type ==8 &&_gdkEventKey->keyval == 65535){ //delete key stroke
    if (projectView != NULL){
      projectView->deleteKeyPressed(get_focus());
    }
  }
    
  /*
 GdkEventType type;   8 press 9 release
 GdkWindow *window; this window
 gint8 send_event; don't know what's this
 guint32 time; event time 
 guint state; usually 16 ctrl keys otherwise
 guint keyval; ascii
 gint length;
 gchar *string;  return ascii code
 guint16 hardware_keycode; keyboard key code
 guint8 group;
  
  
  */
  
  

  return 0;
}



void MainWindow::menuProjectSynthesize(){
	Gtk::Dialog* synthesizeDialog;


  //Load the GtkBuilder file and instantiate its widgets:
  Glib::RefPtr<Gtk::Builder> synthesizeDialogRefBuilder = Gtk::Builder::create();

  #ifdef GLIBMM_EXCEPTIONS_ENABLED
    try{
      synthesizeDialogRefBuilder->add_from_file("./LASSIE/src/UI/SynthesizeDialog.ui");
    }
    catch(const Glib::FileError& ex){
      std::cerr << "FileError: " << ex.what() << std::endl;
    }
    catch(const Gtk::BuilderError& ex){
      std::cerr << "BuilderError: " << ex.what() << std::endl;
    }
  #else
    std::auto_ptr<Glib::Error> error;

    if (!synthesizeDialogRefBuilder->add_from_file("./LASSIE/src/UI/SynthesizeDialog.ui", error)){
      std::cerr << error->what() << std::endl;
    }
  #endif /* !GLIBMM_EXCEPTIONS_ENABLED */

	
  //Get the GtkBuilder-instantiated Dialog:
  synthesizeDialogRefBuilder->get_widget("SynthesizeDialog", synthesizeDialog);
	



  Gtk::Entry* randomSeedEntry;
  synthesizeDialogRefBuilder->get_widget("SynthesizeDialogRandomSeedEntry", randomSeedEntry);


  int result = synthesizeDialog->run();

	synthesizeDialog->hide(); 

  if(result == 1 && projectView) {
    string pathAndName = projectView->getPathAndName();
    string projectPath = pathAndName + "/";
    string projectName = FileOperations::stringToFileName(pathAndName);
    
    //Determine project sound file output.
    PieceHelper::createSoundFilesDirectory(projectPath);
    string soundFilename =
      PieceHelper::getNextSoundFile(projectPath, projectName);
      
    if(soundFilename == "")
      cout << "A new soundfile name could not be reserved." << endl;
      //TODO: Make message box to warn user. (But this error should probably not happen!)
    else
      PieceHelper::createPiece(projectPath, projectName, randomSeedEntry->get_text(), soundFilename);
	}
}

