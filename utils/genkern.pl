#!/usr/bin/perl

open(SIG, $ARGV[0]) || die "open $ARGV[0]: $!";

$n = sysread(SIG, $buf, 130561);

if($n > 130560){
  print STDERR "ERROR: Kernel too large: $n bytes (max 130560)\n";
  exit 1;
}

print "OK: Kernel is $n bytes - Extended to 255 sectors\n";

$buf .= "\0" x (130560-$n);

open(SIG, ">$ARGV[1]") || die "open >$ARGV[1]: $!";
print SIG $buf;
close SIG;
