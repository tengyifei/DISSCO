/*******************************************************************************
 *
 *  File Name     : FileOperations.h
 *  Date created  : Feb. 08 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file contains several File Operations used by LASSIE
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

#include "FileOperations.h"

#include "MainWindow.h"

ProjectViewController* FileOperations::newProject(MainWindow* _mainWindow){
  _mainWindow->set_title("LASSIE");

  // setup the new project dialog window 
  Gtk::FileChooserDialog dialog("New Project", Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for(*_mainWindow);
  
  // Add response buttons the the dialog:
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
  //dialog.set_do_overwrite_confirmation(true); 

  // Show the dialog and wait for a user response:
  int result = dialog.run();

  // Handle the response:
  if(result == Gtk::RESPONSE_OK){
    bool checker = false;
    while(checker == false){
      checker = checkFilePathValidity(dialog.get_filename());
      if(checker == false){
        fileNameError window;
        return newProject(_mainWindow);
      }else{
         std::string command = dialog.get_filename();;
         int mkdirChecker = mkdir(command.c_str(),S_IRWXU);
         if(mkdirChecker == -1){
           fileNameExist window;
           return newProject(_mainWindow);
         }else{
           std::string pathAndName = dialog.get_filename();
           createDirectories(pathAndName);
           return new ProjectViewController(pathAndName, _mainWindow);
         }
      }
    }
  }

  return NULL;
}


void FileOperations::createDirectories(std::string _pathAndName){
  std::string command;
  command = "mkdir "+ _pathAndName + "/T";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/H";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/M";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/L";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/B";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/S";
  system(command.c_str());	
  command = "mkdir "+ _pathAndName + "/PAT";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/REV";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/ENV";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/SIV";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/SPA";
  system(command.c_str());
}


void FileOperations::close(MainWindow* _mainWindow){
//TODO
  /*
   create a dialog message to ask the user if they want to save the file
   Gtk::MessageDialog dialog(*this, 
   "Save current project before creating a new one?",
   false /* use_markup *//*, Gtk::MESSAGE_QUESTION,
                          Gtk::BUTTONS_OK,Gtk::BUTTONS_CLOSE);
                          dialog.set_secondary_text("You will lose your unsaved change in the current"
                          " project if you create a new project without saving the current one.\n"
                          "\n\n"
                          "Press Yes to save your current project.\n"
                          "Press No to create a new project without saving the current one.\n"
                          "Press Cancel to cancel creating a new project.");
                          dialog.add_button(Gtk::Stock::NO, Gtk::RESPONSE_NO);
                          dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
                          int result = dialog.run();
                          
                          //determine action based on the result
                          if (result == Gtk::RESPONSE_OK){ 
                          menuFileSave();
                          menuFileClose();
                          createNewProject();
                          }
                          else if ( result == Gtk::RESPONSE_NO){ // discard current project
                          menuFileClose();
                          createNewProject();
                          }
                          else { // cancel, leave the dialog
                          }
                          */
  
}


std::string FileOperations::stringToFileName(std::string _filePath){
  int j = 0;
  int stringLength = _filePath.length()-1;
  std::string aWord;
  std::string fileName;

  aWord = aWord.assign(_filePath,stringLength,1);
  while(aWord != "/"){
    aWord = aWord.assign(_filePath,stringLength--,1);
    j++;
  }

  fileName = aWord.assign(_filePath,stringLength+2,j-1);
  return fileName;
}


std::string FileOperations::stringToPath(std::string _filePath){
  int stringLength = _filePath.length()-1;
  std::string aWord;

  aWord = aWord.assign(_filePath,stringLength,1);
  while(aWord != "/"){
    aWord = aWord.assign(_filePath,stringLength--,1);
  }
  _filePath = aWord.assign(_filePath,0,stringLength+2);
  return _filePath;
}


bool FileOperations::checkFilePathValidity(std::string _filePath){
  std::string fileName = stringToFileName(_filePath);
  int fileNameLength = fileName.length();

  std::string allowableSymbol(".-_"); // set the allowable symbol.
  std::string checkSymbol;
  int allowableSymbolLength = allowableSymbol.length();

  std::string checkWord;
  bool check = true;

  while(fileNameLength > 0){
    checkWord = checkWord.assign(fileName,fileNameLength-1,1);
    if(!((checkWord >= "0" // check "0~9" and "A~Z" and "a~z"
       && checkWord <= "9")
      || (checkWord >= "A"
       && checkWord <= "Z")
      || (checkWord >= "a"
       && checkWord <= "z"))){
      check = false;

      while(allowableSymbolLength > 0){// check symbol.
        checkSymbol =
        checkSymbol.assign(allowableSymbol,allowableSymbolLength-1,1);
        if(checkWord == checkSymbol) check = true;
        allowableSymbolLength--;
      }

      allowableSymbolLength = allowableSymbol.length();
    }
    fileNameLength--;
  }

  checkWord = checkWord.assign(fileName,0,1);
  fileNameLength = fileName.length();
  if(checkWord == "." || fileNameLength > 256) check = false;
  // check first word whether is "." and fileName length.
  return check;
}


fileNameError::fileNameError(){
  fileNameError::inappropriateProjectName();
}


fileNameError::~fileNameError(){}


void fileNameError::inappropriateProjectName(){
  Gtk::MessageDialog dialog(*this, "Inappropriate Project Name");
  dialog.set_secondary_text(
          "In order to guarantee the functionality of CMOD, the legal project"
          " name should contain only letters and digets.");

  dialog.run();
}


fileNameExist::fileNameExist(){
  fileNameExist::duplicateProjectName();
}


fileNameExist::~fileNameExist(){}


void fileNameExist::duplicateProjectName(){
  Gtk::MessageDialog dialog(*this, "Folder with the same name exists");
  dialog.set_secondary_text(
          "When creating a new project, LASSIE creats a directory with the"
          " new project name in the specified path. However, the directory with " 
          "the same name as the new project being created has already existed." 
          " Please give your new project a different name or rename the"
          " existing directory." );

  dialog.run();
}



ProjectViewController* FileOperations::openProject(MainWindow* _mainWindow){
  _mainWindow->set_title("LASSIE");
  
  // setup the open project dialog window 
  Gtk::FileChooserDialog dialog("Open existing project", Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*_mainWindow);
  
  // Add response buttons the the dialog:
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
  //dialog.set_do_overwrite_confirmation(true); 

  // Show the dialog and wait for a user response:
  int result = dialog.run();
  
  // Handle the response:
  if(result == Gtk::RESPONSE_OK){		
    std::string pathFileString = dialog.get_filename();
    //remove the filename, only the path is left.
    std::string projectTitle = stringToPath(pathFileString);

    //this chunk remove the last / in the path
    size_t pos = projectTitle.find_last_of("/");
    if(pos == projectTitle.length()-1){
      projectTitle.erase(projectTitle.length()-1);
    } 
    return new ProjectViewController(projectTitle, _mainWindow,0);
    
  }
  
  return NULL;
}



