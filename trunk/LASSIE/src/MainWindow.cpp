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
extern YY_BUFFER_STATE yy_create_buffer ( FILE *file, int size );
extern void yy_switch_to_buffer ( YY_BUFFER_STATE new_buffer );
extern void yy_delete_buffer ( YY_BUFFER_STATE buffer );
extern FILE* yyin;

#ifndef YY_BUF_SIZE
#ifdef __ia64__
/* On IA-64, the buffer size is 16k, not 8k.
 * Moreover, YY_BUF_SIZE is 2*YY_READ_BUF_SIZE in the general case.
 * Ditto for the __ia64__ case accordingly.
 */
#define YY_BUF_SIZE 32768
#else
#define YY_BUF_SIZE 16384
#endif /* __ia64__ */
#endif














/*! \brief The constructor of MainWindow
*
*******************************************************************************/
MainWindow::MainWindow(){

  set_title("LASSIE");
  set_default_size(800, 600);
  set_decorated(true);
  

  add(mainBox); // put the main box into the window


  // Create actions for menus and toolbars:
  menuRefActionGroup = Gtk::ActionGroup::create();

  // File|New sub menu:
  menuRefActionGroup->add(
    Gtk::Action::create("FileNewProject",Gtk::Stock::NEW,"Create a new project"),
    sigc::mem_fun(*this, &MainWindow::menuFileNewProject));

  menuRefActionGroup->add(
    Gtk::Action::create("FileNewObject",Gtk::Stock::ADD,"_Create a new Object", "Create a new Object"),
		Gtk::AccelKey("N"),
    sigc::mem_fun(*this, &MainWindow::menuFileNewObject));


  // File and its sub-menu
  menuRefActionGroup->add(Gtk::Action::create("FileMenu", "File"));

  //menuRefActionGroup->add(Gtk::Action::create("FileNew", "New"));

  menuRefActionGroup->add(
    Gtk::Action::create("FileOpen",Gtk::Stock::OPEN, "_Open an existing project","Open an existing project"),
    sigc::mem_fun(*this, &MainWindow::menuFileOpen));

  menuRefActionGroup->add(
    Gtk::Action::create("FileSave",Gtk::Stock::SAVE,"_Save the project","Save the project"),
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



  menuRefActionGroup->get_action("FileNewObject")->set_sensitive(false); 
  //menuRefActionGroup->get_action("FileNewObject")->add_accel_label("N");    
  menuRefActionGroup->get_action("FileSave")->set_sensitive(false);
  menuRefActionGroup->get_action("ProjectProperties")->set_sensitive(false);
  menuRefActionGroup->get_action("Synthesize")->set_sensitive(false);
  



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
//        "      <menu action='FileNew'>"
//        "        <menuitem action='Project'/>"
//        "        <menuitem action='Object'/>"
//        "      </menu>"
//        "      <separator/>"
        "      <menuitem action='FileNewProject'/>"

        "      <menuitem action='FileOpen'/>"
        "      <menuitem action='FileSave'/>"
//        "      <menuitem action='FileSaveAs'/>"
//        "      <menuitem action='FileClose'/>"
//        "      <menuitem action='FilePrint'/>"
        "      <separator/>";

  ui_info_after = 
        "      <separator/>"
        "      <menuitem action='FileQuit'/>"
        "    </menu>"
        "    <menu action='EditMenu'>"
//        "      <menuitem action='EditUndo'/>"
//        "      <menuitem action='EditRedo'/>"
        "      <separator/>"
//        "      <menuitem action='EditCut'/>"
//        "      <menuitem action='EditCopy'/>"
//        "      <menuitem action='EditPaste'/>"
//        "      <menuitem action='EditSelectAll'/>"
//        "      <separator/>"
//        "      <menuitem action='EditPreferences'/>"
        "    </menu>"
        "     <menu action='ProjectMenu'>"
        "      <menuitem action='FileNewObject'/>"
        "      <menuitem action='ProjectProperties'/>"
        "      <menuitem action = 'Synthesize'/>"
        "    </menu>"
        "    <menu action='HelpMenu'>"
        "      <menuitem action='HelpContents'/>"
        "      <menuitem action='HelpAbout'/>"
        "    </menu>"
        "  </menubar>"
        "  <toolbar  name='ToolBar'>"
        "    <toolitem action = 'FileNewProject'/>"
        "    <toolitem action='FileOpen'/>"
        "    <toolitem action='FileSave'/>"
//        "    <toolitem action='FileClose'/>"
//        "    <toolitem action='FilePrint'/>"
        "    <separator/>"
//        "    <toolitem action='EditCut'/>"
//        "    <toolitem action='EditCopy'/>"
//        "    <toolitem action='EditPaste'/>"
        "    <toolitem action='FileNewObject'/>"
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


  //Gtk::MenuItem* newObject = (Gtk::MenuItem*)menuRefUIManager->get_widget("/FileNewObject");
	//newObject->add_accel_label("N");



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
  envelopeLibraryWindow->hide();
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
    menuRefActionGroup->get_action("FileNewObject")->set_sensitive(true);
    menuRefActionGroup->get_action("FileSave")->set_sensitive(true);
    menuRefActionGroup->get_action("ProjectProperties")->set_sensitive(true);
    menuRefActionGroup->get_action("Synthesize")->set_sensitive(true);

    projects.push_back(newProject);
    MainWindow::includeUi_info(newProject->getPathAndName(),"add");
    changeCurrentProjectViewTo(newProject);

    std::string title = " - LASSIE";
    title = "*"+ newProject->getPathAndName()+ title;
    set_title(title);
    newProject->setProperties();
  }
}

