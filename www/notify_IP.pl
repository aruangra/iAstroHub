use LWP::UserAgent;
use IO::Socket::INET;

my $pushover_ap = "";
my $pushover_user = "";
my $local_ip_address = get_local_ip_address();

open FH,"< /home/pi/www/pushover_ap.txt";
$pushover_ap  = <FH>;
close FH;

open FH,"< /home/pi/www/pushover_user.txt";
$pushover_user  = <FH>;
close FH;

print "$local_ip_address\n";

LWP::UserAgent->new()->post(
  "https://api.pushover.net/1/messages.json", [
  "token" => $pushover_ap,
  "user" => $pushover_user,
  "message" => $local_ip_address,
]);

sub get_local_ip_address {
    my $socket = IO::Socket::INET->new(
        Proto       => 'udp',
        PeerAddr    => '198.41.0.4', # a.root-servers.net
        PeerPort    => '53', # DNS
    );

    # A side-effect of making a socket connection is that our IP address
    # is available from the 'sockhost' method
    my $local_ip_address = $socket->sockhost;

    return $local_ip_address;
}
