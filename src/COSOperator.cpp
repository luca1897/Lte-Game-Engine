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
 
#include "COSOperator.h"
#include "engineCompileConfig.h"

#ifdef _engine_WINDOWS_
#include <windows.h>
#else
#include <string.h>
#endif

namespace engine
{


// constructor
COSOperator::COSOperator(const c8* osVersion)
{
	OperationSystem = osVersion;
}


//! destructor
COSOperator::~COSOperator()
{
}


//! returns the current operation system version as string.
const wchar_t* COSOperator::getOperationSystemVersion()
{
	return OperationSystem.c_str();
}


//! copies text to the clipboard
void COSOperator::copyToClipboard(const c8* text)
{
	if (strlen(text)==0)
		return;

// Windows version
#ifdef _engine_WINDOWS_
	if (!OpenClipboard(0) || text == 0)
		return;

	EmptyClipboard();

	HGLOBAL clipbuffer;
	char * buffer;

	clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(text)+1);
	buffer = (char*)GlobalLock(clipbuffer);

	strcpy(buffer, text);

	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();
#endif
}


//! gets text from the clipboard
//! \return Returns 0 if no string is in there.
c8* COSOperator::getTextFromClipboard()
{
#ifdef _engine_WINDOWS_
	if (!OpenClipboard(NULL))
		return 0;

	char * buffer = 0;

	HANDLE hData = GetClipboardData( CF_TEXT );
	buffer = (char*)GlobalLock( hData );
	GlobalUnlock( hData );
	CloseClipboard();
	return buffer;
#else
	return 0;
#endif
}



} // end namespace

