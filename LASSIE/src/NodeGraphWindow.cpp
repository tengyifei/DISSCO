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
#include <JavaScriptCore/JavaScript.h>
#include "ProjectViewController.h"
#include "IEvent.h"
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <set>
#include <vector>

using std::string;
using std::cout;
using std::endl;
using std::vector;
using std::set;

namespace {

constexpr int PIPE_READ = 0;
constexpr int PIPE_WRITE = 1;

string createChild(const char* szCommand,
                   char* const aArguments[],
                   const char* szMessage) {
  int aStdinPipe[2];
  int aStdoutPipe[2];
  int nChild;
  char nChar;
  int nResult;

  if (pipe(aStdinPipe) < 0) {
    throw std::runtime_error("allocating pipe for child input redirect");
  }
  if (pipe(aStdoutPipe) < 0) {
    close(aStdinPipe[PIPE_READ]);
    close(aStdinPipe[PIPE_WRITE]);
    throw std::runtime_error("allocating pipe for child output redirect");
  }

  nChild = fork();
  if (0 == nChild) {
    // child continues here

    // redirect stdin
    if (dup2(aStdinPipe[PIPE_READ], STDIN_FILENO) == -1) {
      throw std::runtime_error("redirecting stdin");
    }

    // redirect stdout
    if (dup2(aStdoutPipe[PIPE_WRITE], STDOUT_FILENO) == -1) {
      throw std::runtime_error("redirecting stdout");
    }

    // all these are for use by parent only
    close(aStdinPipe[PIPE_READ]);
    close(aStdinPipe[PIPE_WRITE]);
    close(aStdoutPipe[PIPE_READ]);
    close(aStdoutPipe[PIPE_WRITE]);

    // run child process image
    // replace this with any exec* function find easier to use ("man exec")
    nResult = execvp(szCommand, aArguments);

    // if we get here at all, an error occurred, but we are in the child
    // process, so just exit
    perror("exec of the child process");
    exit(nResult);
  } else if (nChild > 0) {
    // parent continues here

    // close unused file descriptors, these are for child only
    close(aStdinPipe[PIPE_READ]);
    close(aStdoutPipe[PIPE_WRITE]);

    if (NULL != szMessage) {
      write(aStdinPipe[PIPE_WRITE], szMessage, strlen(szMessage));
    }
    close(aStdinPipe[PIPE_WRITE]);

    int len;
    char buffer[257];
    string result;
    while ((len = read(aStdoutPipe[PIPE_READ], &buffer, 256)) > 0) {
      result += string(buffer, len);
    }
    close(aStdoutPipe[PIPE_READ]);

    return result;
  } else {
    // failed to create child
    close(aStdinPipe[PIPE_READ]);
    close(aStdinPipe[PIPE_WRITE]);
    close(aStdoutPipe[PIPE_READ]);
    close(aStdoutPipe[PIPE_WRITE]);

    throw std::runtime_error("failed to create child");
  }
}

}

template <class Str, class It>
Str join(It begin, const It end, const Str &sep)
{
  typedef typename Str::value_type     char_type;
  typedef typename Str::traits_type    traits_type;
  typedef typename Str::allocator_type allocator_type;
  typedef std::basic_ostringstream<char_type,traits_type,allocator_type>
                                       ostringstream_type;
  ostringstream_type result;

  while (begin != end) {
    result << *begin++;
    result << sep;
  }
  return result.str();
}

void findNodes(IEvent *top, set<IEvent*> &rawNodes, vector<string>& transitionList) {
  for (auto& layer : top->layers) {
    for (auto& child : layer->children) {
      IEvent *event = child->event;
      // add transition regardless
      transitionList.push_back("    " + string("\"") + top->getEventName() + string("\"") + " -> " + string("\"") + event->getEventName() + string("\""));
      if (rawNodes.find(event) == rawNodes.end()) {
        // new node
        rawNodes.insert(event);
        findNodes(event, rawNodes, transitionList);
      }
    }
  }
}

string generateTree(IEvent *top) {
  vector<string> nodeList;
  vector<string> transitionList;

  set<IEvent*> rawNodes;
  findNodes(top, rawNodes, transitionList);

  rawNodes.insert(top);
  for (auto& ev : rawNodes) {
    nodeList.push_back("    " + string("\"") + ev->getEventName() + string("\"") + " [URL=\"#" + ev->getEventName() + "\"]");
  }

  return R"delim(
digraph hierarchy {
    rankdir=TD;
    size="8,5";
)delim"
  + join(nodeList.begin(), nodeList.end(), string(";\n"))
  + "    node [shape = circle];\n"
  + join(transitionList.begin(), transitionList.end(), string(";\n"))
  + "}";
}

