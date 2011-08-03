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

#include "CAudioDriver.h"

#include <psputility_avmodules.h>
#include <pspaudio.h>
#include <pspaudiolib.h>
#include <string.h>
#include <limits.h>
#include <pspdebug.h>
#include <pspthreadman.h>
#include <pspkernel.h>
#include <mad.h>
#include <psputility_netmodules.h>
#include <psputility_netparam.h>
#include <pspwlan.h>
#include <pspnet.h>
#include <pspnet_apctl.h>
//#include <pspnet_inet.h>
#include <malloc.h>
#include <pspsdk.h>
#define printf pspDebugScreenPrintf

extern "C" {
	signed short MadFixedToSshort(mad_fixed_t Fixed);
	void pspAudioStopThisThread(int channel);
	//int pspSdkInstallNoDeviceCheckPatch();
	int sceAudiocodecCheckNeedMem(void *Buffer, int Type);
	int sceAudiocodecInit(void *Buffer, int Type);
	int sceAudiocodecDecode(void *Buffer, int Type);
	int sceAudiocodecGetEDRAM(void *Buffer, int Type);
	int sceAudiocodecReleaseEDRAM(void *Buffer);
	int pspAudioReleaseChannel(int chan);
	int pspAudioChannelSearchFree();

}

extern bool __inet_sdk_modules_loaded;


PSP_MODULE_INFO("LTE_ENGINE", 0, 1, 2);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);
//PSP_HEAP_SIZE_MAX();
PSP_HEAP_SIZE_KB(48000);

extern int engineMain(unsigned int argc, void *argv);

int main()
{
#if 0
	int i;

	pspKernelSetKernelPC();
	pspSdkInstallNoDeviceCheckPatch();

	__inet_sdk_modules_loaded = true;


	if (pspSdkLoadInetModules() < 0)
	{
		__inet_sdk_modules_loaded = false;
	}

	int thid =
	        sceKernelCreateThread("main_function", engineMain,  0x18, 0x10000, PSP_THREAD_ATTR_USER, NULL);

	if (thid >= 0)
	{

		sceKernelStartThread(thid, sizeof(i), &i);
		sceKernelSleepThreadCB();
	}
#else
	int err;

	err = sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
	if (!err)
		err = sceUtilityLoadNetModule(PSP_NET_MODULE_INET);
	__inet_sdk_modules_loaded = err ? false : true;

	engineMain(0, NULL);
#endif

	return 0; // suppress warning
}

