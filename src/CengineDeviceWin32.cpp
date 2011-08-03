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
 
// riscritta da sibe per la psp


#include "engineCompileConfig.h"
#include "CengineDeviceWin32.h"
#include "IEventReceiver.h"
#include "engineList.h"
#include "CAudioDriver.h"
#include "CNetworkDriver.h"

#include "os.h"
#include <pspdisplay.h>
#include <psputils.h>

#include "CTimer.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <pspvfpu.h>
#include <pspaudiolib.h>

struct pspvfpu_context *ctx;
extern engine::video::ITexture* cursor;

int key_state = 0; // disable
int key_type = 0; // pad selector
int key_sel = 4; //  key selector
int key_pos_x = 320; // pad x position
int key_pos_y = 112; // pad y position

int wasStartDown = 0;
int wasLDown = 0;
int wasRDown = 0;
bool __inet_sdk_modules_loaded;

engine::net::INetworkDriver *__int_ndrv;

#include "engineString.h"
#include "COSOperator.h"
#include "dimension2d.h"
#include "engine.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

bool __engine_stop_execution = false;
engine::audio::IAudioDriver *__lte_intdrv;

#define printf pspDebugScreenPrintf
int __table_pressed_down[50];

namespace engine
{

//! constructor
CengineDeviceWin32::CengineDeviceWin32(video::E_DRIVER_TYPE driverType,
                                       core::dimension2d<s32> windowSize,
                                       u32 bits, bool fullscreen,
                                       bool stencilbuffer, bool vsync,
                                       bool antiAlias,
                                       IEventReceiver* receiver,
                                       const char* version, bool showLogo)
	: CengineDeviceStub(version, receiver),
	Win32CursorControl(0),
	FullScreen(fullscreen)
{
	for (int x = 0; x < 50; x++) __table_pressed_down[x] = 0;

	ctx = pspvfpu_initcontext();
	pspvfpu_use_matrices(ctx, 0, VMAT5);
	//initGraphics();

	Win32CursorControl = new CCursorControl();
	CursorControl = Win32CursorControl;
	AudioDriver = new audio::CAudioDriver(FileSystem);
	NetworkDriver = new net::CNetworkDriver(__inet_sdk_modules_loaded);

	__lte_intdrv = AudioDriver;
	__int_ndrv = NetworkDriver;

	createDriver(driverType, windowSize, bits, fullscreen, stencilbuffer, vsync, antiAlias, showLogo);



	createGUIAndScene();

}





//! destructor
CengineDeviceWin32::~CengineDeviceWin32()
{
	// unregister environment
	__engine_stop_execution = 1;
	// vuoto
}



//! create the driver
void CengineDeviceWin32::createDriver(video::E_DRIVER_TYPE driverType,
                                      const core::dimension2d<s32>& windowSize,
                                      u32 bits,
                                      bool fullscreen,
                                      bool stencilbuffer,
                                      bool vsync,
                                      bool antiAlias, bool showLogo)
{
	VideoDriver = video::createSoftwareDriver2(windowSize, fullscreen, FileSystem, this, CursorControl, showLogo);
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
}


int wasMouseDown = 0;

inline int myabs(int x) {
	if (x < 0 ) return -x;return x;
}

#define KEYMAP_NOTHING 0x0
#define KEYMAP_DELETE 0x1
#define KEYMAP_SPACE 0x2

const char key_map_of_codes[][9][4] = {
	{{'a','\'','c','b'}, {'d','.','f','e'}, {'g','!','i','h'},
	 {'j','-','l','k'}, {'m',KEYMAP_DELETE,'n',KEYMAP_SPACE}, {'o','?','q','p'},
	 {'r','(','t','s'}, {'u',':','w','v'}, {'x',')','z','y'}},

	{{'A','^','C','B'}, {'D','@','F','E'}, {'G','*','I','H'},
	 {'J','_','L','K'}, {'M',KEYMAP_DELETE,'N',KEYMAP_SPACE}, {'O','"','Q','P'},
	 {'R','=','T','S'}, {'U',';','W','V'}, {'X','/','Z','Y'}},

	{{ KEYMAP_NOTHING, KEYMAP_NOTHING,'1', KEYMAP_NOTHING},{ KEYMAP_NOTHING, KEYMAP_NOTHING,'2', KEYMAP_NOTHING}, { KEYMAP_NOTHING, KEYMAP_NOTHING,'3', KEYMAP_NOTHING},
	 { KEYMAP_NOTHING, KEYMAP_NOTHING,'4', KEYMAP_NOTHING}, {KEYMAP_NOTHING,KEYMAP_DELETE,'5',KEYMAP_SPACE}, { KEYMAP_NOTHING, KEYMAP_NOTHING,'6', KEYMAP_NOTHING},
	 { KEYMAP_NOTHING, KEYMAP_NOTHING,'7', KEYMAP_NOTHING}, { KEYMAP_NOTHING, KEYMAP_NOTHING,'8', KEYMAP_NOTHING}, { KEYMAP_NOTHING, KEYMAP_NOTHING,'9', '0'}},

	{{'(',',',')','.'}, {'<','"','>','\''}, {'[','-',']','_'},
	 {'{','!','}','?'}, {KEYMAP_NOTHING,KEYMAP_DELETE,KEYMAP_NOTHING,KEYMAP_SPACE}, {'\\','+','/','='},
	 {'@',':','#',';'}, {'$','~','%','`'}, {'^','*','&','|'}}
};


void CengineDeviceWin32::__lte_send_po(EKEY_CODE a)
{
	engine::SEvent ev;
	ev.KeyInput.Char = 0;
	ev.EventType = engine::EET_KEY_INPUT_EVENT;
	ev.KeyInput.PressedDown = false;
	ev.KeyInput.Key = a;
	ev.KeyInput.PressedOnce = true;
	postEventFromUser(ev);
}

int keymapPressed = 0;
//! runs the device. Returns false if device wants to be deleted
bool CengineDeviceWin32::run()
{
	if (__engine_stop_execution) return false;

	os::Timer::tick();

	if(NetworkDriver)
		NetworkDriver->run();

	bool quit = false;
	engine::SEvent event;
	event.KeyInput.Control = false;

	// *------------------------------------------
	// tutte le operazioni per prendere il cursore
	// e i tasti premuti vanno qui
	SceCtrlData pad;
	sceCtrlPeekBufferPositive (&pad, 1);


	// la tastiera è attiva
	if (key_state == 1) {

		if (pad.Buttons & PSP_CTRL_UP)
			key_pos_y -= 5;

		if (pad.Buttons & PSP_CTRL_DOWN)
			key_pos_y += 5;

		if (pad.Buttons & PSP_CTRL_RIGHT)
			key_pos_x += 5;

		if (pad.Buttons & PSP_CTRL_LEFT)
			key_pos_x -= 5;

		// disabilita la keyboard
		if (pad.Buttons & PSP_CTRL_START)
		{
			if (wasStartDown == 0) {
				key_state = 0;
				wasStartDown = 1;
				return !quit;
			}
		} else {
			wasStartDown = 0;
		}



		/* aggiusta la posizione in caso */
		if (key_pos_x < 0) key_pos_x = 0;
		if (key_pos_x > 329) key_pos_x = 329;
		if (key_pos_y < 0) key_pos_y = 0;
		if (key_pos_y > 121) key_pos_y = 121;

		if (pad.Buttons & PSP_CTRL_LTRIGGER){
			if (wasLDown == 0){
				key_type--;
				wasLDown = 1;
			}
		} else wasLDown = 0;

		if (pad.Buttons & PSP_CTRL_RTRIGGER){
			if (wasRDown == 0) {
				key_type++;
				wasRDown = 1;
			}
		} else wasRDown = 0;

		if (key_type < 0) key_type = 3;
		if (key_type > 3) key_type = 0;





		int xpos = 0, ypos = 0;
		if ((pad.Lx > 170) || (pad.Lx < 100)) {
			xpos = (pad.Lx - 128);
		}
		if ((pad.Ly > 170) || (pad.Ly < 100)) {
			ypos = (pad.Ly - 128);
		}

		int currentindex = 4;
		if (xpos < 0) currentindex--;
		if (xpos > 0) currentindex++;
		if (ypos > 0) currentindex+=3;
		if (ypos < 0) currentindex-=3;
		key_sel = currentindex;

		int keycode = KEYMAP_NOTHING;

		if (pad.Buttons & PSP_CTRL_SQUARE) keycode = key_map_of_codes[key_type][key_sel][0];
		if (pad.Buttons & PSP_CTRL_TRIANGLE) keycode = key_map_of_codes[key_type][key_sel][1];
		if (pad.Buttons & PSP_CTRL_CIRCLE) keycode = key_map_of_codes[key_type][key_sel][2];
		if (pad.Buttons & PSP_CTRL_CROSS) keycode = key_map_of_codes[key_type][key_sel][3];


		if (keymapPressed == 0)
			switch (keycode) {
			case KEYMAP_NOTHING:
				break;
			case KEYMAP_DELETE:
				event.KeyInput.Key = engine::KEY_BACK;
				event.EventType = engine::EET_KEY_INPUT_EVENT;
				event.KeyInput.PressedDown = true;
				postEventFromUser(event);
				event.KeyInput.PressedDown = false;
				postEventFromUser(event);
				break;
			case KEYMAP_SPACE:
				event.KeyInput.Key = engine::KEY_SPACE;
				event.EventType = engine::EET_KEY_INPUT_EVENT;
				event.KeyInput.Char = 32;
				event.KeyInput.PressedDown = true;
				postEventFromUser(event);
				event.KeyInput.PressedDown = false;
				postEventFromUser(event);
				break;
			default:
				event.KeyInput.Key = (engine::EKEY_CODE)keycode;
				event.EventType = engine::EET_KEY_INPUT_EVENT;
				event.KeyInput.Char = keycode;
				event.KeyInput.PressedDown = true;
				postEventFromUser(event);
				event.KeyInput.PressedDown = false;
				postEventFromUser(event);
				break;
			}
		if ((pad.Buttons & PSP_CTRL_SQUARE) || (pad.Buttons & PSP_CTRL_TRIANGLE) ||
		    (pad.Buttons & PSP_CTRL_CIRCLE) || (pad.Buttons & PSP_CTRL_CROSS)) keymapPressed = 1;
		else
			keymapPressed = 0;



	} else {

		if (wasStartDown == 1 && (pad.Buttons & PSP_CTRL_START)) return !quit;
		else wasStartDown = 0;

		f32 sensibility = 0.2;
		int fpsc = getVideoDriver()->getFPS();
		if (fpsc == 0) fpsc = 60;

		if (CursorControl->isVisible() == true) sensibility = sensibility = (60/fpsc) *  ((CCursorControl*)CursorControl)->getSpeed();
		// if (CursorControl->isVisible() == true && (pad.Buttons  & PSP_CTRL_RTRIGGER)) sensibility = (60/fpsc) *  0.05;
		bool moved = false;
		const f32 speed = 0.05f;
		core::position2d<s32> CursorPos = CursorControl->getPosition();
		if ((pad.Lx > 160) || (pad.Lx < 110)) {
			f32 xpos = (pad.Lx - 128) * sensibility;
			CursorPos.X += (int)xpos;
			moved=true;
		}
		if ((pad.Ly > 160) || (pad.Ly < 110)) {
			f32 ypos = (pad.Ly - 128) * sensibility;
			CursorPos.Y += (int)ypos;
			moved=true;
		}

		if (CursorPos.Y < 0) CursorPos.Y = 0;
		if (CursorPos.Y > 272) CursorPos.Y = 272;

		if (CursorPos.X < 0) CursorPos.X = 0;
		if  (CursorPos.X > 480) CursorPos.X = 480;
		CursorControl->setPosition(CursorPos);

		if (CursorControl->isEnabled() == true)
		{
			((CCursorControl*)CursorControl)->setPosition2(CursorPos);

		}



		/* Il cursore del mouse è visibile quindi interpreta i pulsanti come mouse */
		if (CursorControl->isVisible() == true && CursorControl->isEnabled() == true)
		{

			if (moved == true)
			{


				event.MouseInput.Event = engine::EMIE_MOUSE_MOVED;
				event.MouseInput.X = CursorPos.X;
				event.MouseInput.Y = CursorPos.Y;
				postEventFromUser(event);


			}

			if (CursorControl->isXEnabled() == true)
			{

				if (wasMouseDown && !(pad.Buttons & PSP_CTRL_CROSS)) {
					event.EventType = engine::EET_MOUSE_INPUT_EVENT;
					event.MouseInput.Event = engine::EMIE_LMOUSE_LEFT_UP;
					event.MouseInput.X = CursorPos.X;
					event.MouseInput.Y = CursorPos.Y;
					postEventFromUser(event);
					wasMouseDown=0;
				}

				if (pad.Buttons & PSP_CTRL_CROSS)
				{
					if (wasMouseDown == 0) {

						event.MouseInput.Event = engine::EMIE_LMOUSE_PRESSED_DOWN;
						event.MouseInput.X = CursorPos.X;
						event.MouseInput.Y = CursorPos.Y;
						postEventFromUser(event);
						wasMouseDown = 1;

					}
				}
			}


		}
		/* Il cursore del mouse non è visibile quindi interpreta i pulsanti
		   come tastiera */


		event.KeyInput.Char = 0;
		event.EventType = engine::EET_KEY_INPUT_EVENT;
		event.KeyInput.PressedDown = true;

		event.KeyInput.Key = engine::KEY_SQUARE;
		if (pad.Buttons & PSP_CTRL_SQUARE){

			event.EventType = engine::EET_KEY_INPUT_EVENT;
			event.KeyInput.PressedDown = true;
			event.KeyInput.PressedOnce = false;



			if (__table_pressed_down[(int)event.KeyInput.Key] == 0)
				__lte_send_po(event.KeyInput.Key);

			__table_pressed_down[(int)event.KeyInput.Key]= 1;
			postEventFromUser(event);
		} else if (__table_pressed_down[(int)event.KeyInput.Key] == 1) __table_pressed_down[(int)event.KeyInput.Key] = -1;


		event.KeyInput.Key = engine::KEY_TRIANGLE;
		if (pad.Buttons & PSP_CTRL_TRIANGLE){
			event.EventType = engine::EET_KEY_INPUT_EVENT;
			event.KeyInput.PressedDown = true;

			if (__table_pressed_down[(int)event.KeyInput.Key] == 0)
				__lte_send_po(event.KeyInput.Key);


			__table_pressed_down[(int)event.KeyInput.Key]= 1;
			postEventFromUser(event);
		} else if (__table_pressed_down[(int)event.KeyInput.Key] == 1) __table_pressed_down[(int)event.KeyInput.Key] = -1;


		event.KeyInput.Key = engine::KEY_CIRCLE;
		if (pad.Buttons & PSP_CTRL_CIRCLE){

			event.EventType = engine::EET_KEY_INPUT_EVENT;
			event.KeyInput.PressedDown = true;


			if (__table_pressed_down[(int)event.KeyInput.Key] == 0)
				__lte_send_po(event.KeyInput.Key);

			__table_pressed_down[(int)event.KeyInput.Key]= 1;
			postEventFromUser(event);
		} else if (__table_pressed_down[(int)event.KeyInput.Key] == 1) __table_pressed_down[(int)event.KeyInput.Key] = -1;


		event.KeyInput.Key = engine::KEY_CROSS;

		if (pad.Buttons & PSP_CTRL_CROSS){
			event.EventType = engine::EET_KEY_INPUT_EVENT;
			event.KeyInput.PressedDown = true;


			if (__table_pressed_down[(int)event.KeyInput.Key] == 0)
				__lte_send_po(event.KeyInput.Key);

			__table_pressed_down[(int)event.KeyInput.Key]= 1;

			postEventFromUser(event);
		} else if (__table_pressed_down[(int)event.KeyInput.Key] == 1) __table_pressed_down[(int)event.KeyInput.Key] = -1;


		event.KeyInput.Key = engine::KEY_UP;
		if (pad.Buttons & PSP_CTRL_UP){
			event.EventType = engine::EET_KEY_INPUT_EVENT;
			event.KeyInput.PressedDown = true;


			if (__table_pressed_down[(int)event.KeyInput.Key] == 0)
				__lte_send_po(event.KeyInput.Key);

			__table_pressed_down[(int)event.KeyInput.Key]= 1;
			postEventFromUser(event);
		} else if (__table_pressed_down[(int)event.KeyInput.Key] == 1) __table_pressed_down[(int)event.KeyInput.Key] = -1;
		event.KeyInput.Key = engine::KEY_DOWN;
		if (pad.Buttons & PSP_CTRL_DOWN){
			event.EventType = engine::EET_KEY_INPUT_EVENT;
			event.KeyInput.PressedDown = true;

			if (__table_pressed_down[(int)event.KeyInput.Key] == 0)
				__lte_send_po(event.KeyInput.Key);

			__table_pressed_down[(int)event.KeyInput.Key]= 1;
			postEventFromUser(event);
		} else if (__table_pressed_down[(int)event.KeyInput.Key] == 1) __table_pressed_down[(int)event.KeyInput.Key] = -1;
		event.KeyInput.Key = engine::KEY_LEFT;
		if (pad.Buttons & PSP_CTRL_LEFT){
			event.EventType = engine::EET_KEY_INPUT_EVENT;
			event.KeyInput.PressedDown = true;

			if (__table_pressed_down[(int)event.KeyInput.Key] == 0)
				__lte_send_po(event.KeyInput.Key);

			__table_pressed_down[(int)event.KeyInput.Key]= 1;
			postEventFromUser(event);
		}  else if (__table_pressed_down[(int)event.KeyInput.Key] == 1) __table_pressed_down[(int)event.KeyInput.Key] = -1;
		event.KeyInput.Key = engine::KEY_RIGHT;
		if (pad.Buttons & PSP_CTRL_RIGHT){
			event.EventType = engine::EET_KEY_INPUT_EVENT;
			event.KeyInput.PressedDown = true;

			if (__table_pressed_down[(int)event.KeyInput.Key] == 0)
				__lte_send_po(event.KeyInput.Key);

			__table_pressed_down[(int)event.KeyInput.Key]= 1;
			postEventFromUser(event);
		} else if (__table_pressed_down[(int)event.KeyInput.Key] == 1) __table_pressed_down[(int)event.KeyInput.Key] = -1;

		event.KeyInput.Key = engine::KEY_START;

		if (pad.Buttons & PSP_CTRL_START){
			event.EventType = engine::EET_KEY_INPUT_EVENT;
			event.KeyInput.PressedDown = true;


			if (__table_pressed_down[(int)event.KeyInput.Key] == 0)
				__lte_send_po(event.KeyInput.Key);

			__table_pressed_down[(int)event.KeyInput.Key]= 1;
			postEventFromUser(event);
		} else if (__table_pressed_down[(int)event.KeyInput.Key] == 1) __table_pressed_down[(int)event.KeyInput.Key] = -1;


		event.KeyInput.Key = engine::KEY_SELECT;
		if (pad.Buttons & PSP_CTRL_SELECT){
			event.EventType = engine::EET_KEY_INPUT_EVENT;
			event.KeyInput.PressedDown = true;

			if (__table_pressed_down[(int)event.KeyInput.Key] == 0)
				__lte_send_po(event.KeyInput.Key);

			__table_pressed_down[(int)event.KeyInput.Key]= 1;
			postEventFromUser(event);
		} else if (__table_pressed_down[(int)event.KeyInput.Key] == 1) __table_pressed_down[(int)event.KeyInput.Key] = -1;
		event.KeyInput.Key = engine::KEY_LBUTTON;
		if (pad.Buttons & PSP_CTRL_LTRIGGER){


			if (__table_pressed_down[(int)event.KeyInput.Key] == 0)
				__lte_send_po(event.KeyInput.Key);
			__table_pressed_down[(int)event.KeyInput.Key]= 1;
			postEventFromUser(event);
		} else if (__table_pressed_down[(int)event.KeyInput.Key] == 1) __table_pressed_down[(int)event.KeyInput.Key] = -1;
		event.KeyInput.Key = engine::KEY_HOME;
		if (pad.Buttons & PSP_CTRL_HOME){
			event.EventType = engine::EET_KEY_INPUT_EVENT;
			event.KeyInput.PressedDown = true;

			if (__table_pressed_down[(int)event.KeyInput.Key] == 0)
				__lte_send_po(event.KeyInput.Key);

			__table_pressed_down[(int)event.KeyInput.Key]= 1;
			postEventFromUser(event);
		} else if (__table_pressed_down[(int)event.KeyInput.Key] == 1) __table_pressed_down[(int)event.KeyInput.Key] = -1;

		event.KeyInput.Key = engine::KEY_NOTE;
		if (pad.Buttons & PSP_CTRL_NOTE){
			event.EventType = engine::EET_KEY_INPUT_EVENT;
			event.KeyInput.PressedDown = true;

			if (__table_pressed_down[(int)event.KeyInput.Key] == 0)
				__lte_send_po(event.KeyInput.Key);

			__table_pressed_down[(int)event.KeyInput.Key]= 1;
			postEventFromUser(event);
		} else if (__table_pressed_down[(int)event.KeyInput.Key] == 1) __table_pressed_down[(int)event.KeyInput.Key] = -1;
		event.KeyInput.Key = engine::KEY_RBUTTON;

		if (pad.Buttons & PSP_CTRL_RTRIGGER ){
			event.EventType = engine::EET_KEY_INPUT_EVENT;
			event.KeyInput.PressedDown = true;

			if (__table_pressed_down[(int)event.KeyInput.Key] == 0)
				__lte_send_po(event.KeyInput.Key);

			__table_pressed_down[(int)event.KeyInput.Key]= 1;
			postEventFromUser(event);
		} else if (__table_pressed_down[(int)event.KeyInput.Key] == 1) __table_pressed_down[(int)event.KeyInput.Key] = -1;

		for (int x = 1; x < 0x19; x++)
		{

			event.KeyInput.Char = 0;
			event.EventType = engine::EET_KEY_INPUT_EVENT;
			event.KeyInput.PressedDown = false;
			event.KeyInput.Key = (engine::EKEY_CODE)x;
			if (__table_pressed_down[(int)event.KeyInput.Key] == -1) {
				__table_pressed_down[(int)event.KeyInput.Key] = 0;
				postEventFromUser(event);



			}

		}

	}





	// sposto il cursore del mouse

	// ------------------------------------------*

	return !quit;
}


void CengineDeviceWin32::resizeIfNecessary()
{
	Resized = false;
}


//! \return Returns a pointer to a list with all video modes supported
//! by the gfx adapter.
video::IVideoModeList* CengineDeviceWin32::getVideoModeList()
{
	return 0;
}

int ruota(int k)
{
	int a = k&0xFF00;
	int b = k&0xFF;
	int c = k&0xFF0000;
	int d = k&0xFF000000;
	b = b << 16;
	c = c >> 16;
	return d+b+a+c;
}


//! presents a surface in the client area
void CengineDeviceWin32::present(video::IImage* image, s32 windowId, core::rect<s32>* src )
{


}



//! notifies the device that it should close itself
void CengineDeviceWin32::closeDevice()
{
	__engine_stop_execution = true;
}

engineDevice* createDeviceEx(
        const SengineCreationParameters& parameters)
{
	CengineDeviceWin32* dev = new CengineDeviceWin32(
	        parameters.DriverType,
	        parameters.WindowSize,
	        parameters.Bits,
	        parameters.Fullscreen,
	        parameters.Stencilbuffer,
	        parameters.Vsync,
	        parameters.AntiAlias,
	        parameters.EventReceiver,
	        parameters.SDK_version_do_not_use, parameters.showLogo);


	return dev;
}


} // end namespace


