/*
 * LTE Game Engine
 * Copyright (C) 2006-2008 SiberianSTAR <haxormail@gmail.com>
 * http://www.ltestudios.com
 *  
 * The LTE Game Engine is based on Irrlicht 1.0
 * Irrlicht Engine is Copyright (C) 2002-2006 Nikolaus Gebhardt
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */
 
#ifndef __C_XML_WRITER_H_INCLUDED__
#define __C_XML_WRITER_H_INCLUDED__

#include "IXMLWriter.h"
#include "IWriteFile.h"

namespace engine
{
namespace io
{

//! Interface providing methods for making it easier to write XML files.
class CXMLWriter : public IXMLWriter
{
public:

//! Constructor
CXMLWriter(IWriteFile* file);

//! Destructor
virtual ~CXMLWriter();

//! Writes a xml 1.0 header like <?xml version="1.0"?>
virtual void writeXMLHeader();

//! Writes an xml element with maximal 5 attributes
virtual void writeElement(const wchar_t* name, bool empty=false,
                          const wchar_t* attr1Name = 0, const wchar_t* attr1Value = 0,
                          const wchar_t* attr2Name = 0, const wchar_t* attr2Value = 0,
                          const wchar_t* attr3Name = 0, const wchar_t* attr3Value = 0,
                          const wchar_t* attr4Name = 0, const wchar_t* attr4Value = 0,
                          const wchar_t* attr5Name = 0, const wchar_t* attr5Value = 0);

//! Writes a comment into the xml file
virtual void writeComment(const wchar_t* comment);

//! Writes the closing tag for an element. Like </foo>
virtual void writeClosingTag(const wchar_t* name);

//! Writes a text into the file. All occurrences of special characters like
//! & (&amp;), < (&lt;), > (&gt;), and " (&quot;) are automaticly replaced.
virtual void writeText(const wchar_t* text);

//! Writes a line break
virtual void writeLineBreak();

struct XMLSpecialCharacters
{
	wchar_t Character;
	wchar_t* Symbol;
};

private:

void writeAttribute(const wchar_t* att, const wchar_t* name);

IWriteFile* File;
};

} // end namespace engine
} // end namespace io

#endif

