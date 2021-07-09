#pragma once
#ifdef TARGET_PLATFORM_WINDOWS

#include "./Base.h"
#include <errhandlingapi.h>

class IOPortEmuWin32 : Base {
    public:
    long pexception_handler(PEXCEPTION_POINTERS pExceptionInfo);
    IOPortEmuWin32() { AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)&this->pexception_handler); }
    ~IOPortEmuWin32(){ RemoveVectoredExceptionHandler( (PVOID)&this->pexception_handler); }
};
#endif