#!/usr/bin/perl -w

# Script to verify that the text xml files are valid
# It doesn't actually understand the xml layout. Instead it looks for <TEXT> and </TEXT>
# Those two has to alternate or it will throw an error. Quite useful as the error without this
# will say error in TEXT, but not which line.
# The error from this script will tell precisely which line and format the error
# to allow MSVC to double click to jump to that line, even if the file isn't mentioned in the project file.
#
# Also checks if there are issues with arguments (like %d1)
#
# Written by Nightinggale

use strict;
use warnings;

use XML::LibXML;
use lib './bin';
use XMLlists;

my $dir_path;
my @files;
my %strings;
my %ignored_strings;

# remove checking arguments for the following TXT_KEYs
$ignored_strings{"TXT_KEY_UNIT_TURNS"} = 1;
$ignored_strings{"TXT_KEY_BUILDING_IN_QUEUE"} = 1;
$ignored_strings{"TXT_KEY_FORTBOMB_HIT_LAND_GOOD"} = 1;
$ignored_strings{"TXT_KEY_FORTBOMB_HIT_LAND_BAD"} = 1;
$ignored_strings{"TXT_KEY_FORTDEFENSE_HIT_LAND_GOOD"} = 1;
$ignored_strings{"TXT_KEY_FORTDEFENSE_HIT_LAND_BAD"} = 1;
$ignored_strings{"TXT_KEY_DOMESTIC_MARKET_BUILDING_INCREASED"} = 1;
$ignored_strings{"TXT_KEY_COMPETITIOR_INDEPENDENCE_WAR"} = 1;
$ignored_strings{"TXT_KEY_MISC_CTRL_SHIFT"} = 1;
$ignored_strings{"TXT_KEY_ERROR_XML_WRONG_TYPE_STRING"} = 1;
$ignored_strings{"TXT_KEY_EVENT_TRIGGER_ERROR"} = 1;
$ignored_strings{"TXT_KEY_EUROPE_WAR_KING_DECREASED_MAX_TAX"} = 1; # french not updated

$dir_path = "DLLsources/";

opendir(DIR, $dir_path);
@files = grep(/\.cpp$/,readdir(DIR));
closedir(DIR);

foreach my $file (@files)
{
	my $full_path = $dir_path . $file;
	open my $handle, '<', $full_path;
	chomp(my @lines = <$handle>);
	close $handle;
	
	foreach my $line (@lines)
	{
		processCPP($line);
		#die $line . "\n" unless $index == -1;
	}
}




# loop through text xml files

$dir_path = getXMLlocation . "/text/";

opendir(DIR, $dir_path);
@files = grep(/\.xml$/,readdir(DIR));
closedir(DIR);

foreach my $file (@files)
{
	#print "$file\n";
	my $full_path = $dir_path . $file;
	open my $handle, '<', $full_path;
	chomp(my @lines = <$handle>);
	close $handle;
	
	my $inside = 0;
	
	my $key = "";
	
	for my $i (0 .. $#lines)
	{
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
		elsif ($inside)
		{
			if (index($line, "<Tag>") != -1)
			{
				$key = substr($line, index($line, ">") + 1);
				$key = substr($key, 0, index($key, "<"));
				$key = "" unless exists $strings{$key};
			}
			elsif ($key ne "")
			{
				#die $full_path . "(" . ($i+1) . ")" unless 0;
				testString($key, $line, $full_path, $i+1);
			}
		}
	}
}

sub testString
{
	my $key = shift;
	my $line = shift;
	my $full_path = shift;
	my $lineno = shift;
	
	$line = substr($line, index($line, ">")+1);
	$line = substr($line, 0, index($line, "<"));
	
	my $str = $line;
	my %found;
	
	while (1)
	{
		my $index = index($str, "%");
		last if $index == -1;
		$str = substr($str, $index+1);
		
		my $next = substr($str, 0, 1);
		if ($next eq "%")
		{
			# two % in a row. Skip. It prints %
			$str = substr($str, 1);
		}
		elsif ($next eq "d" or $next eq "D" or $next eq "s" or $next eq "S" or $next eq "F")
		{
			#push(@found, substr($str, 1, 1));
			$found{substr($str, 1, 1)} = 1;
		}
	}
	
	my @req;
	for my $i (0 .. 10)
	{
		last unless exists $strings{$key}[$i];
		push @req, $strings{$key}[$i];
	}
	
	for my $i (0 .. $#req)
	{
		if ($req[$i] == 1)
		{
			unless (exists $found{$i+1})
			{
				die $full_path . "(" . ($lineno) . ") String lacks mandatory argument " . ($i + 1) . "\n" unless index($line, "[NUM:" . ($i + 1)) != -1;
			}
		}
	}
}

sub processCPP
{
	my $line = shift;
	
	my $index = index($line, "gDLL->getText");
	if ($index != -1)
	{
		$line = substr($line, $index);
		$line = substr($line, index($line, "(",)+1);
		my @arguments = getArguments($line);
		if (substr($arguments[0], 0, 8) eq "\"TXT_KEY")
		{
			my $found_mandatory = 0;
			my $key = substr($arguments[0], 1, -1);
			shift @arguments;
			my @req;
			foreach my $arg (@arguments)
			{
				if ($arg eq "\"\"" or $arg eq "0") # or index($arg, "getShortDescriptionKey") != -1 or index($arg, "getTextKeyWide") != -1)
				{
					push(@req, 0);
				}
				else
				{
					if (index($arg, "get") != -1)
					{
						#if (index($arg, "DescriptionKey") != -1 or index($arg, "getTextKeyWide") != -1 or index($arg, "NameKey") != -1 or index($arg, "NameKey") != -1)
						if (substr($arg, -3) eq "Key" or substr($arg, -7) eq "KeyWide")
						{
							push(@req, 0);
							next;
						}
					}
					$found_mandatory = 1;
					push(@req, 1);
				}
			}
			$strings{$key} = \@req if $found_mandatory and not exists $ignored_strings{$key};
		}
		# call recursive in case there are more than one call in the same line
		processCPP($line);
	}
}

sub getArguments
{
	my $line = shift;
	
	# convert the line to 
	my @arguments = split ",", clearCalls($line);
	
	# return without leading and ending whitespaces
	return grep(s/\s*$//g, @arguments);;
}

sub clearCalls
{
	my $line = shift;
	
	my $left  = index $line, "(";
	my $right = index $line, ")";
	
	if ($left == 0)
	{
		$line = substr($line, 1);
		$left  = index $line, "(";
		$right = index $line, ")";
		while ($left != -1 and $left < $right)
		{
			$line = clearCalls(substr($line, $left));
			$left  = index $line, "(";
			$right = index $line, ")";
		}
		return substr($line, $right+1);
	}
	else
	{
		if ($left == -1 or $left > $right)
		{
			return substr($line, 0, $right);
		}
		
		return clearCalls(substr($line, 0, $left) . clearCalls(substr($line, $left)));
	}
}
