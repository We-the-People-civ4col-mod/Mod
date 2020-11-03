#!/usr/bin/perl -w

#
# Copy first argument into second argument
# Skips copying if the two files are identical (no overwrite with same data
# Adding a third argument will enable output to STDOUT (doesn't matter what the third argument is)
#

use strict;
use warnings;

use File::Compare;
use File::Copy;



die "Needs to be called with source destination arguments\n" unless scalar @ARGV >= 2;
my $verbose = 0;
$verbose = 1 if scalar @ARGV > 2;

my $source = $ARGV[0];
my $destination = $ARGV[1];

die "Source file missing\n" unless -e $source;

my $name = substr($source, rindex($source, "\\")+1);

$destination = substr($destination, 0, -1) . $name if substr($destination, -2) eq "\\.";

if (not -e $destination or compare($destination, $source) != 0)
{
	print "Copying " . $name . "\n" if $verbose;
	copy($source, $destination);
}
else
{
	print "No need to update " . $name . "\n" if $verbose;
}
