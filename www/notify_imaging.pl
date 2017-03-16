use LWP::UserAgent;

my $pushover_ap = "";
my $pushover_user = "";

open FH,"< /home/pi/www/pushover_ap.txt";
$pushover_ap  = <FH>;
close FH;

open FH,"< /home/pi/www/pushover_user.txt";
$pushover_user  = <FH>;
close FH;

LWP::UserAgent->new()->post(
  "https://api.pushover.net/1/messages.json", [
  "token" => $pushover_ap,
  "user" => $pushover_user,
  "message" => "Imaging sequence was completed.",
]);
