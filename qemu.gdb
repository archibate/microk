# vim: ft=gdb ts=2 sts=2 sw=2
# https://github.com/archibate/OS/blob/master/.gdbinit

# Connect to QEMU
target remote localhost:1234

# For bootloader debugging
# change architecture to i386 if debugging STAGE2 32-bit code
#set architecture i8086
set architecture i386
#break *0x7C00

# For kernel debugging
# Add KERNEL file for debugging information
add-symbol-file kernel/bin/kernel.elf 0x100000
add-symbol-file init/bin/init.elf 0x10000000

# For both ;)
set disassemble-next-line on
set disassembly-flavor intel

set $vregs = ((IF_REGS*)0x400000)
set $vpt   = ((ulong*)0xffc00000)
set $vpd   = ((ulong*)0xfffff000)

define qq
	kill
	quit
end

define bc
	break $arg0
	continue
end

define xs
	x/6wx $esp
end

define xsi
	x/10i *(void**)($esp+4)
end

b panic
b exp14
b exp13
b do_break_point
bc syscall
#bc do_sendtx
##bc __int_return
