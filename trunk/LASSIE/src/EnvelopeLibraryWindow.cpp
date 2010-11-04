/*******************************************************************************
 *
 *  File Name     : EnvelopeLibraryWindow.cpp
 *  Date created  : May. 15 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file implement the Envelope Library Window of LASSIE. 
 *                  The envelope library lists all the pre-made envelopes.
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
 
#include "EnvelopeLibraryWindow.h"

#include "EnvLibDrawingArea.h"
#include "ProjectViewController.h"
#include "EnvelopeLibraryEntry.h"
#include "MainWindow.h"

EnvelopeLibraryWindow::EnvelopeLibraryWindow(){
  set_title("Envelope Library");
    set_border_width(3);
    activeEnvelope = NULL;

  
  
  attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file("./LASSIE/src/UI/EnvelopeLibraryWindow.ui");
  }
  catch (const Glib::FileError& ex){
    std::cerr << "FileError: " << ex.what() << std::endl;
  }
  catch (const Gtk::BuilderError& ex){
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
   
   #else
  std::auto_ptr<Glib::Error> error;
  if (!attributesRefBuilder->add_from_file("./LASSIE/src/UI/EnvelopeLibraryWindow.ui", error)){
    std::cerr << error->what() << std::endl;
  }
   
   #endif
  
  /* try to enable save while envelope library window is active, but fail :(
    will do this later 
   menuRefActionGroup = Gtk::ActionGroup::create();  

  menuRefActionGroup->add(
    Gtk::Action::create("FileSave",Gtk::Stock::SAVE),
    sigc::mem_fun(*this, &EnvelopeLibraryWindow::fileSave));
  */
   
  Gtk::VBox* box;
   
  attributesRefBuilder->get_widget("vbox1", box);
   
	Gtk::Alignment* a;
  attributesRefBuilder->get_widget("DrawingAreaAlignment", a);
  drawingArea = new EnvLibDrawingArea(this);
	a->add(*drawingArea);


  add(*box);
  drawingArea->show();
  box->show_all();
  

    attributesRefBuilder->get_widget("EnvelopeListAlignment", a);
  
 
  a->add(scrolledWindow);
   
  
  // Add the TreeView, inside a ScrolledWindow
  scrolledWindow.add(envelopeLibrary);

  // Only show the scrollbars when they are necessary:
  scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);


  attributesRefBuilder->get_widget("CoordinateLabel", mouseCoordinate);



  // Create the Tree model:
  refTreeModel = Gtk::TreeStore::create(columns);
  envelopeLibrary.set_model(refTreeModel);

  // All the items to be reordered with drag-and-drop:
  //palette.set_reorderable();



  // Add the TreeView's view columns:
  envelopeLibrary.append_column("Number of Library", columns.columnObjectNumber);
  //palette.append_column("Name", columns.columnObjectName);
  // Connect signal:
  envelopeLibrary.signal_row_activated().connect(
    sigc::mem_fun(*this, &EnvelopeLibraryWindow::objectActivated) );
  envelopeLibrary.signal_cursor_changed().connect(
    sigc::mem_fun(*this,&EnvelopeLibraryWindow::on_cursor_changed) );

  //////////////////////////test for drag and drop///////////////////////////

  //this section is from palette view, has to modify variables name to use it


  //Make m_Button_Drag a DnD drag source:
  //palette.drag_source_set(projectView->listTargets);
   //viewName.drag_source_set(projectView->listTargets);
   //palette.enable_model_drag_source(
   //projectView->listTargets,Gdk::MODIFIER_MASK,Gdk::ACTION_COPY);


   //Connect signals: bind current event to the signal
   //palette.signal_drag_data_get().connect(
     //sigc::mem_fun(*this, &PaletteViewController::on_button_drag_data_get));

   //viewName.signal_drag_data_get().connect(
     //sigc::mem_fun(*this, &PaletteViewController::on_button_drag_data_get));

  //////////////////////end test ////////////////////////////////////////

  /////////////////////set up popup menu ///////////////////////////////

  m_pMenuPopup = 0;

  //    the signal handler i wished to put was overriden by the object default
  //signal handler, so to avoid this, we have to add a paramater to the connect
  //method (false)

  envelopeLibrary.signal_button_press_event().connect(
    sigc::mem_fun(*this,&EnvelopeLibraryWindow::onRightClick), false);



  //Fill menu:

  m_refActionGroup = Gtk::ActionGroup::create();

  //File|New sub menu:
  //These menu actions would normally already exist for a main menu, because a
  //context menu should not normally contain menu items that are only available
  //via a context menu.
  m_refActionGroup->add(Gtk::Action::create("ContextMenu", "Context Menu"));

   m_refActionGroup->add(
    Gtk::Action::create("ContextAdd", "Create New Envelope"),
    sigc::mem_fun(*this, &EnvelopeLibraryWindow::createNewEnvelope));

  //TODO:
  /*
   //Add a ImageMenuElem:
   menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("_Something", m_Image,
   sigc::mem_fun(*this, &ExampleWindow::on_menu_file_popup_generic) ) ) ;
   */

  m_refUIManager = Gtk::UIManager::create();
  m_refUIManager->insert_action_group(m_refActionGroup);

  //projectView->add_accel_group(m_refUIManager->get_accel_group());

  //Layout the actions in a menubar and toolbar:
  
  
  Glib::ustring ui_info =
    "<ui>"
    "  <popup name='PopupMenu'>"
    "    <menuitem action='ContextAdd'/>"
    "  </popup>"
    "</ui>";

  #ifdef GLIBMM_EXCEPTIONS_ENABLED
    try{
      m_refUIManager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex){
      std::cerr << "building menus failed: " <<  ex.what();
    }
  #else
    std::auto_ptr<Glib::Error> ex;
    m_refUIManager->add_ui_from_string(ui_info, ex);
    if(ex.get()){
      std::cerr << "building menus failed: " <<  ex->what();
    }
  #endif // GLIBMM_EXCEPTIONS_ENABLED

  // Get the menu:
  m_pMenuPopup = dynamic_cast<Gtk::Menu*>(
    m_refUIManager->get_widget("/PopupMenu"));

  if(!m_pMenuPopup) g_warning("menu not found");
  
  
  
   show_all();
  
}

