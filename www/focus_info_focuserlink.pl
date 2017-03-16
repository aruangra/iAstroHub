#!/usr/bin/perl
use Time::localtime;

my $now = "";
my $msg = "";

open FH,"< /home/pi/www/FC_CUR_POS.txt";
my $pos  = <FH>;
close FH;
chomp $pos;

open FH,"< /home/pi/www/FC_TEMP.txt";
my $temp  = <FH>;
close FH;
chomp $temp;

$now = ctime();
$msg = $now . " ... Position = " . $pos . " ... Temp [C] = " . $temp;
system("echo $msg >> /home/pi/www/focus_info.txt");