
#ifdef TARGET_PLATFORM_WINDOWS
#include <cstdio>
#include <cstdlib>

#include "Win32.h"

#ifdef TARGET_ARCH_64
    #define XIP_PTR pExceptionInfo->ContextRecord->Rip
    #define XAX pExceptionInfo->ContextRecord->Rax
    #define XDX pExceptionInfo->ContextRecord->Rdx
#else 
    #define XIP_PTR pExceptionInfo->ContextRecord->Eip
    #define XAX pExceptionInfo->ContextRecord->Eax
    #define XDX pExceptionInfo->ContextRecord->Edx  
#endif 


long IOPortEmuWin32::pexception_handler(PEXCEPTION_POINTERS pExceptionInfo){
    unsigned char bump_ip = this->handle_io((unsigned char*)XIP_PTR,(unsigned int*)&XAX, XDX);
    if(!bump_ip){
        return EXCEPTION_EXECUTE_HANDLER;
    }

    XIP_PTR += bump_ip;
    return EXCEPTION_CONTINUE_EXECUTION;
}
#endif