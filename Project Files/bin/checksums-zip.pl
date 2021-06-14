#!/usr/bin/perl -w

# Generate MD5 and sha1 checksums of all zip files in Project Files
# Used for releases as some people prefer to know the checksums to verify they have downloaded the correct file.
#
# Written by Nightinggale

use strict;
use warnings;

use Digest::file qw(digest_file_hex);
#use Digest::SHA1  qw(sha1 sha1_hex sha1_base64);


use Digest::SHA;


my $dir_path = "./";

opendir(DIR, $dir_path);
my @files = grep(/\.zip$/,readdir(DIR));
closedir(DIR);

foreach my $file (@files)
{
	print "Checksums $file\n";
	print "DM5  ", digest_file_hex($file, "MD5"), "\n";
	
	print "sha1 ";
	my $sha = Digest::SHA->new(1);
	$sha->addfile($file);
	print $sha->hexdigest;
	print "\n";
}
