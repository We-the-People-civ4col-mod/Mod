#!/usr/bin/perl -w

#
# Script to configure MSVC to start Colonization with the correct mod when asked to start the game from the debug menu
# Starts with debugger attached if requested by MSVC
#
# Note: if the exe can't be automatically found, the script will use the first argument given as the install dir path.
#       This way it should have a fallback in case the automation doesn't work.
#

use strict;
use warnings;

use Cwd;


chdir("..") if -e "setup_debugger.pl";

my $script = getcwd;

my @path_tokens = split "/", $script;
my $mod = $path_tokens[-2];
my $path = $script;

if (scalar @ARGV >= 1)
{
	$path = $ARGV[0];
}
else
{
	for (1..3)
	{
		$path = substr($path, 0, rindex($path, "/"));
	}
}

$path .= "/";
my $exe = $path . "Colonization.exe";

print "\nFound mod: $mod\n";

die "Failed to find Colonization.exe\nAutodetection only works if it is executed from the MODS folder in the install dir.\nYou can set the install dir as the first argument when calling the script.\n\n" unless -e $exe;

$path =~ tr/\//\\/;
$exe =~ tr/\//\\/;

my @files = <*.vcxproj>;
foreach my $file (@files)
{
	print "Generating $file.user\n";
	open my $input, $file or die "Could not open $file: $!";

	open(my $output, '>', $file . ".user");

	while( my $line = <$input>)
	{
		writeTop($output, $line) if index($line, "http://schemas.microsoft.com/developer/msbuild/2003") != -1;
		writeMiddle($output, $line) if index($line, "<ProjectConfiguration Include=\"") != -1;
	}

	writeEnd($output);
	close $output;
	close $input;
}

print "\nDone\n\n";

sub writeTop
{
	my $output = shift;
	my $line = shift;
	
	my @tokens = split "\"", $line;
	my $version = $tokens[3];
	print $output "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	print $output "<Project ToolsVersion=\"$version\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";
}

sub writeMiddle
{
	my $output = shift;
	my $line = shift;
	
	my $target = substr($line, index($line, "\"") + 1);
	$target = substr($target, 0, index($target, "\""));
	
	print $output "  <PropertyGroup Condition=\"\'\$(Configuration)|\$(Platform)\'==\'$target\'\">\n";
	print $output "    <LocalDebuggerCommand>$exe</LocalDebuggerCommand>\n";
	print $output "    <LocalDebuggerCommandArguments>mod=\"$mod\"</LocalDebuggerCommandArguments>\n";
	print $output "    <LocalDebuggerWorkingDirectory>$path</LocalDebuggerWorkingDirectory>\n";
	print $output "    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>\n";
	print $output "  </PropertyGroup>\n";
}

sub writeEnd
{
	my $output = shift;
	
	print $output "</Project>";
}
