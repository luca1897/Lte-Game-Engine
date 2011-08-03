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
 
#include "os.h"
#include "engineString.h"
#include <stdio.h>
#include <stdlib.h>
#include "engineCompileConfig.h"

#define u64 long long
#if defined (_engine_WINDOWS_) || defined (_XBOX)
// ----------------------------------------------------------------
// Windows specific functions
// ----------------------------------------------------------------

#ifdef _engine_WINDOWS_
#include <windows.h>
#endif
#ifdef _XBOX
#include <xtl.h>
#endif


#include <pspdebug.h>
#define printf pspDebugScreenPrintf
namespace engine
{
namespace os
{
//! prints a debuginfo string
void Printer::print(const c8* message)
{
	c8* tmp = new c8[strlen(message) + 2];
	sprintf(tmp, "%s\n", message);
	printf(tmp);
	delete [] tmp;
}

LARGE_INTEGER HighPerformanceFreq;
BOOL HighPerformanceTimerSupport = FALSE;

void Timer::initTimer()
{
	HighPerformanceTimerSupport = QueryPerformanceFrequency(&HighPerformanceFreq);
	initVirtualTimer();
}

u32 Timer::getRealTime()
{
	if (HighPerformanceTimerSupport)
	{
		LARGE_INTEGER nTime;
		QueryPerformanceCounter(&nTime);
		return u32((nTime.QuadPart) * 1000 / HighPerformanceFreq.QuadPart);
	}

	return GetTickCount();
}

} // end namespace os


#else

// ----------------------------------------------------------------
// linux/ansi version
// ----------------------------------------------------------------
extern "C" {
	int sceRtcGetCurrentTick(u64 *tick);
}
#include <stdio.h>
#include <time.h>
#include <sys/time.h>


namespace engine
{
namespace os
{

//! prints a debuginfo string
void Printer::print(const c8* message)
{
	//	printf("%s\n", message);
}

void Timer::initTimer()
{
	initVirtualTimer();
}

u32 Timer::getRealTime()
{

	static timeval tv;
	gettimeofday(&tv, 0);
	return (u32)(tv.tv_sec * 1000) + (tv.tv_usec / 1000);

}

} // end namespace os

#endif // end linux / windows

namespace os
{
// The platform independent implementation of the printer
ILogger* Printer::Logger = 0;

void Printer::log(const c8* message, ELOG_LEVEL ll)
{
	if (Logger)
		Logger->log(message, ll);
}

void Printer::log(const c8* message, const c8* hint, ELOG_LEVEL ll)
{
	if (!Logger)
		return;

	Logger->log(message, hint, ll);
}

void Printer::log(const wchar_t* message, ELOG_LEVEL ll)
{
	if (Logger)
		Logger->log(message, ll);
}


// our Randomizer is not really os specific, so we
// code one for all, which should work on every platform the same,
// which is desireable.

s32 Randomizer::seed = 0x0f0f0f0f;

//! generates a pseudo random number
s32 Randomizer::rand()
{
	const s32 m = 2147483399;               // a non-Mersenne prime
	const s32 a = 40692;                    // another spectral success story
	const s32 q = 52774;                    // m/a
	const s32 r = 3791;                             // m%a; again less than q

	seed = a * (seed%q) - r* (seed/q);
	if (seed<0) seed += m;

	return seed;
}

//! resets the randomizer
void Randomizer::reset()
{
	seed = 0x0f0f0f0f;
}


// ------------------------------------------------------
// virtual timer implementation

f32 Timer::VirtualTimerSpeed = 1.0f;
s32 Timer::VirtualTimerStopCounter = 0;
u32 Timer::LastVirtualTime = 0;
u32 Timer::StartRealTime = 0;
u32 Timer::StaticTime = 0;

//! returns current virtual time
u32 Timer::getTime()
{
	if (isStopped())
		return LastVirtualTime;

	return LastVirtualTime + (u32)((StaticTime - StartRealTime) * VirtualTimerSpeed);
}

//! ticks, advances the virtual timer
void Timer::tick()
{
	StaticTime = getRealTime();
}

//! sets the current virtual time
void Timer::setTime(u32 time)
{
	StaticTime = getRealTime();
	LastVirtualTime = time;
	StartRealTime = StaticTime;
}

//! stops the virtual timer
void Timer::stopTimer()
{
	if (!isStopped())
	{
		// stop the virtual timer
		LastVirtualTime = getTime();
	}

	--VirtualTimerStopCounter;
}

//! starts the virtual timer
void Timer::startTimer()
{
	++VirtualTimerStopCounter;

	if (!isStopped())
	{
		// restart virtual timer
		setTime(LastVirtualTime);
	}
}

//! sets the speed of the virtual timer
void Timer::setSpeed(f32 speed)
{
	setTime(getTime());

	VirtualTimerSpeed = speed;
	if (VirtualTimerSpeed < 0.0f)
		VirtualTimerSpeed = 0.0f;
}

//! gets the speed of the virtual timer
f32 Timer::getSpeed()
{
	return VirtualTimerSpeed;
}

//! returns if the timer currently is stopped
bool Timer::isStopped()
{
	return VirtualTimerStopCounter != 0;
}

void Timer::initVirtualTimer()
{
	StaticTime = getRealTime();
	StartRealTime = StaticTime;
}

} // end namespace os
} // end namespace engine

