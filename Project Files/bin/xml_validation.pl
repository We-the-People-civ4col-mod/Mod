#!/usr/bin/perl -w

# Script to verify that the text xml files are valid
# It doesn't actually understand the xml layout. Instead it looks for <TEXT> and </TEXT>
# Those two has to alternate or it will throw an error. Quite useful as the error without this
# will say error in TEXT, but not which line.
# The error from this script will tell precisely which line and format the error
# to allow MSVC to double click to jump to that line, even if the file isn't mentioned in the project file.
#
# Written by Nightinggale

use strict;
use warnings;

use XML::LibXML;
use lib './bin';
use XMLlists;

my $dir_path = getXMLlocation . "/text/";

opendir(DIR, $dir_path);
my @files = grep(/\.xml$/,readdir(DIR));
closedir(DIR);

foreach my $file (@files)
{
	#print "$file\n";
	my $full_path = $dir_path . $file;
	open my $handle, '<', $full_path;
	chomp(my @lines = <$handle>);
	close $handle;
	
	my $inside = 0;
	
	for my $i (0 .. $#lines) {
		my $line = $lines[$i];
		if (index($line, "<TEXT>") != -1)
		{
			die $full_path . "(" . ($i+1) . ") Starting <TEXT> before ending previous <TEXT>\n" if $inside == 1;
			$inside = 1;
		}
		elsif (index($line, "</TEXT>") != -1)
		{
			die $full_path . "(" . ($i+1) . ") </TEXT> without starting with <TEXT>\n" unless $inside == 1;
			$inside = 0;
		}
	}
}
