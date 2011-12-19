/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __engine_XML_H_INCLUDED__
#define __engine_XML_H_INCLUDED__

#include <stdio.h>

namespace engine
{
namespace io
{
	//! Enumeration of all supported source text file formats 
	enum ETEXT_FORMAT
	{
		//! ASCII, file without byte order mark, or not a text file
		ETF_ASCII,

		//! UTF-8 format
		ETF_UTF8,

		//! UTF-16 format, big endian
		ETF_UTF16_BE,

		//! UTF-16 format, little endian
		ETF_UTF16_LE,

		//! UTF-32 format, big endian
		ETF_UTF32_BE,

		//! UTF-32 format, little endian
		ETF_UTF32_LE,
	};


	//! Enumeration for all xml nodes which are parsed by engineXMLReader
	enum EXML_NODE
	{
		//! No xml node. This is usually the node if you did not read anything yet.
		EXN_NONE,

		//! A xml element, like <foo>
		EXN_ELEMENT,

		//! End of an xml element, like </foo>
		EXN_ELEMENT_END,

		//! Text within a xml element: <foo> this is the text. </foo>
		EXN_TEXT,

		//! An xml comment like &lt;!-- I am a comment --&gt; or a DTD definition.
		EXN_COMMENT,

		//! An xml cdata section like &lt;![CDATA[ this is some CDATA ]]&gt;
		EXN_CDATA,

		//! Unknown element.
		EXN_UNKNOWN
	};

	//! Callback class for file read abstraction. 
	/** With this, it is possible to make the xml parser read in other things 
	than just files. The engine engine is using this for example to 
	read xml from compressed .zip files. To make the parser read in 
	any other data, derive a class from this interface, implement the 
	two methods to read your data and give a pointer to an instance of
	your implementation when calling createengineXMLReader(), 
	createengineXMLReaderUTF16() or createengineXMLReaderUTF32() */
	class IFileReadCallBack
	{
	public:

		//! virtual destructor
		virtual ~IFileReadCallBack() {};

		//! Reads an amount of bytes from the file.
		/** \param buffer: Pointer to buffer where to read bytes will be written to.
		\param sizeToRead: Amount of bytes to read from the file.
		\return Returns how much bytes were read. */
		virtual int read(void* buffer, int sizeToRead) = 0;

		//! Returns size of file in bytes
		virtual int getSize() = 0;
	};

	//! Empty class to be used as parent class for engineXMLReader.
	/** If you need another class as base class for the xml reader, you can do this by creating
	the reader using for example new CXMLReaderImpl<char, YourBaseClass>(yourcallback);
	The engine Engine for example needs IUnknown as base class for every object to
	let it automaticly reference countend, hence it replaces IXMLBase with IUnknown.
	See engineXML.cpp on how this can be done in detail. */
	class IXMLBase
	{
	};	

	//! Interface providing easy read access to a XML file.
	/** You can create an instance of this reader using one of the factory functions
	createengineXMLReader(), createengineXMLReaderUTF16() and createengineXMLReaderUTF32().
	If using the parser from the engine Engine, please use IFileSystem::createXMLReader() 
	instead.
	For a detailed intro how to use the parser, see \ref enginexmlexample and \ref features.

	The typical usage of this parser looks like this:
	\code
	#include <engineXML.h>
	using namespace engine; // engineXML is located in the namespace engine::io
	using namespace io;

	void main()
	{
		// create the reader using one of the factory functions
		engineXMLReader* xml = createengineXMLReader("config.xml");

		if (xml == 0)
			return; // file could not be opened

		// parse the file until end reached
		while(xml->read())
		{
			// based on xml->getNodeType(), do something.
		}

		// delete the xml parser after usage
		delete xml;
	}
	\endcode
	See \ref enginexmlexample for a more detailed example.
	*/
	template<class char_type, class super_class>
	class IengineXMLReader : public super_class
	{
	public:

		//! Destructor
		virtual ~IengineXMLReader() {};

		//! Reads forward to the next xml node. 
		/** \return Returns false, if there was no further node.  */
		virtual bool read() = 0;

