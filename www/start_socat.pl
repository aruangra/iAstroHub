#!/usr/bin/perl 

my $IP = "";

open FH,"< /home/pi/www/IP_TSX.txt";
my $IP  = <FH>;
close FH;

system("socat pty,link=/dev/netcom0,raw  tcp:$IP:3040 &");
