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
 
#ifndef __C_LOGGER_H_INCLUDED__
#define __C_LOGGER_H_INCLUDED__

#include "ILogger.h"
#include "os.h"
#include "engineString.h"
#include "IEventReceiver.h"

namespace engine
{

//! Interface for logging messages, warnings and errors
class CLogger : public ILogger
{
public:

CLogger(IEventReceiver* r);

//! Returns the current set log level.
virtual ELOG_LEVEL getLogLevel();

//! Sets a new log level.	virtual void setLogLevel(ELOG_LEVEL ll);
virtual void setLogLevel(ELOG_LEVEL ll);

//! Prints out a text into the log
virtual void log(const c8* text, ELOG_LEVEL ll=ELL_INFORMATION);

//! Prints out a text into the log
virtual void log(const wchar_t* text, ELOG_LEVEL ll=ELL_INFORMATION);

//! Prints out a text into the log
virtual void log(const c8* text, const c8* hint, ELOG_LEVEL ll=ELL_INFORMATION);

//! Prints out a text into the log
virtual void log(const wchar_t* text, const wchar_t* hint, ELOG_LEVEL ll=ELL_INFORMATION);

//! Sets a new event receiver
void setReceiver(IEventReceiver* r);

private:

ELOG_LEVEL LogLevel;
IEventReceiver* Receiver;
};

} // end namespace

#endif

