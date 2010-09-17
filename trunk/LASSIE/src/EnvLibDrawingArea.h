#ifndef ENV_LIB_DRAWINGAREA_H
#define ENV_LIB_DRAWINGAREA_H

#include "LASSIE.h"

class EnvelopeLibraryEntry;
class EnvLibEntrySegment;
class EnvelopeLibraryWindow;
class EnvLibDrawingArea : public Gtk::DrawingArea
{
public:
  EnvLibDrawingArea(EnvelopeLibraryWindow* _envelopeLibraryWindow);
  
  virtual ~EnvLibDrawingArea();
  
  void showGraph(EnvelopeLibraryEntry* _envelope);
  void clearGraph();
  bool mouseMoving (GdkEventMotion* event);

protected:
  //Override default signal handler:
  virtual bool on_expose_event(GdkEventExpose* event);
  
  bool mouseButtonPressed(GdkEventButton* event);
  bool mouseButtonReleased(GdkEventButton* event);
  bool mouseLeftButtonPressedDown;

  
  double mouseY; //temp variables for exchanging values among methods;
  double mouseX;

  
  void moveNode();
  void insertEnvelopeSegment();
  void setFlexible();
  void setFixed();
  void setLinear();
  void setExponential();
  void setSpline();
private: 
  EnvelopeLibraryWindow* envelopeLibraryWindow;
  


  // these three are for setting up pop up menu
  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
  Gtk::Menu* m_pMenuPopup;

  EnvLibEntrySegment* activeSegment;
  bool headChosen;// flag to indicate if the head is chosen by the user to modify


  
};

#endif 