		//! Returns the type of the current XML node.
		virtual EXML_NODE getNodeType() const = 0;

        //! Returns attribute count of the current XML node. 
		/** This is usually
		non null if the current node is EXN_ELEMENT, and the element has attributes.
		\return Returns amount of attributes of this xml node. */
		virtual int getAttributeCount() const = 0;

		//! Returns name of an attribute. 
		/** \param idx: Zero based index, should be something between 0 and getAttributeCount()-1.
		\return Name of the attribute, 0 if an attribute with this index does not exist. */
		virtual const char_type* getAttributeName(int idx) const = 0;

		//! Returns the value of an attribute. 
		/** \param idx: Zero based index, should be something between 0 and getAttributeCount()-1.
		\return Value of the attribute, 0 if an attribute with this index does not exist. */
		virtual const char_type* getAttributeValue(int idx) const = 0;

		//! Returns the value of an attribute. 
		/** \param name: Name of the attribute.
		\return Value of the attribute, 0 if an attribute with this name does not exist. */
		virtual const char_type* getAttributeValue(const char_type* name) const = 0;

		//! Returns the value of an attribute in a safe way.
		/** Like getAttributeValue(), but does not 
		return 0 if the attribute does not exist. An empty string ("") is returned then.
		\param name: Name of the attribute.
		\return Value of the attribute, and "" if an attribute with this name does not exist */
		virtual const char_type* getAttributeValueSafe(const char_type* name) const = 0;

		//! Returns the value of an attribute as integer. 
		/** \param name Name of the attribute.
		\return Value of the attribute as integer, and 0 if an attribute with this name does not exist or
		the value could not be interpreted as integer. */
		virtual int getAttributeValueAsInt(const char_type* name) const = 0;

		//! Returns the value of an attribute as integer. 
		/** \param idx: Zero based index, should be something between 0 and getAttributeCount()-1.
		\return Value of the attribute as integer, and 0 if an attribute with this index does not exist or
		the value could not be interpreted as integer. */
		virtual int getAttributeValueAsInt(int idx) const = 0;

		//! Returns the value of an attribute as float. 
		/** \param name: Name of the attribute.
		\return Value of the attribute as float, and 0 if an attribute with this name does not exist or
		the value could not be interpreted as float. */
		virtual float getAttributeValueAsFloat(const char_type* name) const = 0;

		//! Returns the value of an attribute as float. 
		/** \param idx: Zero based index, should be something between 0 and getAttributeCount()-1.
		\return Value of the attribute as float, and 0 if an attribute with this index does not exist or
		the value could not be interpreted as float. */
		virtual float getAttributeValueAsFloat(int idx) const = 0;

		//! Returns the name of the current node. 
		/** Only non null, if the node type is EXN_ELEMENT.
		\return Name of the current node or 0 if the node has no name. */
		virtual const char_type* getNodeName() const = 0;

		//! Returns data of the current node. 
		/** Only non null if the node has some
		data and it is of type EXN_TEXT or EXN_UNKNOWN. */
		virtual const char_type* getNodeData() const = 0;

		//! Returns if an element is an empty element, like <foo />
		virtual bool isEmptyElement() const = 0;

		//! Returns format of the source xml file. 
		/** It is not necessary to use
		this method because the parser will convert the input file format
		to the format wanted by the user when creating the parser. This
		method is useful to get/display additional informations. */
		virtual ETEXT_FORMAT getSourceFormat() const = 0;

		//! Returns format of the strings returned by the parser. 
		/** This will be UTF8 for example when you created a parser with
		engineXMLReaderUTF8() and UTF32 when it has been created using 
		engineXMLReaderUTF32. It should not be necessary to call this
		method and only exists for informational purposes. */
		virtual ETEXT_FORMAT getParserFormat() const = 0;
	};


	//! defines the utf-16 type.
	/** Not using wchar_t for this because 
	wchar_t has 16 bit on windows and 32 bit on other operating systems. */
	typedef unsigned short char16;

