/*******************************************************************************
 *
 *  File Name     : EnvLibDrawingArea.cpp
 *  Date created  : Jan. 29 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : 
 *
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
 
#include "EnvLibDrawingArea.h"

#include "EnvelopeLibraryWindow.h"
#include "EnvelopeLibraryEntry.h"
#include "ProjectViewController.h"
EnvLibDrawingArea::EnvLibDrawingArea(EnvelopeLibraryWindow* _envelopeLibraryWindow)
{
  envelopeLibraryWindow = _envelopeLibraryWindow;
  activeSegment = NULL;
  headChosen = false;
	add_events (Gdk::POINTER_MOTION_MASK | Gdk::ENTER_NOTIFY_MASK | Gdk::BUTTON_PRESS_MASK |Gdk::BUTTON_RELEASE_MASK);
	//signal_enter_notify_event().connect(sigc::mem_fun(*this,&MyArea::test));
	/* toolUndo->signal_button_press_event().connect(sigc::mem_fun(*this,&MainWindow::nothingClick)); toolUndo->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::nothingToDo));
*/

	signal_motion_notify_event().connect(sigc::mem_fun(*this,&EnvLibDrawingArea::mouseMoving));
	signal_button_press_event().connect(sigc::mem_fun(*this,&EnvLibDrawingArea::mouseButtonPressed));
	signal_button_release_event().connect(sigc::mem_fun(*this,&EnvLibDrawingArea::mouseButtonReleased));
	mouseLeftButtonPressedDown = false;
	
	
	
	 /////////////////////set up popup menu ///////////////////////////////

  m_pMenuPopup = 0;

  //    the signal handler i wished to put was overriden by the object default
  //signal handler, so to avoid this, we have to add a paramater to the connect
  //method (false)


  //Fill menu:

  m_refActionGroup = Gtk::ActionGroup::create();

  //File|New sub menu:
  //These menu actions would normally already exist for a main menu, because a
  //context menu should not normally contain menu items that are only available
  //via a context menu.
  m_refActionGroup->add(Gtk::Action::create("ContextMenu", "Context Menu"));


   m_refActionGroup->add(
    Gtk::Action::create("ContextAdd", "Insert Node"),
    sigc::mem_fun(*this, &EnvLibDrawingArea::insertEnvelopeSegment));
   m_refActionGroup->add(
    Gtk::Action::create("ContextSetFlexible", "Set Flexible"),
    sigc::mem_fun(*this, &EnvLibDrawingArea::setFlexible));
   m_refActionGroup->add(
    Gtk::Action::create("ContextSetFixed", "Set Fixed"),
    sigc::mem_fun(*this, &EnvLibDrawingArea::setFixed)); 

   m_refActionGroup->add(
    Gtk::Action::create("ContextSetLinear", "Set Linear"),
    sigc::mem_fun(*this, &EnvLibDrawingArea::setLinear));
   m_refActionGroup->add(
    Gtk::Action::create("ContextSetSpline", "Set Spline"),
    sigc::mem_fun(*this, &EnvLibDrawingArea::setSpline));  
   m_refActionGroup->add(
    Gtk::Action::create("ContextSetExponential", "Set Exponential"),
    sigc::mem_fun(*this, &EnvLibDrawingArea::setExponential));
    
    
    
       
    
    

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
    "    <separator/>"
    "    <menuitem action ='ContextSetFixed' />"    
    "    <menuitem action ='ContextSetFlexible' />"
    "    <separator/>"
    "    <menuitem action ='ContextSetLinear' />"    
    "    <menuitem action ='ContextSetSpline' />"    
    "    <menuitem action ='ContextSetExponential' />"
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
  
	
	
	
	
}

EnvLibDrawingArea::~EnvLibDrawingArea()
{
}

