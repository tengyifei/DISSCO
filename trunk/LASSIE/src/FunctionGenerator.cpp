/*******************************************************************************
 *
 *  File Name     : FunctionGenerator.cpp
 *  Date created  : Jun. 8 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This File contains the definitions of FunctionGenerator 
 *                  class, which is responsible for the interface of generating
 *                  function string
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


#include "FunctionGenerator.h"
#include "IEvent.h"




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







FunctionGenerator::FunctionGenerator(FunctionReturnType _returnType,std::string _originalString){

  set_title("Function Generator");
    set_border_width(3);
    result = "";
    returnType = _returnType;
    stochosSubAlignments = NULL;
    stochosNumOfNodes = 0;
    stochosMethodFlag = 0;
    makeEnvelopeSubAlignments = NULL;
    makeEnvelopeNumOfNodes = 0;
    SPANumOfChannels = 0;
    SPANumOfPartials = 0;
    SPAApplyFlag = 0; //junk here,
    SPAChannelAlignments = NULL;
    SPAMethodFlag = 0;//same here
  
  
  attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file("./LASSIE/src/UI/FunctionGenerator.ui");
  }
  catch (const Glib::FileError& ex){
    std::cerr << "FileError: " << ex.what() << std::endl;
  }
  catch (const Gtk::BuilderError& ex){
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
   
   #else
  std::auto_ptr<Glib::Error> error;
  if (!attributesRefBuilder->add_from_file("./LASSIE/src/UI/FunctionGenerator.ui", error)){
    std::cerr << error->what() << std::endl;
  }
   
   #endif

  Gtk::VBox* vbox;
  attributesRefBuilder->get_widget("mainVBox", vbox);
  get_vbox()->pack_start(*vbox, Gtk::PACK_EXPAND_WIDGET);
  
 
  
  
  
  
  Gtk::ComboBox* combobox;
  attributesRefBuilder->get_widget("FunctionListComboBox", combobox);

  functionListTreeModel = Gtk::ListStore::create(functionListColumns);
  combobox->set_model(functionListTreeModel);
  
  if (_returnType ==functionReturnInt){
  Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandomInt;
    row[functionListColumns.m_col_name] = "RandomInt";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionStochos;
    row[functionListColumns.m_col_name] = "Stochos";  
 
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionSelect;
    row[functionListColumns.m_col_name] = "Select";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionValuePick;
    row[functionListColumns.m_col_name] = "ValuePick";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionChooseL;
    row[functionListColumns.m_col_name] = "ChooseL";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionGetPattern;
    row[functionListColumns.m_col_name] = "GetPattern";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = NOT_A_FUNCTION;
    row[functionListColumns.m_col_name] = "----------"; 

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_TYPE;
    row[functionListColumns.m_col_name] = "CURRENT_TYPE";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_CHILD_NUM;
    row[functionListColumns.m_col_name] = "CURRENT_CHILD_NUM";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_PARTIAL_NUM;
    row[functionListColumns.m_col_name] = "CURRENT_PARTIAL_NUM";   

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_SEGMENT;
    row[functionListColumns.m_col_name] = "CURRENT_SEGMENT";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticAVAILABLE_EDU;
    row[functionListColumns.m_col_name] = "AVAILABLE_EDU";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_LAYER;
    row[functionListColumns.m_col_name] = "CURRENT_LAYER";  
  
  }
  else if (_returnType ==functionReturnFloat){
  Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandom;
    row[functionListColumns.m_col_name] = "Random";
  
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandomInt;
    row[functionListColumns.m_col_name] = "RandomInt";
  
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandomizer;
    row[functionListColumns.m_col_name] = "Randomizer";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionInverse;
    row[functionListColumns.m_col_name] = "Inverse";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionLN;
    row[functionListColumns.m_col_name] = "LN";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionDecay;
    row[functionListColumns.m_col_name] = "Decay";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = NOT_A_FUNCTION;
    row[functionListColumns.m_col_name] = "----------";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionStochos;
    row[functionListColumns.m_col_name] = "Stochos";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionSelect;
    row[functionListColumns.m_col_name] = "Select";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionValuePick;
    row[functionListColumns.m_col_name] = "ValuePick";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionChooseL;
    row[functionListColumns.m_col_name] = "ChooseL";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionGetPattern;
    row[functionListColumns.m_col_name] = "GetPattern";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = NOT_A_FUNCTION;
    row[functionListColumns.m_col_name] = "----------"; 

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_TYPE;
    row[functionListColumns.m_col_name] = "CURRENT_TYPE";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_CHILD_NUM;
    row[functionListColumns.m_col_name] = "CURRENT_CHILD_NUM";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_PARTIAL_NUM;
    row[functionListColumns.m_col_name] = "CURRENT_PARTIAL_NUM";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_SEGMENT;
    row[functionListColumns.m_col_name] = "CURRENT_SEGMENT";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticAVAILABLE_EDU;
    row[functionListColumns.m_col_name] = "AVAILABLE_EDU";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_LAYER;
    row[functionListColumns.m_col_name] = "CURRENT_LAYER";  
  
  }
 else if (_returnType ==functionReturnMakeListFun){
  Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandom;
    row[functionListColumns.m_col_name] = "Random";
  
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandomInt;
    row[functionListColumns.m_col_name] = "RandomInt";
  
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandomizer;
    row[functionListColumns.m_col_name] = "Randomizer";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionInverse;
    row[functionListColumns.m_col_name] = "Inverse";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionLN;
    row[functionListColumns.m_col_name] = "LN";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionDecay;
    row[functionListColumns.m_col_name] = "Decay";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = NOT_A_FUNCTION;
    row[functionListColumns.m_col_name] = "----------";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionStochos;
    row[functionListColumns.m_col_name] = "Stochos";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionSelect;
    row[functionListColumns.m_col_name] = "Select";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionValuePick;
    row[functionListColumns.m_col_name] = "ValuePick";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionChooseL;
    row[functionListColumns.m_col_name] = "ChooseL";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionGetPattern;
    row[functionListColumns.m_col_name] = "GetPattern";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionEnvLib;
    row[functionListColumns.m_col_name] = "EnvLib"; 
    
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionMakeEnvelope;
    row[functionListColumns.m_col_name] = "MakeEnvelope"; 
    
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionReadENVFile;
    row[functionListColumns.m_col_name] = "ReadENVFile"; 

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = NOT_A_FUNCTION;
    row[functionListColumns.m_col_name] = "----------"; 

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_TYPE;
    row[functionListColumns.m_col_name] = "CURRENT_TYPE";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_CHILD_NUM;
    row[functionListColumns.m_col_name] = "CURRENT_CHILD_NUM";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_PARTIAL_NUM;
    row[functionListColumns.m_col_name] = "CURRENT_PARTIAL_NUM";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_SEGMENT;
    row[functionListColumns.m_col_name] = "CURRENT_SEGMENT";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticAVAILABLE_EDU;
    row[functionListColumns.m_col_name] = "AVAILABLE_EDU";  

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_LAYER;
    row[functionListColumns.m_col_name] = "CURRENT_LAYER";  
  
  }
  
  else if (_returnType ==functionReturnList){
  Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionMakeList;
    row[functionListColumns.m_col_name] = "MakeList"; 

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRawList;
    row[functionListColumns.m_col_name] = "RawList";  
 
  }
  
  else if (_returnType ==functionReturnENV){
    Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionMakeEnvelope;
    row[functionListColumns.m_col_name] = "MakeEnvelope";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionReadENVFile;
    row[functionListColumns.m_col_name] = "ReadENVFile"; 
    
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionEnvLib;
    row[functionListColumns.m_col_name] = "EnvLib";     
          
  }
  
  else if (_returnType ==functionReturnSPA){
    Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionSPA;
    row[functionListColumns.m_col_name] = "SPA";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionReadSPAFile;
    row[functionListColumns.m_col_name] = "ReadSPAFile"; 
        
  }
  
  else if (_returnType ==functionReturnPAT){
    Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionMakePattern;
    row[functionListColumns.m_col_name] = "MakePattern";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionExpandPattern;
    row[functionListColumns.m_col_name] = "ExpandPattern"; 

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionReadPATFile;
    row[functionListColumns.m_col_name] = "ReadPATFile"; 
        
  }  
  
 
  else if (_returnType ==functionReturnREV){
    Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionREV_Simple;
    row[functionListColumns.m_col_name] = "REV_Simple";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionREV_Medium;
    row[functionListColumns.m_col_name] = "REV_Medium"; 
    
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionREV_Advanced;
    row[functionListColumns.m_col_name] = "REV_Advanced"; 
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionReadREVFile;
    row[functionListColumns.m_col_name] = "ReadREVFile"; 
        
  }   
  
  else if (_returnType ==functionReturnSIV){
    Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionMakeSieve;
    row[functionListColumns.m_col_name] = "MakeSieve";


    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionReadSIVFile;
    row[functionListColumns.m_col_name] = "ReadSIVFile"; 
        
  } 



    
  
  combobox->pack_start(functionListColumns.m_col_name);
  
  combobox->signal_changed().connect( sigc::mem_fun(*this,    &FunctionGenerator::function_list_combo_changed) );

   add_button( "gtk-ok", 0);  
   add_button("gtk-cancel", 1);
   
   
   
   //connecting signals to functions
  Gtk::Entry* entry;
  Gtk::Button* button;
  Gtk::RadioButton* radiobutton;
   
   // Random
  attributesRefBuilder->get_widget(
    "RandomLowBoundFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::randomLowBoundFunButtonClicked));

  attributesRefBuilder->get_widget(
    "RandomHighBoundFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::randomHighBoundFunButtonClicked));   
   
  attributesRefBuilder->get_widget(
    "RandomLowBoundEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::randomEntryChanged));   

  attributesRefBuilder->get_widget(
    "RandomHighBoundEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::randomEntryChanged)); 
  
  
  
  //RandomInt

  attributesRefBuilder->get_widget(
    "RandomIntLowBoundFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::randomIntLowBoundFunButtonClicked));

  attributesRefBuilder->get_widget(
    "RandomIntHighBoundFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::randomIntHighBoundFunButtonClicked));   
   
  attributesRefBuilder->get_widget(
    "RandomIntLowBoundEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::randomIntEntryChanged));   

  attributesRefBuilder->get_widget(
    "RandomIntHighBoundEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::randomIntEntryChanged)); 
  
  
  //Randomizer

  attributesRefBuilder->get_widget(
    "RandomizerBaseFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::randomizerBaseFunButtonClicked));

  attributesRefBuilder->get_widget(
    "RandomizerDeviationFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::randomizerDeviationFunButtonClicked));   
   
  attributesRefBuilder->get_widget(
    "RandomizerBaseEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::randomizerEntryChanged));   

  attributesRefBuilder->get_widget(
    "RandomizerDeviationEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::randomizerEntryChanged)); 
  


  //Decay

  attributesRefBuilder->get_widget(
    "DecayBaseFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::decayBaseFunButtonClicked));

  attributesRefBuilder->get_widget(
    "DecayRateFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::decayRateFunButtonClicked));  

  attributesRefBuilder->get_widget(
    "DecayIndexFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::decayIndexFunButtonClicked));
  
  attributesRefBuilder->get_widget(
    "DecayTypeExponentialRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::decayTypeRadioButtonClicked));

  attributesRefBuilder->get_widget(
    "DecayTypeLinearRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::decayTypeRadioButtonClicked));    
   
  attributesRefBuilder->get_widget(
    "DecayBaseEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::decayEntryChanged));   

  attributesRefBuilder->get_widget(
    "DecayRateEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::decayEntryChanged)); 
    
  attributesRefBuilder->get_widget(
    "DecayIndexEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::decayEntryChanged)); 
 
 

  //inverse
 
  attributesRefBuilder->get_widget(
    "InverseFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::inverseFunButtonClicked));

  attributesRefBuilder->get_widget(
    "InverseEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::inverseEntryChanged));  
 


  //LN
 
  attributesRefBuilder->get_widget(
    "LNFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::LNFunButtonClicked));

  attributesRefBuilder->get_widget(
    "LNEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::LNEntryChanged)); 
  
  
  //select 
  
  attributesRefBuilder->get_widget(
    "SelectListFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::selectListFunButtonClicked));

  attributesRefBuilder->get_widget(
    "SelectIndexFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::selectIndexFunButtonClicked));   
   
  attributesRefBuilder->get_widget(
    "SelectListEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::selectEntryChanged));   

  attributesRefBuilder->get_widget(
    "SelectIndexEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::selectEntryChanged));   
  


  //stochos
  
  
  attributesRefBuilder->get_widget(
    "StochosRangeDistribRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator:: stochosMethodRadioButtonClicked));

  attributesRefBuilder->get_widget(
    "StochosFunctionsRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator:: stochosMethodRadioButtonClicked));

  attributesRefBuilder->get_widget(
    "StochosOffsetEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::stochosTextChanged)); 
  
  
  attributesRefBuilder->get_widget(
    "StochosAddNodeButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::stochosAddNodeButtonClicked)); 
	
  attributesRefBuilder->get_widget(
    "StochosInsertFunctionButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::stochosFunButtonClicked));
  //button->can_focus(false);
  
  




  //ValuePick
  
  attributesRefBuilder->get_widget(
    "ValuePickElementsMeaningfulRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickElementsModsRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickElementsFakeRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickTextChanged));
  
  attributesRefBuilder->get_widget(
    "ValuePickWeightsPeriodicRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickTextChanged));
  
  attributesRefBuilder->get_widget(
    "ValuePickWeightsHierarchicRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickTextChanged));
  
  attributesRefBuilder->get_widget(
    "ValuePickWeightsIncludeRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickTextChanged));
  
  attributesRefBuilder->get_widget(
    "ValuePickTypeVariableRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickTextChanged));
  
  attributesRefBuilder->get_widget(
    "ValuePickTypeConstantRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickTextChanged));  
  
  
  attributesRefBuilder->get_widget(
    "ValuePickAbsRangeEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickTextChanged));
  
  attributesRefBuilder->get_widget(
    "ValuePickLowEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickTextChanged));
  
  attributesRefBuilder->get_widget(
    "ValuePickHighEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickTextChanged));
  
  attributesRefBuilder->get_widget(
    "ValuePickDistEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickTextChanged));
  
  attributesRefBuilder->get_widget(
    "ValuePickElementsEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickTextChanged));
  
  attributesRefBuilder->get_widget(
    "ValuePickWeightsEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickTextChanged));



  attributesRefBuilder->get_widget(
    "ValuePickAbsRangeFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickAbsRangeFunButtonClicked));
  
  attributesRefBuilder->get_widget(
    "ValuePickLowFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickLowFunButtonClicked));  

  attributesRefBuilder->get_widget(
    "ValuePickHighFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickHighFunButtonClicked));

  attributesRefBuilder->get_widget(
    "ValuePickDistFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::valuePickDistFunButtonClicked));



  //ChooseL
 
  attributesRefBuilder->get_widget(
    "ChooseLFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::chooseLFunButtonClicked));

  attributesRefBuilder->get_widget(
    "ChooseLEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::chooseLEntryChanged)); 


  //GetPattern
 
  attributesRefBuilder->get_widget(
    "GetPatternFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::getPatternFunButtonClicked));

  attributesRefBuilder->get_widget(
    "GetPatternEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::getPatternEntryChanged)); 
  
  
  //MakeList
  
  attributesRefBuilder->get_widget(
    "MakeListFunctionFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::makeListFunctionFunButtonClicked));

  attributesRefBuilder->get_widget(
    "MakeListSizeFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::makeListSizeFunButtonClicked));   
   
  attributesRefBuilder->get_widget(
    "MakeListFunctionEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::makeListTextChanged));   

  attributesRefBuilder->get_widget(
    "MakeListSizeEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::makeListTextChanged));   
  
  //EnvLib
  
  attributesRefBuilder->get_widget(
    "EnvLibEnvelopeFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::envLibEnvelopeFunButtonClicked));

  attributesRefBuilder->get_widget(
    "EnvLibScalingFactorFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::envLibScalingFactorFunButtonClicked));   
   
  attributesRefBuilder->get_widget(
    "EnvLibEnvelopeEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::envLibTextChanged));   

  attributesRefBuilder->get_widget(
    "EnvLibScalingFactorEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::envLibTextChanged)); 
   
   
  //ReadENVFile 
   
  attributesRefBuilder->get_widget(
    "ReadENVFileEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::readENVFileTextChanged));
     
   
	//MakeEnvelope
	   
  attributesRefBuilder->get_widget(
    "MakeEnvelopeScalingFactorFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::makeEnvelopeScalingFactorFunButtonClicked));   
   
  attributesRefBuilder->get_widget(
    "MakeEnvelopeXValueFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::makeEnvelopeXValueFunButtonClicked));    
   
  attributesRefBuilder->get_widget(
    "MakeEnvelopeYValueFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::makeEnvelopeYValueFunButtonClicked));   
  
  
  attributesRefBuilder->get_widget(
    "MakeEnvelopeScalingFactorEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::makeEnvelopeTextChanged)); 

  attributesRefBuilder->get_widget(
    "MakeEnvelopeXValueEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::makeEnvelopeTextChanged)); 
  //entry->set_text("1.0");
  //entry->set_editable(false);
     
   
  attributesRefBuilder->get_widget(
    "MakeEnvelopeYValueEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::makeEnvelopeTextChanged));
  
  makeEnvelopeSubAlignments = new MakeEnvelopeSubAlignment(this);
  makeEnvelopeNumOfNodes = 1;
  

  attributesRefBuilder->get_widget(
    "MakeEnvelopeInnerVBox", vbox);
  vbox->pack_start(*makeEnvelopeSubAlignments,Gtk::PACK_SHRINK);

  //MakePattern
    attributesRefBuilder->get_widget(
    "MakePatternOriginFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::makePatternOriginFunButtonClicked));  
    attributesRefBuilder->get_widget(
    "MakePatternIntervalsFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::makePatternIntervalsFunButtonClicked));  

  attributesRefBuilder->get_widget(
    "MakePatternOriginEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::makePatternTextChanged));  

  attributesRefBuilder->get_widget(
    "MakePatternIntervalsEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::makePatternTextChanged));    
  
  //ExpandPattern
  
  attributesRefBuilder->get_widget(
    "ExpandPatternModuloFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::expandPatternModuloFunButtonClicked));  
  
  attributesRefBuilder->get_widget(
    "ExpandPatternLowFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::expandPatternLowFunButtonClicked));   
  attributesRefBuilder->get_widget(
    "ExpandPatternHighFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::expandPatternHighFunButtonClicked));
  attributesRefBuilder->get_widget(
    "ExpandPatternPatternFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::expandPatternPatternFunButtonClicked));
  
  attributesRefBuilder->get_widget(
    "ExpandPatternModuloEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::expandPatternTextChanged));  
  
  attributesRefBuilder->get_widget(
    "ExpandPatternLowEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::expandPatternTextChanged));  
  
  attributesRefBuilder->get_widget(
    "ExpandPatternHighEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::expandPatternTextChanged));  
  
  attributesRefBuilder->get_widget(
    "ExpandPatternPatternEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::expandPatternTextChanged));  
  
  
  //ReadPATFile 
    attributesRefBuilder->get_widget(
    "ReadPATFileOriginFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::readPATFileOriginFunButtonClicked));  
  
  attributesRefBuilder->get_widget(
    "ReadPATFileNameEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::readPATFileTextChanged));   
  
  attributesRefBuilder->get_widget(
    "ReadPATFileOriginEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::readPATFileTextChanged));  
  

  //ReadREVFile 
   
  attributesRefBuilder->get_widget(
    "ReadREVFileEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::readREVFileTextChanged));  
  
  //REV_Simple
    attributesRefBuilder->get_widget(
    "REV_SimpleEntryFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::REV_SimpleEntryFunButtonClicked));   

  attributesRefBuilder->get_widget(
    "REV_SimpleEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::REV_SimpleEntryTextChanged)); 
  
  //REV_Medium
  
  attributesRefBuilder->get_widget(
    "REV_MediumReverbPercentFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::REV_MediumReverbPercentFunButtonClicked));   

  
  attributesRefBuilder->get_widget(
    "REV_MediumHilowSpreadFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::REV_MediumHilowSpreadFunButtonClicked));   

  
  attributesRefBuilder->get_widget(
    "REV_MediumGainAllPassFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::REV_MediumGainAllPassFunButtonClicked));   

  
  attributesRefBuilder->get_widget(
    "REV_MediumDelayFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::REV_MediumDelayFunButtonClicked));   
  
  attributesRefBuilder->get_widget(
    "REV_MediumReverbPercentEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::REV_MediumTextChanged));   
  
  attributesRefBuilder->get_widget(
    "REV_MediumHilowSpreadEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::REV_MediumTextChanged));   
   
  attributesRefBuilder->get_widget(
    "REV_MediumGainAllPassEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::REV_MediumTextChanged));   
  attributesRefBuilder->get_widget(
    "REV_MediumDelayEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::REV_MediumTextChanged));   
  
  //REV_Advanced
  attributesRefBuilder->get_widget(
    "REV_AdvancedReverbPercentFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::REV_AdvancedReverbPercentFunButtonClicked));   

  
  attributesRefBuilder->get_widget(
    "REV_AdvancedCombGainListFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::REV_AdvancedCombGainListFunButtonClicked));   

  attributesRefBuilder->get_widget(
    "REV_LPGainListFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::REV_AdvancedLPGainListFunButtonClicked));   
  
  attributesRefBuilder->get_widget(
    "REV_AdvancedGainAllPassFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::REV_AdvancedGainAllPassFunButtonClicked));   

  
  attributesRefBuilder->get_widget(
    "REV_AdvancedDelayFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::REV_AdvancedDelayFunButtonClicked));   
  
  attributesRefBuilder->get_widget(
    "REV_AdvancedReverbPercentEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::REV_AdvancedTextChanged));   
  
  attributesRefBuilder->get_widget(
    "REV_AdvancedCombGainListEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::REV_AdvancedTextChanged));   

  attributesRefBuilder->get_widget(
    "REV_LPGainListEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::REV_AdvancedTextChanged));     
  attributesRefBuilder->get_widget(
    "REV_AdvancedGainAllPassEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::REV_AdvancedTextChanged));   
  attributesRefBuilder->get_widget(
    "REV_AdvancedDelayEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::REV_AdvancedTextChanged));   
    
  //ReadSIVFile 
   
  attributesRefBuilder->get_widget(
    "ReadSIVFileEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::readSIVFileTextChanged));



  //MakeSieve

  
  attributesRefBuilder->get_widget(
    "MakeSieveElementsMeaningfulRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::makeSieveTextChanged));

  attributesRefBuilder->get_widget(
    "MakeSieveElementsModsRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::makeSieveTextChanged));

  attributesRefBuilder->get_widget(
    "MakeSieveElementsFakeRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::makeSieveTextChanged));
  
  attributesRefBuilder->get_widget(
    "MakeSieveWeightsPeriodicRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::makeSieveTextChanged));
  
  attributesRefBuilder->get_widget(
    "MakeSieveWeightsHierarchicRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::makeSieveTextChanged));
  
  attributesRefBuilder->get_widget(
    "MakeSieveWeightsIncludeRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::makeSieveTextChanged));
  

  

  
  attributesRefBuilder->get_widget(
    "MakeSieveLowEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::makeSieveTextChanged));
  
  attributesRefBuilder->get_widget(
    "MakeSieveHighEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::makeSieveTextChanged));

  
  attributesRefBuilder->get_widget(
    "MakeSieveElementsEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::makeSieveTextChanged));
  
  attributesRefBuilder->get_widget(
    "MakeSieveWeightsEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::makeSieveTextChanged));

  
  attributesRefBuilder->get_widget(
    "MakeSieveLowFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::makeSieveLowFunButtonClicked));  

  attributesRefBuilder->get_widget(
    "MakeSieveHighFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::makeSieveHighFunButtonClicked));


  

  //ReadSPAFile 
   
  attributesRefBuilder->get_widget(
    "ReadSPAFileEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::readSPAFileTextChanged));
  
  
  
  //SPA
  attributesRefBuilder->get_widget(
    "SPAStereoRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::SPAMethodRadioButtonClicked));
  attributesRefBuilder->get_widget(
    "SPAMulti_PanRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::SPAMethodRadioButtonClicked));  
  attributesRefBuilder->get_widget(
    "SPAPolarRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::SPAMethodRadioButtonClicked));  
  
  attributesRefBuilder->get_widget(
    "SPAPartialRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::SPAApplyByRadioButtonClicked));    
  
  attributesRefBuilder->get_widget(
    "SPASoundRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::SPAApplyByRadioButtonClicked));  
  
  //Stereo as default, apply by sound


  Gtk::HBox* hbox;

  
  SPAChannelAlignments = new SPAChannelAlignment(this, 1, 1);
  attributesRefBuilder->get_widget( "SPAMainHBox", hbox);  
  hbox->pack_start(*SPAChannelAlignments, Gtk::PACK_SHRINK);
  SPAChannelAlignments->hideButtons();
  SPANumOfChannels = 1;
  SPANumOfPartials = 1;
  SPAApplyFlag = 0;
  SPAMethodFlag = 0;
           
  
  
  
  size_t locationOfKeyword;
  Gtk::TreeModel::iterator iter;
  Gtk::TreeModel::Row row; 
  string parsingString;
  FileValue* value;
  list<FileValue>::iterator argumentsIter;
    

  //check if RandomInt
  locationOfKeyword =_originalString.find("RandomInt");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
   
    while(row[functionListColumns.m_col_name]!= "RandomInt"){
      iter++;
      row = *iter;     
    }
    combobox->set_active(iter);
    
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //RandomInt has 2 arguments
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument
      attributesRefBuilder->get_widget("RandomIntLowBoundEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt));
      
      argumentsIter++;
      
      value =&(*argumentsIter); // second argument
      attributesRefBuilder->get_widget("RandomIntHighBoundEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt));      
      

    }
  }   


  

  
  //check if Stochos
  locationOfKeyword =_originalString.find("Stochos");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "Stochos"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //Stochos has 3 arguments (string, list of envelopes, int)
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument
      //attributesRefBuilder->get_widget("RandomIntLowBoundEntry",entry);
      //entry->set_text(getFunctionString(value,functionReturnInt));

      string method = getFunctionString(value,functionReturnString);
      
      if (method == "\"FUNCTIONS\""){
        attributesRefBuilder->get_widget("StochosFunctionsRadioButton",radiobutton);
        radiobutton->set_active();     
      }
      
      else {  //RANGE_DISTRIB
        attributesRefBuilder->get_widget("StochosRangeDistribRadioButton",radiobutton);
        radiobutton->set_active();       
      }


      argumentsIter++;
      value =&(*argumentsIter);
      list<std::string> envelopeStringList = fileValueListToStringList (value->getList(), functionReturnEnvelopeList); 
      //entry->set_text(getFunctionString(value,functionReturnInt));      
      list<std::string>::iterator stringIter = envelopeStringList.begin();


      if (method =="\"FUNCTIONS\""){



        while (stringIter!= envelopeStringList.end()){
          StochosSubAlignment* newSubAlignment = new StochosSubAlignment(this, 1);
          if ( stochosSubAlignments ==NULL){
            stochosSubAlignments = newSubAlignment;
          }
          else {
            stochosSubAlignments->appendNewNode(newSubAlignment);
          }
          Gtk::VBox* vbox;
          attributesRefBuilder->get_widget(
             "StochosInnerVBox", vbox);
          vbox->pack_start(*newSubAlignment,Gtk::PACK_SHRINK);
          newSubAlignment->setFunctionsEntry(*stringIter);
          newSubAlignment->switchTo(1);
          stringIter++;
          stochosNumOfNodes ++;
        }      
        stochosTextChanged();      
        
      }
      else {

        while (stringIter!= envelopeStringList.end()){
          StochosSubAlignment* newSubAlignment = new StochosSubAlignment(this, 0);
          if ( stochosSubAlignments ==NULL){
            stochosSubAlignments = newSubAlignment;
          }
          else {
            stochosSubAlignments->appendNewNode(newSubAlignment);
          }
          Gtk::VBox* vbox;
          attributesRefBuilder->get_widget(
             "StochosInnerVBox", vbox);
          vbox->pack_start(*newSubAlignment,Gtk::PACK_SHRINK);
          newSubAlignment->setMinEntry(*stringIter);  
          stringIter++;
          newSubAlignment->setMaxEntry(*stringIter);  
          stringIter++;
          newSubAlignment->setDistEntry(*stringIter);  
          stringIter++;                    
          newSubAlignment->switchTo(0);
          stochosNumOfNodes ++;
        }      
        stochosTextChanged();        

      }
      
      

      //third argument
      argumentsIter++;
      value =&(*argumentsIter);
      attributesRefBuilder->get_widget("StochosOffsetEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt));      
      
    }
  }  
  




  //check if Select
  locationOfKeyword =_originalString.find("Select");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "Select"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
  
    // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //RandomInt has 2 arguments
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a list
      
      string listString = getFunctionString(value,functionReturnInt);
      attributesRefBuilder->get_widget("SelectListEntry",entry);
      entry->set_text(listString);
      
      argumentsIter++;
      
      value =&(*argumentsIter); // second argument is an int
      attributesRefBuilder->get_widget("SelectIndexEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt));      
    }
    
    //end parsing
  } 
 
  //check if CURRENT_TYPE
  locationOfKeyword =_originalString.find("CURRENT_TYPE");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "CURRENT_TYPE"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

  } 

  //check if CURRENT_CHILD_NUM
  locationOfKeyword =_originalString.find("CURRENT_CHILD_NUM");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "CURRENT_CHILD_NUM"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

  } 
 
  //check if CURRENT_PARTIAL_NUM
  locationOfKeyword =_originalString.find("CURRENT_PARTIAL_NUM");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "CURRENT_PARTIAL_NUM"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

  } 

  //check if CURRENT_SEGMENT
  locationOfKeyword =_originalString.find("CURRENT_SEGMENT");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "CURRENT_SEGMENT"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

  } 
  
  //check if AVAILABLE_EDU
  locationOfKeyword =_originalString.find("AVAILABLE_EDU");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "AVAILABLE_EDU"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

  }  
 
  //check if CURRENT_LAYER
  locationOfKeyword =_originalString.find("CURRENT_LAYER");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "CURRENT_LAYER"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

  }
  //check if ValuePick
  locationOfKeyword =_originalString.find("ValuePick");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ValuePick"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
        // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //RandomInt has 9 arguments
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a float (absolute range
      
      attributesRefBuilder->get_widget("ValuePickAbsRangeEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat));
      
      argumentsIter++;
      
      value =&(*argumentsIter); // second argument is an envelope (low)
      attributesRefBuilder->get_widget("ValuePickLowEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt)); 
      
      argumentsIter++;
      value =&(*argumentsIter); // third argument is an envelope (high)
      attributesRefBuilder->get_widget("ValuePickHighEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt));      
      
      argumentsIter++;
      value =&(*argumentsIter); // fourth argument is an envelope (distribution)
      attributesRefBuilder->get_widget("ValuePickDistEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt));      
      
      argumentsIter++;
      value =&(*argumentsIter); // 5th argument is a string  (Elements)

      if (value->getString() == "MEANINGFUL"){
        attributesRefBuilder->get_widget("ValuePickElementsMeaningfulRadioButton",radiobutton);
        radiobutton->set_active(); 
      
      }
      else if (value->getString() == "MODS"){
        attributesRefBuilder->get_widget("ValuePickElementsModsRadioButton",radiobutton);
        radiobutton->set_active();       
      }
      
      else {  //FAKE
        attributesRefBuilder->get_widget("ValuePickElementsFakeRadioButton",radiobutton);
        radiobutton->set_active(); 
      
      }

      argumentsIter++;
      value =&(*argumentsIter); // 6th argument is a list without bracket
      string listString = fileValueListToString( value->getList(),functionReturnFloat);
      
      listString = listString.substr(1, listString.length()-2);
      
      attributesRefBuilder->get_widget("ValuePickElementsEntry",entry);
      entry->set_text(listString);       
      

      argumentsIter++;
      value =&(*argumentsIter); // 7th argument is a string  (weight)

      if (value->getString() == "PERIODIC"){
        attributesRefBuilder->get_widget("ValuePickWeightsPeriodicRadioButton",radiobutton);
        radiobutton->set_active(); 
      
      }
      else if (value->getString() == "HIERARCHIC"){
        attributesRefBuilder->get_widget("ValuePickWeightsHierarchicRadioButton",radiobutton);
        radiobutton->set_active();       
      }
      
      else {  //Include
        attributesRefBuilder->get_widget("ValuePickWeightsIncludeRadioButton",radiobutton);
        radiobutton->set_active(); 
      
      }

      argumentsIter++;
      value =&(*argumentsIter); // 8th argument is a list without bracket
      listString = fileValueListToString( value->getList(),functionReturnFloat);
      
      listString = listString.substr(1, listString.length()-2);
      
      attributesRefBuilder->get_widget("ValuePickWeightsEntry",entry);
      entry->set_text(listString);        
      
      argumentsIter++;
      value =&(*argumentsIter); // 9th argument is a string  (Elements)

      if (value->getString() == "VARIABLE"){
        attributesRefBuilder->get_widget("ValuePickTypeVariableRadioButton",radiobutton);
        radiobutton->set_active(); 
      
      }

      
      else {  //constant
        attributesRefBuilder->get_widget("ValuePickTypeConstantRadioButton",radiobutton);
        radiobutton->set_active(); 
      
      }   
           
    }
    
    //end parsing
  } 


  //check if ChooseL
  locationOfKeyword =_originalString.find("ChooseL");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ChooseL"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    
    // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //chooseL has 1 argument
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a function

      
      attributesRefBuilder->get_widget("ChooseLEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt));  
    }
    
    //end parsing
  } 
 

  //check if GetPattern
  locationOfKeyword =_originalString.find("GetPattern");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "GetPattern"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    
    // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //getpattern has 1 argument
      
      argumentsIter = arguments.begin();
      argumentsIter++;
      value =&(*argumentsIter); // first argument is a function

      
      attributesRefBuilder->get_widget("GetPatternEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt));  
    }
    
    //end parsing
  } 


  //check if Random
  locationOfKeyword =_originalString.find("Random");
  size_t checkNotRandomInt = _originalString.find("RandomInt");
  size_t checkNotRandomize = _originalString.find("Randomizer");
  size_t checkNotRandomSeed = _originalString.find("RandomSeed");  
  if (int(locationOfKeyword)==0
      &&int(checkNotRandomInt)!=0
      &&int(checkNotRandomize)!=0
      &&int(checkNotRandomSeed)!=0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "Random"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
  // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //random has 2 arguments
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a float

      
      attributesRefBuilder->get_widget("RandomLowBoundEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat)); 
      
      argumentsIter ++;
      value =&(*argumentsIter); // second argument is a float
      attributesRefBuilder->get_widget("RandomHighBoundEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat));
      
       
    }
    
    //end parsing
  }  
 
  //check if Randomizer
  locationOfKeyword =_originalString.find("Randomizer");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "Randomizer"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
  // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //randomizer has 2 arguments
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a float

      
      attributesRefBuilder->get_widget("RandomizerBaseEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat)); 
      
      argumentsIter ++;
      value =&(*argumentsIter); // second argument is a float
      attributesRefBuilder->get_widget("RandomizerDeviationEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat));
      
       
    }
    
    //end parsing
  }  
  
  //check if Inverse
  locationOfKeyword =_originalString.find("Inverse");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "Inverse"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
  // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //inverse has 1 argument
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a float

      
      attributesRefBuilder->get_widget("InverseEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat)); 
      
       
    }
    
    //end parsing
  }  
  
  //check if LN
  locationOfKeyword =_originalString.find("LN");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "LN"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
  // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //LN has 1 argument
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a float

      
      attributesRefBuilder->get_widget("LNEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat)); 
      
       
    }
    
    //end parsing
  }  

  //check if Decay
  locationOfKeyword =_originalString.find("Decay");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "Decay"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
  // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //Decay has 4 argument
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a float (base
      
      attributesRefBuilder->get_widget("DecayBaseEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat)); 
      
      argumentsIter++;
      value =&(*argumentsIter); // 2nd argument is a string
      
      if (value->getString() == "EXPONENTIAL"){
        attributesRefBuilder->get_widget("DecayTypeExponentialRadioButton",radiobutton);
        radiobutton->set_active(); 
      }
      else{
        attributesRefBuilder->get_widget("DecayTypeLinearRadioButton",radiobutton);
        radiobutton->set_active();      
      }
     
      argumentsIter++;
      value =&(*argumentsIter); // 3nd argument is a string   
      attributesRefBuilder->get_widget("DecayRateEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat));       
      
      argumentsIter++;
      value =&(*argumentsIter); // 4nd argument is an int   
      attributesRefBuilder->get_widget("DecayIndexEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat));        
       
    }
    
    //end parsing
  } 
  
  //check if EnvLib
  locationOfKeyword =_originalString.find("EnvLib");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "EnvLib"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
  // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //EnvLib has 2 arguments
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is an int
      
      attributesRefBuilder->get_widget("EnvLibEnvelopeEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt)); 
            
      
      argumentsIter++;
      value =&(*argumentsIter); // 2nd argument is an Float   
      attributesRefBuilder->get_widget("EnvLibScalingFactorEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat));        
       
    }
    
    //end parsing
  } 
  
  //check if MakeEnvelope
  locationOfKeyword =_originalString.find("MakeEnvelope");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "MakeEnvelope"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    //begin parsing

    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //MakeEnvelope has 5 arguments (string, list of envelopes, int)
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a float list



      list<std::string> firstList =
        fileValueListToStringList(value->getList(),functionReturnFloat);
      
      list<std::string>::iterator stringIter = firstList.begin();  

      
      makeEnvelopeSubAlignments->setXValueString(*stringIter);
      stringIter++;
      
      //makeEnvelopeInsertNode(MakeEnvelopeSubAlignment* _insertAfter){
      MakeEnvelopeSubAlignment* insertAfter = makeEnvelopeSubAlignments; 
      
      
      list<std::string>::iterator SecondToTheLastIter = firstList.end();  
      SecondToTheLastIter --;
      for (stringIter; stringIter != SecondToTheLastIter; stringIter++){
        makeEnvelopeInsertNode(insertAfter);
        insertAfter = insertAfter->next;
        insertAfter->setXValueString(*stringIter);
      }
       
       
      attributesRefBuilder->get_widget("MakeEnvelopeXValueEntry",entry);
      entry->set_text(*stringIter);   
      
       
       
      
      argumentsIter++;
      value =&(*argumentsIter); // 2nd argument is a float list


      list<std::string> secondList =
        fileValueListToStringList(value->getList(),functionReturnFloat);
      
      stringIter = secondList.begin();  
      SecondToTheLastIter = secondList.end();  
      SecondToTheLastIter --;
    
      MakeEnvelopeSubAlignment* thisAlignment = makeEnvelopeSubAlignments;
      for (stringIter; stringIter != SecondToTheLastIter; stringIter++){
        thisAlignment->setYValueString(*stringIter);
        thisAlignment = thisAlignment->next;
      }    
    
    
      attributesRefBuilder->get_widget("MakeEnvelopeYValueEntry",entry);
      entry->set_text(*stringIter);  
    
    
    
      argumentsIter++;
      value =&(*argumentsIter); // 3rd argument is a string list


      list<std::string> thirdList =
        fileValueListToStringList(value->getList(),functionReturnString);
      
      stringIter = thirdList.begin();  
      thisAlignment = makeEnvelopeSubAlignments;
      for (stringIter; stringIter != thirdList.end(); stringIter++){
      

      
        if (*stringIter == "\"LINEAR\""){
          thisAlignment->setEnvSegmentType(0);
        }
        else if (*stringIter == "\"EXPONENTIAL\""){
          thisAlignment->setEnvSegmentType(1);
        }
        else {
          thisAlignment->setEnvSegmentType(2);        
        }
        thisAlignment = thisAlignment->next;
      }    
    
    
      
      argumentsIter++;
      value =&(*argumentsIter); // 4th argument is a string list


      list<std::string> fourthList =
        fileValueListToStringList(value->getList(),functionReturnString);
      
      stringIter = fourthList.begin();  
      thisAlignment = makeEnvelopeSubAlignments;
      for (stringIter; stringIter != fourthList.end(); stringIter++){
        if (*stringIter == "\"FLEXIBLE\""){
          thisAlignment->setEnvSegmentProperty(0);
        }

        else {
          thisAlignment->setEnvSegmentProperty(1);        
        }
        thisAlignment = thisAlignment->next;
      }     
    
      argumentsIter++;
      value =&(*argumentsIter);  // 5th argument is a float  
    
      attributesRefBuilder->get_widget("MakeEnvelopeScalingFactorEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat));     
      
    }
    //end parsing
  } 

  //check if ReadENVFile
  locationOfKeyword =_originalString.find("ReadENVFile");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ReadENVFile"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
  // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //ReadEnvFile has 1 arguments
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a string
      
      attributesRefBuilder->get_widget("ReadENVFileEntry",entry);
      string functionString = getFunctionString(value,functionReturnInt);
      functionString = functionString.substr(5, functionString.length()-6);
      
      entry->set_text(functionString); 
                   
    }
    
    //end parsing
  } 

  //check if MakeList
  locationOfKeyword =_originalString.find("MakeList");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "MakeList"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
  // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //ReadEnvFile has 2 arguments
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a string
      
      attributesRefBuilder->get_widget("MakeListFunctionEntry",entry);      
      entry->set_text(getFunctionString(value,functionReturnString)); 
      
      argumentsIter++;
      value =&(*argumentsIter); // 2nd argument is an int      
      attributesRefBuilder->get_widget("MakeListSizeEntry",entry);      
      entry->set_text(getFunctionString(value,functionReturnInt)); 
      
      
                   
    }
    
    //end parsing
  } 

  // check if RawList
  locationOfKeyword = _originalString.find("<");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "RawList"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
  //TODO parse list

  }

  //check if SPA
  locationOfKeyword =_originalString.find("STEREO");
  size_t lok2 = _originalString.find("MULTI_PAN");
  size_t lok3 = _originalString.find("POLAR");
  
  
  if (int(locationOfKeyword)==2||int(lok2) ==2 ||int(lok3) ==2){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "SPA"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
// start parsing 
  
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getList();  //SPA is actually a list with 3 elements
         
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a string
      
      if(value->getString()=="STEREO"){
        attributesRefBuilder->get_widget("SPAStereoRadioButton",radiobutton);
        radiobutton->set_active();       
      }
      
      else if (value->getString() == "MULTI_PAN"){
        attributesRefBuilder->get_widget("SPAMulti_PanRadioButton",radiobutton);
        radiobutton->set_active();       
      }
      
      else {   //polar
        attributesRefBuilder->get_widget("SPAPolarRadioButton",radiobutton);
        radiobutton->set_active();       
      }
      
      argumentsIter++;
      value =&(*argumentsIter); // second argument is a string      
      
      if(value->getString()=="SOUND"){
        attributesRefBuilder->get_widget("SPASoundRadioButton",radiobutton);
        radiobutton->set_active();       
      }
     
      else {   //partial
        attributesRefBuilder->get_widget("SPAPartialRadioButton",radiobutton);
        radiobutton->set_active();       
      }      
      

      argumentsIter++;
      value =&(*argumentsIter); // third argument is a list

      
      
      if ( SPAMethodFlag ==0){ //stereo
        SPAPartialAlignment* currentPartial = SPAChannelAlignments->partials;
        
        list<string> listOfString = fileValueListToStringList(value->getList(),functionReturnFloat);
        list<string>::iterator stringIter = listOfString.begin();
        
        currentPartial->setText(*stringIter);
        stringIter++;
        for (stringIter; stringIter!= listOfString.end(); stringIter++){
          currentPartial->insertPartialButtonClicked();
          currentPartial = currentPartial->next;
          currentPartial->setText(*stringIter);
        }
    
      }
      
      else {//multi-pan     and polar  
        SPAChannelAlignment* currentChannel = SPAChannelAlignments;
        SPAPartialAlignment* currentPartial = SPAChannelAlignments->partials;
        
        
        
        //these two for loops construct the cells
        list<FileValue> channelsList = value->getList();
        int numChannels = channelsList.size();
        for (int i = 0; i < numChannels-1; i ++){
          currentChannel->insertChannelButtonClicked();
          currentChannel = currentChannel->next;
        }
        
        int numPartials = channelsList.begin()->getList().size();
        
        for (int i = 0; i < numPartials-1; i ++){
          currentPartial->insertPartialButtonClicked();
          currentPartial = currentPartial->next;
        }        
        
        currentChannel = SPAChannelAlignments;
  
        
        
        list<FileValue>::iterator channelIter = channelsList.begin();
        for (channelIter; channelIter!= channelsList.end(); channelIter++){
          currentPartial = currentChannel->partials;           
          value =&(*channelIter);
          list<string> listOfString = fileValueListToStringList(value->getList(),functionReturnFloat);
          list<string>::iterator stringIter = listOfString.begin();
          for (stringIter; stringIter!= listOfString.end(); stringIter++){
            currentPartial->setText(*stringIter);
            currentPartial = currentPartial->next;
          }          
          currentChannel = currentChannel->next;
        }     
      } 
             
    }
      
    //end parsing
  } 

  //check if ReadSPAFile
  locationOfKeyword =_originalString.find("ReadSPAFile");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ReadSPAFile"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //ReadSPAFile has 1 arguments
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a string
      
      attributesRefBuilder->get_widget("ReadSPAFileEntry",entry);
      string functionString = getFunctionString(value,functionReturnInt);
      functionString = functionString.substr(5, functionString.length()-6);
      
      entry->set_text(functionString); 
                   
    }
    
    //end parsing
  } 

  //check if MakePattern
  locationOfKeyword =_originalString.find("MakePattern");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "MakePattern"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //makepattern has 2 arguments
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is an int
      attributesRefBuilder->get_widget("MakePatternOriginEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt));      
      
      argumentsIter++;
      value =&(*argumentsIter); // second argument is a list
      string listString = getFunctionString(value,functionReturnInt);
      attributesRefBuilder->get_widget("MakePatternIntervalsEntry",entry);
      entry->set_text(listString);
      
    }
    
    //end parsing
  } 
  
  //check if ExpandPattern
  locationOfKeyword =_originalString.find("ExpandPattern");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ExpandPattern"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //expandpattern has 5 arguments
      
      argumentsIter = arguments.begin();
      
      argumentsIter ++; //the first argument is fixed.
      value =&(*argumentsIter); // second argument is an int
      attributesRefBuilder->get_widget("ExpandPatternModuloEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt));      

      argumentsIter ++; 
      value =&(*argumentsIter); // 3rd argument is an int
      attributesRefBuilder->get_widget("ExpandPatternLowEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt));  
      
      argumentsIter ++; 
      value =&(*argumentsIter); // 4th argument is an int
      attributesRefBuilder->get_widget("ExpandPatternHighEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt)); 
      
      argumentsIter ++; 
      value =&(*argumentsIter); // 5th argument is a function string
      attributesRefBuilder->get_widget("ExpandPatternPatternEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt));       

            
    }
    
    //end parsing
  }   

  //check if ReadPATFile
  locationOfKeyword =_originalString.find("ReadPATFile");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ReadPATFile"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //ReadPATFile has 2 arguments
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a string
      
      attributesRefBuilder->get_widget("ReadPATFileNameEntry",entry);
      string functionString = getFunctionString(value,functionReturnInt);
      functionString = functionString.substr(5, functionString.length()-6);
      entry->set_text(functionString); 

      argumentsIter ++;
      value =&(*argumentsIter); // first argument is a string      
      
      attributesRefBuilder->get_widget("ReadPATFileOriginEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt));        
                   
    }
    
    //end parsing
  }  
  //check if REV_Simple
  locationOfKeyword =_originalString.find("<\"SIMPLE");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "REV_Simple"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getList();  //Rev_Simple has 2 elements in the list
      
      argumentsIter = arguments.begin();
      argumentsIter ++;
      value =&(*argumentsIter); // the only useful argument (2nd) is a float
      
      attributesRefBuilder->get_widget("REV_SimpleEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat)); 
                   
    }
    
    //end parsing
  }  
  //check if REV_Medium
  locationOfKeyword =_originalString.find("<\"MEDIUM");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "REV_Medium"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getList();  //Rev_medium has 5 elements in the list
      
      argumentsIter = arguments.begin();
      argumentsIter ++;
      value =&(*argumentsIter); // 2nd argument is an envelope
      
      attributesRefBuilder->get_widget("REV_MediumReverbPercentEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat)); 
      argumentsIter ++;
      value =&(*argumentsIter); // 3rd argument is an float
      
      attributesRefBuilder->get_widget("REV_MediumHilowSpreadEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat)); 
      argumentsIter ++;
      value =&(*argumentsIter); // 4th argument is an float
      
      attributesRefBuilder->get_widget("REV_MediumGainAllPassEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat)); 
      
      argumentsIter ++;
      value =&(*argumentsIter); // 5th argument is an float
      
      attributesRefBuilder->get_widget("REV_MediumDelayEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat));             
                   
    }
    
    //end parsing
  }  
  
  //check if REV_Advanced
  locationOfKeyword =_originalString.find("<\"ADVANCED");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "REV_Advanced"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getList();  //Rev_advanced has 6 elements in the list
      
      argumentsIter = arguments.begin();
      argumentsIter ++;
      value =&(*argumentsIter); // 2nd argument is an envelope
      
      attributesRefBuilder->get_widget("REV_AdvancedReverbPercentEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat)); 
      argumentsIter ++;
      value =&(*argumentsIter); // 3rd argument is an list
      
      attributesRefBuilder->get_widget("REV_AdvancedCombGainListEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat)); 
      argumentsIter ++;
      value =&(*argumentsIter); // 4th argument is an list
      
      attributesRefBuilder->get_widget("REV_LPGainListEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat)); 
      
      argumentsIter ++;
      value =&(*argumentsIter); // 5th argument is an float
      
      attributesRefBuilder->get_widget("REV_AdvancedGainAllPassEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat));    
      
      argumentsIter ++;
      value =&(*argumentsIter); // 6th argument is an float
      
      attributesRefBuilder->get_widget("REV_AdvancedDelayEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnFloat));                  
                   
    }
    
    //end parsing
  }  
  
  //check if ReadREVFile
  locationOfKeyword =_originalString.find("ReadREVFile");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ReadREVFile"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //ReadREVFile has 1 argument
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a string
      
      attributesRefBuilder->get_widget("ReadREVFileEntry",entry);
      string functionString = getFunctionString(value,functionReturnInt);
      functionString = functionString.substr(5, functionString.length()-6);
      entry->set_text(functionString); 
    }
    
    //end parsing
  }  

  //check if MakeSieve
  locationOfKeyword =_originalString.find("MakeSieve");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "MakeSieve"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
// start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //makesieve has 6 arguments
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is an int (low bound)
      
      attributesRefBuilder->get_widget("MakeSieveLowEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt));
      
      argumentsIter++;
      
      value =&(*argumentsIter); // second argument is an int (high bound)
      attributesRefBuilder->get_widget("MakeSieveHighEntry",entry);
      entry->set_text(getFunctionString(value,functionReturnInt)); 
         
      
      argumentsIter++;
      value =&(*argumentsIter); // 3rd argument is a string  (Elements)

      if (value->getString() == "MEANINGFUL"){
        attributesRefBuilder->get_widget("MakeSieveElementsMeaningfulRadioButton",radiobutton);
        radiobutton->set_active(); 
      
      }
      else if (value->getString() == "MODS"){
        attributesRefBuilder->get_widget("MakeSieveElementsModsRadioButton",radiobutton);
        radiobutton->set_active();       
      }
      
      else {  //FAKE
        attributesRefBuilder->get_widget("MakeSieveElementsFakeRadioButton",radiobutton);
        radiobutton->set_active(); 
      
      }

      argumentsIter++;
      value =&(*argumentsIter); // 4th argument is a list without bracket
      string listString = fileValueListToString( value->getList(),functionReturnInt);
      
      listString = listString.substr(1, listString.length()-2);
      
      attributesRefBuilder->get_widget("MakeSieveElementsEntry",entry);
      entry->set_text(listString);       
      

      argumentsIter++;
      value =&(*argumentsIter); // 5th argument is a string  (weight)

      if (value->getString() == "PERIODIC"){
        attributesRefBuilder->get_widget("MakeSieveWeightsPeriodicRadioButton",radiobutton);
        radiobutton->set_active(); 
      
      }
      else if (value->getString() == "HIERARCHIC"){
        attributesRefBuilder->get_widget("MakeSieveWeightsHierarchicRadioButton",radiobutton);
        radiobutton->set_active();       
      }
      
      else {  //FAKE
        attributesRefBuilder->get_widget("MakeSieveWeightsIncludeRadioButton",radiobutton);
        radiobutton->set_active(); 
      
      }

      argumentsIter++;
      value =&(*argumentsIter); // 6th argument is a list without bracket
      listString = fileValueListToString( value->getList(),functionReturnFloat);
      
      listString = listString.substr(1, listString.length()-2);
      
      attributesRefBuilder->get_widget("MakeSieveWeightsEntry",entry);
      entry->set_text(listString);          
           
    }
    
    //end parsing
  }
  
  //check if ReadSIVFile
  locationOfKeyword =_originalString.find("ReadSIVFile");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ReadSIVFile"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing 
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;  
    yy_scan_string(parsingString.c_str());//set parser buffer
    
    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //ReadSIVFile has 1 argument
      
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a string
      
      attributesRefBuilder->get_widget("ReadSIVFileEntry",entry);
      string functionString = getFunctionString(value,functionReturnInt);
      functionString = functionString.substr(5, functionString.length()-6);
      entry->set_text(functionString); 
    }
    
    //end parsing
  }
  

  
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  textview->get_buffer()->set_text(_originalString);

  show_all_children();

}

FunctionGenerator::~FunctionGenerator(){
  if (stochosSubAlignments != NULL){
    stochosSubAlignments->clear();
  }
  if (makeEnvelopeSubAlignments != NULL){
   	makeEnvelopeSubAlignments->clear();
	} 
	if (SPAChannelAlignments!=NULL){
	  SPAChannelAlignments->clear();
	}
}




void FunctionGenerator::on_response (int response_id){
  hide();
  
  Gtk::TextView* textView;
  attributesRefBuilder->get_widget("resultStringTextView", textView);  
  result = textView->get_buffer()->get_text();
  
}

std::string FunctionGenerator::getResultString(){
  return result;
}



void FunctionGenerator::function_list_combo_changed(){
  //declare pointers and initialize if needed;
  Gtk::Alignment* alignment;
  attributesRefBuilder->get_widget("FunctionParametersAlignment", alignment);  
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::VBox* vbox;
  Gtk::Entry* entry;
  Gtk::RadioButton* radiobutton;

  Gtk::ComboBox* combobox;
  attributesRefBuilder->get_widget("FunctionListComboBox", combobox);
  
  Gtk::TreeModel::iterator iter = combobox->get_active();
  if(iter)
  {
    Gtk::TreeModel::Row row = *iter;
    if(row)
    {
      //Get the data for the selected row, using our knowledge of the tree
      //model:
      
      CMODFunction function = row[functionListColumns.m_col_id];

      Gtk::Entry* entry;
      if (function == functionRandom){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("RandomVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        textview->get_buffer()->set_text("Random( 0, 1)");
        attributesRefBuilder->get_widget(
          "RandomLowBoundEntry", entry);
        entry->set_text("0");
  

        attributesRefBuilder->get_widget(
          "RandomHighBoundEntry", entry);
        entry->set_text("1");


      }
      else if (function == functionRandomInt){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("RandomIntVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        textview->get_buffer()->set_text("RandomInt( 0, 1)");
        attributesRefBuilder->get_widget(
          "RandomIntLowBoundEntry", entry); 
        entry->set_text("0");
  

        attributesRefBuilder->get_widget(
          "RandomIntHighBoundEntry", entry);
        entry->set_text("1");
  
      }
      else if (function == functionRandomSeed){
      
        //TODO: implement the right behavior
  
      }
      else if (function == functionRandomizer ){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("RandomizerVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        textview->get_buffer()->set_text("Randomizer( FLOAT, FLOAT)");
        attributesRefBuilder->get_widget(
          "RandomizerBaseEntry", entry);
        entry->set_text("");
  

        attributesRefBuilder->get_widget(
          "RandomizerDeviationEntry", entry);
        entry->set_text("");
  
      }
      else if (function == functionInverse ){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("InverseVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        textview->get_buffer()->set_text("Inverse( FLOAT)");
        attributesRefBuilder->get_widget(
          "InverseEntry", entry);
        entry->set_text("");
  
      }
      else if (function == functionLN){
       alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("LNVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        textview->get_buffer()->set_text("LN( FLOAT)");
        attributesRefBuilder->get_widget(
          "LNEntry", entry);
        entry->set_text("");
  
      }
      else if (function == functionFibonacci){
        //TODO: implement the right behavior
  
      }
      else if (function == functionDecay){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("DecayVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        
        textview->get_buffer()->set_text("Decay( FLOAT, \"EXPONENTIAL\", FLOAT, CURRENT_PARTIAL_NUM)");
        attributesRefBuilder->get_widget(
          "DecayBaseEntry", entry);
        entry->set_text("");
  
        attributesRefBuilder->get_widget(
          "DecayRateEntry", entry);
        entry->set_text("");
        
        attributesRefBuilder->get_widget(
          "DecayIndexEntry", entry);
        entry->set_text("CURRENT_PARTIAL_NUM");
        
        attributesRefBuilder->get_widget(
          "DecayTypeExponentialRadioButton", radiobutton);
        radiobutton->set_active(true);  
  
      }
      else if (function == functionStochos){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("StochosVBox", vbox);
        alignment->add (*vbox); //add vbox in
        //reset all data
        
        textview->get_buffer()->set_text("Stochos( \"RANGE_DISTRIB\", <MinEnv, MaxEnv, DistEnv>, INT>");
        attributesRefBuilder->get_widget(
          "StochosOffsetEntry", entry);
        entry->set_text("");
        
        attributesRefBuilder->get_widget(
          "StochosRangeDistribRadioButton", radiobutton);
        radiobutton->set_active(true);  
  
      }
      else if (function == functionEnvLib){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("EnvLibVBox", vbox);
        alignment->add (*vbox); //addvbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "EnvLibEnvelopeEntry", entry);
        entry->set_text("");
  

        attributesRefBuilder->get_widget(
          "EnvLibScalingFactorEntry", entry);
        entry->set_text("1.0");
        
        envLibTextChanged();
  
      }
      else if (function == functionSelect){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("SelectVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        if (returnType == functionReturnInt){
          textview->get_buffer()->set_text("Select( < INT1, INT2, INT3....>, INT)");
          attributesRefBuilder->get_widget(
            "SelectListEntry", entry);
          entry->set_text("< INT1, INT2, INT3....>");
        }
        else { //need to return a float
          textview->get_buffer()->set_text("Select( < FLOAT1, FLOAT2, FLOAT3....>, INT)");
          attributesRefBuilder->get_widget(
            "SelectListEntry", entry);
          entry->set_text("< FLOAT1, FLOAT2, FLOAT3....>");
         
        }
          attributesRefBuilder->get_widget(
            "SelectIndexEntry", entry);
          entry->set_text("");         
  
      }
      else if (function == functionValuePick){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ValuePickVBox", vbox);
        alignment->add (*vbox); //add vbox in
        //reset all data

        attributesRefBuilder->get_widget(
          "ValuePickAbsRangeEntry", entry);
        entry->set_text("");
        
        attributesRefBuilder->get_widget(
          "ValuePickLowEntry", entry);
        entry->set_text("ENV");
        
        attributesRefBuilder->get_widget(
          "ValuePickHighEntry", entry);
        entry->set_text("ENV");
        
        attributesRefBuilder->get_widget(
          "ValuePickDistEntry", entry);
        entry->set_text("ENV");
                
        attributesRefBuilder->get_widget(
          "ValuePickElementsEntry", entry);
        entry->set_text("INT1, INT2, INT3 ...");
        
        
        attributesRefBuilder->get_widget(
          "ValuePickWeightsEntry", entry);
        entry->set_text("INT1, INT2, INT3 ...");
        
        valuePickTextChanged();
        
  
      }
      else if (function == functionChooseL){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ChooseLVBox", vbox);
        alignment->add (*vbox); //add vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ChooseLEntry", entry);
        entry->set_text("SIV");
        
        chooseLEntryChanged();
  
      }
      else if (function == functionGetPattern){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("GetPatternVBox", vbox);
        alignment->add (*vbox); //add vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "GetPatternEntry", entry);
        entry->set_text("PAT");
        
        getPatternEntryChanged();
  
      }
      else if (function == functionExpandPattern){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ExpandPatternVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ExpandPatternModuloEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "ExpandPatternLowEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "ExpandPatternHighEntry", entry);
        entry->set_text("");          
        attributesRefBuilder->get_widget(
          "ExpandPatternPatternEntry", entry);
        entry->set_text("PAT");
        
        expandPatternTextChanged();
  
      }
      else if (function == functionAdjustPattern){
        //TODO: implement the right behavior
  
      }
      else if (function == functionMakeEnvelope){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("MakeEnvelopeVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "MakeEnvelopeScalingFactorEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "MakeEnvelopeXValueEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "MakeEnvelopeYValueEntry", entry);
        entry->set_text("");          

        
        makeEnvelopeTextChanged();
  
      }
      else if (function == functionMakeSieve){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("MakeSieveVBox", vbox);
        alignment->add (*vbox); //add vbox in
        //reset all data


        
        attributesRefBuilder->get_widget(
          "MakeSieveLowEntry", entry);
        entry->set_text("");
        
        attributesRefBuilder->get_widget(
          "MakeSieveHighEntry", entry);
        entry->set_text("");
        

                
        attributesRefBuilder->get_widget(
          "MakeSieveElementsEntry", entry);
        entry->set_text("INT1, INT2, INT3 ...");
        
        
        attributesRefBuilder->get_widget(
          "MakeSieveWeightsEntry", entry);
        entry->set_text("INT1, INT2, INT3 ...");
        
        makeSieveTextChanged();
  
      }
      else if (function == functionMakePattern){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("MakePatternVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "MakePatternOriginEntry", entry);
        entry->set_text("");
  

        attributesRefBuilder->get_widget(
          "MakePatternIntervalsEntry", entry);
        entry->set_text("<INT1, INT2, INT3 ...>");
        
        makePatternTextChanged();
  
      }
      else if (function == functionMakeList){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("MakeListVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "MakeListFunctionEntry", entry);
        entry->set_text("FUNCTION");
  

        attributesRefBuilder->get_widget(
          "MakeListSizeEntry", entry);
        entry->set_text("");
        
        makeListTextChanged();
  
      }
      else if (function == functionReadENVFile){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ReadENVFileVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ReadENVFileEntry", entry);
        entry->set_text("");
        entry->grab_focus();
        readENVFileTextChanged();
  
      }
      else if (function == functionReadSIVFile){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ReadSIVFileVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ReadSIVFileEntry", entry);
        entry->set_text("");
        entry->grab_focus();
        readSIVFileTextChanged();
  
      }
      else if (function == functionReadPATFile){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ReadPATFileVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ReadPATFileNameEntry", entry);
        entry->set_text("");
        entry->grab_focus();
        attributesRefBuilder->get_widget(
          "ReadPATFileOriginEntry", entry);
        entry->set_text("");
        readPATFileTextChanged();
  
      }
      else if (function == functionReadSPAFile){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ReadSPAFileVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ReadSPAFileEntry", entry);
        entry->set_text("");
        entry->grab_focus();
        readSPAFileTextChanged();
  
      }
      else if (function == functionReadREVFile){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ReadREVFileVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ReadREVFileEntry", entry);
        entry->set_text("");
        entry->grab_focus();
        readREVFileTextChanged();
  
      }
      else if (function == functionREV_Simple){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("REV_SimpleVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "REV_SimpleEntry", entry);
        entry->set_text("0.5");
        REV_SimpleEntryTextChanged();
  
      }      
      else if (function == functionREV_Medium){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("REV_MediumVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "REV_MediumReverbPercentEntry", entry);
        entry->set_text("ENV");
        attributesRefBuilder->get_widget(
          "REV_MediumHilowSpreadEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "REV_MediumGainAllPassEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "REV_MediumDelayEntry", entry);
        entry->set_text(""); 
        
        REV_MediumTextChanged();
  
      }    
      else if (function == functionREV_Advanced){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("REV_AdvancedVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "REV_AdvancedReverbPercentEntry", entry);
        entry->set_text("ENV");
        attributesRefBuilder->get_widget(
          "REV_AdvancedCombGainListEntry", entry);
        entry->set_text("<0.46, 0.48, 0.50, 0.52, 0.53, 0.55>");
        attributesRefBuilder->get_widget(
          "REV_LPGainListEntry", entry);
        entry->set_text("<0.05, 0.06, 0.07, 0.05, 0.04, 0.02>");        
        
        attributesRefBuilder->get_widget(
          "REV_AdvancedGainAllPassEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "REV_AdvancedDelayEntry", entry);
        entry->set_text(""); 
        
        REV_AdvancedTextChanged();
  
      }        
      else if (function == functionSPA){

        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("SPAVBox", vbox);
        alignment->add (*vbox); //add vbox in
        SPATextChanged();

  
      }       
      else if (function == function_staticCURRENT_TYPE){
        alignment->remove();
        textview->get_buffer()->set_text("CURRENT_TYPE");
  
      }
      else if (function == function_staticCURRENT_CHILD_NUM){
        alignment->remove();
        textview->get_buffer()->set_text("CURRENT_CHILD_NUM");
  
      }
      else if (function == function_staticCURRENT_PARTIAL_NUM){
        alignment->remove();
        textview->get_buffer()->set_text("CURRENT_PARTIAL_NUM");
  
      }
      else if (function == function_staticCURRENT_DENSITY){
        alignment->remove();
        textview->get_buffer()->set_text("CURRENT_DENSITY");
  
      }
      else if (function == function_staticCURRENT_SEGMENT){
        alignment->remove();
        textview->get_buffer()->set_text("CURRENT_SEGMENT");
  
      }
      else if (function == function_staticAVAILABLE_EDU){
        alignment->remove();
        textview->get_buffer()->set_text("AVAILABLE_EDU");
  
      }
      else if (function == function_staticCURRENT_LAYER){
        alignment->remove();
        textview->get_buffer()->set_text("CURRENT_LAYER");
  
      }
      else {  //not a function
        alignment->remove();
        textview->get_buffer()->set_text("");
      }
      

       
    }// end handling changed comboentry
    

  } //end if iter
  //else
    //std::cout << "invalid iter" << std::endl;
  show_all_children();
}


void FunctionGenerator::randomLowBoundFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "RandomLowBoundEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat, entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

  

}
void FunctionGenerator::randomHighBoundFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "RandomHighBoundEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;


}

void FunctionGenerator::randomEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "RandomLowBoundEntry", entry);
  std::string stringbuffer = "Random( " + entry->get_text() + ", ";
  
  attributesRefBuilder->get_widget(
    "RandomHighBoundEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + ")";
  
  textview->get_buffer()->set_text(stringbuffer);
  

}






void FunctionGenerator::randomIntLowBoundFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "RandomIntLowBoundEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

  

}
void FunctionGenerator::randomIntHighBoundFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "RandomIntHighBoundEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;


}

void FunctionGenerator::randomIntEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "RandomIntLowBoundEntry", entry);
  std::string stringbuffer = "RandomInt( " + entry->get_text() + ", ";
  
  attributesRefBuilder->get_widget(
    "RandomIntHighBoundEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + ")";
  
  textview->get_buffer()->set_text(stringbuffer);
  

}





void FunctionGenerator::randomizerBaseFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "RandomizerBaseEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

  

}
void FunctionGenerator::randomizerDeviationFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "RandomizerDeviationEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;


}

void FunctionGenerator::randomizerEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "RandomizerBaseEntry", entry);
  std::string stringbuffer = "Randomizer( " + entry->get_text() + ", ";
  
  attributesRefBuilder->get_widget(
    "RandomizerDeviationEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + ")";
  
  textview->get_buffer()->set_text(stringbuffer);
  

}









void FunctionGenerator::decayBaseFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "DecayBaseEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

  

}
void FunctionGenerator::decayRateFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "DecayRateEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;


}

void FunctionGenerator::decayIndexFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "DecayIndexEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;


}

void FunctionGenerator::decayTypeRadioButtonClicked(){

  decayTextChange();
}
void FunctionGenerator::decayEntryChanged(){
  decayTextChange();
}


void FunctionGenerator::decayTextChange(){
  Gtk::RadioButton* radiobutton;
  Gtk::Entry* entry;
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  
  
  attributesRefBuilder->get_widget(
    "DecayBaseEntry", entry);
  
  std::string stringbuffer = "Decay( " + entry->get_text(); 
    
  attributesRefBuilder->get_widget(
    "DecayTypeExponentialRadioButton", radiobutton);
  
  if (radiobutton->get_active()){ //if exponential
    stringbuffer = stringbuffer + ", \"EXPONENTIAL\", ";
  }
  else { //linear
    stringbuffer = stringbuffer + ", \"LINEAR\", ";
  }
  attributesRefBuilder->get_widget(
    "DecayRateEntry", entry);
  stringbuffer = stringbuffer+ entry->get_text()+ ", ";  

  attributesRefBuilder->get_widget(
    "DecayIndexEntry", entry);
  stringbuffer = stringbuffer+ entry->get_text()+ ")";  

  textview->get_buffer()->set_text(stringbuffer);

}



void FunctionGenerator::inverseFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "InverseEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::inverseEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "InverseEntry", entry);
  std::string stringbuffer = "Inverse( " + entry->get_text() +  ")";
  textview->get_buffer()->set_text(stringbuffer);
}



void FunctionGenerator::LNFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "LNEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::LNEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "LNEntry", entry);
  std::string stringbuffer = "LN( " + entry->get_text() +  ")";
  textview->get_buffer()->set_text(stringbuffer);
}






void FunctionGenerator::selectListFunButtonClicked(){
  Gtk::Entry* entry; 
    FunctionGenerator* generator;
  attributesRefBuilder->get_widget(
    "SelectListEntry", entry);
  if (returnType == functionReturnInt){
    generator = new FunctionGenerator(functionReturnList,entry->get_text());//was functionReturnIntList
    generator->run(); 
  } 
  else {  //return float
    generator = new FunctionGenerator(functionReturnList,entry->get_text()); //was functionReturnFloatList
    generator->run();   
  }
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

  

}
void FunctionGenerator::selectIndexFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "SelectIndexEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;


}

void FunctionGenerator::selectEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "SelectListEntry", entry);
  std::string stringbuffer = "Select( " + entry->get_text() + ", ";
  
  attributesRefBuilder->get_widget(
    "SelectIndexEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + ")";
  
  textview->get_buffer()->set_text(stringbuffer);
  

}



void FunctionGenerator::stochosAddNodeButtonClicked(){
  StochosSubAlignment* newSubAlignment = new StochosSubAlignment(this, stochosMethodFlag);
  if ( stochosSubAlignments ==NULL){
    stochosSubAlignments = newSubAlignment;
  }
  else {
    stochosSubAlignments->appendNewNode(newSubAlignment);
  }
  Gtk::VBox* vbox;
  attributesRefBuilder->get_widget(
    "StochosInnerVBox", vbox);
  vbox->pack_start(*newSubAlignment,Gtk::PACK_SHRINK);
  stochosTextChanged();
  stochosNumOfNodes ++;
  
  show_all_children();
}



void FunctionGenerator::stochosTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::RadioButton* radiobutton;
  attributesRefBuilder->get_widget("StochosRangeDistribRadioButton", radiobutton);

  Gtk::Entry* entry; 
  
  std::string stringbuffer = (radiobutton->get_active())?"Stochos( \"RANGE_DISTRIB\", < ":"Stochos( \"FUNCTIONS\", < ";
  StochosSubAlignment* current = stochosSubAlignments;
  while (current != NULL){
    stringbuffer = stringbuffer + current->toString(stochosMethodFlag);
    if (current->next != NULL){
      stringbuffer= stringbuffer + ", ";
    }
    current = current ->next;
  }
  
  attributesRefBuilder->get_widget(
    "StochosOffsetEntry", entry);

  stringbuffer = stringbuffer + ">, "+ entry->get_text() + ")";
  
  textview->get_buffer()->set_text(stringbuffer);

}





void FunctionGenerator::stochosMethodRadioButtonClicked(){
  Gtk::RadioButton* radiobutton;
  attributesRefBuilder->get_widget(
    "StochosFunctionsRadioButton", radiobutton);
  if (radiobutton->get_active()){
    stochosMethodFlag = 1;
  }
  else {
    stochosMethodFlag = 0;
  }
  
  if (stochosSubAlignments!= NULL){
    stochosSubAlignments->switchTo( stochosMethodFlag);
    show_all_children();
  }
  
  stochosTextChanged();
}


void FunctionGenerator::stochosRemoveNode(StochosSubAlignment* _remove){
  Gtk::VBox* vbox;
  attributesRefBuilder->get_widget(
    "StochosInnerVBox", vbox);     
  vbox->remove ( *_remove);  
  stochosNumOfNodes --;
  
  if (_remove == stochosSubAlignments){ //if removing head
    stochosSubAlignments = stochosSubAlignments->next;
    if (stochosSubAlignments != NULL){
      stochosSubAlignments->prev = NULL;  
    }
  }
  
  else {  //normal case
  
    _remove->prev->next = _remove->next;
    if (_remove->next != NULL){
      _remove->next->prev = _remove->prev;
    }
  }
  
  delete _remove;
  stochosTextChanged();
  show_all_children();
  
}  


void FunctionGenerator::stochosFunButtonClicked(){
  
  //Gtk::Entry* entry = (Gtk::Entry*)get_focus();
  //cout<<focus ->get_text()<<endl;
  
  Gtk::Entry* entry = dynamic_cast< Gtk::Entry * >(get_focus() );
  if( entry ){
    Gtk::Entry* offset;
    attributesRefBuilder->get_widget(
      "StochosOffsetEntry", offset);   
    
    FunctionGenerator* generator = new FunctionGenerator((entry==offset)?functionReturnInt:functionReturnENV, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() !=""&& result ==0){
      entry->set_text(generator->getResultString());
    }
    delete generator;
  }
  else {
    return;
  }

 


}


FunctionGenerator::StochosSubAlignment::StochosSubAlignment(FunctionGenerator* _parent, int _methodFlag){
  parent = _parent;
  next = NULL;
  prev = NULL;
  attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file("./LASSIE/src/UI/FunGenStochosSubAlignment.ui");
  }
  catch (const Glib::FileError& ex){
    std::cerr << "FileError: " << ex.what() << std::endl;
  }
  catch (const Gtk::BuilderError& ex){
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
   
   #else
  std::auto_ptr<Glib::Error> error;
  if (!attributesRefBuilder->add_from_file("./LASSIE/src/UI/FunGenStochosSubAlignment.ui", error)){
    std::cerr << error->what() << std::endl;
  }
   
   #endif
  if (_methodFlag == 0){
    Gtk::VBox* vbox;
    attributesRefBuilder->get_widget("RangeDistribVBox", vbox);
    add(*vbox);   

  }
  else {
    Gtk::HBox* hbox;
    attributesRefBuilder->get_widget("FunctionsHBox", hbox);
    add (*hbox);
  }
  
  
  Gtk::Button* button;
  Gtk::Entry* entry;

  
  
  attributesRefBuilder->get_widget(
    "removeNodeButton1", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::StochosSubAlignment::removeButtonClicked));
  
  attributesRefBuilder->get_widget(
    "removeNodeButton2", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, & FunctionGenerator::StochosSubAlignment::removeButtonClicked));  
  
  
  attributesRefBuilder->get_widget(
    "FunctionsEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::StochosSubAlignment::textChanged)); 
  entry->set_text("ENV");


  attributesRefBuilder->get_widget(
    "MinEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::StochosSubAlignment::textChanged)); 
  entry->set_text("ENV");
  
  attributesRefBuilder->get_widget(
    "MaxEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::StochosSubAlignment::textChanged)); 
  entry->set_text("ENV");
  
  attributesRefBuilder->get_widget(
    "DistEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::StochosSubAlignment::textChanged)); 
  entry->set_text("ENV");

}


FunctionGenerator::StochosSubAlignment::~StochosSubAlignment(){
}

void FunctionGenerator::StochosSubAlignment::switchTo(int _methodFlag){
  remove();
  if (_methodFlag == 0){
    Gtk::VBox* vbox;
    attributesRefBuilder->get_widget("RangeDistribVBox", vbox);
    add(*vbox);   

  }
  else {
    Gtk::HBox* hbox;
    attributesRefBuilder->get_widget("FunctionsHBox", hbox);
    add (*hbox);
  }
  if (next != NULL){
    next-> switchTo (_methodFlag);
  }
}

void FunctionGenerator::StochosSubAlignment::appendNewNode(StochosSubAlignment* _newNode){
  if (next == NULL){
    next = _newNode;
    _newNode->prev = this;
  }
  
  else {
    next-> appendNewNode(_newNode);
  }

}

std::string FunctionGenerator::StochosSubAlignment::toString( int _methodFlag){
  Gtk::Entry* entry;
  
  if (_methodFlag == 1){
    attributesRefBuilder->get_widget("FunctionsEntry", entry);
    return entry->get_text();
  }
    
  else {  
    attributesRefBuilder->get_widget("MinEntry", entry);
    std::string stringbuffer = entry->get_text() + ", ";
    attributesRefBuilder->get_widget("MaxEntry", entry);
    stringbuffer = stringbuffer + entry->get_text() + ", ";    
    attributesRefBuilder->get_widget("DistEntry", entry);
    stringbuffer = stringbuffer + entry->get_text();
    
    return stringbuffer;    
  }
}

void FunctionGenerator::StochosSubAlignment::setFunctionsEntry(std::string _string){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("FunctionsEntry", entry);
  entry->set_text(_string);
}

void FunctionGenerator::StochosSubAlignment::setMinEntry(std::string _string){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("MinEntry", entry);
  entry->set_text(_string);
}


void FunctionGenerator::StochosSubAlignment::setMaxEntry(std::string _string){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("MaxEntry", entry);
  entry->set_text(_string);
}


void FunctionGenerator::StochosSubAlignment::setDistEntry(std::string _string){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("DistEntry", entry);
  entry->set_text(_string);
}






void FunctionGenerator::StochosSubAlignment::removeButtonClicked(){
  parent->stochosRemoveNode(this);
}

void FunctionGenerator::StochosSubAlignment::textChanged(){
  parent->stochosTextChanged();
}


void FunctionGenerator::StochosSubAlignment::clear(){
  if (next != NULL){
    next->clear();
  }
  delete this;  
}



void FunctionGenerator::valuePickAbsRangeFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "ValuePickAbsRangeEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::valuePickLowFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "ValuePickLowEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnENV,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::valuePickHighFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "ValuePickHighEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnENV,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::valuePickDistFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "ValuePickDistEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnENV,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}



void FunctionGenerator::valuePickTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  Gtk::RadioButton* radiobutton1;
  Gtk::RadioButton* radiobutton2;  
  
  attributesRefBuilder->get_widget(
    "ValuePickAbsRangeEntry", entry);
  std::string stringbuffer = "ValuePick( "+ entry->get_text() + ", ";
  
  attributesRefBuilder->get_widget(
    "ValuePickLowEntry", entry);
  stringbuffer = stringbuffer + entry->get_text()+ ", ";
  
  attributesRefBuilder->get_widget(
    "ValuePickHighEntry", entry);
  stringbuffer = stringbuffer + entry->get_text()+ ", ";
  
  attributesRefBuilder->get_widget(
    "ValuePickDistEntry", entry);
  stringbuffer = stringbuffer + entry->get_text()+ ", ";
  
  attributesRefBuilder->get_widget("ValuePickElementsMeaningfulRadioButton", radiobutton1);  
  
  attributesRefBuilder->get_widget("ValuePickElementsModsRadioButton", radiobutton2); 
  
  if (radiobutton1->get_active()){
    stringbuffer = stringbuffer + "\"MEANINGFUL\", <";
  
  }
  else if (radiobutton2->get_active()){
    stringbuffer = stringbuffer + "\"MODS\", <";
  }
  else{
    stringbuffer = stringbuffer + "\"FAKE\", <";  
  }
  
  
  attributesRefBuilder->get_widget(
    "ValuePickElementsEntry", entry);
  stringbuffer = stringbuffer + entry->get_text()+ ">, ";
  

  attributesRefBuilder->get_widget("ValuePickWeightsPeriodicRadioButton", radiobutton1);  
  
  attributesRefBuilder->get_widget("ValuePickWeightsHierarchicRadioButton", radiobutton2); 

  if (radiobutton1->get_active()){
    stringbuffer = stringbuffer + "\"PERIODIC\", <";
  
  }
  else if (radiobutton2->get_active()){
    stringbuffer = stringbuffer + "\"HIERARCHIC\", <";
  }
  else{
    stringbuffer = stringbuffer + "\"INCLUDE\", <";  
  }

  attributesRefBuilder->get_widget(
    "ValuePickWeightsEntry", entry);
  stringbuffer = stringbuffer + entry->get_text()+ ">, ";  
  
  attributesRefBuilder->get_widget("ValuePickTypeVariableRadioButton", radiobutton1);   

  if (radiobutton1->get_active()){
    stringbuffer = stringbuffer + "\"VARIABLE\")";
  }
  else {
    stringbuffer = stringbuffer + "\"CONSTANT\")";
  }
  
  
  textview->get_buffer()->set_text(stringbuffer);

}





void FunctionGenerator::chooseLFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "ChooseLEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnSIV,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::chooseLEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "ChooseLEntry", entry);
  std::string stringbuffer = "ChooseL( " + entry->get_text() +  ")";
  textview->get_buffer()->set_text(stringbuffer);
}





void FunctionGenerator::getPatternFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "GetPatternEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnPAT,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::getPatternEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "GetPatternEntry", entry);
  std::string stringbuffer = "GetPattern( \"IN_ORDER\", " + entry->get_text() +  ")";
  textview->get_buffer()->set_text(stringbuffer);
}





void FunctionGenerator::makeListFunctionFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "MakeListFunctionEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnMakeListFun,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::makeListSizeFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "MakeListSizeEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::makeListTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  
  
  attributesRefBuilder->get_widget(
    "MakeListFunctionEntry", entry);
  std::string stringbuffer = "MakeList( " + entry->get_text() + ", ";
  
  attributesRefBuilder->get_widget(
    "MakeListSizeEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + ")";
  
  textview->get_buffer()->set_text(stringbuffer);
  

}

/*

FunctionGenerator::RawListHBox::RawListHBox(FunctionGenerator* _parentGenerator){
  parentGenerator = _parentGenerator;
  attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file("./LASSIE/src/UI/RawList.ui");
  }
  catch (const Glib::FileError& ex){
    std::cerr << "FileError: " << ex.what() << std::endl;
  }
  catch (const Gtk::BuilderError& ex){
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
   
   #else
  std::auto_ptr<Glib::Error> error;
  if (!attributesRefBuilder->add_from_file("./LASSIE/src/UI/RawList.ui", error)){
    std::cerr << error->what() << std::endl;
  }
   
   #endif

}

*/



