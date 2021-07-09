#include <stdio.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#if _WIN64
    #define TEST_DLL "ioport64.dll"
#else
    #define TEST_DLL "ioport32.dll"    
#endif
#endif



int main(){
    // Test on Windows by Dynload
    #ifdef PLATFORM_WINDOWS
    printf("Windows - Dynload %s...\n",TEST_DLL);
    LoadLibraryA(TEST_DLL);
    #endif
    unsigned int out_val = 0x1337ABCD;
    unsigned int res = 0;
    // -- Input Tests --
    printf("Testing IN [0xE4] imm8 8bit...\n");
    asm volatile ("in $0x60,%%al":"=a" (res));
    printf("Result: %02X\n",res);
    res = 0;
    
    printf("Testing IN [0xE5] imm8 16bit...\n");
    asm volatile ("in $0x60,%%ax":"=a" (res));
    printf("Result: %04X\n",res & 0xFFFF);
    res = 0;
    
    printf("Testing IN [0xE5] imm8 32bit...\n");
    asm volatile ("in $0x60,%%eax":"=a" (res));
    printf("Result: %04X\n",res);
    res = 0;
    
    printf("Testing IN [0xEC] 16bit port 8bit...\n");
    asm volatile ("in %%dx,%%al":"=a" (res):"d"(0x376));
    printf("Result: %04X\n",res);
    res = 0;
    
    printf("Testing IN [0xED] 16bit port 16bit...\n");
    asm volatile ("in %%dx,%%ax":"=a" (res):"d"(0x376));
    printf("Result: %04X\n",res);
    res = 0;
    
    printf("Testing IN [0xED] 16bit port 32bit...\n");
    asm volatile ("in %%dx,%%eax":"=a" (res):"d"(0x376));
    printf("Result: %04X\n",res);
    res = 0;

    // -- Output Tests --    
    printf("Testing OUT [0xE6] imm8 8bit...\n");
    asm volatile ("out %%al,$0x60"::"a" (0x15));

    printf("Testing OUT [0xE7] imm8 16bit...\n");
    asm volatile ("out %%ax,$0x60"::"a" (0x1516));

    printf("Testing OUT [0xE7] imm8 32bit...\n");
    asm volatile ("out %%eax,$0x60"::"a" (0x15161718));    
    
    printf("Testing OUT [0xEE] 16bit port 8bit...\n");
    asm volatile ("out %%al,%%dx"::"d" (0x376), "a"(0x15));        
    
    printf("Testing OUT [0xEF] 16bit port 16bit...\n");
    asm volatile ("out %%ax,%%dx"::"d" (0x376), "a"(0x1516));    
    
    printf("Testing OUT [0xEF] 16bit port 32bit...\n");
    asm volatile ("out %%eax,%%dx"::"d" (0x376), "a"(0x15161718));
    
    printf("Tests OK!\n");
    return 0;   
}
