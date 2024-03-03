#!/usr/bin/perl

open(SIG, $ARGV[0]) || die "open $ARGV[0]: $!";

$n = sysread(SIG, $buf, 30000);

if($n > 512 *20){
	print STDERR "ERROR: App too large: $n bytes (max 10240)\n";
	exit 1;
}

print STDERR "OK: App block is $n bytes (max 512 * 20)\n";

$buf .= "\0" x (10240-$n);

open(SIG, ">$ARGV[0]") || die "open >$ARGV[0]: $!";
print SIG $buf;
close SIG;