void FunctionGenerator::envLibEnvelopeFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "EnvLibEnvelopeEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::envLibScalingFactorFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "EnvLibScalingFactorEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::envLibTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  
  
  attributesRefBuilder->get_widget(
    "EnvLibEnvelopeEntry", entry);
  std::string stringbuffer = "EnvLib( " + entry->get_text() + ", ";
  
  attributesRefBuilder->get_widget(
    "EnvLibScalingFactorEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + ")";
  
  textview->get_buffer()->set_text(stringbuffer);
  

}


void FunctionGenerator::readENVFileTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  
  
  attributesRefBuilder->get_widget(
    "ReadENVFileEntry", entry);
  std::string stringbuffer = "ReadENVFile( \"ENV/" + entry->get_text() + "\")";
  
  
  textview->get_buffer()->set_text(stringbuffer);
  

}


FunctionGenerator::MakeEnvelopeSubAlignment::MakeEnvelopeSubAlignment(FunctionGenerator* _parent){
	parent = _parent;
	prev = NULL;
	next = NULL;
	
	  attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file("./LASSIE/src/UI/FunGenMakeEnvelopeSubAlignment.ui");
  }
  catch (const Glib::FileError& ex){
    std::cerr << "FileError: " << ex.what() << std::endl;
  }
  catch (const Gtk::BuilderError& ex){
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
   
   #else
  std::auto_ptr<Glib::Error> error;
  if (!attributesRefBuilder->add_from_file("./LASSIE/src/UI/FunGenMakeEnvelopeSubAlignment.ui", error)){
    std::cerr << error->what() << std::endl;
  }
   
   #endif

  Gtk::VBox* vbox;
  attributesRefBuilder->get_widget("mainVBox", vbox);
  add(*vbox);
  
  Gtk::ComboBox* combobox;
  attributesRefBuilder->get_widget("SegTypeComboBox", combobox);

  typeTreeModel = Gtk::ListStore::create(typeColumns);
  combobox->set_model(typeTreeModel);
  
  Gtk::TreeModel::Row row = *(typeTreeModel->append());
  row[typeColumns.m_col_type] = envSegmentTypeLinear;
  row[typeColumns.m_col_name] = "LINEAR";
  
	row = *(typeTreeModel->append());
  row[typeColumns.m_col_type] = envSegmentTypeExponential;
  row[typeColumns.m_col_name] = "EXPONENTIAL";   

	row = *(typeTreeModel->append());
  row[typeColumns.m_col_type] = envSegmentTypeSpline;
  row[typeColumns.m_col_name] = "SPLINE"; 	
  
  combobox->signal_changed().connect( sigc::mem_fun(*this,&FunctionGenerator::MakeEnvelopeSubAlignment::textChanged) );
combobox->set_active(0);
    combobox->pack_start(typeColumns.m_col_name);
  
  
  attributesRefBuilder->get_widget("SegPropertyComboBox", combobox);

  propertyTreeModel = Gtk::ListStore::create(propertyColumns);
  combobox->set_model(propertyTreeModel);
  
  row = *(propertyTreeModel->append());
  row[propertyColumns.m_col_property] = envSegmentPropertyFlexible;
  row[propertyColumns.m_col_name] = "FLEXIBLE";
  
	row = *(propertyTreeModel->append());
  row[propertyColumns.m_col_property] = envSegmentPropertyFixed;
  row[propertyColumns.m_col_name] = "FIXED";  
  
  combobox->signal_changed().connect( sigc::mem_fun(*this,&FunctionGenerator::MakeEnvelopeSubAlignment::textChanged) );  
	combobox->set_active(0);  
    combobox->pack_start(propertyColumns.m_col_name);	
	
	Gtk::Button* button;  

  attributesRefBuilder->get_widget(
    "XValueFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::MakeEnvelopeSubAlignment::xValueFunButtonClicked));

  attributesRefBuilder->get_widget(
    "YValueFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::MakeEnvelopeSubAlignment::yValueFunButtonClicked));   
  
  attributesRefBuilder->get_widget(
    "InsertNodeButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::MakeEnvelopeSubAlignment::insertNodeButtonClicked));

  attributesRefBuilder->get_widget(
    "RemoveNodeButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::MakeEnvelopeSubAlignment::removeButtonClicked)); 
  
	Gtk::Entry* entry;
   
  attributesRefBuilder->get_widget(
    "XValueEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, &FunctionGenerator::MakeEnvelopeSubAlignment::textChanged)); 
  entry->set_text("");  

  attributesRefBuilder->get_widget(
    "YValueEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, &FunctionGenerator::MakeEnvelopeSubAlignment::textChanged)); 
  entry->set_text("");
   
   	
	

}


FunctionGenerator::MakeEnvelopeSubAlignment::~MakeEnvelopeSubAlignment(){
}


std::string FunctionGenerator::MakeEnvelopeSubAlignment::getXValueString(){
	Gtk::Entry* entry; 
  attributesRefBuilder->get_widget("XValueEntry", entry);
	return entry->get_text();
}

void FunctionGenerator::MakeEnvelopeSubAlignment::setXValueString(std::string _string){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget("XValueEntry", entry);
	return entry->set_text(_string);
}


std::string FunctionGenerator::MakeEnvelopeSubAlignment::getYValueString(){
	Gtk::Entry* entry; 
  attributesRefBuilder->get_widget("YValueEntry", entry);
	return entry->get_text();
}

void FunctionGenerator::MakeEnvelopeSubAlignment::setYValueString(std::string _string){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget("YValueEntry", entry);
	return entry->set_text(_string);
}


envSegmentType FunctionGenerator::MakeEnvelopeSubAlignment::getEnvSegmentType(){
  Gtk::ComboBox* combobox;
  attributesRefBuilder->get_widget("SegTypeComboBox", combobox);
  Gtk::TreeModel::iterator iter = combobox->get_active();
  Gtk::TreeModel::Row row = *iter;
  return row[typeColumns.m_col_type];
}

void FunctionGenerator::MakeEnvelopeSubAlignment::setEnvSegmentType(int _type){
  Gtk::ComboBox* combobox;
  attributesRefBuilder->get_widget("SegTypeComboBox", combobox);
  combobox->set_active(_type);
}

envSegmentProperty FunctionGenerator::MakeEnvelopeSubAlignment::getEnvSegmentProperty(){
  Gtk::ComboBox* combobox;
  attributesRefBuilder->get_widget("SegPropertyComboBox", combobox);
  Gtk::TreeModel::iterator iter = combobox->get_active();
  Gtk::TreeModel::Row row = *iter;
  return row[propertyColumns.m_col_property];

}


void FunctionGenerator::MakeEnvelopeSubAlignment::setEnvSegmentProperty(int _property){
  Gtk::ComboBox* combobox;
  attributesRefBuilder->get_widget("SegPropertyComboBox", combobox);
  combobox->set_active(_property);


}


void FunctionGenerator::MakeEnvelopeSubAlignment::clear(){
	if (next != NULL){
		next->clear();
	}
 	delete this;
}

void FunctionGenerator::MakeEnvelopeSubAlignment::xValueFunButtonClicked(){

  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "XValueEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::MakeEnvelopeSubAlignment::yValueFunButtonClicked(){

  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "YValueEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::MakeEnvelopeSubAlignment::insertNodeButtonClicked(){
	parent->makeEnvelopeInsertNode(this);

}

void FunctionGenerator::MakeEnvelopeSubAlignment::removeButtonClicked(){
	parent->makeEnvelopeRemoveNode(this);
}

void FunctionGenerator::MakeEnvelopeSubAlignment::textChanged(){
	parent->makeEnvelopeTextChanged();
}


void FunctionGenerator::makeEnvelopeTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  
  

  std::string stringbuffer = "MakeEnvelope( <";
  MakeEnvelopeSubAlignment* iter = makeEnvelopeSubAlignments;
  
  while (iter != NULL){
    stringbuffer = stringbuffer + iter->getXValueString();
    //if (iter->next != NULL){
      stringbuffer = stringbuffer + ", ";
    //}
    iter = iter->next;
  }
  
  attributesRefBuilder->get_widget( "MakeEnvelopeXValueEntry", entry);
  stringbuffer += entry->get_text() + ">,<";
 
  iter = makeEnvelopeSubAlignments;
  while (iter != NULL){
    stringbuffer = stringbuffer + iter->getYValueString();
    //if (iter->next != NULL){
      stringbuffer = stringbuffer + ", ";
    //}
    iter = iter->next;
  }
  
  attributesRefBuilder->get_widget( "MakeEnvelopeYValueEntry", entry);
  
  stringbuffer = stringbuffer + entry->get_text() + ">, <";
  iter = makeEnvelopeSubAlignments;
  while (iter != NULL){
    if(iter->getEnvSegmentType()==envSegmentTypeLinear){
      stringbuffer = stringbuffer + "\"LINEAR\"";
    }
    else if (iter->getEnvSegmentType()==envSegmentTypeExponential){
      stringbuffer = stringbuffer + "\"EXPONENTIAL\"";    
    }
    else {
     stringbuffer = stringbuffer + "\"SPLINE\"";
    }
  
    if (iter->next != NULL){
      stringbuffer = stringbuffer + ", ";
    }
    iter = iter->next;
  }
  
  stringbuffer = stringbuffer + ">, <";
  iter = makeEnvelopeSubAlignments;
  while (iter != NULL){
    if(iter->getEnvSegmentProperty()==envSegmentPropertyFlexible){
      stringbuffer = stringbuffer + "\"FLEXIBLE\"";
    }
    else {
     stringbuffer = stringbuffer + "\"FIXED\"";
    }
  
    if (iter->next != NULL){
      stringbuffer = stringbuffer + ", ";
    }
    iter = iter->next;
  }
  
  
  attributesRefBuilder->get_widget( "MakeEnvelopeScalingFactorEntry", entry);
  stringbuffer = stringbuffer + ">, " + entry->get_text() + ")";
  
  textview->get_buffer()->set_text(stringbuffer);

}

void FunctionGenerator::makeEnvelopeInsertNode(MakeEnvelopeSubAlignment* _insertAfter){
	MakeEnvelopeSubAlignment* newNode = new MakeEnvelopeSubAlignment(this);
	newNode->next = _insertAfter->next;
	newNode->prev = _insertAfter;
	_insertAfter->next = newNode;
	if (newNode->next != NULL){
		newNode->next->prev = newNode;
	}
	
	Gtk::VBox* vbox;
	attributesRefBuilder->get_widget( "MakeEnvelopeInnerVBox", vbox);
	//vbox->clear();
	
	MakeEnvelopeSubAlignment* temp = makeEnvelopeSubAlignments;
	while (temp != NULL){
		vbox->remove(*temp);
		temp = temp->next;
	}

  temp = makeEnvelopeSubAlignments;
	while (temp != NULL){
		vbox->pack_start(*temp, Gtk::PACK_SHRINK);
		temp = temp->next;
	}

	
	
	
	makeEnvelopeTextChanged();
	makeEnvelopeNumOfNodes++;	 
	show_all_children();
}

void FunctionGenerator::makeEnvelopeRemoveNode(MakeEnvelopeSubAlignment* _remove){
  if (makeEnvelopeNumOfNodes ==1) {
    return; //not allow to remove the last node
  }
  
  if( makeEnvelopeSubAlignments == _remove){ //special case
    makeEnvelopeSubAlignments = makeEnvelopeSubAlignments->next;
    makeEnvelopeSubAlignments->prev = NULL;
  }
  
  else {
  _remove->prev->next = _remove->next;
    if (_remove->next != NULL){
      _remove->next->prev = _remove->prev;
    }  
  }
  delete _remove;
  makeEnvelopeNumOfNodes--;
	Gtk::VBox* vbox;
	attributesRefBuilder->get_widget( "MakeEnvelopeInnerVBox", vbox);
	//vbox->clear();
	
	MakeEnvelopeSubAlignment* temp = makeEnvelopeSubAlignments;
	while (temp != NULL){
		vbox->pack_start(*temp, Gtk::PACK_SHRINK);
		temp = temp->next;
	}	 
	
	
	makeEnvelopeTextChanged();
	show_all_children();
}




void FunctionGenerator::makeEnvelopeScalingFactorFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "MakeEnvelopeScalingFactorEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}






void FunctionGenerator::makeEnvelopeXValueFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "MakeEnvelopeXValueEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::makeEnvelopeYValueFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "MakeEnvelopeYValueEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::makePatternOriginFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "MakePatternOriginEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::makePatternIntervalsFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "MakePatternIntervalsEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnList,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::makePatternTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "MakePatternOriginEntry", entry);
  std::string stringbuffer = "MakePattern( " + entry->get_text() +  ", ";
  attributesRefBuilder->get_widget(
    "MakePatternIntervalsEntry", entry);
  stringbuffer =stringbuffer + entry->get_text() + ")";
  textview->get_buffer()->set_text(stringbuffer);
  
}


void FunctionGenerator::expandPatternModuloFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "ExpandPatternModuloEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::expandPatternLowFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "ExpandPatternLowEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::expandPatternHighFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "ExpandPatternHighEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::expandPatternPatternFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "ExpandPatternPatternEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnPAT,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::expandPatternTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "ExpandPatternModuloEntry", entry);
  std::string stringbuffer = "ExpandPattern( \"IN_ORDER\", " + entry->get_text() +  ", ";
  attributesRefBuilder->get_widget(
    "ExpandPatternLowEntry", entry);
  stringbuffer =stringbuffer + entry->get_text() + ", ";
  
  attributesRefBuilder->get_widget(
    "ExpandPatternHighEntry", entry);
  stringbuffer =stringbuffer + entry->get_text() + ", ";  
  
  attributesRefBuilder->get_widget(
    "ExpandPatternPatternEntry", entry);
  stringbuffer =stringbuffer + entry->get_text() + ") ";  
  
  
  textview->get_buffer()->set_text(stringbuffer);
  
}

void FunctionGenerator::readPATFileOriginFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "ReadPATFileOriginEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::readPATFileTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "ReadPATFileNameEntry", entry);
  std::string stringbuffer = "ReadPATFile( \"PAT/" + entry->get_text() +  "\", ";
  attributesRefBuilder->get_widget(
    "ReadPATFileOriginEntry", entry);
  stringbuffer =stringbuffer + entry->get_text() + ")";
  textview->get_buffer()->set_text(stringbuffer);
  
}


