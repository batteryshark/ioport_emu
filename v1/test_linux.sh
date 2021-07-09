echo "Testing 64bit Version..."
LD_PRELOAD=./ioport64.so ./test_ioport_u64.elf
echo "Testing 32bit Version..."
LD_PRELOAD=./ioport32.so ./test_ioport_u32.elf