bool EnvLibDrawingArea::on_expose_event(GdkEventExpose* event)
{
  EnvelopeLibraryEntry* activeEnvelope = envelopeLibraryWindow->getActiveEnvelope();
  
  if (activeEnvelope!=NULL){
    showGraph(activeEnvelope);
  }  
  
  
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
    
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    // coordinates for the center of the window
    //int xc, yc;
   // xc = width / 2;
   // yc = height / 2;

    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    //cr->set_line_width(10.0);

    // clip to the area indicated by the expose event so that we only redraw
    // the portion of the window that needs to be redrawn
    //cr->rectangle(event->area.x, event->area.y,
      //      event->area.width, event->area.height);
    //cr->clip();

    // draw red lines out from the center of the window
    //cr->set_source_rgb(0.8, 0.0, 0.0);
    //cr->move_to(0, 0);
    //cr->line_to(xc, yc);
    //cr->line_to(0, height);
    //cr->move_to(xc, yc);
    //cr->line_to(width, yc);
    //cr->stroke();
  }

  return true;
}


bool EnvLibDrawingArea::mouseMoving (GdkEventMotion* event){

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

 double x = event->x * (width + 1) /( width* width);
 double y = 1.0 - (event->y * (height + 1) / (height* height)); // adjust pixals off by one
 
 //get rid of floating digits
 int xalias = x* 1000;
 int yalias = y* 1000;
 x = xalias / 1000.0;
 y = yalias / 1000.0;
 

  char xs[10];
  char ys[10];
  
  sprintf(xs, "%.3f", x);
  sprintf(ys, "%.3f", y);
  
  std::string xss = xs;
  std::string yss = ys;
  
  std::string n = "x:"+ xss +"\ny:" + yss;
	envelopeLibraryWindow->mouseCoordinate->set_text(n);
	
	
	if (mouseLeftButtonPressedDown){
    if (x>1){
      mouseX = 1;
    }
    else if (x< 0){
      mouseX = 0;
    }    
    else {
      mouseX = x;
    }

    if (y>1){
      mouseY = 1;
    }
    else if (y< 0){
      mouseY = 0;
    }    
    else {
      mouseY = y;
    }



    //mouseX = x;   //here mouseX and mouseY is between 0 and 1
    //mouseY = y;	  // different from the usage of mouseButtonPressed
 	  moveNode();
  }
  
}

bool EnvLibDrawingArea::mouseButtonPressed (GdkEventButton* event){
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ){ // test if right click
    mouseX = event->x;
    mouseY = event->y;
        if(m_pMenuPopup) m_pMenuPopup->popup(event->button, event->time);
  }
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 1)){
  	  activeSegment =NULL; //reset activeSegment
  	  headChosen = false;//reset headChosen
    mouseLeftButtonPressedDown = true;
	}
	//startX = event->x;
	//startY = event->y;  
  return true;

}
bool EnvLibDrawingArea::mouseButtonReleased(GdkEventButton* event){
  if( event->button == 1){
		mouseLeftButtonPressedDown = false;
		activeSegment = NULL;
		headChosen = false;
	}
		
}


void EnvLibDrawingArea::clearGraph(){
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();
   

    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
 
    // clear the drawing area
    cr->set_source_rgb(1, 1, 1);
    cr->rectangle(0,0, width, height);
    cr->fill();
   }
      
}