	//! defines the utf-32 type. 
	/** Not using wchar_t for this because 
	wchar_t has 16 bit on windows and 32 bit on other operating systems. */
	typedef unsigned long char32;

	//! A UTF-8 or ASCII character xml parser.
	/** This means that all character data will be returned in 8 bit ASCII or UTF-8 by this parser. 
	The file to read can be in any format, it will be converted to UTF-8 if it is not
	in this format.
	Create an instance of this with createengineXMLReader(); 
	See IengineXMLReader for description on how to use it. */
	typedef IengineXMLReader<char, IXMLBase> engineXMLReader;

	//! A UTF-16 xml parser. 
	/** This means that all character data will be returned in UTF-16 by this parser. 
	The file to read can be in any format, it will be converted to UTF-16 if it is not
	in this format.
	Create an instance of this with createengineXMLReaderUTF16(); 
	See IengineXMLReader for description on how to use it.  */
	typedef IengineXMLReader<char16, IXMLBase> engineXMLReaderUTF16;

	//! A UTF-32 xml parser. 
	/** This means that all character data will be returned in UTF-32 by this parser. 
	The file to read can be in any format, it will be converted to UTF-32 if it is not
	in this format.
	Create an instance of this with createengineXMLReaderUTF32(); 
	See IengineXMLReader for description on how to use it. */
	typedef IengineXMLReader<char32, IXMLBase> engineXMLReaderUTF32;


	//! Creates an instance of an UFT-8 or ASCII character xml parser.
	/** This means that all character data will be returned in 8 bit ASCII or UTF-8. 
	The file to read can be in any format, it will be converted to UTF-8 if it is not in this format.
	If you are using the engine Engine, it is better not to use this function but
	IFileSystem::createXMLReaderUTF8() instead.
	\param filename: Name of file to be opened.
	\return Returns a pointer to the created xml parser. This pointer should be 
	deleted using 'delete' after no longer needed. Returns 0 if an error occured
	and the file could not be opened. */
	engineXMLReader* createengineXMLReader(const char* filename);

	//! Creates an instance of an UFT-8 or ASCII character xml parser.
	/** This means that all character data will be returned in 8 bit ASCII or UTF-8. The file to read can 
	be in any format, it will be converted to UTF-8 if it is not in this format.
	If you are using the engine Engine, it is better not to use this function but
	IFileSystem::createXMLReaderUTF8() instead.
	\param file: Pointer to opened file, must have been opened in binary mode, e.g.
	using fopen("foo.bar", "wb"); The file will not be closed after it has been read.
	\return Returns a pointer to the created xml parser. This pointer should be 
	deleted using 'delete' after no longer needed. Returns 0 if an error occured
	and the file could not be opened. */
	engineXMLReader* createengineXMLReader(FILE* file);

	//! Creates an instance of an UFT-8 or ASCII character xml parser. 
	/** This means that all character data will be returned in 8 bit ASCII or UTF-8. The file to read can 
	 be in any format, it will be converted to UTF-8 if it is not in this format.
	 If you are using the engine Engine, it is better not to use this function but
	 IFileSystem::createXMLReaderUTF8() instead.
	 \param callback: Callback for file read abstraction. Implement your own
	 callback to make the xml parser read in other things than just files. See
	 IFileReadCallBack for more information about this.
	 \return Returns a pointer to the created xml parser. This pointer should be 
	 deleted using 'delete' after no longer needed. Returns 0 if an error occured
	 and the file could not be opened. */
	engineXMLReader* createengineXMLReader(IFileReadCallBack* callback);

	//! Creates an instance of an UFT-16 xml parser. 
	/** This means that
	all character data will be returned in UTF-16. The file to read can 
	be in any format, it will be converted to UTF-16 if it is not in this format.
	If you are using the engine Engine, it is better not to use this function but
	IFileSystem::createXMLReader() instead.
	\param filename: Name of file to be opened.
	\return Returns a pointer to the created xml parser. This pointer should be 
	deleted using 'delete' after no longer needed. Returns 0 if an error occured
	and the file could not be opened. */
	engineXMLReaderUTF16* createengineXMLReaderUTF16(const char* filename);

