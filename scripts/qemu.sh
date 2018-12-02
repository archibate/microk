set -e
flgs="$QEMUFLAGS"
flgs="$flgs -m 256"
flgs="$flgs -boot a -fda bin/a.img" # -soundhw all
QEMU=${QEMU-qemu-system-i386}
GDB=${GDB-cgdb}

if [[ "x$1" = "x-d" ]]; then
	$QEMU $flgs -S -gdb tcp::1234 &
	$GDB -x qemu.gdb
elif [[ "x$1" = "x-t" ]]; then
	$QEMU $flgs -chardev file,id=char0,path=/tmp/qemuout$$ -serial chardev:char0 &
	sleep 2.0
	kill %1
	wait %1 || true 2>/dev/null
	sed '/iPXE/d' /tmp/qemuout$$ | sed '/Seti/d' | awk -F : -f scripts/avgtime.awk >> /tmp/l4benchsamps
	rm -f /tmp/qemuout$$
else
	$QEMU $flgs -serial stdio
fi
