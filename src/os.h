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
 
#ifndef __engine_OS_H_INCLUDED__
#define __engine_OS_H_INCLUDED__

#include "engineTypes.h"
#include "ILogger.h"

namespace engine
{

namespace os
{
class Printer
{
public:
// prints out a string to the console out stdout or debug log or whatever
static void print(const c8* message);
static void log(const c8* message, ELOG_LEVEL ll = ELL_INFORMATION);
static void log(const c8* message, const c8* hint, ELOG_LEVEL ll = ELL_INFORMATION);
static void log(const wchar_t* message, ELOG_LEVEL ll = ELL_INFORMATION);
static ILogger* Logger;
};


class Randomizer
{
public:

//! resets the randomizer
static void reset();

//! generates a pseudo random number
static s32 rand();

private:

static s32 seed;
};




class Timer
{
public:

//! returns the current time in milliseconds
static u32 getTime();

//! initializes the real timer
static void initTimer();

//! sets the current virtual (game) time
static void setTime(u32 time);

//! stops the virtual (game) timer
static void stopTimer();

//! starts the game timer
static void startTimer();

//! sets the speed of the virtual timer
static void setSpeed(f32 speed);

//! gets the speed of the virtual timer
static f32 getSpeed();

//! returns if the timer currently is stopped
static bool isStopped();

//! makes the virtual timer update the time value based on the real time
static void tick();

//! returns the current real time in milliseconds
static u32 getRealTime();

private:

static void initVirtualTimer();

static f32 VirtualTimerSpeed;
static s32 VirtualTimerStopCounter;
static u32 StartRealTime;
static u32 LastVirtualTime;
static u32 StaticTime;
};

} // end namespace os
} // end namespace engine


#endif

