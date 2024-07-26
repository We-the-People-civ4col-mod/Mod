#!/usr/bin/perl -w

#
# Script to run through all source files line by line to check for illegal code.
# Examples of this is calling a function in the exe where we added the same function in the DLL to avoid an exe limitation.
# This is intended to avoid introducing new bugs, which are easy to add by mistake and possibly hard or time consuming to fix later.
#


use strict;
use warnings;
use File::Slurp;

my $dir = "DLLSources";

processdir($dir);

open my $handle, '<', "Makefile.project" or die "Failed to open file Makefile.project";
chomp(my @lines = <$handle>);
close $handle;

foreach my $line (@lines)
{
	next unless substr($line, 0, 6) eq "SUBDIR";
	$line = substr($line, index($line, "=")+1);
	$line=~ s/^\s+|\s+$//g;
	processdir($dir . "/" . $line);
}

#processdir("DLLSources/Types");

exit();


sub processdir
{
	my $dir = shift;
	
	opendir(DIR, $dir) or die $!;

	while (my $file = readdir(DIR))
	{
		next unless (-f "$dir/$file");
		next unless ($file =~ m/\.h$/) or ($file =~ m/\.cpp$/) or ($file =~ m/\.def$/);
		processfile($file, "$dir/$file");
	}
	closedir(DIR);
}

sub processfile
{
	my $file = shift;
	my $path = shift;
	
	my $text = read_file($path);
	
	$text = substr($text, -2);
	
	
	
	open my $handle, '<', $path or die "Failed to open file $file";
	chomp(my @lines = <$handle>);
	close $handle;
	
	# check for files not ending with a newline
	# useful as git will mark the last line as modified unless the file ends with a blank line
	die  $path . "(" . (scalar @lines) . ") No end of line at end of file\n" unless $text eq "\r\n" || substr($text, 1) eq "\n";
	
	# loop through all lines to check for errors
	my $i = 0;
	for my $line (@lines)
	{
		
		die $path . "(" . ($i+1) . ") gDLL->getInterfaceIFace->getSymbolID is broken. Use GC.getSymbolID instead.\n" if index($lines[$i], "gDLL->getInterfaceIFace()->getSymbolID") != -1;
		die $path . "(" . ($i+1) . ") gDLL->getSymbolID is broken. Use GC.getSymbolID instead.\n" if index($lines[$i], "gDLL->getSymbolID") != -1;
		$i += 1;
	}
}
