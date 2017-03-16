#!/usr/bin/perl -w
use IO::Socket;
use Cwd;

#
# example program to send sequential command to the program
#

$host = "127.0.0.1";
$port = "3292";
$eol = "\x0D\x0A";
$path = cwd;

  connectCDC();

  system("/home/pi/www/skychart/./getRADEC");
  
  open FH,"< /home/pi/www/skychart/mount_RA.txt";
  $RA_h = <FH>;
  $RA_h = $RA_h*1;
  $RA_m = <FH>;
  $RA_m = $RA_m*1;
  $RA_s = <FH>;
  $RA_s = $RA_s*1;
  $RA = ((($RA_s/60)+$RA_m)/60)+$RA_h;

  open FH,"< /home/pi/www/skychart/mount_DEC.txt";
  $DE_sign = <FH>;
  $DE_sign = $DE_sign*1;
  $DE_d = <FH>;
  $DE_d = $DE_d*1;
  $DE_m = <FH>;
  $DE_m = $DE_m*1;
  $DE_s = <FH>;
  $DE_s = $DE_s*1;
  $DE=(((($DE_s/60)+$DE_m)/60)+$DE_d)*$DE_sign;
  
 sendcmd("SETRA " . $RA);
 sendcmd("SETDEC " . $DE);

 sendcmd("redraw");
  


sub sendcmd {
  my $cmd = shift;
  print STDOUT " Send CMD : $cmd \n";
  print $handle $cmd.$eol;                       # send command

  $line = <$handle>;
  if ($line =~ /$client/) {       # click form our client
     print STDOUT $line;
     }
  while (($line =~/^\.\r\n$/) or ($line =~ /^>/)) # keepalive and click on the chart
    {
     $line = <$handle>;
     if ($line =~ /$client/) {       # click form our client
        print STDOUT $line;
        }
    }
  # we go here after receiving response from our command
  if (($line =~ /^OK!/) or ($line =~ /^Bye!/) )
     {
     print STDOUT "Command success\n";
     }
  else {
     print STDOUT "$line";
     }
  
}


sub connectCDC {

# do the connection
$handle = IO::Socket::INET->new(Proto     => "tcp",
                                PeerAddr  => $host,
                                PeerPort  => $port)
          or die "cannot connect to Cartes du Ciel at $host port $port : $!";

$handle->autoflush(1);

print STDOUT "[Connected to $host:$port]\n";

# wait connection and get client chart name
  $line = <$handle>;
  print STDOUT $line;
  $line =~ /OK! id=(.*) chart=(.*)$/;
  $client = $1;
  $chart = $2;
  chop $chart;
  if ($client)
    {
     print STDOUT " We are connected as client $client , the active chart is $chart\n";
    }
    else { die " We are not connected \n"};
}
