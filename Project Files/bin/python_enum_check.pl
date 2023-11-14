#!/usr/bin/perl -w


use strict;
use warnings;
use threads;
use Thread::Queue;

use constant STATE_1_INNER => 10;
use constant STATE_3_INNER => 11;

my $nthreads = 8;

if ($^O eq 'MSWin32')
{
	$nthreads = $ENV{"NUMBER_OF_PROCESSORS"};
}
else
{
	# linux only
	# TODO: make a fallback, which doesn't cause errors while using Strawberry Perl
	#use Sys::Info;
	#use Sys::Info::Constants qw( :device_cpu );
	#my $info = Sys::Info->new;
	#my $cpu  = $info->device( CPU => %options );
	#$nthreads = $cpu->ht || $cpu->count || 1;
}

print "Testing python files using $nthreads threads\n";

my $process_q = Thread::Queue -> new();
my $error_q = Thread::Queue -> new();

use File::Slurp;

use XML::LibXML;
use lib './bin';
use XMLlists;

my %headerCheck;
my %enums;
$enums{GlobalDefines} = ();

handleCyEnumInterface();
readHeader(getSourceDir() . "CvEnums.h");
readHeader(getAutoDir() . "AutoXmlEnum.h");
readHeader(getAutoDir() . "AutoGlobalDefineEnum.h");

testPythonFiles();

#start some threads
for ( 1..$nthreads )
{
	threads->create( \&worker );
}

#Wait for threads to all finish processing.
foreach my $thr ( threads->list() )
{
	$thr->join();
}
$error_q->end();

my $errorStr = "";
while ( my $error = $error_q -> dequeue() )
{
	$errorStr .= $error;
}

die $errorStr unless $errorStr eq "";

exit();

for my $enum (sort keys %enums)
{
	foreach my $type (keys %{$enums{$enum}})
	{
		print $enum;
		print ".";
		print $type;
		print "---\n";
	}
}

sub handleCyEnumInterface
{
	my $file = getSourceDir() . "CyEnumsInterface.cpp";
	
	my $lineno = 1;
	
	open my $info, $file or die "Could not open $file: $!\n";

	my $state = 0;
	my $type = "";

	while( my $line = <$info>)
	{
		if ($state <= 7 and substr($line, 0, 34) eq "// python_enum_check.pl NEXT STATE")
		{
			die "$file($.) File contains more python_enum_check.pl NEXT STATE than script expected\n" if $state > 5;
			$lineno = $.;
			$state += 1;
			next;
		}
		
		if ($state == 1)
		{
			if (index($line, "void EnumContainer<") != -1)
			{
				$state = STATE_1_INNER;
				$type = substr($line, index($line, "<")+1);
				$type = substr($type, 0, index($type, ">"));
				$type = "GlobalDefines" if $type eq "int";
			}
		}
		elsif ($state == STATE_1_INNER)
		{
			if (substr($line, 0, 1) eq "}")
			{
				$state = 1;
				next;
			}
			if (index($line, "enumTable.value(") != -1)
			{
				my $key = substr($line, index($line, "\"")+1);
				$key = substr($key, 0, index($key, "\""));
				$enums{$type}{$key} = 1;
				next;
			}
		}
		elsif ($state == 3)
		{
			if (index($line, "addEnumValues") != -1)
			{
				my $types = substr($line, index($line, "\"")+1);
				$types = substr($types, 0, index($types, "\""));
				my $max = substr($line, 0, rindex($line, "\""));
				$max = substr($max, rindex($max, "\"")+1);
				$headerCheck{$types} = $max;
			}
		}
		elsif ($state == 5)
		{
			if (index($line, "python::enum_") != -1)
			{
				$type = substr($line, index($line, "\"")+1);
				$type = substr($type, 0, index($type, "\""));
				$enums{$type} = ();
				$state = STATE_3_INNER;
			}
		}
		elsif ($state == STATE_3_INNER)
		{
			if (index($line, ";") != -1)
			{
				$state = 5;
				next;
			}
			if (index($line, ".value(") != -1)
			{
				my $value = substr($line, index($line, "\"")+1);
				$value = substr($value, 0, index($value, "\""));
				$enums{$type}{$value} = 1;
			}
		}
	}

	die "$file($lineno) Didn't encounter 6 python_enum_check.pl NEXT STATE\n" unless $state == 6;

	close $info;
}

