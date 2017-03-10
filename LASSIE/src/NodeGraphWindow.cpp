/*******************************************************************************
 *
 *  File Name     : NodeGraphWindow.cpp
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

#include "NodeGraphWindow.h"
#include <thread>

NodeGraphWindow::NodeGraphWindow() {
  new std::thread([this](){
    webkit_web_context_set_process_model (
        webkit_web_context_get_default (),
        WEBKIT_PROCESS_MODEL_MULTIPLE_SECONDARY_PROCESSES);
    webViewContainerGtk = webkit_web_view_new();
    webViewContainerGtkmm = Glib::wrap(webViewContainerGtk);
    add(*webViewContainerGtkmm);
    webkit_web_view_load_uri(webView, "http://www.webkitgtk.org/");
  });
}

NodeGraphWindow::~NodeGraphWindow() {

}