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
 
#ifndef __C_engine_C_TIMER_H_INCLUDED__
#define __C_engine_C_TIMER_H_INCLUDED__

#include "ITimer.h"
#include "os.h"

namespace engine
{
//! Device independent implementation of the timer
class CTimer : public ITimer
{
public:

CTimer()
{
	os::Timer::initTimer();
}

//! Returns current real time in milliseconds of the system.
/** This value does not start with 0 when the application starts.
   For example in one implementation the value returned could be the
   amount of milliseconds which have elapsed since the system was started. */
virtual u32 getRealTime()
{
	return os::Timer::getRealTime();
}

//! Returns current virtual time in milliseconds.
/** This value starts with 0 and can be manipulated using setTime(), stopTimer(),
   startTimer(), etc. This value depends on the set speed of the timer if the timer
   is stopped, etc. If you need the system time, use getRealTime() */
virtual u32 getTime()
{
	return os::Timer::getTime();
}

//! sets current virtual time
virtual void setTime(u32 time)
{
	os::Timer::setTime(time);
}

//! Stops the game timer.
/** The timer is reference counted, which means everything which calls
   stopTimer() will also have to call startTimer(), otherwise the timer may not start/stop
   corretly again. */
virtual void stop()
{
	os::Timer::stopTimer();
}

//! Starts the game timer.
/** The timer is reference counted, which means everything which calls
   stopTimer() will also have to call startTimer(), otherwise the timer may not start/stop
   corretly again. */
virtual void start()
{
	os::Timer::startTimer();
}

//! Sets the speed of the timer
/** The speed is the factor with which the time is running faster or slower then the
   real system time. */
virtual void setSpeed(f32 speed = 1.0f)
{
	os::Timer::setSpeed(speed);
}

//! Returns current speed of the timer
/** The speed is the factor with which the time is running faster or slower then the
   real system time. */
virtual f32 getSpeed()
{
	return os::Timer::getSpeed();
}

//! Returns if game timer is currently stopped
virtual bool isStopped()
{
	bool ret = os::Timer::isStopped();
	_engine_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return ret;
}

//! Advances the virtual time
/** Makes the virtual timer update the time value based on the real time. This is
   called automaticly when calling engineDevice::run(), but you can call it manually
   if you don't use this method. */
virtual void tick()
{
	os::Timer::tick();
}
};

} // end namespace

#endif
