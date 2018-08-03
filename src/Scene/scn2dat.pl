#/usr/bin/perl
#this is the perl preprocessor for scene files.
#it converts scn files to dat files

use strict;

my $custom;

while( <STDIN> ) {
    chomp;
    if($_ =~ /\#(.*)) {
	if($1 =~ /custom (.*)/) {
	    $custom = $1;
	}
    } else {
	print $_;
    }
}
