TARGET = Collision
OBJS = main.o

INCDIR = -I ../../include/
CFLAGS = -G0  -O2
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PSP_FW_VERSION = 390
PSP_LARGE_MEMORY = 1

LIBDIR = -L ../../libs
LDFLAGS =
LIBS= -lengine -lpspwlan -lglut -lGLU  -lGL -lpspvfpu -lm -lstdc++ -lpsppower -lpsprtc -lpspaudio

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = LTE GE Example - Collision

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
