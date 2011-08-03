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
 
#include "CLogger.h"
#include <stdio.h>

namespace engine
{

CLogger::CLogger(IEventReceiver* r)
	: LogLevel(ELL_INFORMATION), Receiver(r)
{
}

//! Returns the current set log level.
ELOG_LEVEL CLogger::getLogLevel()
{
	return LogLevel;
}

//! Sets a new log level.	virtual void setLogLevel(ELOG_LEVEL ll);
void CLogger::setLogLevel(ELOG_LEVEL ll)
{
	LogLevel = ll;
}

//! Prints out a text into the log
void CLogger::log(const c8* text, ELOG_LEVEL ll)
{
	if (ll < LogLevel)
		return;

	if (Receiver)
	{
		SEvent event;
		event.EventType = EET_LOG_TEXT_EVENT;
		event.LogEvent.Text = text;
		event.LogEvent.Level = ll;
		if (Receiver->OnEvent(event))
			return;
	}

	os::Printer::print(text);
}


//! Prints out a text into the log
void CLogger::log(const c8* text, const c8* hint, ELOG_LEVEL ll)
{
	if (ll < LogLevel)
		return;

	core::stringc s = text;
	s += ": ";
	s += hint;
	log (s.c_str(), ll);
}

//! Prints out a text into the log
void CLogger::log(const wchar_t* text, ELOG_LEVEL ll)
{
	if (ll < LogLevel)
		return;

	core::stringc s = text;
	log(s.c_str(), ll);
}


//! Prints out a text into the log
void CLogger::log(const wchar_t* text, const wchar_t* hint, ELOG_LEVEL ll)
{
	if (ll < LogLevel)
		return;

	core::stringc s1 = text;
	core::stringc s2 = hint;
	log(s1.c_str(), s2.c_str(), ll);
}

//! Sets a new event receiver
void CLogger::setReceiver(IEventReceiver* r)
{
	Receiver = r;
}


} // end namespace engine

