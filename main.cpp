#include "IOPortEmu/Win32.h"

static IOPortEmuWin32* ioport_emu = nullptr;

// Entry-Point
static void __attribute__((constructor)) init_library(void){
   ioport_emu = new IOPortEmuWin32();
}

static void __attribute__((destructor)) deinit_library(void){
    if(ioport_emu){
        delete ioport_emu;
    }
}

