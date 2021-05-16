#!/usr/bin/perl -w

#
# Script to change the version number within the MSVS project file
# This way we only need to maintain one file, which helps because the file lists all source files.
# Using the correct version of the project file unlocks Intellisense, which is very useful.
# Look it up if you don't know it, but it's like spellcheck for C++, click on funcion call to jump to function etc.
#

use strict;
use warnings;

my $version   = 0;

if (scalar @ARGV == 1)
{
	$version = $ARGV[0];
}

if ($version == 2010)
{
}
elsif ($version == 2017)
{
}
else
{
	die "Argument needs to set year for target version\n";
}


my $dir = './';

opendir(DIR, $dir) or die $!;

while (my $file = readdir(DIR))
{
	next unless (-f "$dir/$file");

	next unless ($file =~ m/\.vcxproj$/);

	handleFile($file);
}

closedir(DIR);


sub handleFile
{
	my $file = shift;
	
	open my $handle, '<', $file;
	chomp(my @lines = <$handle>);
	close $handle;
	
	# change the tools version
	my @versionLine = split "\"", $lines[1];
	
	# bail out if the version is already correct
	return if $versionLine[3] eq "4.0" and $version == 2010;
	return if $versionLine[3] eq "15.0" and $version == 2017;
	
	$versionLine[3] = "4.0" if $version == 2010;
	$versionLine[3] = "15.0" if $version == 2017;
	
	$lines[1] = join "\"", @versionLine;
	
	@lines = to2010(\@lines) if $version == 2010;
	@lines = to2017(\@lines) if $version == 2017;
	
	open (my $output, "> " . $file) or die "Can't open file " . $file . "\n" . $!;
	
	print $output join("\n", @lines);
	close $output;
}

sub to2010
{
	my $arrayref = $_[0];
	my @lines = @$arrayref;
	
	my @output = ();
	
	for my $line (@lines)
	{
		next if index($line, "<WindowsTargetPlatformVersion>") != -1;
		next if index($line, "<PlatformToolset>") != -1;
		
		push @output, $line;
	}
	return @output;
}

sub to2017
{
	my $arrayref = $_[0];
	my @lines = @$arrayref;
	
	my @output = ();
	
	my $added_line = "    <WindowsTargetPlatformVersion>10.0.17763.0</WindowsTargetPlatformVersion>";
	
	my $active = 1;
	
	for my $line (@lines)
	{
		if ($active == 1 and index($line, "</PropertyGroup>") != -1)
		{
			push (@output, $added_line);
			$added_line = "    <PlatformToolset>v141</PlatformToolset>";
			
		}
		$active = 0 if index($line, "</ImportGroup>") != -1;
		
		push @output, $line;
	}
	return @output;
	
}