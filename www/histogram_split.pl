#!/usr/bin/perl

open FH,"< /home/pi/www/histogram.txt";
my $header = <FH>;

open (HX, '> /home/pi/www/histogram_x.txt');
open (HY, '> /home/pi/www/histogram_y.txt');

print HX "0, ";
print HY "0, ";

for (my $i=0; $i <= 255; $i++) {
    my $ADU = <FH>;
	my @values = split(':',$ADU);
	chomp($values[1]);
	print HX ($values[0]+1)*256-1 . ", ";
	print HY $values[1] . ", ";
}

close (HY);
close (HX);
close (FH);
