// Support layer for IOPORT Commands.
// Useful for emulation of direct HW legacy stuff, LPT, etc.
// cc -shared -D_GNU_SOURCE -w ./ioport.c -o ioport_u64.so
// cc -D_GNU_SOURCE -w ./ioport.c -m32 -o ioport_u32.elf
// x86_64-w64-mingw32-gcc ./ioport.c -o ./ioport_w64.exe
// i686-w64-mingw32-gcc   ./ioport.c -o ./ioport_w32.exe

#include <stdio.h>

// Check PLATFORM
#if _WIN32 || _WIN64
    #define PLATFORM_WINDOWS
    #include <errhandlingapi.h>
#else
    #define PLATFORM_UNIX  
    #include <signal.h>
    #include <sys/io.h>
    #include <sys/ucontext.h>   
#endif

// Check Architecture
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32    
#endif

#include "ioport.h"

void handle_out(unsigned int port_val,unsigned int port_mask, unsigned int data_val, unsigned int data_mask){
    // TODO: Add Port Handler Stuff
    //printf("OUT[%04X]: %04X\n",port_val,data_val);
}

void handle_in(unsigned int port_val, unsigned int port_mask, unsigned int* data_val,unsigned int data_mask){
    // TODO: Add Port Handler Stuff
    //printf("IN[%04X]\n",port_val);
    // Remove this - for testing only.
    *data_val = 0;
}

// Primary Handler - Determine port, data, and instruction size, pass to impl handlers.
unsigned char handle_ioport(unsigned char ci[3],void** xax, void** xdx){
    unsigned int eax = *xax;
    unsigned int edx = *xdx;
    
    // For 16bit imm8 calls, there is a preface instruction of 0x66
    // Probably due to the fact that Intel re-used the instruction 
    // for 32bit?
    unsigned int inst_indx = 0;
    if(ci[0] == 0x66){inst_indx++;}
    
    //printf("Opcode: %02X\n",ci[inst_indx]);
    switch(ci[inst_indx]){
        // -- OUTPUTS --
        // E6: OUT 8bit port, 8bit data, +2 ip.
        case OPCODE_OUT_IMM8_AL:
            handle_out((unsigned int)ci[inst_indx+1],0xFF,eax,0xFF);
            return 2 + inst_indx;        
        // E7: OUT 8bit port, 32bit data, +2 ip.
        case OPCODE_OUT_IMM8_EAX:
            handle_out((unsigned int)ci[inst_indx+1],0xFF,eax,0xFFFFFFFF);
            return 2 + inst_indx;            
        // EE: OUT 16bit port, 8bit data, +1 ip.
        case OPCODE_OUT_DX_AL:
            handle_out(edx,0xFFFF,eax,0xFF);
            return 1;
        // EF: OUT 16bit port, 32bit data, +1 ip.
        case OPCODE_OUT_DX_EAX:
            handle_out(edx,0xFFFF,eax,0xFFFFFFFF);
            return 1;
        // -- INPUTS --
        // E4  IN 8bit port, 8bit data, +2 ip.
        case OPCODE_IN_AL_IMM8:
            handle_in((unsigned int)ci[inst_indx+1],0xFF,xax,0xFF);
            return 2 + inst_indx;
        // E5  IN 8bit port, 32bit data, +2 ip.
        case OPCODE_IN_EAX_IMM8:
            handle_in((unsigned int)ci[inst_indx+1],0xFF,xax,0xFFFFFFFF);
            return 2 + inst_indx;
        // EC: IN 16bit port, 8bit data, +1 ip.
        case OPCODE_IN_AL_DX:
            handle_in(edx,0xFFFF,xax,0xFF);
            return 1;
        // EC: IN 16bit port, 32bit data, +1 ip.
        case OPCODE_IN_EAX_DX:
            handle_in(edx,0xFFFF,xax,0xFFFFFFFF);
            return 1;            
        default:
        printf("Invalid Instruction: %02X\n",ci[0]);
        return 0;
   
    }
    return 0;
}

#ifdef PLATFORM_WINDOWS
#ifdef ENVIRONMENT32
    #define XIP_PTR pExceptionInfo->ContextRecord->Eip
    #define XAX_PTR pExceptionInfo->ContextRecord->Eax
    #define XDX_PTR pExceptionInfo->ContextRecord->Edx  
#else
    #define XIP_PTR pExceptionInfo->ContextRecord->Rip
    #define XAX_PTR pExceptionInfo->ContextRecord->Rax
    #define XDX_PTR pExceptionInfo->ContextRecord->Rdx
#endif    
long pinst_handler(PEXCEPTION_POINTERS pExceptionInfo){
    unsigned char ci[3] = {0x00};
    ci[0] = *(unsigned char*)XIP_PTR;
    ci[1] = *(unsigned char*)(XIP_PTR+1);
    ci[2] = *(unsigned char*)(XIP_PTR+2);

    unsigned char bump_ip = handle_ioport(ci,&XAX_PTR,&XDX_PTR);
    if(!bump_ip){
        return EXCEPTION_EXECUTE_HANDLER;
    }

    XIP_PTR += bump_ip;
    return EXCEPTION_CONTINUE_EXECUTION;
    
}
#else   
#ifdef ENVIRONMENT32
    #define XIP_PTR ctx->uc_mcontext.gregs[REG_EIP]
    #define XAX_PTR ctx->uc_mcontext.gregs[REG_EAX]
    #define XDX_PTR ctx->uc_mcontext.gregs[REG_EDX]
#else
    #define XIP_PTR ctx->uc_mcontext.gregs[REG_RIP]
    #define XAX_PTR ctx->uc_mcontext.gregs[REG_RAX]
    #define XDX_PTR ctx->uc_mcontext.gregs[REG_RDX]
#endif    
void pinst_handler(int signal, siginfo_t *si, void *arg){

    unsigned char ci[3] = {0x00};
    ucontext_t *ctx = (ucontext_t *)arg;

    ci[0] = *(unsigned char*)XIP_PTR;
    ci[1] = *(unsigned char*)(XIP_PTR+1);
    ci[2] = *(unsigned char*)(XIP_PTR+2);

    unsigned char bump_ip = handle_ioport(ci,&XAX_PTR,&XDX_PTR);
    if(!bump_ip){
        printf("Signal %d received\n", signal);
        exit(signal);
    }

    XIP_PTR += bump_ip;
    return;
}
#endif


// Entry-Point
// Register our exception handler.
static void __attribute__((constructor)) ioport_init(void){
    #ifdef _WIN32
        AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)pinst_handler);           
    #else
        struct sigaction sa;
        memset(&sa, 0, sizeof(struct sigaction));
        sigemptyset(&sa.sa_mask);
        sa.sa_sigaction = pinst_handler;
        sa.sa_flags   = SA_SIGINFO;
        sigaction(SIGSEGV, &sa, 0);    
    #endif
}

