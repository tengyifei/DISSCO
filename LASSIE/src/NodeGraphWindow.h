/*******************************************************************************
 *
 *  File Name     : NodeGraphWindow.h
 *  Date created  : Mar. 9 2017
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file implement the graph view of LASSIE.
 *                  The graph view displays the hierarchy of the composition as a graph.
 *==============================================================================
 *
 *  This file is part of LASSIE.
 *  2010 Ming-ching Chiu, Sever Tipei
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
#ifndef NODE_GRAPH_WINDOW_H
#define NODE_GRAPH_WINDOW_H

#include <string>
#include <gtkmm.h>
#include <webkit2/webkit2.h>

class NodeGraphWindow : public Gtk::Window {

  public:

    //==========================================================================
    //===============================Functions==================================
    //==========================================================================

    /*! \brief The constructor of NodeGraphWindow
    *
    ***************************************************************************/
    NodeGraphWindow();
    ~NodeGraphWindow();

  private:
    WebKitWebView *webView;
    GtkWidget *webViewContainerGtk;
    Gtk::Widget *webViewContainerGtkmm;
};

#endif