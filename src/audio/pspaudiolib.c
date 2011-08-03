/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * pspaudiolib.c - Audio library build on top of sceAudio, but to provide
 *                 multiple thread usage and callbacks.
 *
 * Copyright (c) 2005 Adresd
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2006 SiberianSTAR <haxormail@gmail.com>
 *
 */
#include <stdlib.h>
#include <string.h>
#include <pspthreadman.h>
#include <pspaudio.h>
#include <pspdebug.h>
#define printf pspDebugScreenPrintf

#include "pspaudiolib.h"

static int audio_ready=0;
static short audio_sndbuf[PSP_NUM_AUDIO_CHANNELS][2][PSP_NUM_AUDIO_SAMPLES][2];

//char audio_zerobuf[4096];


int deletingthread;

static psp_audio_channelinfo AudioStatus[PSP_NUM_AUDIO_CHANNELS];
static int ChannelFree[PSP_NUM_AUDIO_CHANNELS];


static volatile int audio_terminate=0;

void pspAudioSetVolume(int channel, int left, int right)
{
  AudioStatus[channel].volumeright = right;
  AudioStatus[channel].volumeleft  = left;
}

void pspAudioChannelThreadCallback(int channel, void *buf, unsigned int reqn)
{
	pspAudioCallback_t callback;
	callback=AudioStatus[channel].callback;
}

int pspAudioChannelSearchFree()
{
	 int i;
	 for ( i = 0; i < PSP_NUM_AUDIO_CHANNELS; i++)
	 {
	   if (ChannelFree[i] == 1)
	   {
	   	ChannelFree[i] = 0;
	   	return i;
	   	break;
	   	
	   } 
	 }
	 return -1;
}

int pspAudioReleaseChannel(int chan)
{
	
	ChannelFree[chan] = 1;
	 return 1;
}

void pspAudioSetChannelCallback(int channel, pspAudioCallback_t callback, void *pdata)
{
	
	
  volatile psp_audio_channelinfo *pci = &AudioStatus[channel];
  if (callback == 0)
  { 
   deletingthread = sceKernelGetThreadId();;
   pci->callback = 0;
   sceKernelSleepThread();
  }
	else {
	pci->callback=callback;
  sceKernelWakeupThread(pci->threadhandle);
  }
}

void pspAudioStopThisThread(int channel)
{
  volatile psp_audio_channelinfo *pci = &AudioStatus[channel];
  pci->callback = 0;
}

int pspAudioOutBlocking(unsigned int channel, unsigned int vol1, unsigned int vol2, void *buf)
{
	if (!audio_ready) return -1;
	if (channel>=PSP_NUM_AUDIO_CHANNELS) return -1;
	if (vol1>PSP_VOLUME_MAX) vol1=PSP_VOLUME_MAX;
	if (vol2>PSP_VOLUME_MAX) vol2=PSP_VOLUME_MAX;
	return sceAudioOutputPannedBlocking(AudioStatus[channel].handle,vol1,vol2,buf);
}

static int AudioChannelThread(int args, void *argp)
{
	volatile int bufidx=0;
	int channel=*(int *)argp;
	
	while (audio_terminate==0) {
		void *bufptr=&audio_sndbuf[channel][bufidx];
		pspAudioCallback_t callback;
		callback=AudioStatus[channel].callback;
		if (callback) {
		  char*c = bufptr;
		  
		  c[0]='s';
		  c[1]='I';
		  c[2]='b';
		  c[3]='E';
		  
			callback(bufptr, PSP_NUM_AUDIO_SAMPLES, AudioStatus[channel].pdata);
			
			
			if (c[0] != 's'+10 ||
				  c[1] != 'I'+10 ||
				  c[2] != 'b'+10 ||
				  c[3] != 'E'+10 )
      pspAudioOutBlocking(channel,AudioStatus[channel].volumeleft,AudioStatus[channel].volumeright,bufptr);
		} else { 
      sceKernelWakeupThread(deletingthread);
      sceKernelSleepThread();
   //   pspAudioOutBlocking(channel,AudioStatus[channel].volumeleft,AudioStatus[channel].volumeright,audio_zerobuf);
      
      
      
    }

		
    bufidx=(bufidx?0:1);
		
	}
	sceKernelExitThread(0);
	return 0;
}



/******************************************************************************/



int pspAudioInit()
{
	int i,ret;
	int failed=0;
	char str[32];

	audio_terminate=0;
	audio_ready=0;


	for (i=0; i<PSP_NUM_AUDIO_CHANNELS; i++) {
    AudioStatus[i].handle = -1;
    AudioStatus[i].threadhandle = -1;
    AudioStatus[i].volumeright = PSP_VOLUME_MAX;
    AudioStatus[i].volumeleft  = PSP_VOLUME_MAX;
    AudioStatus[i].callback = 0;
    AudioStatus[i].pdata = 0;
    ChannelFree[i] = 1;
	}
	for (i=0; i<PSP_NUM_AUDIO_CHANNELS; i++) {
		if ((AudioStatus[i].handle = sceAudioChReserve(-1,PSP_NUM_AUDIO_SAMPLES,0))<0) 
      failed=1;
	}
	if (failed) {
		for (i=0; i<PSP_NUM_AUDIO_CHANNELS; i++) {
			if (AudioStatus[i].handle != -1) 
        sceAudioChRelease(AudioStatus[i].handle);
			AudioStatus[i].handle = -1;
		}
		return -1;
	}
	audio_ready = 1;
	strcpy(str,"audiot0");
	for (i=0; i<PSP_NUM_AUDIO_CHANNELS; i++) {
		str[6]='0'+i;
		AudioStatus[i].threadhandle = sceKernelCreateThread(str,(void*)&AudioChannelThread,0x12,0x10000,0,NULL);
		if (AudioStatus[i].threadhandle < 0) {
			AudioStatus[i].threadhandle = -1;
			failed=1;
			break;
		}

		ret=sceKernelStartThread(AudioStatus[i].threadhandle,sizeof(i),&i);
		if (ret!=0) {
			failed=1;
			break;
		}
	}
	if (failed) {
		audio_terminate=1;
		for (i=0; i<PSP_NUM_AUDIO_CHANNELS; i++) {
			if (AudioStatus[i].threadhandle != -1) {
				//sceKernelWaitThreadEnd(AudioStatus[i].threadhandle,NULL);
				sceKernelDeleteThread(AudioStatus[i].threadhandle);
			}
			AudioStatus[i].threadhandle = -1;
		}
		audio_ready=0;
		return -1;
	}
	return 0;
}


void pspAudioEndPre()
{
	audio_ready=0;
	audio_terminate=1;
}


void pspAudioEnd()
{
	int i;
	audio_ready=0;
	audio_terminate=1;

	for (i=0; i<PSP_NUM_AUDIO_CHANNELS; i++) {
		if (AudioStatus[i].threadhandle != -1) {
			//sceKernelWaitThreadEnd(AudioStatus[i].threadhandle,NULL);
			sceKernelDeleteThread(AudioStatus[i].threadhandle);
		}
		AudioStatus[i].threadhandle = -1;
	}

	for (i=0; i<PSP_NUM_AUDIO_CHANNELS; i++) {
		if (AudioStatus[i].handle != -1) {
			sceAudioChRelease(AudioStatus[i].handle);
			AudioStatus[i].handle = -1;
		}
	}
}


