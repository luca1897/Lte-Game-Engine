/* 
  Common include for the LTE Game Engine examples, here there are the functions
  to setup psp.
*/

#define PSP_BOOST_CPU
// Undefine this if you don't want to
// overclock your cpu to 333MHz

#include <pspkernel.h>
#ifdef PSP_BOOST_CPU
#include <psppower.h>
#endif

#ifdef PSP_ENABLE_DEBUG
#include <pspdebug.h>
#define printf pspDebugScreenPrintf
#endif


int exitCallback(int arg1, int arg2, void *common)
{

	sceKernelExitGame();
	return 0;
}

int callbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exitCallback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

int setupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", callbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

int setupPSP(void)
{
  
   #ifdef PSP_BOOST_CPU
    scePowerSetClockFrequency(333, 333, 166);
   #endif

   // setup callbacks
   setupCallbacks();

   #ifdef PSP_ENABLE_DEBUG
    pspDebugScreenInit();
   #endif

}