void FunctionGenerator::readREVFileTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  
  
  attributesRefBuilder->get_widget(
    "ReadREVFileEntry", entry);
  std::string stringbuffer = "ReadREVFile( \"REV/" + entry->get_text() + "\")";
  
  
  textview->get_buffer()->set_text(stringbuffer);
  

}


void FunctionGenerator::REV_SimpleEntryFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "REV_SimpleEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_SimpleEntryTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  
  
  attributesRefBuilder->get_widget(
    "REV_SimpleEntry", entry);
  std::string stringbuffer = "<\"SIMPLE\", " + entry->get_text() + ">";
  
  
  textview->get_buffer()->set_text(stringbuffer);
  

}


void FunctionGenerator::REV_MediumReverbPercentFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "REV_MediumReverbPercentEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnENV,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_MediumHilowSpreadFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "REV_MediumHilowSpreadEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_MediumGainAllPassFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "REV_MediumGainAllPassEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_MediumDelayFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "REV_MediumDelayEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_MediumTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  
  
  attributesRefBuilder->get_widget(
    "REV_MediumReverbPercentEntry", entry);
  std::string stringbuffer = "<\"MEDIUM\", " + entry->get_text() + ", ";

  attributesRefBuilder->get_widget(
    "REV_MediumHilowSpreadEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + ", ";  

  attributesRefBuilder->get_widget(
    "REV_MediumGainAllPassEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + ", ";   
  
  attributesRefBuilder->get_widget(
    "REV_MediumDelayEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + ">";      
  textview->get_buffer()->set_text(stringbuffer);
  

}




void FunctionGenerator::REV_AdvancedReverbPercentFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "REV_AdvancedReverbPercentEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnENV,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_AdvancedCombGainListFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "REV_AdvancedCombGainListEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnList,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_AdvancedLPGainListFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "REV_LPGainListEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnList,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}



void FunctionGenerator::REV_AdvancedGainAllPassFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "REV_AdvancedGainAllPassEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_AdvancedDelayFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "REV_AdvancedDelayEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}



void FunctionGenerator::REV_AdvancedTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  
  
  attributesRefBuilder->get_widget(
    "REV_AdvancedReverbPercentEntry", entry);
  std::string stringbuffer = "<\"ADVANCED\", " + entry->get_text() + ", ";

  attributesRefBuilder->get_widget(
    "REV_AdvancedCombGainListEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + ", ";  

  attributesRefBuilder->get_widget(
    "REV_LPGainListEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + ", ";  

  attributesRefBuilder->get_widget(
    "REV_AdvancedGainAllPassEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + ", ";   
  
  attributesRefBuilder->get_widget(
    "REV_AdvancedDelayEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + ">";      
  textview->get_buffer()->set_text(stringbuffer);
  

}

void FunctionGenerator::readSIVFileTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  
  
  attributesRefBuilder->get_widget(
    "ReadSIVFileEntry", entry);
  std::string stringbuffer = "ReadSIVFile( \"SIV/" + entry->get_text() + "\")";
  
  
  textview->get_buffer()->set_text(stringbuffer);
  

}





void FunctionGenerator::makeSieveLowFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "MakeSieveLowEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::makeSieveHighFunButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "MakeSieveHighEntry", entry);
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}




void FunctionGenerator::makeSieveTextChanged(){
  
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  Gtk::RadioButton* radiobutton1;
  Gtk::RadioButton* radiobutton2;  
  
  

  
  attributesRefBuilder->get_widget(
    "MakeSieveLowEntry", entry);
  std::string stringbuffer = "MakeSieve( "+ entry->get_text() + ", ";
  
  attributesRefBuilder->get_widget(
    "MakeSieveHighEntry", entry);
  stringbuffer = stringbuffer + entry->get_text()+ ", ";
  
  
  attributesRefBuilder->get_widget("MakeSieveElementsMeaningfulRadioButton", radiobutton1);  
  
  attributesRefBuilder->get_widget("MakeSieveElementsModsRadioButton", radiobutton2); 
  
  if (radiobutton1->get_active()){
    stringbuffer = stringbuffer + "\"MEANINGFUL\", <";
  
  }
  else if (radiobutton2->get_active()){
    stringbuffer = stringbuffer + "\"MODS\", <";
  }
  else{
    stringbuffer = stringbuffer + "\"FAKE\", <";  
  }
  
  
  attributesRefBuilder->get_widget(
    "MakeSieveElementsEntry", entry);
  stringbuffer = stringbuffer + entry->get_text()+ ">, ";
  

  attributesRefBuilder->get_widget("MakeSieveWeightsPeriodicRadioButton", radiobutton1);  
  
  attributesRefBuilder->get_widget("MakeSieveWeightsHierarchicRadioButton", radiobutton2); 

  if (radiobutton1->get_active()){
    stringbuffer = stringbuffer + "\"PERIODIC\", <";
  
  }
  else if (radiobutton2->get_active()){
    stringbuffer = stringbuffer + "\"HIERARCHIC\", <";
  }
  else{
    stringbuffer = stringbuffer + "\"INCLUDE\", <";  
  }

  attributesRefBuilder->get_widget(
    "MakeSieveWeightsEntry", entry);
  stringbuffer = stringbuffer + entry->get_text()+ ">) ";  

  
  textview->get_buffer()->set_text(stringbuffer);
  

}


void FunctionGenerator::readSPAFileTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry; 
  
  
  attributesRefBuilder->get_widget(
    "ReadSPAFileEntry", entry);
  std::string stringbuffer = "ReadSPAFile( \"SPA/" + entry->get_text() + "\")";
  
  
  textview->get_buffer()->set_text(stringbuffer);
}

FunctionGenerator::SPAPartialAlignment::SPAPartialAlignment( FunctionGenerator* _parent, SPAChannelAlignment* _parentChannel){
  parent = _parent;
  parentChannel = _parentChannel;
  prev = NULL;
  next = NULL;
  prevChan = NULL;
  nextChan = NULL;
	attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file("./LASSIE/src/UI/FunGenSPAPartialAlignment.ui");
  }
  catch (const Glib::FileError& ex){
    std::cerr << "FileError: " << ex.what() << std::endl;
  }
  catch (const Gtk::BuilderError& ex){
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
   
   #else
  std::auto_ptr<Glib::Error> error;
  if (!attributesRefBuilder->add_from_file("./LASSIE/src/UI/FunGenSPAPartialAlignment.ui", error)){
    std::cerr << error->what() << std::endl;
  }
   
   #endif

  Gtk::HBox* hbox;
  attributesRefBuilder->get_widget("MainHBox", hbox);
  add(*hbox);
  
  Gtk::Button* button;
  attributesRefBuilder->get_widget( "RemoveButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::SPAPartialAlignment::removePartialButtonClicked));  

  attributesRefBuilder->get_widget( "InsertButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::SPAPartialAlignment::insertPartialButtonClicked));

  attributesRefBuilder->get_widget( "FunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::SPAPartialAlignment::funButtonClicked));
  
  
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget( "entry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::SPAPartialAlignment::textChanged));   
}

FunctionGenerator::SPAPartialAlignment::~SPAPartialAlignment(){
}

void FunctionGenerator::SPAPartialAlignment::setNumber(int _number){
  std::stringstream out;
  out<< _number;
  std::string stringbuffer = "Partial " + out.str()+ " ";
  Gtk::Label* label;
  attributesRefBuilder->get_widget( "TitleLabel", label);  
  label->set_text(stringbuffer);
}

void FunctionGenerator::SPAPartialAlignment::clearPartialsInSameChannel(){
  if (next != NULL){
    next->clearPartialsInSameChannel();
  }
  delete this;
}  


void FunctionGenerator::SPAPartialAlignment::clearPartialsWithSameNumber(){
  if (nextChan != NULL){
    nextChan->clearPartialsWithSameNumber();
  }
  delete this;
  
}

std::string FunctionGenerator::SPAPartialAlignment::getText(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget( "entry", entry);
  return entry->get_text();    
}


void FunctionGenerator::SPAPartialAlignment::setText(std::string _string){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget( "entry", entry);
  entry->set_text(_string);    
}



void FunctionGenerator::SPAPartialAlignment::textChanged(){
  parent->SPATextChanged();
}

void FunctionGenerator::SPAPartialAlignment::funButtonClicked(){
  Gtk::Entry* entry; 
  attributesRefBuilder->get_widget(
    "entry", entry);
  if (!entry->get_sensitive()){
    return;
  }
    
  FunctionGenerator* generator = new FunctionGenerator(functionReturnENV,entry->get_text());
  generator->run(); 
   
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::SPAPartialAlignment::insertPartialButtonClicked(){
  parent->SPAInsertPartial(this);
}

void FunctionGenerator::SPAPartialAlignment::removePartialButtonClicked(){
  parent->SPARemovePartial(this);
}

void FunctionGenerator::SPAPartialAlignment::entryEditSwitch(int _flag){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget( "entry", entry);
  if (_flag ==0){
    entry->set_sensitive(false);
  }
  else {
    entry->set_sensitive(true);
  }
}



FunctionGenerator::SPAChannelAlignment::SPAChannelAlignment( FunctionGenerator* _parent, int _numOfPartial, int _flagButtonsShown){
  parent = _parent;
  prev = NULL;
  next = NULL;
  flagButtonsShown = 1;

  
  attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file("./LASSIE/src/UI/FunGenSPAChannelAlignment.ui");
  }
  catch (const Glib::FileError& ex){
    std::cerr << "FileError: " << ex.what() << std::endl;
  }
  catch (const Gtk::BuilderError& ex){
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
   
   #else
  std::auto_ptr<Glib::Error> error;
  if (!attributesRefBuilder->add_from_file("./LASSIE/src/UI/FunGenSPAChannelAlignment.ui", error)){
    std::cerr << error->what() << std::endl;
  }
   
   #endif

  Gtk::HBox* hbox;
  attributesRefBuilder->get_widget("MainHBox", hbox);
  add(*hbox);

  attributesRefBuilder->get_widget("OuterVBox", outerVBox); 
  attributesRefBuilder->get_widget("MainVBox", mainVBox);

  attributesRefBuilder->get_widget( "InsertChannelButton", insertButton);
  insertButton->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::SPAChannelAlignment::insertChannelButtonClicked));  

  attributesRefBuilder->get_widget( "RemoveChannelButton", removeButton);
  removeButton->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::SPAChannelAlignment::removeChannelButtonClicked));
  
  if (_flagButtonsShown == 0){
    hideButtons();
  }
  
  partials = new SPAPartialAlignment(_parent, this);
  mainVBox->pack_start( *partials, Gtk::PACK_SHRINK);
  SPAPartialAlignment* temp1 = partials;
  SPAPartialAlignment* temp2;
  int counter = 1;
  while (counter < _numOfPartial){
    temp2 = new SPAPartialAlignment(_parent, this);
    temp1->next = temp2;
    temp2 ->prev = temp1;
    mainVBox->pack_start( *temp2, Gtk::PACK_SHRINK);    
    temp1 = temp2;
    counter++;
  }
  refreshPartialNumbers();
 
}

FunctionGenerator::SPAChannelAlignment::~SPAChannelAlignment(){
  if (partials!= NULL){
    partials->clearPartialsInSameChannel();
  }
}

void FunctionGenerator::SPAChannelAlignment::setChannelTitle(std::string _title){
  Gtk::Label* label;
  attributesRefBuilder->get_widget( "TitleLabel", label);
  label ->set_text (_title);  
}

void FunctionGenerator::SPAChannelAlignment::setChannelNumber(int _number){

  std::stringstream out;
  out<< _number;

  std::string buffer = "Channel "+out.str();
  
  Gtk::Label* label;
  attributesRefBuilder->get_widget( "TitleLabel", label);
  label ->set_text (buffer); 

}




void FunctionGenerator::SPAChannelAlignment::hideButtons(){
  if (flagButtonsShown ==0 ){
    return;
  }
  outerVBox->remove(*removeButton);
  outerVBox->remove(*insertButton);
  flagButtonsShown = 0;
  show_all_children();
}

void FunctionGenerator::SPAChannelAlignment::showButtons(){
  if (flagButtonsShown ==1){
    return;
  }
  outerVBox->remove(*mainVBox);
  outerVBox->pack_start(*insertButton,Gtk::PACK_SHRINK);
  outerVBox->pack_start(*removeButton,Gtk::PACK_SHRINK);
  outerVBox->pack_start(*mainVBox,Gtk::PACK_EXPAND_WIDGET);
  flagButtonsShown = 1;
  show_all_children();
}

void FunctionGenerator::SPAChannelAlignment::clear(){
  if (next != NULL){
    next->clear();
  }
  delete this;
}

void FunctionGenerator::SPAChannelAlignment::refreshPartialNumbers(){
  int counter = 1;
  SPAPartialAlignment* current = partials;
  while (current != NULL){
    current -> setNumber(counter);
    counter ++;
    current = current ->next;
  }
} 

void FunctionGenerator::SPAChannelAlignment::refreshLayout(){
  SPAPartialAlignment* current = partials;
  while (current != NULL){
    mainVBox->remove(*current);
    current = current ->next;
  }
  current = partials;
  while (current!= NULL){
    mainVBox->pack_start(*current,Gtk::PACK_SHRINK);
    current = current ->next;
  }
} 



std::string FunctionGenerator::SPAChannelAlignment::getText(){
  std::string stringbuffer = "";
  SPAPartialAlignment* temp = partials;
  while (temp != NULL){
    stringbuffer = stringbuffer + temp->getText();
    if (temp->next!= NULL){
      stringbuffer = stringbuffer + ", ";
    }
    temp = temp ->next;
  }
  return stringbuffer;
  
}

int FunctionGenerator::SPAChannelAlignment::getButtonsShownStatus(){
  return flagButtonsShown;
}

void FunctionGenerator::SPAChannelAlignment::insertChannelButtonClicked(){
  parent->SPAInsertChannel(this);
}

void FunctionGenerator::SPAChannelAlignment::removeChannelButtonClicked(){
  parent->SPARemoveChannel(this);
}


void FunctionGenerator::SPARemoveChannel(SPAChannelAlignment* _remove){
  if (SPAMethodFlag!= 1||SPANumOfChannels ==1){
    return;
  }
  
  SPANumOfChannels --;
  
  if (_remove == SPAChannelAlignments){ //specialCase
    SPAChannelAlignments = SPAChannelAlignments->next;
    SPAChannelAlignments->prev = NULL;
    SPAPartialAlignment* partial = SPAChannelAlignments->partials;
    while (partial!= NULL){
      partial->prevChan = NULL;
      partial = partial->next;
    }
  }
  
  else { //normal cases
    _remove->prev->next = _remove->next;
    
    SPAPartialAlignment* partial = _remove->partials;
    while (partial!= NULL){
      partial->prevChan->nextChan = partial->nextChan;
      partial = partial->next;
    }
    if (_remove->next!= NULL){
      _remove->next->prev = _remove->prev;
      partial = _remove->partials;
      while (partial!= NULL){
        partial->nextChan->prevChan = partial->prevChan;
        partial = partial->next;
      }  
    }
  }
  
  delete _remove;
  
  int counter = 1;
  SPAChannelAlignment* current = SPAChannelAlignments;
  while (current != NULL){
    current ->setChannelNumber( counter);
    current = current ->next;
    counter++;
  }
  SPATextChanged();
  show_all_children();
}

void FunctionGenerator::SPAInsertChannel(SPAChannelAlignment* _insertAfter){
  if (SPAMethodFlag!= 1){
    return;
  }

  SPANumOfChannels ++;
  SPAChannelAlignment* newChannel = new SPAChannelAlignment(this, SPANumOfPartials, _insertAfter->getButtonsShownStatus());

  newChannel->prev = _insertAfter;
  newChannel->next = _insertAfter->next;
  _insertAfter->next = newChannel;
  if (newChannel->next != NULL){
    newChannel->next->prev = newChannel;
  }

  SPAPartialAlignment* temp1 = _insertAfter->partials;
  SPAPartialAlignment* temp2 = newChannel->partials;

  while (temp1 != NULL){
    temp2->nextChan = temp1->nextChan;
    temp1->nextChan = temp2;
    temp2->prevChan = temp1;
    temp1 = temp1->next;
    temp2 = temp2->next;
  }

  temp2 = newChannel->partials;
  if (temp2->nextChan != NULL){
    while (temp2 != NULL){
      temp2->nextChan->prevChan = temp2;
      temp2 = temp2->next;
    }
  }

  Gtk::HBox* hbox;
  attributesRefBuilder->get_widget( "SPAMainHBox", hbox);

  SPAChannelAlignment* temp = SPAChannelAlignments;
  while (temp != NULL){
    hbox->remove(*temp);
    temp = temp->next;
  }

  temp = SPAChannelAlignments;
  while (temp !=NULL){
    hbox->pack_start(*temp,Gtk::PACK_SHRINK);
    temp = temp->next;
  }
  
    int counter = 1;
  SPAChannelAlignment* current = SPAChannelAlignments;
  while (current != NULL){
    current ->setChannelNumber( counter);
    current = current ->next;
    counter++;
  }
  SPAApplyByRadioButtonClicked();
  SPATextChanged();
  
  show_all_children();
}
  
void FunctionGenerator::SPARemovePartial(SPAPartialAlignment* _remove){
  
  if (SPANumOfPartials==1||SPAApplyFlag==0){
    return;
  }

  SPANumOfPartials --;
  SPAPartialAlignment* leftmost = _remove;
  while (leftmost->prevChan!= NULL){
    leftmost = leftmost->prevChan;
  }

  
  SPAPartialAlignment* temp = leftmost;

  if (temp->prev ==NULL){//specialCase
    while (temp!= NULL){
      temp->parentChannel->partials = temp->next;
      temp->next->prev = NULL;
      temp = temp->nextChan;
    }

  }
  else {// normal case
    while (temp != NULL){
      temp->prev->next = temp->next;
      if (temp->next!= NULL){
        temp->next->prev = temp->prev;
      }
      temp = temp->nextChan;
    }

  }

  SPAChannelAlignment* temp1 = SPAChannelAlignments;

  while (temp1 != NULL){
    temp1->refreshPartialNumbers();
    temp1 = temp1->next;
  }

  leftmost->clearPartialsWithSameNumber();
  SPATextChanged();
  show_all_children();
  
  

}
  
  
void FunctionGenerator::SPAInsertPartial (SPAPartialAlignment* _insertAfter){  
  if (SPAApplyFlag== 0){
    return;
  }
  
  SPANumOfPartials ++;
  SPAPartialAlignment* leftmost = _insertAfter;
  while (leftmost->prevChan!= NULL){
    leftmost = leftmost->prevChan;
  }
  
  
  SPAPartialAlignment* temp1 = leftmost;
  SPAPartialAlignment* temp2 = NULL;
  SPAPartialAlignment* newPartial;
  
  while (temp1!= NULL){
    newPartial = new SPAPartialAlignment( this, temp1->parentChannel);
    newPartial->next = temp1->next;
    
    temp1->next = newPartial;
    newPartial->prev = temp1;
    if (newPartial->next != NULL){
      newPartial->next->prev = newPartial;
    }
    newPartial->prevChan = temp2;
    if (temp2!= NULL){
      temp2->nextChan = newPartial;
    }
    temp2 = newPartial;
    temp1 = temp1->nextChan;
  }
  
  SPAChannelAlignment* temp = SPAChannelAlignments;
  
  while (temp != NULL){
    temp->refreshPartialNumbers();
    temp->refreshLayout();
    temp = temp->next;
  }
  
  SPATextChanged();
  show_all_children();
    
}  
  
void FunctionGenerator::SPAMethodRadioButtonClicked(){
  Gtk::RadioButton* stereo;
  Gtk::RadioButton* multi_pan;

  attributesRefBuilder->get_widget("SPAStereoRadioButton", stereo);
  attributesRefBuilder->get_widget("SPAMulti_PanRadioButton", multi_pan);
  
  
  if (stereo->get_active()){
    if (SPAMethodFlag ==0){
      return;
    }
    SPAChannelAlignment* toDelete =SPAChannelAlignments->next;

    if (toDelete != NULL){
      toDelete->clear();

      SPAPartialAlignment* partial = SPAChannelAlignments->partials;

      while (partial!= NULL){
        partial->nextChan = NULL;
        partial= partial->next;
      }
    }  
    SPAChannelAlignments->next = NULL;

    SPAChannelAlignments->setChannelTitle("");
    SPANumOfChannels = 1;
    SPAMethodFlag = 0;
    SPAChannelAlignments->hideButtons();
    
  }//end stereo
  else if (multi_pan->get_active()){
    if (SPAMethodFlag ==1){
      return;
    }

    SPAMethodFlag = 1;
    SPAChannelAlignments->setChannelTitle("Channel 1");
    SPAChannelAlignments->showButtons();
    if (SPAChannelAlignments->next != NULL){

      SPAChannelAlignments->next->setChannelTitle("Channel 2");
      SPAChannelAlignments->next->showButtons();
    }

  }//end multipan
  else{ //polar
    if (SPAMethodFlag ==2){
      return;
    }  

    if (SPANumOfChannels ==1){
      SPAMethodFlag =1; //just for excuting next statement;
      SPAInsertChannel(SPAChannelAlignments);

    }
    else if (SPANumOfChannels >2){

      SPAChannelAlignments->next->next->clear();
      SPAPartialAlignment* partial = SPAChannelAlignments->next->partials;

      while (partial!= NULL){
        partial->nextChan = NULL;
        partial= partial->next;
      }      
    }


    SPANumOfChannels = 2;
    SPAMethodFlag = 2;
    SPAChannelAlignments->setChannelTitle("Theta");
    SPAChannelAlignments->next->setChannelTitle("Radius"); 
    SPAChannelAlignments->hideButtons();
    SPAChannelAlignments->next->hideButtons();
    SPAChannelAlignments->next->next = NULL;

        SPAApplyByRadioButtonClicked(); //make sure it shows up as partial or sound
  }//end polar
  
  SPATextChanged();
  show_all_children();

}
  

void FunctionGenerator::SPAApplyByRadioButtonClicked(){
  Gtk::RadioButton* radiobutton;
  attributesRefBuilder->get_widget( "SPASoundRadioButton", radiobutton);
    
  if (radiobutton->get_active()){
    SPAApplyFlag = 0;
    SPAChannelAlignment* temp = SPAChannelAlignments;
    
    while (temp != NULL){
      SPAPartialAlignment* partial = temp->partials->next;
      while (partial!= NULL){
        partial->entryEditSwitch(0);
        partial = partial->next;
      }
      temp = temp->next;    
    }
    
  }  
  else {
    SPAApplyFlag = 1;
    SPAChannelAlignment* temp = SPAChannelAlignments;
    
    while (temp != NULL){
      SPAPartialAlignment* partial = temp->partials->next;
      while (partial!= NULL){
        partial->entryEditSwitch(1);
        partial = partial->next;
      }
      temp = temp->next;    
    }
  }
  SPATextChanged();
  show_all_children();
}  
  
  
  
void FunctionGenerator::SPATextChanged(){
  std::string stringbuffer = "";
  int method;
  int apply;

  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::RadioButton* radiobutton1;
  Gtk::RadioButton* radiobutton2;
  attributesRefBuilder->get_widget( "SPAStereoRadioButton", radiobutton1);
  attributesRefBuilder->get_widget( "SPAMulti_PanRadioButton", radiobutton2);
  
  if (radiobutton1->get_active()){
    stringbuffer = "<\"STEREO\", ";
    method = 0;
  }
  else if (radiobutton2->get_active()){
    stringbuffer = "<\"MULTI_PAN\", ";
    method = 1;
  }
  else {
    stringbuffer = "<\"POLAR\", ";
    method = 2;
  } 
  attributesRefBuilder->get_widget( "SPASoundRadioButton", radiobutton1);  
  if (radiobutton1->get_active()){
    stringbuffer = stringbuffer + "\"SOUND\", ";
    apply = 0;
  }
  else {
    stringbuffer = stringbuffer + "\"PARTIAL\", ";
    apply =1;
  }
  
  if (method ==0){
    if (apply == 0){
      stringbuffer = stringbuffer + "<" + SPAChannelAlignments->partials->getText()+ ">>";
    }
    else {
      stringbuffer = stringbuffer + "<" + SPAChannelAlignments->getText()+ ">>";
    }
  }//end method = 0
  else if (method ==1||method ==2){
    if (apply ==0){
      stringbuffer = stringbuffer + "<<";
      SPAChannelAlignment* temp = SPAChannelAlignments;
      while (temp != NULL){
        stringbuffer = stringbuffer + temp->partials->getText();
        if (temp->next!=NULL){
          stringbuffer = stringbuffer + ">, <";
        }
        temp = temp->next;
      }
      stringbuffer = stringbuffer + ">>>";
    }
    else {
      stringbuffer = stringbuffer + "<<";
      SPAChannelAlignment* temp = SPAChannelAlignments;
      while (temp != NULL){
        stringbuffer = stringbuffer + temp->getText();
        if (temp->next!=NULL){
          stringbuffer = stringbuffer + ">, <";
        }
        temp = temp->next;
      }
      stringbuffer = stringbuffer + ">>>";    
    
    }
  
  
  
  }//end method = 1
  
  else {
  
  
  } 
  
    textview->get_buffer()->set_text(stringbuffer);
   

}
  
  
  
std::string FunctionGenerator::getFunctionString(FileValue* _value,FunctionReturnType _returnType){
  std::string stringbuffer;
  char charbuffer[20];
  if (_value->isFunction()){
    stringbuffer= _value->getFtnString();
    
    bool isNotStatic = (stringbuffer.find("CURRENT") == string::npos &&
        stringbuffer.find("AVAILA") == string::npos);
        
    if (isNotStatic){
      stringbuffer = stringbuffer + "(";
    }
    list<FileValue> arguments = _value->getFtnArgs();
    list<FileValue>::iterator iter = arguments.begin();
    FileValue* argument;    
    while (iter != arguments.end()){
      argument = &(*iter);
      stringbuffer = stringbuffer + getFunctionString (argument, _returnType);
      
      iter++;
      if (iter != arguments.end()){
        stringbuffer = stringbuffer + ",";   
      }
    }

    if (isNotStatic){
      stringbuffer = stringbuffer + ")";
    } 
       
  }
  else if (_value->isNumber()){
    if (_returnType == functionReturnInt){    
      int intNumber = _value->getInt();
      sprintf( charbuffer, "%d", intNumber);
      stringbuffer = string(charbuffer);
    } 
    else { //float
      float floatNumber = _value->getFloat();
      
      if (floatNumber -(int)floatNumber ==0){
        int intNumber = _value->getInt();
        sprintf( charbuffer, "%d", intNumber);
        stringbuffer = string(charbuffer);          
      }
      else {      
        sprintf( charbuffer, "%.5f", floatNumber);
        stringbuffer = string(charbuffer); 
      }
    }
  }
  
  else if (_value->isString()){
    stringbuffer ="\""+ _value->getString() + "\"";
   // stringbuffer =_value->getString();
  }
  else if (_value->isList()){
    stringbuffer = fileValueListToString(_value->getList(), _returnType);
  
  
  }
  /*
  else {
    if (_returnType == functionReturnInt){    
      int intNumber = _value->getInt();
      sprintf( charbuffer, "%d", intNumber);
      stringbuffer = string(charbuffer);
    }
    else if (_returnType == functionReturnFloat){
      float floatNumber = _value->getFloat();
      sprintf( charbuffer, "%.5f", floatNumber);
      stringbuffer = string(charbuffer);    
    }
    else if (_returnType == functionReturnString){
      stringbuffer = _value->getString();
    }
    // else if (_returnType == functionReturnEnvelopeList){
      //stringbuffer = listToString(_value->getList(),functionReturnENV);
    //}
    else if (_returnType == functionReturnENV){
      if (_value->isInt()){

      
      }
      else if (_value->isFloat()){
      float floatNumber = _value->getFloat();
      sprintf( charbuffer, "%.5f", floatNumber);
      stringbuffer = string(charbuffer);
      
      }
      else if (_value->isString()){
      
      }
          
    }
    
  }
  */
  return stringbuffer;
}  
  


list<std::string> FunctionGenerator::fileValueListToStringList(list<FileValue> _valueList,FunctionReturnType _returnType){
  list<string> stringList;
  
  list<FileValue>::iterator iter = _valueList.begin();
  FileValue* value;
  
  while (iter != _valueList.end()){
    value = &(*iter);
    stringList.push_back( getFunctionString(value, _returnType));
    iter++;
  }
  return stringList;
}
 
  
  
std::string FunctionGenerator::stringListToString(list<std::string> _list){
  string stringbuffer = "<";
  list<std::string>::iterator iter = _list.begin();
  while (iter != _list.end()){
    stringbuffer += *iter;
    iter ++;
    if (iter!= _list.end()){
      stringbuffer += ",";
    }
  }
  stringbuffer += ">";
  return stringbuffer;

}

std::string FunctionGenerator::fileValueListToString(list<FileValue> _valueList,FunctionReturnType _returnType){
  return stringListToString(fileValueListToStringList( _valueList, _returnType));
}
  
  