void EnvLibDrawingArea::showGraph(EnvelopeLibraryEntry* _envelope){



  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();
   

    EnvLibEntrySegment* segment = _envelope->segments;

    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
 
    // clear the drawing area
    cr->set_source_rgb(1, 1, 1);
    cr->rectangle(0,0, width, height);
    cr->fill();
    cr->stroke();
   
      

    while (segment!=NULL){ // draw this segment
      // clip to the area indicated by the expose event so that we only redraw
      // the portion of the window that needs to be redrawn
      //cr->rectangle(event->area.x, event->area.y,
      //      event->area.width, event->area.height);
      //cr->clip();
      
      double startx = segment->xStart * width * width / (width +1);
      double starty = (segment->prev ==NULL)? 
        (height - _envelope->yStart * height * height / (height + 1)) :
         height - ( segment->prev->y * height* height/ (height +1));
      
      
      double endx = (segment->next ==NULL)?
        width: segment->next->xStart *width * width / (width+1);
      
      
      double endy =height - ( segment->y * height *height / (height + 1));  
     
     //set color here
      if (segment->segmentType == envSegmentTypeLinear){
            cr->set_source_rgb(0.0, 0.5, 0.8);
      }
      else if (segment->segmentType == envSegmentTypeExponential){
          cr->set_source_rgb(0.8, 0.0, 0.0);
      }
      else {
          cr->set_source_rgb(0.0, 1, 0.2);
        
      }
     
      if (segment->segmentProperty == envSegmentPropertyFixed){
        cr->set_line_width(1.0);
         
      }
      else {
        cr->set_line_width(3.0);
      }
      
      

      cr->move_to(startx, starty);
      cr->line_to(endx, endy);
      //cr->line_to(0, height);
      //cr->move_to(xc, yc);
      //cr->line_to(width, yc);
      cr->stroke();
      segment = segment->next;
    }
    
    segment = _envelope->segments;
    while(segment!=NULL) { // draw this segment
      // clip to the area indicated by the expose event so that we only redraw
      // the portion of the window that needs to be redrawn
      //cr->rectangle(event->area.x, event->area.y,
      //      event->area.width, event->area.height);
      //cr->clip();
      
      double startx = segment->xStart * width * width / (width +1);
      double starty = (segment->prev ==NULL)? 
        (height - _envelope->yStart * height * height / (height + 1)) :
         height - ( segment->prev->y * height* height/ (height +1));
      
      
      double endx = (segment->next ==NULL)?
        width: segment->next->xStart *width * width / (width+1);
      
      
      double endy = height - ( segment->y * height *height / (height + 1));  
      
      cr->set_source_rgb(0.8, 0.5, 0.4);
      cr->arc(startx, starty, 5, 0, 2 * 3.141592654);
      cr->fill();
      cr->arc(endx, endy, 5, 0, 2 * 3.141592654);
      cr->fill();
      segment = segment->next;
    }
  }

}


void EnvLibDrawingArea::insertEnvelopeSegment(){
    EnvelopeLibraryEntry* activeEnvelope = envelopeLibraryWindow->getActiveEnvelope();
    if (activeEnvelope ==NULL) return;
    envelopeLibraryWindow->activeProject->modified();

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();


  
  double insertx = mouseX * ( width +1) / (width* width)  ;  //get rid of ugly floating digits
  double inserty = 1 - mouseY * ( height+1) / (height * height) ;
  
  int aliasx = insertx* 1000;
  int aliasy = inserty* 1000;
  
  insertx = aliasx / 1000.0;         
  inserty = aliasy / 1000.0;
  
  
  
  
  
  EnvLibEntrySegment* left = activeEnvelope ->segments;
  EnvLibEntrySegment* right = activeEnvelope->segments->next;
  while (true){  //find insert point
    if (left->xStart<= insertx && right ==NULL){ //insert before the last segment
      break;
    }
    else if (left->xStart<= insertx &&right ->xStart >= insertx){
      break;
    }
    else {
      left = right;
      right = right ->next;
    }
  }

  if ( right ==NULL) { // insert after the last segment
  
    EnvLibEntrySegment* insert = new EnvLibEntrySegment();
  
    insert -> xStart = insertx;
    insert->y = left -> y;
    insert->xDuration = 1 - insertx;  
    insert->prev = left;
    insert->next = NULL;
    insert->segmentType = envSegmentTypeLinear;
    insert->segmentProperty = envSegmentPropertyFixed;
    
    
    left->xDuration = insert->xStart - left->xStart;
    left -> next = insert;
    left ->y = inserty;
  
  }
  
  else {  //right != NULL
  
    
    EnvLibEntrySegment* insert = new EnvLibEntrySegment();
  
    insert -> xStart = insertx;
    insert->y = left -> y;
    insert->xDuration = right ->xStart - insertx;  
    insert->prev = left;
    insert->next = right;
    insert->segmentType = envSegmentTypeLinear;
    insert->segmentProperty = envSegmentPropertyFixed;
    
    
    left->xDuration = insert->xStart - left->xStart;
    left -> next = insert;
    left ->y = inserty;
    right ->prev = insert;
  
 
  }
  


  showGraph(activeEnvelope);

}



