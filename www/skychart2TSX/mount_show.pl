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

  system("/home/pi/www/skychart2TSX/./getRADEC");
  
  open FH,"< /home/pi/www/skychart2TSX/mount_RADEC.txt";
  
  $RADEC = <FH>;
  chomp($RADEC);
  my @values = split(',', $RADEC);
  $RA = $values[0];
  $RA = $RA*1;
  $DE = $values[1];
  $DE = $DE*1;
  
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
