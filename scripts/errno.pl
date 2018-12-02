#!/usr/bin/env perl

open I, '<scripts/errno.txt' or die;
open H, '>libc/errno.h' or die;
open M, '>libc/strerr.c' or die;
open MH, '>libc/strerr.h' or die;

print H qq[#pragma once\n\n];

print M <<EOF
static
const char *err_msgs[] = {
	"success",
EOF
;

print MH <<EOF
#pragma once

const char *strerror(int err);
EOF
;


my $i = 0;
while (<I>)
{
	$i++;

	my ($err, $msg) = map {
		s/^\s+//;
		s/\s+$//;
		$_;
	} split /:/;

	print H qq[#define $err\t$i\t// $msg\n];
	print M qq[\t"$msg",\n];
}

print M <<EOF
};

const char *strerror(int err)
{
	if (err < 0)
		err = -err;
	if (err >= sizeof(err_msgs))
		return "error";
	return err_msgs[err];
}
EOF
;