EnvelopeLibraryWindow::~EnvelopeLibraryWindow(){

}


  


bool EnvelopeLibraryWindow::onRightClick(GdkEventButton* event){
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ){ // test if right click
    if(m_pMenuPopup) m_pMenuPopup->popup(event->button, event->time);

    return true; // It has been handled.
  }else{
    return false;
  }
}

void EnvelopeLibraryWindow::createNewEnvelope(){
  EnvelopeLibraryEntry* newEnvelope = activeProject->createNewEnvelope();
  
  Gtk::TreeModel::Row childrow= *(refTreeModel->append());

  //Gtk::TreeModel::Row childrow = *(refTreeModel->append(selectedRow.children()));
  childrow[columns.columnObjectNumber] = newEnvelope->getNumberString();

  //childrow[columns.columnButton].set_text(_event->getEventName());
  childrow[columns.columnEntry] =newEnvelope;


  


}



void EnvelopeLibraryWindow::refreshEnvelopeList(){
	//this method is very inefficient, but for the first version we'll do this for
	//now.
	
	//remove the current refTreeModel and make a new one
	refTreeModel = Gtk::TreeStore::create(columns);
  envelopeLibrary.set_model(refTreeModel);
	



}





void EnvelopeLibraryWindow::objectActivated(
  const Gtk::TreeModel::Path& _path, Gtk::TreeViewColumn* /* _column */){

  Gtk::TreeModel::iterator iter = refTreeModel->get_iter(_path);

  if(iter){
    Gtk::TreeModel::Row row = *iter;
    selectedRow = *iter;
    //std::cout<<"some object is activated in EnvelopeLibraryWindow::objectActivated()"<<std::endl;
    //projectView->showAttributes(row[columns.columnEntry]);
  }
}

/*
void PaletteViewController::insertEvent(Event* _event){
  Gtk::TreeModel::Row childrow;
  if(palette.get_selection()->get_selected() ==0){ //see if some row is selected
    childrow = *(refTreeModel->append());
  }else{
    Gtk::TreeModel::Children::iterator iter = palette.get_selection()->get_selected();
    Gtk::TreeModel::Row parent = *iter;
    childrow = *(refTreeModel->append(parent.children()));
  }

  //Gtk::TreeModel::Row childrow = *(refTreeModel->append(selectedRow.children()));
  childrow[columns.columnObjectType] = _event->getEventTypeString();
  childrow[columns.columnObjectName] = _event->getEventName();
  //childrow[columns.columnButton].set_text(_event->getEventName());
  childrow[columns.columnEntry] =_event;
  //_event->setRowInPalette(&childrow);
}

*/


