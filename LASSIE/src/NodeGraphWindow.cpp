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

NodeGraphWindow::NodeGraphWindow() {
  webViewContainerGtk = webkit_web_view_new();
  webViewContainerGtkmm = Glib::wrap(webViewContainerGtk);
  webkit_web_view_load_uri((WebKitWebView *) webViewContainerGtk, "http://www.example.com");
  add(*webViewContainerGtkmm);
  show_all_children();
}

NodeGraphWindow::~NodeGraphWindow() {

}