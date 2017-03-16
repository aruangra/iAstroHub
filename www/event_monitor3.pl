#!/usr/bin/perl -w
use IO::Socket;
use Time::localtime;

# flush after every write
$| = 1;

my $server_port = 5001;
my ($sock, $received_data);
my ($peeraddress,$peerport);

$sock = new IO::Socket::INET( LocalPort => $server_port, Proto     => "udp")
   or die "Couldn't be a udp server on port $server_port : $@\n";
   
# print "Lin_guider event monitor\n-------------------\nListening...\n";

while( 1 )
{

	my $msg = "";
	my $now = "";
	
	$sock->recv( $received_data, 1024, 0);
  
#	$peer_address = $sock->peerhost();
#	$peer_port = $sock->peerport();

	system("echo $received_data >> /home/pi/www/guiding.txt");
        
	$var = substr($received_data, 15);

  	my @values = split(' ', $var);

	open FH,"< /home/pi/www/TH.txt";
	$TH  = <FH>;
	close FH;
	
	$TH = $TH*1;

        if ($values[0] > $TH) {
			$msg = " X:" . $values[0];
        }

        if ($values[0] < ($TH*-1)) {
			$msg = " X:" . $values[0];
        }

        if ($values[1] > $TH) {
			$msg = $msg . " Y:" . $values[1];
        }

        if ($values[1] < ($TH*-1)) {
			$msg = $msg . " Y:" . $values[1];
        }
		
		if (length($msg) > 1) {
			$now = ctime();
			$msg = $now . " ..." . $msg;
			system("echo $msg >> /home/pi/www/alert.txt");
		}


#	$data = "received from lin_guider\n";
#	print $sock "$data";
}

$sock->close();