/*
void PaletteViewController::insertEvent(Event* _event, std::string _parentName){
  Gtk::TreeModel::Row insertTo;
  //find the row of the given name
  Gtk::TreeModel::Children children = refTreeModel->children();

  for(Gtk::TreeModel::Children::iterator iter = children.begin();
      iter != children.end();
      ++iter){
    Gtk::TreeModel::Row row = *iter;
    if (row[columns.columnObjectName] == _parentName) insertTo = row;
  }

  //insert new event
  Gtk::TreeModel::Row childrow = *(refTreeModel->append(insertTo.children()));
  childrow[columns.columnObjectType] = _event->getEventTypeString();
  childrow[columns.columnObjectName] = _event->getEventName();
  childrow[columns.columnEntry] = _event;
}

*/

/*
//called when object name is changed in the atributesview
void PaletteViewController::refreshObjectName(Event* _event){
  Gtk::TreeModel::Row toChange;
  //find the row of the given event
  Gtk::TreeModel::Children children = refTreeModel->children();
  for(Gtk::TreeModel::Children::iterator iter = children.begin();
      iter != children.end();
      iter++){
    Gtk::TreeModel::Row row = *iter;
    //std::cout<<row[columns.columnObjectName]<<std::endl;
    if (row[columns.columnEntry] == _event) toChange = row;
}
 //std::cout<<toChange[columns.columnObjectName]<<std::endl;
//toChange[columns.columnObjectName] = _event->getEventName();
}
*/

/*
void PaletteViewController::on_button_drag_data_get(
  const Glib::RefPtr<Gdk::DragContext>& context,
  Gtk::SelectionData& selection_data,
  guint info,
  guint time){
   //keep this statement alive. removing it causes the recieving site not responding :(
   //selection_data.set(selection_data.get_target(), 8 /* 8 bits format */ //,
     //    (const guchar*)"I'm Data!",
       //9 /* the length of I'm Data! in bytes */);*/

  //std::cout<<"187  "<<palette.get_selection()->get_model()->get_string(palette.get_selection()->get_selected())<<std::endl;
  //std::cout<<"188 "<<palette.get_selection()->get_model()->get_path(palette.get_selection()->get_selected())<<std::endl;
/*
  Gtk::TreeModel::Children::iterator iter = palette.get_selection()->get_selected();
  Gtk::TreeModel::Row row = *iter;


  //right here set up a pointer to the right event object
  //std::cout<<"Copy Signal sent. Object: "<<row[columns.columnObjectName]<<std::endl;
  Glib::ustring name = row[columns.columnObjectName];

  selection_data.set(
    selection_data.get_target(),
    8 /* 8 bits format *//* ,  
    (const guchar*)name.c_str(),
    name.length() /* the length of I'm Data! in bytes );
}

*/
//originally want to show the attributes whenever the cursor move, but it's dangerous so skipped. leave the function body for future usage.
void EnvelopeLibraryWindow::on_cursor_changed(){
//TODO
  //std::cout<<"cursor changed!! show graph of envelope #";
  Gtk::TreeModel::Children::iterator iter = envelopeLibrary.get_selection()->get_selected();
  Gtk::TreeModel::Row row = *iter;
  //std::cout<<row[columns.columnObjectNumber]<<std::endl;

  activeEnvelope = row[columns.columnEntry];
  drawingArea->showGraph(row[columns.columnEntry]);

  //projectView->showAttributes(row[columns.columnEntry]);
}



//return None if not a folder is selected


/*


Event* PaletteViewController::getCurrentSelectedEvent(){
  //std::cout<<"cursor changed!!"<<std::endl;
  Gtk::TreeModel::Children::iterator iter = palette.get_selection()->get_selected();
  Gtk::TreeModel::Row row = *iter;
  return row[columns.columnEntry];
}
*/




void EnvelopeLibraryWindow::setActiveProject(ProjectViewController* _project){
	//clean up the drawing area
	drawingArea->clearGraph();
  
  
  activeProject = _project;
  refreshEnvelopeList();
  
  EnvelopeLibraryEntry* thisEntry = activeProject->getEnvelopeLibraryEntries();
  while (thisEntry != NULL){
    Gtk::TreeModel::Row childrow= *(refTreeModel->append());
    childrow[columns.columnObjectNumber] = thisEntry->getNumberString();
    childrow[columns.columnEntry] =thisEntry;
    thisEntry = thisEntry->next;  
  }
  activeEnvelope = NULL;

}

EnvelopeLibraryEntry* EnvelopeLibraryWindow::getActiveEnvelope(){
  
  return activeEnvelope;
}



  

