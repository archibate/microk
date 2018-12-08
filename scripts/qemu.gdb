# vim: ft=gdb ts=2 sts=2 sw=2
# https://github.com/archibate/OS/blob/master/.gdbinit

# Connect to QEMU
target remote localhost:1234

# For bootloader debugging
# change architecture to i386 if debugging STAGE2 32-bit code
#set architecture i8086
set architecture i386
#break *0x7C00
#break *0x10000000
#delete 1

# For both ;)
set disassemble-next-line on
set disassembly-flavor intel

# For kernel debugging
# Add KERNEL file for debugging information
define asf
	add-symbol-file $arg0/bin/$arg0.elf $arg1
end

define asf-k
	asf kernel 0x100000
end

define asf-v
	asf vnix 0xc0000000
end

define asfu
	asf $arg0 0x10000000
end

asf-k
asf-v
b panic
b exp14
b exp13
b do_break_point

set $vregs = ((IF_REGS*)0x400000)
set $wregs = ((IF_REGS*)0x403000)
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

asfu init
c
