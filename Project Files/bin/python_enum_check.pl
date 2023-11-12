#!/usr/bin/perl -w


use strict;
use warnings;

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
	
	open my $info, $file or die "Could not open $file: $!\n";

	my $state = 0;
	my $type = "";

	while( my $line = <$info>)
	{
		if ($state == 0)
		{
			if (index($line, "python::enum_<int> enumTable = python::enum_<int>(\"GlobalDefines\")") != -1)
			{
				$state = 1;
			}
			else
			{
				if ($type ne "")
				{
					if (substr($line, 0, 1) eq "}")
					{
						$type = "";
					}
					else
					{
						next if index($line, "enumTable.value") == -1;
						my $value = substr($line, index($line, "\"")+1);
						$value = substr($value, 0, index($value, "\""));
						$enums{$type}{$value} = 1;
					}
				}
				elsif (substr($line, 0, 40) eq "static void addEnumValues(python::enum_<")
				{
					$type = substr($line, 40);
					$type = substr($type, 0, index($type, ">"));
				}
				
			}
		}
		
		elsif ($state == 1)
		{
			if (index($line, "enumTable.value(") != -1)
			{
				my $key = substr($line, index($line, "\"")+1);
				$key = substr($key, 0, index($key, "\""));
				$enums{GlobalDefines}{$key} = 1;
				next;
			}
			 
			$state = 2 if index($line, "}") == 0;
		}
		elsif ($state == 2)
		{
			$state = 3 if index($line, "CyEnumsPythonInterface()") != -1;
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
			else
			{
				$state = 4 if index($line, "generateEnumValues();") != -1;
			}
		}
		elsif ($state == 4)
		{
			if (index($line, "python::enum_") != -1)
			{
				$type = substr($line, index($line, "\"")+1);
				$type = substr($type, 0, index($type, "\""));
				$enums{$type} = ();
			}
			elsif (index($line, ".value(") != -1)
			{
				my $value = substr($line, index($line, "\"")+1);
				$value = substr($value, 0, index($value, "\""));
				$enums{$type}{$value} = 1;
			}
		}
	}

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
			handleFile($file)
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

		die "$file $enum.$key doesn't exist\n" unless exists $enums{$enum}{$key};
	}
}

sub lineErrorCheck
{
	my $file = shift;
	my $line = shift;
	
	die "$file gc.getDefineINT is no longer allowed. See CyEnumsInterface.cpp for details\n" unless index($line, "gc.getDefineINT") == -1;
}