void MainWindow::menuFileNewObject(){
  if (projectView!= NULL &&projectView->getEmptyProject() == false){
    projectView->insertObject();
  }
}

void MainWindow::menuFileOpen(){
  ProjectViewController* openProject = FileOperations::openProject(this);

  if(openProject!= NULL){
    menuRefActionGroup->get_action("FileNewObject")->set_sensitive(true);
    menuRefActionGroup->get_action("FileSave")->set_sensitive(true);
    menuRefActionGroup->get_action("ProjectProperties")->set_sensitive(true);
    menuRefActionGroup->get_action("Synthesize")->set_sensitive(true);
      
    projects.push_back(openProject);
    MainWindow::includeUi_info(openProject->getPathAndName(),"add");
    changeCurrentProjectViewTo(openProject);
    std::string title = " - LASSIE";
    title = openProject->getPathAndName() + title;
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
void MainWindow::setSavedTitle(){
    std::string title = " - LASSIE";
    title = projectView->getPathAndName() + title;
    set_title(title);
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


int MainWindow::captureKeyStroke(Gtk::Widget* _widget,GdkEventKey* _gdkEventKey){

  if (_gdkEventKey->type ==8 &&_gdkEventKey->keyval == 65535){ //delete key stroke
    if (projectView != NULL&& projectView->getPathAndName() != " "){
      projectView->deleteKeyPressed(get_focus());
    }
  }
   
  /*  //this chunk is handled by accelkey of FileNewObject
	if (_gdkEventKey->type == 8 &&_gdkEventKey->keyval == 110){ //n stroke!
		if (projectView!= NULL&& projectView->getPathAndName() != " "){
			projectView->nKeyPressed(get_focus());// for palette to add object
		}
	} 
	*/
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
    string dat = projectPath + projectName + ".dat";
    
    
    
    //FILE* newBufferFile = fopen (dat.c_str(), "r");
    yyin = fopen (dat.c_str(), "r");
    
    //cout<<"LASSIE::MainWindow::synthesize: Synthesizing, filename is: "<< dat<<endl;
    // create a new buffer for parser
    YY_BUFFER_STATE newParserBuffer = yy_create_buffer ( yyin,YY_BUF_SIZE);
    //YY_BUFFER_STATE newParserBuffer = yy_create_buffer ( newBufferFile,YY_BUF_SIZE);
    //cout<<"LASSIE::MainWindow::synthesize: create new yy_buffer_state done"<<endl;
    
    yy_switch_to_buffer (newParserBuffer);
    
    //Determine project sound file output.
    PieceHelper::createSoundFilesDirectory(projectPath);
    string soundFilename =
      PieceHelper::getNextSoundFile(projectPath, projectName);
      
      
      
    if(soundFilename == "")
      cout << "A new soundfile name could not be reserved." << endl;
      //TODO: Make message box to warn user. (But this error should probably not happen!)
    else {
      
      //if (fork() == 0){
      //cout<<"this is the child. execute cmod"<<endl;
      
      string command = "./cmod " + projectPath + "&"; 
        system(command.c_str()) ;
      //}
      //else {
        //cout<<"This is the parent"<<endl;
    	  yy_delete_buffer(newParserBuffer);      
      //}
      
      
      //PieceHelper::createPiece(projectPath, projectName, randomSeedEntry->get_text(), soundFilename);
	
	  //yy_delete_buffer(newParserBuffer);
	  
	  //fclose(newBufferFile);
	  }
	  
	}
	
}




void MainWindow::setUnsavedTitle(){
  std::string title = " - LASSIE";
  title = "*" + projectView->getPathAndName() + title;
  set_title(title);
   
}