void EnvLibDrawingArea::moveNode(){


  EnvelopeLibraryEntry* activeEnvelope = envelopeLibraryWindow->getActiveEnvelope();
  if (activeEnvelope ==NULL) return;
  envelopeLibraryWindow->activeProject->modified();  
  
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
    
  double errorX = 10.0* (width + 1.0) / (width*width);
  double errorY = 10.0* (height+ 1.0)/(height* height);
  EnvLibEntrySegment* left;
  EnvLibEntrySegment* right;
  
  
  //set activeSegment and left / right
  if (activeSegment!=NULL){
    left = activeSegment;
    right = activeSegment->next;
  }
    
  else {
    
    left = activeEnvelope ->segments;
    right = activeEnvelope->segments->next;
    while (true){  //find insert point
      if (left->xStart<= mouseX && right ==NULL){ //insert before the last segment
        break;
      }
      else if (left->xStart<= mouseX &&right ->xStart >= mouseX){
        break;
      }
     else {
        left = right;
        right = right ->next;
      }
    }
  }


  //handle head chosen case
  if (headChosen){
     activeEnvelope->yStart = mouseY;

       showGraph(activeEnvelope);

       return;
  }
  else if (left->prev ==NULL){
    if (!headChosen){
      if (mouseX< errorX &&  mouseX <= (left->xDuration / 2.0)&&
          (mouseY< activeEnvelope->yStart + errorY )&& 
          (mouseY > activeEnvelope->yStart - errorY)){// modified head of envelope, also make sure no other nodes are within errorX
        headChosen = true;
      }
    }
    if (headChosen){
       activeEnvelope->yStart = mouseY;
       showGraph(activeEnvelope);

       return;
    }
  }


  if (mouseX<= left->xStart + left->xDuration /2){   //close to head of segment
    if ( left ->prev ==NULL){  //handle mouse click close to head
        if (!headChosen){
          if (mouseX< errorX &&  mouseX <= (left->xDuration / 2.0)&&
            (mouseY< activeEnvelope->yStart + errorY )&& 
            (mouseY > activeEnvelope->yStart - errorY)){// modified head of envelope, also make sure no other nodes are within errorX
            headChosen = true;
          }
        }
      if (headChosen){
        activeEnvelope->yStart = mouseY;
        showGraph(activeEnvelope);

        return;
      }
      
    }
    else{
      if ( activeSegment == NULL&& mouseX <=left->xStart+errorX ){ //setup activeSegment
        activeSegment = left;
      }
      if (activeSegment !=NULL){
        activeSegment->prev->y = mouseY;
        activeSegment->xStart = ( mouseX > activeSegment->prev->xStart)? mouseX: activeSegment->prev->xStart + 0.001;
        activeSegment->prev->xDuration = activeSegment->xStart - activeSegment->prev->xStart;
        activeSegment->xDuration = (activeSegment->next ==NULL)? (1.0 - activeSegment->xStart):(activeSegment->next->xStart - activeSegment->xStart);
           showGraph(activeEnvelope);

        return;     
      }
      
    }
    
    
    
  }  
  else { //close to the end of segment
    //lazy to implement.  code works fine now.  mouse has to be at the right side of the node to trigger the moving
  
  }



  if ( right ==NULL) { //adjust y of last segment
    if (activeSegment==NULL){ //set activeSegment if necessary
      if (mouseX >= (1-errorX) && (mouseY < left->y + errorY) && mouseY> left->y - errorY){
        activeSegment = left;
      }
    }

    if (activeSegment!=NULL){

        activeSegment->y = mouseY;

    }
  }
  

  
  showGraph(activeEnvelope);

}