	//! Creates an instance of an UFT-16 xml parser. 
	/** This means that all character data will be returned in UTF-16. The file to read can 
	be in any format, it will be converted to UTF-16 if it is not in this format.
	If you are using the engine Engine, it is better not to use this function but
	IFileSystem::createXMLReader() instead.
	\param file: Pointer to opened file, must have been opened in binary mode, e.g.
	using fopen("foo.bar", "wb"); The file will not be closed after it has been read.
	\return Returns a pointer to the created xml parser. This pointer should be 
	deleted using 'delete' after no longer needed. Returns 0 if an error occured
	and the file could not be opened. */
	engineXMLReaderUTF16* createengineXMLReaderUTF16(FILE* file);

	//! Creates an instance of an UFT-16 xml parser. 
	/** This means that all character data will be returned in UTF-16. The file to read can 
	be in any format, it will be converted to UTF-16 if it is not in this format.
	If you are using the engine Engine, it is better not to use this function but
	IFileSystem::createXMLReader() instead.
	\param callback: Callback for file read abstraction. Implement your own
	callback to make the xml parser read in other things than just files. See
	IFileReadCallBack for more information about this.
	\return Returns a pointer to the created xml parser. This pointer should be 
	deleted using 'delete' after no longer needed. Returns 0 if an error occured
	and the file could not be opened. */
	engineXMLReaderUTF16* createengineXMLReaderUTF16(IFileReadCallBack* callback);


	//! Creates an instance of an UFT-32 xml parser. 
	/** This means that all character data will be returned in UTF-32. The file to read can 
	be in any format, it will be converted to UTF-32 if it is not in this format.
	If you are using the engine Engine, it is better not to use this function but
	IFileSystem::createXMLReader() instead.
	\param filename: Name of file to be opened.
	\return Returns a pointer to the created xml parser. This pointer should be 
	deleted using 'delete' after no longer needed. Returns 0 if an error occured
	and the file could not be opened. */
	engineXMLReaderUTF32* createengineXMLReaderUTF32(const char* filename);

	//! Creates an instance of an UFT-32 xml parser. 
	/** This means that all character data will be returned in UTF-32. The file to read can 
	be in any format, it will be converted to UTF-32 if it is not in this format.
	if you are using the engine Engine, it is better not to use this function but
	IFileSystem::createXMLReader() instead.
	\param file: Pointer to opened file, must have been opened in binary mode, e.g.
	using fopen("foo.bar", "wb"); The file will not be closed after it has been read.
	\return Returns a pointer to the created xml parser. This pointer should be 
	deleted using 'delete' after no longer needed. Returns 0 if an error occured
	and the file could not be opened. */
	engineXMLReaderUTF32* createengineXMLReaderUTF32(FILE* file);

	//! Creates an instance of an UFT-32 xml parser. 
	/** This means that
	all character data will be returned in UTF-32. The file to read can 
	be in any format, it will be converted to UTF-32 if it is not in this format.
	If you are using the engine Engine, it is better not to use this function but
	IFileSystem::createXMLReader() instead.
	\param callback: Callback for file read abstraction. Implement your own
	callback to make the xml parser read in other things than just files. See
	IFileReadCallBack for more information about this.
	\return Returns a pointer to the created xml parser. This pointer should be 
	deleted using 'delete' after no longer needed. Returns 0 if an error occured
	and the file could not be opened. */
	engineXMLReaderUTF32* createengineXMLReaderUTF32(IFileReadCallBack* callback);
	

	/*! \file enginexml.h
    \brief Header file of the engineXML, the engine XML parser.
    
    This file includes everything needed for using engineXML, 
    the XML parser of the engine Engine. To use engineXML,
	you only need to include this file in your project:

	\code
	#include <engineXML.h>
	\endcode

	It is also common to use the two namespaces in which engineXML is included, 
	directly after #including engineXML.h:

	\code
	#include <engineXML.h>
	using namespace engine;
	using namespace io;
	\endcode
	*/

} // end namespace io
} // end namespace engine

#endif // __engine_XML_H_INCLUDED__