namespace engine
{

namespace audio {


int Load_AudioCodec(void) {
#if 0
	SceUID PRxId;
	SceKernelLMOption Options;
	SceUID PrxHandle;
	int Start_Result;

	int ret = 0;

	PrxHandle = sceIoOpen("flash0:/kd/audiocodec.prx", PSP_O_RDONLY, 0);
	if(PrxHandle >= 0) {
		memset(&Options, 0, sizeof(SceKernelLMOption));
		Options.size = sizeof(SceKernelLMOption);
		Options.mpidtext = 1;
		Options.mpiddata = 1;
		Options.access = 1;
		PRxId = sceKernelLoadModuleByID(PrxHandle, 0, &Options);
		if(PRxId >= 0) {
			ret = 1;
			sceKernelStartModule(PRxId, 0, NULL, &Start_Result, 0);
		}
		sceIoClose(PrxHandle);
	}
	return ret;
#else
	sceUtilityLoadAvModule(PSP_AV_MODULE_AVCODEC);
	return 1;
#endif
}

CAudioDriver::CAudioDriver(io::IFileSystem *fs) :
	FileSystem(fs) {
	// init  psp audio
	pspAudioInit();
	Load_AudioCodec();
	audioCache.reallocate(  10 );
	audioCache.set_used(0);

}

IAudioSound* CAudioDriver::addSound(const c8* filename, s32 loopCount, bool startAndPlay, bool disableBuffering)
{
	io::IReadFile *file = FileSystem->createAndOpenFile(filename);

	if (!file)
		return 0;

	CAudioSound *audio = new CAudioSound(file, loopCount, startAndPlay, 0, disableBuffering);
	audioCache.push_back(audio);

	return audio;
}
IAudioSound* CAudioDriver::addSound(io::IReadFile* file, s32 loopCount, bool startAndPlay, bool disableBuffering)
{

	if (!file)
		return 0;

	CAudioSound *audio = new CAudioSound(file, loopCount, startAndPlay, 0, disableBuffering);

	audioCache.push_back(audio);

	return audio;
}
IAudioSound* CAudioDriver::getSound(const c8* filename)
{
	IAudioSound **ptr = audioCache.pointer();
	core::stringc c = filename;

	for (int i  = 0;  i < audioCache.size(); i++)
		if (ptr[i]->getSoundInfo()->filename == c)
			return ptr[i];

	return addSound(filename, 0, false);
}
IAudioSound* CAudioDriver::getSound(io::IReadFile* file)
{
	return getSound(file->getFileName());
}
void CAudioDriver::removeSound(io::IReadFile *file)
{
	removeSound(file->getFileName());
}

void CAudioDriver::removeSound(const c8* filename){

	IAudioSound *t = 0;

	IAudioSound **ptr = audioCache.pointer();
	core::stringc c = filename;

	for (int i  = 0;  i < audioCache.size(); i++)
		if (ptr[i]->getSoundInfo()->filename == c)
		{ t =  ptr[i]; break; }

	if (t)
		removeSound(t);


}
void CAudioDriver::removeSound(IAudioSound *snd)
{

	IAudioSound **ptr = audioCache.pointer();
	for (int i = 0; i < audioCache.size(); i++)
		if (ptr[i] == snd)
		{

			audioCache.erase(i);


			snd->drop();

		}
}

core::array<audio::IAudioSound*>* CAudioDriver::getAudioCache() {
	return &audioCache;
}

CAudioDriver::~CAudioDriver() {

	IAudioSound **ptr = audioCache.pointer();
	for (int i = 0; i < audioCache.size(); i++)
		delete ptr[i];

	pspAudioEnd();
}
CAudioSound *__psp_audio_reference[8];

static int __mp3_buffer_thread(SceSize args, void *argp)
{


	int channel = *(int *)argp;
	CAudioSound *audio = __psp_audio_reference[channel];

	int curseek = audio->currentseek + (audio->Stream.this_frame - audio->Stream.buffer) + audio->Stream.skiplen;


	const unsigned char  *curframe = audio->Stream.this_frame + audio->Stream.skiplen;

	audio->readBlock(curseek);


	int v = audio->Stream.this_frame - curframe;
	mad_stream_buffer(&audio->Stream, audio->buffer,  audio->getSoundInfo()->totframes * audio->blocksize);
	audio->Stream.skiplen = v;
	audio->currentseek = curseek;


	audio->buffering_requested = false;


	sceKernelExitThread(0);
	return 0;

}

void __audioCallback(int channel, void *buff, unsigned int reqn)
{

	CAudioSound *audio = __psp_audio_reference[channel];
	short *buffer = (short*)buff;

	unsigned int samplesOut = 0;
	int cnt = 0;

	if (audio->stoptime != 0) {

		if (os::Timer::getRealTime() - audio->startstop > audio->stoptime) {

			char *pctr = (char*)buff;
			pctr[0]+=10;
			pctr[1]+=10;
			pctr[2]+=10;
			pctr[3]+=10;

			audio->stop_in_thread();
			return;
		}

	}

	if (audio->getSoundInfo()->format == FORMAT_AT3PLUS)
	{
		sceKernelDcacheInvalidateRange(buff, 4096);
		audio->AT3_Codec_Buffer[AT3_TEMPORARY_BUFFER] = (int)buff;


		sceAudiocodecDecode(audio->AT3_Codec_Buffer, AT3_TYPE_ATRAC3);

		audio->AT3_Codec_Buffer[AT3_CURRENT_BUFFER] += audio->AT3_Codec_Buffer[AT3_POS_INPUT_BUFFER];
		if(audio->AT3_Codec_Buffer[AT3_CURRENT_BUFFER] >= ((audio->AT3_Codec_Buffer[AT3_LENGTH_BUFFER] - audio->AT3Start) + audio->AT3_Codec_Buffer[AT3_INITIAL_BUFFER]))
		{
			// l'audio è finito
			if (audio->loopCount == 0 || audio->loopCount == audio->PlayCount) {
				audio->stop_in_thread();
				return;
			} else {
				if (audio->loopCount == -1) { audio->rewind(); return; }
				else {
					audio->rewind();
					audio->PlayCount++;
					return;
				}
			}

		}

	}

	if (audio->getSoundInfo()->format == FORMAT_WAV)
	{

		for (int x = 0; x < reqn; x++)
		{
			if (audio->currentseek >= audio->filesize)
			{   audio->stop_in_thread(); return; }

			u8 *ptr = &audio->buffer[audio->currentseek];
			short c = *(short*)ptr;
			audio->currentseek+=2;
			buffer[x*2] = c;

			if (audio->getSoundInfo()->stereo == true)
			{
				ptr = &audio->buffer[audio->currentseek];
				c = *(short*)ptr;
				audio->currentseek+=2;
			}

			buffer[x*2+1] = c;
		}
		return;

	}

	// è un mp3! yo!
	if (audio->getSoundInfo()->format == FORMAT_MP3)
	{

		/***************************/
		if (audio->samplesInOutput > 0)
		{
			if (audio->samplesInOutput > reqn)
			{
				memcpy( (char*)buffer, (char*)audio->OutputBuffer, reqn * 4);
				samplesOut = reqn;
				audio->samplesInOutput -= reqn;

			}
			else
			{
				memcpy( (char*)buffer, (char*)audio->OutputBuffer, audio->samplesInOutput * 4);
				samplesOut = audio->samplesInOutput;
				audio->samplesInOutput = 0;

			}
		}

		/***************************/
		while (samplesOut < reqn)
		{


			if (mad_frame_decode(&audio->Frame, &audio->Stream))
			{
				if (!MAD_RECOVERABLE(audio->Stream.error))
				{

					// l'audio è finito
					if (audio->loopCount == 0 || audio->loopCount == audio->PlayCount) {
						audio->stop_in_thread();
						return;
					} else {
						if (audio->loopCount == -1) { audio->rewind(); return; }
						else {
							audio->rewind();
							audio->PlayCount++;
							return;
						}
					}
				}
				else {
					int retry = 0;
					while ( mad_frame_decode(&audio->Frame, &audio->Stream) && retry < 5) retry++;
				}
			}


			if (audio->disableBuffering == false){

				if ((audio->currentseek+audio->BUFFER_SIZE < audio->filesize) && (audio->Stream.next_frame - audio->Stream.buffer + audio->blocksize > audio->BUFFER_SIZE/2 && audio->buffering_requested == false)) {
					// merda devo fare un po' di buffering
					//printf("started buffering at position : %d %d\n", audio->filesize, audio->currentseek + audio->Stream.this_frame - audio->Stream.buffer);
					audio->buffering_requested = true;
					memcpy(audio->doublebuffer, audio->buffer, audio->BUFFER_SIZE);

					int skiplen = audio->Stream.this_frame - audio->Stream.buffer;

					mad_stream_buffer(&audio->Stream, audio->doublebuffer, audio->getSoundInfo()->totframes * audio->blocksize);
					mad_stream_skip(&audio->Stream,(skiplen) + audio->blocksize);

					sceKernelStartThread(audio->thid,sizeof(channel),&channel);

				}

			}


			audio->position = (audio->Stream.skiplen + (audio->Stream.this_frame - audio->Stream.buffer) + audio->currentseek) / audio->blocksize;
			audio->position = (int)(((f32)audio->position / (f32)audio->getSoundInfo()->totframes) * (f32)audio->getSoundInfo()->duration);


			audio->FrameCount++;
			mad_timer_add(&audio->Timer, audio->Frame.header.duration);

			mad_synth_frame(&audio->Synth, &audio->Frame);


			if (audio->position >= audio->getSoundInfo()->duration)
			{
				audio->position = audio->getSoundInfo()->duration;
				// l'audio è finito
				if (audio->loopCount == 0 || audio->loopCount == audio->PlayCount) {
					audio->stop_in_thread();
					return;
				} else {
					if (audio->loopCount == -1) { audio->rewind(); return; }
					else {
						audio->rewind();
						audio->PlayCount++;
						return;
					}
				}
			}


			for (int i = 0; i < audio->Synth.pcm.length; i++)
			{
				signed short Sample;
				if (samplesOut < reqn)
				{
					Sample = MadFixedToSshort(audio->Synth.pcm.samples[0][i]);
					buffer[samplesOut * 2] = Sample;
					if (audio->getSoundInfo()->stereo == true)
						Sample = MadFixedToSshort(audio->Synth.pcm.samples[1][i]);
					buffer[samplesOut * 2 + 1] = Sample;
					samplesOut++;
				} else {

					Sample = MadFixedToSshort(audio->Synth.pcm.samples[0][i]);
					audio->OutputBuffer[audio->samplesInOutput * 2] = Sample;
					if (audio->getSoundInfo()->stereo == true)
						Sample = MadFixedToSshort(audio->Synth.pcm.samples[1][i]);
					audio->OutputBuffer[audio->samplesInOutput * 2 + 1] = Sample;
					audio->samplesInOutput++;
				}
			}
		}
	}
}


// unpacka l'mp3 in wave e rende l'esecuzione molto più veloce
bool CAudioSound::unpack()
{
	if (info.format != FORMAT_MP3)
		return false;

	if (buffer)
		delete buffer;

	if (doublebuffer)
		delete doublebuffer;

	sceKernelDeleteThread(thid);


	doublebuffer = 0;

	File->seek(seekstart);
	currentseek = 0;

	buffer = new u8[blocksize * info.totframes];
	int totread =  File->read(buffer, blocksize * info.totframes);
	short *samples;

	mad_stream_buffer(&Stream, buffer, info.totframes * blocksize);
	int count = 0;


	for (int y = 0; y < totread / blocksize; y++)
	{
		int retry = 0;
		if (mad_frame_decode(&Frame, &Stream))
		{
			mad_frame_decode(&Frame, &Stream);
		}
		mad_synth_frame(&Synth, &Frame);
		if (doublebuffer == 0)
		{

			doublebuffer = new u8[Synth.pcm.length * (info.stereo == true ? 4 : 2) * info.totframes];
			samples = (short*)doublebuffer;
			info.samplerate = Synth.pcm.samplerate;
			info.bitrate = info.samplerate * 16 * (info.stereo == true ? 2 : 1);
		}

		for (int z = 0; z < Synth.pcm.length; z++)
		{
			short Sample;
			Sample = MadFixedToSshort(Synth.pcm.samples[0][z]);
			samples[count++] = Sample;
			if (info.stereo == true)
			{ Sample = MadFixedToSshort(Synth.pcm.samples[1][z]);
			  samples[count++] = Sample; }
		}
	}
	filesize = count * 2;

	delete buffer;
	buffer = doublebuffer;
	doublebuffer = 0;
	info.format = FORMAT_WAV;

	if (File)
		File->drop();

	return true;
}
void __audioCallback0(void *buf, unsigned int reqn, void *userdata) {
	__audioCallback(0, buf, reqn);
}
void __audioCallback1(void *buf, unsigned int reqn, void *userdata) {
	__audioCallback(1, buf, reqn);
}
void __audioCallback2(void *buf, unsigned int reqn, void *userdata) {
	__audioCallback(2, buf, reqn);
}
void __audioCallback3(void *buf, unsigned int reqn, void *userdata) {
	__audioCallback(3, buf, reqn);
}
void __audioCallback4(void *buf, unsigned int reqn, void *userdata) {
	__audioCallback(4, buf, reqn);
}
void __audioCallback5(void *buf, unsigned int reqn, void *userdata) {
	__audioCallback(5, buf, reqn);
}
void __audioCallback6(void *buf, unsigned int reqn, void *userdata) {
	__audioCallback(6, buf, reqn);
}
void __audioCallback7(void *buf, unsigned int reqn, void *userdata) {
	__audioCallback(7, buf, reqn);
}

void CAudioSound::readBlock(int seek)
{
	if (seek != -1)
		File->seek(seek);

	if (disableBuffering == false) {

		memset(buffer, 0, BUFFER_SIZE);
		File->read(buffer, BUFFER_SIZE);

	}
	else
	{
		memset(buffer, 0, filesize);
		File->read(buffer, filesize);
	}

}


int AT3_Init(void *File_Mem, int File_Length, unsigned long *AT3_Codec_Buffer, short *AT3_Mix_Buffer, int *AT3_DataStart) {
	unsigned int *dwFile_Mem;
	unsigned char *tmpFile_Mem;

	int AT3_Datas_Start;



	dwFile_Mem = (unsigned int *) File_Mem;
	if(*dwFile_Mem++ == 0x46464952) {
		dwFile_Mem++;
		if(*dwFile_Mem++ == 0x45564157) {
			tmpFile_Mem = (unsigned char *) (dwFile_Mem + 2);
			while(*dwFile_Mem++ != 0x61746164) {
				dwFile_Mem = (unsigned int *) (tmpFile_Mem + *dwFile_Mem);
				tmpFile_Mem = (unsigned char *) (dwFile_Mem + 2);
			}
			AT3_Datas_Start = (int) tmpFile_Mem - (int) File_Mem;
			memset(AT3_Codec_Buffer, 0, 260);
			memset(AT3_Mix_Buffer, 0, AT3_SAMPLES * 2 * 2);

			AT3_Codec_Buffer[AT3_CURRENT_BUFFER] = (int) ( (char*)File_Mem + AT3_Datas_Start);
			AT3_Codec_Buffer[AT3_TEMPORARY_BUFFER] = (int) AT3_Mix_Buffer;
			AT3_Codec_Buffer[10] = 4;
			AT3_Codec_Buffer[44] = 2;
			AT3_Codec_Buffer[AT3_INITIAL_BUFFER] = (int) File_Mem;
			AT3_Codec_Buffer[AT3_LENGTH_BUFFER] = File_Length;

			*AT3_DataStart = AT3_Datas_Start;

			return(1);
		}
	}
	return(1010);
}


CAudioSound::CAudioSound(io::IReadFile* file, s32 count, bool start, int id, bool db)
	: eventId(id), loopCount(count), newEvent(false), bPlaying(false), chanId(-1), buffer(0), samplesInOutput(0),
	samplesOut(0), FrameCount(0), voll(100), volr(100), PlayCount(1), seekstart(0), currentseek(0), File(file),
	blocksize(0), buffering_requested(0), startstop(0), stoptime(0), BUFFER_SIZE(256*1024), disableBuffering(db)
{

	if (disableBuffering == true)
	{
		BUFFER_SIZE=1024*1024;
		disableBuffering=false;
	}

	// legge l'mp3 in memoria
	file->seek(0);

	filesize = file->getSize();
	buffer = new u8[BUFFER_SIZE]; // Stream buffer
	doublebuffer = new u8[BUFFER_SIZE];
	readBlock(0);

	info.duration = 0;
	info.format = FORMAT_UNKNOWN;

	position = 0;



	// controlla se il file è mp3
	// *******************************
	if (buffer[0] == 0xFF && buffer[1] == 0xFB)
		info.format = FORMAT_MP3;

	// mp3 with ID3 tag
	if (buffer[0] == 'I' && buffer[1] == 'D' && buffer[2] == '3')
	{
		seekstart = buffer[9] + buffer[8] * 128 + buffer[7] * 16384 + 10;
		info.format = FORMAT_MP3;
	}
	// ********************************





	if (seekstart != 0)
		readBlock(seekstart);

	currentseek = seekstart;

	info.filename = file->getFileName();

	info.stereo = false;
	str_in_use = false;

	if (info.format == FORMAT_UNKNOWN)
	{
		delete buffer;
		delete doublebuffer;
		doublebuffer=0;


		doublebuffer = 0;
		buffer = new u8[filesize];
		file->seek(0);

		file->read(buffer, filesize);

		int ret = AT3_Init(buffer, filesize, AT3_Codec_Buffer, AT3_Mix_Buffer, &AT3Start);

		if (ret == 1)
		{

			info.format = FORMAT_AT3PLUS;
			info.stereo = true;
			info.duration = 0;
			info.totframes = 0;


		} else {delete buffer; buffer = 0; }

	}



	// inizializza lo stream mp3
	if( info.format == FORMAT_MP3 )
	{

		if (disableBuffering == true)
		{

			/*delete buffer;
			   delete doublebuffer;

			   buffer = new u8[filesize];
			   		readBlock(0);*/



		}


		mad_stream_init(&Stream);
		mad_frame_init(&Frame);
		mad_synth_init(&Synth);
		mad_timer_reset(&Timer);
		mad_stream_buffer(&Stream, buffer, filesize - seekstart);
		mad_frame_decode(&Frame, &Stream);

		Stream.options |= MAD_OPTION_IGNORECRC;

		if (MAD_NCHANNELS(&Frame.header) == 2)
			info.stereo = true;

		info.bitrate = Frame.header.bitrate;
		info.samplerate = Frame.header.samplerate;
		blocksize = Stream.next_frame - Stream.this_frame;

		info.duration =  (int)(( (f32)(filesize-seekstart) / (f32)(info.bitrate/8)) * 1000.f);
		info.totframes = (filesize-seekstart) / blocksize;

		if (disableBuffering == false)
			thid = sceKernelCreateThread("buffer_thread",__mp3_buffer_thread,0x12,0x10000,0,NULL);

	}


	if (info.format != FORMAT_MP3) {


		if (File)
			File->drop();

	}

	if (start == true)
		play();

}
void CAudioSound::setAutoStop(int time ) {
	stoptime = time;
}
const ISoundInfo *CAudioSound::getSoundInfo() const {
	return &info;
}
void CAudioSound::setVolume(s32 vol)
{
	setVolume(vol, vol);
}
void CAudioSound::setVolume(s32 volLeft, s32 volRight)
{
	voll = volLeft; volr = volRight;
	if (chanId != -1)
	{
		pspAudioSetVolume ( chanId, (voll * PSP_VOLUME_MAX) / 100, (volr * PSP_VOLUME_MAX) / 100);
	}

}
void CAudioSound::stop()
{
	releaseChannel();
	bPlaying = false;
	inPause=false;
	PlayCount = 1;
	position = 0;
	FrameCount = 0;
	currentseek = seekstart;

	if (info.format==FORMAT_AT3PLUS)
		sceAudiocodecReleaseEDRAM(AT3_Codec_Buffer);

}

void CAudioSound::stop_in_thread()
{

	pspAudioReleaseChannel(chanId);
	bPlaying = false;
	inPause=false;
	PlayCount = 1;
	position = 0;
	FrameCount = 0;
	currentseek = seekstart;

	if (info.format==FORMAT_AT3PLUS)
		sceAudiocodecReleaseEDRAM(AT3_Codec_Buffer);
	pspAudioStopThisThread(chanId);

	chanId = -1;



}

void CAudioSound::rewind()
{
	FrameCount = 0;
	currentseek = seekstart;

	if (info.format == FORMAT_MP3)
	{
		readBlock();
		mad_stream_buffer(&Stream, buffer, filesize - seekstart);
		mad_frame_decode(&Frame, &Stream);

	}
	if (info.format == FORMAT_AT3PLUS)
	{


		AT3_Codec_Buffer[AT3_CURRENT_BUFFER] = (int) ( (char*)buffer + AT3Start);
		AT3_Codec_Buffer[AT3_TEMPORARY_BUFFER] = (int) AT3_Mix_Buffer;
		AT3_Codec_Buffer[10] = 4;
		AT3_Codec_Buffer[44] = 2;
		AT3_Codec_Buffer[AT3_INITIAL_BUFFER] = (int) buffer;
		AT3_Codec_Buffer[AT3_LENGTH_BUFFER] = filesize;


	}

}
void CAudioSound::pause()
{
	inPause = true;
	pspAudioSetChannelCallback(chanId,0,0);
}
bool CAudioSound::setPosition(s32 millisec)
{

	if (millisec > info.duration)
		return false;

	if (millisec < 0)
		return false;

	position = millisec;
	if (info.format == FORMAT_MP3)
	{
		bool playAfter = !inPause && (bPlaying == true);
		// devo mandare avanti
		if (playAfter)
			pause(); // metto in pausa, ovvero fermo il thread

		// calcolo la posizione del frame
		// furbo no ?
		f32 framepos = (int)(((f32)millisec/(f32)info.duration) * info.totframes);
		int frame = (int)framepos * blocksize;

		samplesInOutput = 0;

		int cseek = seekstart + frame;
		if (  currentseek >= cseek && ( cseek < currentseek+BUFFER_SIZE)  )
		{
			// ci sta dentro
			int framecur = currentseek - cseek;
			Stream.this_frame = Stream.buffer + framecur;
			Stream.next_frame = Stream.this_frame + blocksize;

		} else
		{

			currentseek = cseek;
			readBlock(currentseek);
			Stream.skiplen = 0;
			Stream.this_frame = Stream.buffer;
			Stream.next_frame = Stream.buffer + blocksize;

		}
		if (playAfter)
			play();

	}

	return true;
}
s32 CAudioSound::getPosition()
{
	return position;
}
s32 CAudioSound::getLoopCount()
{
	return loopCount;
}
s32 CAudioSound::getPlayCount()
{
	return PlayCount;
}
void CAudioSound::setLoopCount(s32 loop)
{
	loopCount = loop;
}


bool CAudioSound::isPlaying() {
	return bPlaying;
}

void CAudioSound::releaseChannel()
{
	if (chanId != -1)
	{
		pspAudioSetChannelCallback(chanId,0,0);
		pspAudioReleaseChannel(chanId);
		chanId = -1;
	}
}

// restituisce millisecondi ore minuti e così via
void CAudioSound::getTimeValue(s32 millisec, s32 *hour, s32 *mins, s32 *secs, s32 *ms)
{
	*ms = millisec % 1000;
	*secs = (millisec / 1000) % 60;
	*mins = (millisec / 60000) % 60;
	*hour = (millisec / 3600000) % 24;
}


// restituisce una stringa formattata della durata
core::stringw CAudioSound::getStringDuration()
{
	return getFormatString(info.duration);
}

//! restituisce una stringa formattata della posizione
core::stringw CAudioSound::getStringPosition()
{
	return getFormatString(position);
}

//r restituisce una stringa formattata in base a millisec
core::stringw CAudioSound::getFormatString(s32 millisec)
{
	int h,m,s,ms;
	getTimeValue(millisec, &h, &m, &s, &ms);

	core::stringw p;
	p += h;
	p += L":";
	p += m;
	p += L":";
	p += s;
	p += L".";
	p += ms;
	return p;
}


void CAudioSound::play()
{
	if (info.format == FORMAT_UNKNOWN)
		return;

	if (inPause == true)
	{
		inPause = false;
		switch (chanId)
		{
		case 0:
			pspAudioSetChannelCallback(chanId, __audioCallback0, 0);
			break;
		case 1:
			pspAudioSetChannelCallback(chanId, __audioCallback1, 0);
			break;
		case 2:
			pspAudioSetChannelCallback(chanId, __audioCallback2, 0);
			break;
		case 3:
			pspAudioSetChannelCallback(chanId, __audioCallback3, 0);
			break;
		case 4:
			pspAudioSetChannelCallback(chanId, __audioCallback4, 0);
			break;
		case 5:
			pspAudioSetChannelCallback(chanId, __audioCallback5, 0);
			break;
		case 6:
			pspAudioSetChannelCallback(chanId, __audioCallback6, 0);
			break;
		case 7:
			pspAudioSetChannelCallback(chanId, __audioCallback7, 0);
			break;
		}
		return;
	}

	startstop = os::Timer::getRealTime();
	if (bPlaying == true) {
		// riavvolge la posizione
		rewind();
	} else {
		// apre un canale audio


		if (info.format == FORMAT_AT3PLUS)
		{
			rewind();

			if(sceAudiocodecCheckNeedMem(AT3_Codec_Buffer, AT3_TYPE_ATRAC3) < 0) return;
			if(sceAudiocodecGetEDRAM(AT3_Codec_Buffer, AT3_TYPE_ATRAC3) < 0) return;
			if(sceAudiocodecInit(AT3_Codec_Buffer, AT3_TYPE_ATRAC3) < 0) return;



		}
		int chan = pspAudioChannelSearchFree();

		// non ci sono+ canali audio
		if (chan < 0)
			return;

		chanId = chan;


		pspAudioSetVolume ( chanId, (voll * PSP_VOLUME_MAX) / 100, (volr * PSP_VOLUME_MAX) / 100);

		__psp_audio_reference[chanId] = this;




		// Apre il canale audio
		switch (chanId)
		{
		case 0:
			pspAudioSetChannelCallback(chanId, __audioCallback0, 0);
			break;
		case 1:
			pspAudioSetChannelCallback(chanId, __audioCallback1, 0);
			break;
		case 2:
			pspAudioSetChannelCallback(chanId, __audioCallback2, 0);
			break;
		case 3:
			pspAudioSetChannelCallback(chanId, __audioCallback3, 0);
			break;
		case 4:
			pspAudioSetChannelCallback(chanId, __audioCallback4, 0);
			break;
		case 5:
			pspAudioSetChannelCallback(chanId, __audioCallback5, 0);
			break;
		case 6:
			pspAudioSetChannelCallback(chanId, __audioCallback6, 0);
			break;
		case 7:
			pspAudioSetChannelCallback(chanId, __audioCallback7, 0);
			break;
		}
		bPlaying = true;


	}
}



CAudioSound::~CAudioSound() {

	if (bPlaying == true)
		stop();

	if (buffer)
		delete buffer;

	if (doublebuffer)
		delete doublebuffer;

	if (info.format == FORMAT_MP3)
		sceKernelDeleteThread(thid);

	if (File)
		File->drop();
}


} // end of namespace

}
