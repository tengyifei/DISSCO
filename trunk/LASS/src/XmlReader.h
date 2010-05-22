/*
LASS (additive sound synthesis library)
Copyright (C) 2005  Sever Tipei (s-tipei@uiuc.edu)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//----------------------------------------------------------------------------//
//
//	XmlReader.h
//
//----------------------------------------------------------------------------//

#ifndef __XML_READER_H
#define __XML_READER_H
#define XML_BUFFER_SIZE 1024
#define XML_DEBUG
#include <stdio.h>

//----------------------------------------------------------------------------//

/**
*	@file XmlReader.h
	\deprecated
*	Defines the XmlReader object and it's related objects.
**/



class XmlReader
{
	public:

	// Forward decleration
	class xmltagset;

/**
*	tagparam class
*	A linked list of tag parameters
*	I suppose to be more "lasslike" this should probably just
*	be a Collection.
**/
	class tagparam
	{
	friend class XmlReader;
		public:
			/**
			*	Initializes object with set values
			**/
			tagparam(char *n, char *v);
			char *name;
			char *value;
		
			tagparam *next;

			~tagparam();
	};

/**
*	xmltag class
*	This is the object generated by the xml reader to represent
*	tags that are read in from the file.
**/
	class xmltag
	{
	friend class XmlReader;
		public:
			const char *name;
			tagparam *params;
			bool isClosing;
			xmltag();
			~xmltag();

			xmltagset *children;

			/**
			*	Sets the name of the tag
			**/
			void setName(char *in);

			/**
			*	gets the value of a parameter by name
			*	returns NULL if not defined.
			**/
			char* getParamValue(const char *pname);

			/**
			*	checks to see if a certian parameter is
			*	defined in the XML.
			**/
			bool isParamDefined(char *pname);

			/**
			*	first does a find for an immediate child tag
			*	of given name, then searches that tag for a
			*	parameter value.
			*	Repeatable if no other find calls are made 
			*	on children list of this tag in between.
			*/
			char* findChildParamValue(const char *childName,const char *paramName);

		protected:
			tagparam* findParam(const char *pname);

			/**
			*	This frees memeory and clears values inside
			*	the tag object
			**/
			void destroyTag();
		
	};

	class xmltagset
	{
	friend class XmlReader;
		public:
			xmltagset();
			~xmltagset();
			xmltag *tag;
			xmltagset *next;
			void add(xmltag *itag);
			xmltag *find(const char *name);

		protected:
			xmltagset *curSearch;
			const char *searchName;
			xmltag* auxfind(xmltagset *set,const char *name);
	};

	public:
		XmlReader();
		~XmlReader();

		/**
		*	Opens an XML file for reading
		**/
		bool openFile(char *file);

		/**
		*	Closes the XML file
		**/
		bool closeFile();
		
		/**
		*	The guts of the object, this gets the next
		*	tag from the file.
		**/
		bool readTag(xmltag *tag);


		xmltagset* readXMLDocument();
		void readXMLDocument(xmltag*);
		
	protected:
		/**
		*	Called by readTag, this fill acutally get the
		*	tag out of the file
		**/
		bool fillTagBuffer();
	
		/**
		*	Internal function to remove excess whitespace
		**/
		void dewhitespace(char *c);

		FILE *fp;
		char *inputbuffer;
		char *tagbuffer;
		int nibuf;
};

//----------------------------------------------------------------------------//
#endif //__XML_READER_H

