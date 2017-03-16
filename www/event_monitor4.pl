#!/usr/bin/perl -w
use IO::Socket;
use Time::localtime;
use Time::HiRes qw(usleep);
use LWP::UserAgent;

# flush after every write
$| = 1;

my $server_port = 5001;
my ($sock, $received_data);
my ($peeraddress,$peerport);
my $hattrick = 0;
my $pushover_ap = "";
my $pushover_user = "";

open FH,"< /home/pi/www/pushover_ap.txt";
$pushover_ap  = <FH>;
close FH;

open FH,"< /home/pi/www/pushover_user.txt";
$pushover_user  = <FH>;
close FH;

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

	if ((-e '/home/pi/www/status_capture') or (-e '/home/pi/www/status_dslr')) {
	
		system("tail -1 /home/pi/www/guiding.txt > /home/pi/www/guiding_last.txt");
	
		open FH,"< /home/pi/www/guiding_last.txt";
		$received_data  = <FH>;
		close FH;
		$var = substr($received_data, 15);
		my @values = split(' ', $var);

		open FH,"< /home/pi/www/ccd_ex.txt";
		$exptime  = <FH>;
		close FH;
		$exptime = $exptime*1;
		
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

			if (-e '/home/pi/www/notify_guiding') {
				LWP::UserAgent->new()->post(
				  "https://api.pushover.net/1/messages.json", [
				  "token" => $pushover_ap,
				  "user" => $pushover_user,
				  "message" => "Guiding errors exceed the tolerance!",
				]);
			}			

			if (($hattrick == 0) and ($exptime >= 30) and (-e '/home/pi/www/hattrick') and (-e '/home/pi/www/status_guiding')) {
				$hattrick = 1;
				system("echo 'cmbCConfShutOpen Close' > /home/pi/www/Commands.txt");
				usleep(300000);
				system("echo btnCConfSetShutOpen > /home/pi/www/Commands.txt");
				usleep(300000);
				$now = ctime();
				$msg = $now . " ... Hat-trick: Shutter Closed.";
				system("echo $msg >> /home/pi/www/alert.txt");
			}
		} else {
			if (($hattrick == 1) and ($exptime >= 30) and (-e '/home/pi/www/hattrick') and (-e '/home/pi/www/status_guiding')) {
				$hattrick = 0;
				system("echo 'cmbCConfShutOpen Open' > /home/pi/www/Commands.txt");
				usleep(300000);
				system("echo btnCConfSetShutOpen > /home/pi/www/Commands.txt");
				usleep(300000);
				$now = ctime();
				$msg = $now . " ... Hat-trick: Shutter Open.";
				system("echo $msg >> /home/pi/www/alert.txt");
			}
		}
	} else {
		$hattrick = 0;
	}
	

#	$data = "received from lin_guider\n";
#	print $sock "$data";
}

$sock->close();
