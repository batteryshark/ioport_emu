# Compile Libraries
echo "Compile Libraries..."
cc -w -shared -D_GNU_SOURCE ./ioport.c -fPIC -m32 -o ioport32.so
cc -w -shared -D_GNU_SOURCE ./ioport.c -fPIC -o ioport64.so
i686-w64-mingw32-gcc   -w -shared ./ioport.c -o ./ioport32.dll
x86_64-w64-mingw32-gcc -w -shared ./ioport.c -o ./ioport64.dll
echo "Compile Tests..."
cc -w ./test_ioport.c -m32 -o ./test_ioport_u32.elf
cc -w ./test_ioport.c -o ./test_ioport_u64.elf
i686-w64-mingw32-gcc   -DPLATFORM_WINDOWS -w ./test_ioport.c -o ./test_ioport_w32.exe
x86_64-w64-mingw32-gcc -DPLATFORM_WINDOWS -w ./test_ioport.c -o ./test_ioport_w64.exe
echo "Done!"