string getJsResultAsString(WebKitJavascriptResult *js_result) {
  JSValueRef              value;
  JSGlobalContextRef      context;

  if (!js_result) {
    throw std::runtime_error("Invalid JavaScript Result");
  }

  context = webkit_javascript_result_get_global_context(js_result);
  value = webkit_javascript_result_get_value(js_result);
  if (JSValueIsString(context, value)) {
    JSStringRef js_str_value;
    gchar      *str_value;
    gsize       str_length;

    js_str_value = JSValueToStringCopy(context, value, NULL);
    str_length = JSStringGetMaximumUTF8CStringSize(js_str_value);
    str_value = (gchar *) g_malloc(str_length);
    JSStringGetUTF8CString(js_str_value, str_value, str_length);
    JSStringRelease(js_str_value);
    string str = string(str_value);
    g_free(str_value);
    return str;
  } else {
    throw std::runtime_error("Unexpected JavaScript Result Type");
  }
}

static void
onUserClickNodeCallback(WebKitUserContentManager *manager,
                        WebKitJavascriptResult *message,
                        gpointer user_data)
{
  string messageString = getJsResultAsString(message);
  cout << messageString << endl;
  webkit_javascript_result_unref(message);
}

NodeGraphWindow::NodeGraphWindow(ProjectViewController *projectView) {
  projectView_ = projectView;

  // initial size
  resize(600, 600);

  webkit_web_context_set_process_model (
    webkit_web_context_get_default(),
    WEBKIT_PROCESS_MODEL_MULTIPLE_SECONDARY_PROCESSES);

  WebKitUserContentManager *userContent = webkit_user_content_manager_new();
  webViewContainerGtk_ = webkit_web_view_new_with_user_content_manager(userContent);
  WebKitWebView *view = (WebKitWebView *) webViewContainerGtk_;
  WebKitSettings *settings = webkit_web_view_get_settings(view);

  // settings
  webkit_settings_set_enable_java(settings, false);
  webkit_settings_set_enable_javascript(settings, true);
  webkit_web_view_set_settings(view, settings);

  webViewContainerGtkmm_ = Glib::wrap(webViewContainerGtk_);

  // create Top event
  IEvent *top = new IEvent();
  top->setEventName("Top");
  EventLayer *topLayer = top->addLayer();
  for (auto& event : projectView_->events) {
    if (event->getEventType() == EventType::eventTop) {
      topLayer->children.push_back(new EventDiscretePackage(event));
    }
  }
  string graphDef;
  if (topLayer->children.size() != 0) {
    graphDef = generateTree(top);
    cout << graphDef << endl;
  } else {
    graphDef = R"delim(
digraph hierarchy {
  rankdir=TD;
  size="80,50";
  node [shape = circle];
})delim";
  }

  // run dot to produce image and map file
  const char* args[] = {"dot", "-Gdpi=3000", "-Tcmapx", "-oNodeGraphVisualizationMap.map", "-Tgif"};
  char* argsWritable[] = {strdup(args[0]), strdup(args[1]), strdup(args[2]), strdup(args[3]), strdup(args[4]), NULL};
  string image = createChild("dot", argsWritable, graphDef.c_str());
  for (int i = 0; i < 5; i++) free(argsWritable[i]);

  // read map file
  std::ifstream is;
  is.open("./NodeGraphVisualizationMap.map");
  string mapFile(std::istreambuf_iterator<char>(is), {});
  is.close();

  // convert image to inline html
  gchar *imageB64c = g_base64_encode((const unsigned char *) image.c_str(), image.length());
  string imageB64(imageB64c);
  g_free(imageB64c);

  // script to handle clicks
  string scriptStr = R"delim(
<script>
window.onhashchange = function() {
  if (location.hash) {
    // kill the pound sign
    window.webkit.messageHandlers.nodeClick.postMessage(location.hash.substr(1));
    // reset the hash so it can be retriggered
    location.hash = "";
  }
}
</script>
)delim";

  // load the resulting html
  string imageTag = "<img src=\"data:image/gif;base64," + imageB64 + "\" usemap=\"#hierarchy\">\n";
  string html = imageTag + mapFile + scriptStr;

  // register communication callbacks
  webkit_user_content_manager_register_script_message_handler(userContent, "nodeClick");
  g_signal_connect(userContent, "script-message-received::nodeClick",
                  G_CALLBACK(onUserClickNodeCallback), NULL);

  GBytes *gbytes = g_bytes_new_take((void *) html.c_str(), html.length());
  webkit_web_view_load_bytes(view, gbytes, "text/html", "ascii", NULL);

  webkit_web_view_set_zoom_level(view, 0.5);

  add(*webViewContainerGtkmm_);
  webViewContainerGtkmm_->grab_focus();
  show_all_children();
}

NodeGraphWindow::~NodeGraphWindow() {

}
