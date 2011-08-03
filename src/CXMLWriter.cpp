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
 
#include "CXMLWriter.h"
#include <wchar.h>
#include "engineString.h"

namespace engine
{
namespace io
{


//! Constructor
CXMLWriter::CXMLWriter(IWriteFile* file)
	: File(file)
{
	if (File)
		File->grab();
}



//! Destructor
CXMLWriter::~CXMLWriter()
{
	if (File)
		File->drop();
}



//! Writes a xml 1.0 header like <?xml version="1.0"?>
void CXMLWriter::writeXMLHeader()
{
	if (!File)
		return;

	u16 h = 0xFEFF;
	File->write(&h, 2);

	wchar_t* p = L"<?xml version=\"1.0\"?>";
	File->write(p, wcslen(p)*2);

	writeLineBreak();
}



//! Writes an xml element with maximal 5 attributes
void CXMLWriter::writeElement(const wchar_t* name, bool empty,
                              const wchar_t* attr1Name, const wchar_t* attr1Value,
                              const wchar_t* attr2Name, const wchar_t* attr2Value,
                              const wchar_t* attr3Name, const wchar_t* attr3Value,
                              const wchar_t* attr4Name, const wchar_t* attr4Value,
                              const wchar_t* attr5Name, const wchar_t* attr5Value)
{
	if (!File || !name)
		return;

	// write name

	File->write(L"<", 2);
	File->write(name, wcslen(name)*2);

	// write attributes

	writeAttribute(attr1Name, attr1Value);
	writeAttribute(attr2Name, attr2Value);
	writeAttribute(attr3Name, attr3Value);
	writeAttribute(attr4Name, attr4Value);
	writeAttribute(attr5Name, attr5Value);

	// write closing tag
	if (empty)
		File->write(L" />", 6);
	else
		File->write(L">", 2);
}


void CXMLWriter::writeAttribute(const wchar_t* name, const wchar_t* value)
{
	if (!name || !value)
		return;

	File->write(L" ", 2);
	File->write(name, wcslen(name)*2);
	File->write(L"=\"", 4);
	File->write(value, wcslen(value)*2); // TODO: replace special characters
	File->write(L"\"", 2);
}


//! Writes a comment into the xml file
void CXMLWriter::writeComment(const wchar_t* comment)
{
	if (!File || !comment)
		return;

	File->write(L"<!--", 8);
	File->write(comment, wcslen(comment)*2); // TODO: replace special characters
	File->write(L"-->", 6);
}


//! Writes the closing tag for an element. Like </foo>
void CXMLWriter::writeClosingTag(const wchar_t* name)
{
	if (!File || !name)
		return;

	File->write(L"</", 4);
	File->write(name, wcslen(name)*2);
	File->write(L">", 2);
}



const CXMLWriter::XMLSpecialCharacters XMLWSChar[] =
{
	{ L'&', L"&amp;" },
	{ L'<', L"&lt;" },
	{ L'>', L"&gt;" },
	{ L'"', L"&quot;" },
	{ L'\0', 0}
};


//! Writes a text into the file. All occurrences of special characters like
//! & (&amp;), < (&lt;), > (&gt;), and " (&quot;) are automaticly replaced.
void CXMLWriter::writeText(const wchar_t* text)
{
	if (!File || !text)
		return;

	core::stringw s;
	const wchar_t* p = text;

	while(*p)
	{
		// check if it is matching
		bool found = false;
		for (s32 i=0; XMLWSChar[i].Character != '\0'; ++i)
			if (*p == XMLWSChar[i].Character)
			{
				s.append(XMLWSChar[i].Symbol);
				found = true;
				break;
			}

		if (!found)
			s.append(*p);
		++p;
	}

	// write new string
	File->write(s.c_str(), s.size()*2);
}


//! Writes a line break
void CXMLWriter::writeLineBreak()
{
	if (!File)
		return;

	File->write("\n", 2);
}


} // end namespace engine
} // end namespace io