void EnvLibDrawingArea::setFixed(){
      EnvelopeLibraryEntry* activeEnvelope = envelopeLibraryWindow->getActiveEnvelope();
    if (activeEnvelope ==NULL) return;
  envelopeLibraryWindow->activeProject->modified();

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();


  
  double insertx = mouseX * ( width +1) / (width* width)  ;  
  double inserty = 1 - mouseY * ( height+1) / (height * height) ;
  
  
  EnvLibEntrySegment* left = activeEnvelope ->segments;
  EnvLibEntrySegment* right = activeEnvelope->segments->next;
  while (true){  //find insert point
    if (left->xStart<= insertx && right ==NULL){ //insert before the last segment
      break;
    }
    else if (left->xStart<= insertx &&right ->xStart >= insertx){
      break;
    }
    else {
      left = right;
      right = right ->next;
    }
  }

  left->segmentProperty = envSegmentPropertyFixed;
 
  showGraph(activeEnvelope);


}
void EnvLibDrawingArea::setFlexible(){

      EnvelopeLibraryEntry* activeEnvelope = envelopeLibraryWindow->getActiveEnvelope();
    if (activeEnvelope ==NULL) return;
  envelopeLibraryWindow->activeProject->modified();

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();


  
  double insertx = mouseX * ( width +1) / (width* width)  ;  
  double inserty = 1 - mouseY * ( height+1) / (height * height) ;
  
  
  EnvLibEntrySegment* left = activeEnvelope ->segments;
  EnvLibEntrySegment* right = activeEnvelope->segments->next;
  while (true){  //find insert point
    if (left->xStart<= insertx && right ==NULL){ //insert before the last segment
      break;
    }
    else if (left->xStart<= insertx &&right ->xStart >= insertx){
      break;
    }
    else {
      left = right;
      right = right ->next;
    }
  }

  left->segmentProperty = envSegmentPropertyFlexible;
 
  showGraph(activeEnvelope);


}


void EnvLibDrawingArea::setLinear(){

      EnvelopeLibraryEntry* activeEnvelope = envelopeLibraryWindow->getActiveEnvelope();
    if (activeEnvelope ==NULL) return;
  envelopeLibraryWindow->activeProject->modified();

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();


  
  double insertx = mouseX * ( width +1) / (width* width)  ;  
  double inserty = 1 - mouseY * ( height+1) / (height * height) ;
  
  
  EnvLibEntrySegment* left = activeEnvelope ->segments;
  EnvLibEntrySegment* right = activeEnvelope->segments->next;
  while (true){  //find insert point
    if (left->xStart<= insertx && right ==NULL){ //insert before the last segment
      break;
    }
    else if (left->xStart<= insertx &&right ->xStart >= insertx){
      break;
    }
    else {
      left = right;
      right = right ->next;
    }
  }

  left->segmentType = envSegmentTypeLinear;
 
  showGraph(activeEnvelope);


}



void EnvLibDrawingArea::setSpline(){

      EnvelopeLibraryEntry* activeEnvelope = envelopeLibraryWindow->getActiveEnvelope();
    if (activeEnvelope ==NULL) return;
  envelopeLibraryWindow->activeProject->modified();

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();


  
  double insertx = mouseX * ( width +1) / (width* width)  ;  
  double inserty = 1 - mouseY * ( height+1) / (height * height) ;
  
  
  EnvLibEntrySegment* left = activeEnvelope ->segments;
  EnvLibEntrySegment* right = activeEnvelope->segments->next;
  while (true){  //find insert point
    if (left->xStart<= insertx && right ==NULL){ //insert before the last segment
      break;
    }
    else if (left->xStart<= insertx &&right ->xStart >= insertx){
      break;
    }
    else {
      left = right;
      right = right ->next;
    }
  }

  left->segmentType = envSegmentTypeSpline;
 
  showGraph(activeEnvelope);


}



void EnvLibDrawingArea::setExponential(){

      EnvelopeLibraryEntry* activeEnvelope = envelopeLibraryWindow->getActiveEnvelope();
    if (activeEnvelope ==NULL) return;
  envelopeLibraryWindow->activeProject->modified();

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();


  
  double insertx = mouseX * ( width +1) / (width* width)  ;  
  double inserty = 1 - mouseY * ( height+1) / (height * height) ;
  
  
  EnvLibEntrySegment* left = activeEnvelope ->segments;
  EnvLibEntrySegment* right = activeEnvelope->segments->next;
  while (true){  //find insert point
    if (left->xStart<= insertx && right ==NULL){ //insert before the last segment
      break;
    }
    else if (left->xStart<= insertx &&right ->xStart >= insertx){
      break;
    }
    else {
      left = right;
      right = right ->next;
    }
  }

  left->segmentType = envSegmentTypeExponential;
 
  showGraph(activeEnvelope);


}