sub readHeader
{
	my $file = shift;
	
	my $enum = "";
	my $max = "";
	
	open my $info, $file or die "Could not open $file: $!\n";

	while( my $line = <$info>)
	{
		if ($enum eq "")
		{
			if (my $index = index($line, "enum ") != -1)
			{
				my $tmp = substr($line, $index+4);
				chomp($tmp);
				if (exists $headerCheck{$tmp})
				{
					$enum = $tmp;
					$max = $headerCheck{$tmp};
				}
			}
		}
		else
		{
			my $index = index($line, ",");
			next if $index == -1;
			my $key = substr($line, 0, $index);
			$index = index($key, "=");
			$key = substr($line, 0, $index) if $index != -1;
			$key=~ s/^\s+|\s+$//g;
			$enums{$enum}{$key} = 1;
			$enum = "" if $key eq $max;
		}
	}
	close $info;
}

sub testPythonFiles
{
	handleDir(getPythonDir());
	handleDir(getMapDir());
	$process_q->end();
}

sub handleDir
{
	my $dir = shift;
	
	opendir DIR,$dir;
	my @dir = readdir(DIR);
	close DIR;
	foreach(@dir)
	{
		next if substr($_, 0, 1) eq ".";
		
		my $file = "$dir$_";
		if (-f $file)
		{
			next if $_ eq "CvMainInterface_Backup.py";
			next if $_ eq "CvMainInterface_Backup_Terrain_Backgrounds.py";
			next unless substr($file, -3) eq ".py";
			$process_q->enqueue($file);
		}
		elsif (-d $file)
		{
			handleDir($file . "/");
		}
	}
}

sub handleFile
{
	my $file = shift;
	
	my $commentSection = 0;
	
	open my $info, $file or die "Could not open $file: $!\n";
	while( my $line = <$info>)
	{
		chomp($line);
		my $comment = index($line, "#");
		$line = substr($line, 0, $comment) if $comment != -1;
		if (index($line, "\"\"\"") != -1)
		{
			$commentSection = not $commentSection;
		}
		next if $commentSection;
		
		lineErrorCheck("$file($.)", $line);
		for my $enum (sort keys %enums)
		{
			handleLine("$file($.)", $line, $enum);
		}
	}
	close $info;
}

sub handleLine
{
	my $file = shift;
	my $line = shift;
	my $enum = shift;
	
	
	my $match = $enum . ".";
	
	while ((my $index = index($line, $match)) != -1)
	{
		if ($enum eq "BuildingTypes" or $enum eq "UnitTypes")
		{
			if (substr($line, $index-7, 7) eq "Special")
			{
				# avoid UnitTypes triggering on SpecialUnitTypes
				$line = substr($line, $index+1);
				next;
			}
		}
		if ($enum eq "EventTypes")
		{
			if (substr($line, $index-5, 5) eq "Diplo")
			{
				# avoid EventTypes triggering on DiploEventTypes
				$line = substr($line, $index+1);
				next;
			}
		}
		if ($enum eq "DirectionTypes")
		{
			if (substr($line, $index-8, 8) eq "Cardinal")
			{
				# avoid DirectionTypes triggering on CardinalDirectionTypes
				$line = substr($line, $index+1);
				next;
			}
		}
		
		$line = substr($line, $index+1);
		
		my $key = substr($line, index($line, ".")+1);
		for my $char (" ", ")", ",", ":", "]", "\t")
		{
			$index = index($key, $char);
			$key = substr($key, 0, $index) if ($index != -1);
			last if exists $enums{$enum}{$key};
		}

		$error_q->enqueue("$file $enum.$key doesn't exist\n") unless exists $enums{$enum}{$key};
	}
}

sub lineErrorCheck
{
	my $file = shift;
	my $line = shift;
	
	$error_q->enqueue("$file gc.getDefineINT is no longer allowed. See CyEnumsInterface.cpp for details\n") unless index($line, "gc.getDefineINT") == -1;
	return; # disable checks for getInfoTypeForString until all prior existing cases have been converted
	if (index($line, "gc.getInfoTypeForString") != -1)
	{
		# only trigger on gc.getInfoTypeForString + " as there are a few valid use cases for variables rather than hardcoded strings.
		# an example of a valid use case is passing scenario files where the code looks up whatever string was read from the file.
		$error_q->enqueue("$file gc.getInfoTypeForString is no longer allowed. See CyEnumsInterface.cpp for details\n") unless index($line, "\"") == -1;
	}
}

# subroutine to handle each thread
# goal is to take one file off the queue and handle it
# kill thread when queue is empty
sub worker
{
	while (my $file = $process_q->dequeue())
	{
		handleFile($file);
	}
}

