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

#include "engineTypes.h"
#include "IAudioDriver.h"
#include "os.h"
#include "engineArray.h"
#include "IFileSystem.h"
#define HAVE_CONFIG
//#define HAVE_ERRNO_H
//#define HAVE_SYS_TYPES_H
//#define HAVE_UNISTD_H
//#define HAVE_FCNTL_H
#include "mad.h"

#define AT3_CURRENT_BUFFER 6
#define AT3_POS_INPUT_BUFFER 7
#define AT3_TEMPORARY_BUFFER 8
#define AT3_INITIAL_BUFFER 59
#define AT3_LENGTH_BUFFER 60

#define AT3_SAMPLES 1024
#define AT3_THREAD_PRIORITY 16 - 2

#define AT3_TYPE_ATRAC3 0x1001


namespace engine
{

namespace audio
{

class CAudioDriver : public IAudioDriver
{
public:
CAudioDriver(io::IFileSystem *fs);        // costruttore
~CAudioDriver();        // distruttore

virtual IAudioSound* addSound(const c8* filename, s32 loopCount = 0, bool startAndPlay = false, bool disableBuffering=false);
virtual IAudioSound* addSound(io::IReadFile* file, s32 loopCount = 0, bool startAndPlay = false, bool disableBuffering=false);
virtual IAudioSound* getSound(const c8* filename);
virtual IAudioSound* getSound(io::IReadFile* file);
virtual void removeSound(const c8* filename);
virtual void removeSound(io::IReadFile* file);
virtual void removeSound(IAudioSound *snd);

core::array<audio::IAudioSound*>* getAudioCache();

private:
core::array<audio::IAudioSound*> audioCache;
io::IFileSystem *FileSystem;


};

class CAudioSound : public IAudioSound
{


public:
CAudioSound(io::IReadFile* file, s32 count = 0, bool start = false, int id = 0, bool db = false);
~CAudioSound();
//! Start playing the sound
virtual void  play();

//! Stop the sound
virtual void  stop();

//! Pause the sound
virtual void  pause();

//! Rewind the sound
virtual void  rewind();

//! Returns true if the sound is playing
virtual bool isPlaying();

//! Sets the volume of this sound
//* \param vol: a value between 0 and 100
virtual void setVolume(s32 vol);

//! Sets the volume of this sound in stereo mode
//* \param volLeft: volume of the left speaker (value between 0 and 100)
//* \param volRight: volume of the right speaker (value between 0 and 100)
virtual void setVolume(s32 volLeft, s32 volRight);

//! Returns true if the position was changed, value is in ms
virtual bool setPosition(s32 millisec);

//! Returns the current position in ms.
virtual s32 getPosition();

//! Automatically stop the sound after a time
//! \param time: 0 disable the function, otherwise after time milliseconds the sound will be stopped automatically
virtual void setAutoStop(int time = 0);

//! Returns the count of loops
virtual s32 getLoopCount();

//! Sets the num of loops (-1 = infinite, 0 = no loop)
virtual void setLoopCount(s32 loop);

//! Returns the num of times the sound is played
virtual s32 getPlayCount();

//! Returns the pointer to the sound spec / tags
virtual const ISoundInfo* getSoundInfo() const;

//! Get mins, secs, and ms from millisec
virtual void getTimeValue(s32 millisec, s32 *hour, s32 *mins, s32 *secs, s32 *ms);

//! Returns millisec formatted as hh:mm:ss.millisec
virtual core::stringw getFormatString(s32 millisec);



//! Returns a formatted string of the duration
virtual core::stringw getStringDuration();

//! Returns a formatted string of the position
virtual core::stringw getStringPosition();



virtual bool unpack();
void releaseChannel();
void stop_in_thread();
bool newEvent;

void readBlock(int seek = -1);

int samplesInOutput;
int samplesOut;
u8* doublebuffer;
struct mad_stream Stream;
struct mad_frame Frame;
struct mad_synth Synth;
mad_timer_t Timer;
signed short OutputBuffer[2048];
int FrameCount;
u8 *buffer;
int filesize;
s32 PlayCount;
int seekstart;
int bitpersample;
int currentseek;
int blocksize;
bool buffering_requested;
s32 loopCount;
bool str_in_use;

unsigned long AT3_Codec_Buffer[65] __attribute__((aligned(64)));
short AT3_Mix_Buffer[AT3_SAMPLES * 2] __attribute__((aligned(64)));
int AT3Start;
int position;
int thid;
int BUFFER_SIZE;
int stoptime;
int startstop;
bool disableBuffering;
private:
s32 eventId;
int bitplay;


bool bPlaying;
int chanId;
s32 volr, voll;
ISoundInfo info;
bool inPause;



io::IReadFile *File;



};


}

};
