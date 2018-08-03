#/usr/bin/perl

my $insum;
my $outsum;
my $incount;
my $outcount;

while(<STDIN>) {
    chomp;
#    $_ =~ /(.*): Photon: (.*) :dRand = (.*); dRand2 = (.*)/;
    $_ =~ /(.*): Photon: (.*) :dRand = (.*); dRand2 = (.*)/;
    if($1 eq "Inner") {
	$incount++;
	$insum+=$3;
    } elsif ($1 eq "Outer") {
	$outcount++;
	$outsum+=$3;
    }
}

print "Average dRand value of outer loop: ";
print $outsum / $outcount;
print "\nAverage dRand value of inner loop: ";
print $insum / $incount;
