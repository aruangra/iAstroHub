#!/usr/bin/perl 
use Time::localtime;

my $offset = 0;

my $now = "";
my $msg = "";

open FH,"< /home/pi/www/current_pos.txt";
my $pos  = <FH>;
close FH;
	
open FH,"< /home/pi/www/current_temp.txt";
my $temp  = <FH>;
$temp = $temp*1/2-273-$offset;
close FH;

$now = ctime();
$msg = $now . " ... Position = " . $pos . " ... Temp [C] = " . $temp;
system("echo $msg >> /home/pi/www/focus_info.txt");

	