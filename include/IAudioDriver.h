/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __engine_I_AUDIO_DRIVER_H_INCLUDED__
#define __engine_I_AUDIO_DRIVER_H_INCLUDED__

#include "engineTypes.h"
#include "IReadFile.h"
#include "IUnknown.h"
#include "engineString.h"

namespace engine
{
namespace audio
{ 
    enum AUDIO_TYPE { 
      //! Unknow format, media not loaded
      FORMAT_UNKNOWN = -1,

      //! Wave format
      FORMAT_WAV = 0,

      //! MP3 format
      FORMAT_MP3 = 1,
      
      //! AT3 Plus format
      FORMAT_AT3PLUS = 2,
      
      //! Numbers of format
      FORMAT_COUNT = 3
    };

    struct ISoundInfo{

      //! Sound filename
      core::stringc filename;
 
      //! Format of the audio file   
      audio::AUDIO_TYPE format;
  
      //! Bitrate
      s32 bitrate;
       
      //! Sample rate
      s32 samplerate;
 
      //! Duration (in secs)
      s32 duration;

      //! Total frames
      s32 totframes;
 
      //! Is stereo?
      bool stereo;

    } ;
   
    //! Audio sound
    class IAudioSound : public IUnknown
    {
      public:
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
      /** \param vol: a value between 0 and 100 **/
      virtual void setVolume(s32 vol);

      //! Sets the volume of this sound in stereo mode
      /** \param volLeft: volume of the left speaker (value between 0 and 100)
        \param volRight: volume of the right speaker (value between 0 and 100) **/
      virtual void setVolume(s32 volLeft, s32 volRight);
  
      //! Returns true if the position was changed, value is in ms
      /** \param millisec: time in milliseconds **/
      virtual bool setPosition(s32 millisec);

      //! Returns the current position in ms.
      virtual s32 getPosition();
      
      //! Automatically stop the sound after a time
      //! \param time: 0 disable the function, otherwise after time milliseconds the sound will be stopped automatically
      virtual void setAutoStop(int time = 0) = 0;
  
      //! Returns the count of loops
      virtual s32 getLoopCount();

      //! Sets the num of loops
      /** \param loop: -1 = infinite, 0 = 1 = no loop, > 1 loops **/
      virtual void setLoopCount(s32 loop);

      //! Returns the num of times the sound is played
      virtual s32 getPlayCount();
 
      //! Returns the pointer to the sound spec / tags
      virtual const ISoundInfo* getSoundInfo() const ;

      //! Get mins, secs, and ms from millisec
      virtual void getTimeValue(s32 millisec, s32 *hour, s32 *mins, s32 *secs, s32 *ms);

      //! Returns millisec formatted as hh:mm:ss.millisec
      virtual core::stringw getFormatString(s32 millisec);
 
      //! Returns a formatted string of the duration
      virtual core::stringw getStringDuration();

      //! Returns a formatted string of the position
      virtual core::stringw getStringPosition();

      //! Unpacks an MP3 stream to make execution faster
      /** Note that this should be used only for little effects,
        when a MP3 is unpacked you cannot control the position of the
        stream */
      virtual bool unpack();
      
      
    
    };

    //! Interface to the Audio Driver
    /** This interface provides you the functions to load audio files, at the moment 
        you can only load MP3 files; MP3 playing is slow so if you want to use some effects
        in your game use the unpack() function of the sound, it will unpack the mp3 data
        into PCM 16bits, pay attention because this will use a lot of memory, so use it only
        for little effects. */
		class IAudioDriver : public IUnknown
    {
       public:
  
			 //! Add a sound into the audio driver.
       /** Note that when you add a sound it remains cached into the driver. To remove the sound please use the function
        removeSound()
        \param filename: file name of the sound
        \param loopCount: loop count, -1 infinite, 0 no loop
        \param startAndPlay: if true the sound is played after added 
        \return Returns the pointer to the new sound **/
       virtual IAudioSound* addSound(const c8* filename, s32 loopCount = 0, bool startAndPlay = false, bool disableBuffering = false);

			 //! Add a sound into the audio driver.
       /** Note that when you add a sound it remains cached into the driver. To remove the sound please use the function
        removeSound()
        
         If the audio file is not an mp3 it will be automatically dropped after loading. You have to
         grab() the file to avoid this.
        
        \param file: pointer to a read file, you must not drop the file!
        \param loopCount: loop count, -1 infinite, 0 no loop
        \param startAndPlay: if true the sound is played after added
        \return Returns the pointer to the new sound **/
       virtual IAudioSound* addSound(io::IReadFile* file, s32 loopCount = 0, bool startAndPlay = false, bool disableBuffering = false);

			 //! Gets a sound from the cache
       /** \param filename: file name of the sound
           \return Returns the pointer to the found sound, if sound hadn't been found it returns null **/
       virtual IAudioSound* getSound(const c8* filename);

			 //! Gets a sound from the cache
       /** \param filename: file pointer of the sound
           \return Returns the pointer to the found sound, if sound hadn't been found it returns null **/
       virtual IAudioSound* getSound(io::IReadFile* file);

			 //! Remove a sound
       /** \param filename: file name of the sound **/
       virtual void removeSound(const c8* filename);

			 //! Remove a sound
       /** \param filename: file pointer of the sound **/
       virtual void removeSound(io::IReadFile* file);

			 //! Remove a sound
       /** \param snd: the pointer returned with addSound or getSound **/
       virtual void removeSound(IAudioSound *snd);

  };




} // end namespace video
} // end namespace audio


#endif


