#!/usr/bin/perl -w

#
# Run through all header files and make sure that all DllExport lines are unchanged.
# Also verify that no extra DllExport has shown up.
# As long as none of them change, the EXE<->DLL interface should remain stable.
#
# Adding default values to arguments is allowed. If done so, then update DllExport.txt accordingly.
# The reason why it works is that the function remain unchanged. It still expects the arguments to be in certain registers.
# However the calling code will place the default value in the registers in question while making the call.
# This means calls from the exe will still place the data in the arguments regardless of default values and it will work.
#
# Adding a new argument and adding a default value will NOT work because then the EXE needs to be updated to add the default value.
# Needless to say, we can't update the EXE to do that.
#


use strict;
use warnings;

my $dir   = 0;
my %classes = ();
my %function_names = ();


if (scalar @ARGV == 1)
{
	$dir = $ARGV[0];
	$dir .= "/";
}

readCPP();

open my $handle, '<', "bin/DllExport.txt" or die "Failed to open file bin/DllExport.txt";
chomp(my @txt_lines = <$handle>);
close $handle;

my $next_file = shift @txt_lines;

opendir(DIR, $dir) or die $!;

while (my $file = readdir(DIR))
{
	next unless (-f "$dir/$file");
	next unless ($file =~ m/\.h$/);
	processfile($file, "$dir/$file");
}
closedir(DIR);

die "\nDllExport ERROR: Failure to reach end of file\n\n\n" unless $next_file eq "";

testAllFunctionsFound();
testDEF();

exit();

sub processfile
{
	my $file = shift;
	my $path = shift;

	my $next_line = "";
	my $known_file = 0;

	if ($file eq $next_file)
	{
		$known_file = 1;
		$next_line = shift @txt_lines;
	}

	open my $handle, '<', $path or die "Failed to open file $file";
	chomp(my @lines = <$handle>);
	close $handle;

	my $i = 0;

	my $class = "";

	for my $line (@lines)
	{
		$i = $i + 1;
		if (index($line, "DllExport") != -1)
		{
			if ($line eq $next_line)
			{
				$next_line = shift @txt_lines;
				while (testFunctionInCPP($next_line, $class))
				{
					$next_line = shift @txt_lines;
				}
			}
			else
			{
				die "\n$path($i): Found DllExport on line not used by the exe: $line\nExpected: $next_line\nFor more information see https://github.com/We-the-People-civ4col-mod/Mod/wiki/DLL-interface-to-exe-and-python#exe-calling-dll\n\n";
			}
		}
		elsif (substr($line, 0, 5) eq "class")
		{
			$class = substr($line, 6);
			$class = getFirstWord($class);
			while (testFunctionInCPP($next_line, $class))
			{
				$next_line = shift @txt_lines;
			}
		}
	}

	if ($known_file)
	{
		die "\nDllExport ERROR: $file: Failed to locate line: $next_line\n\n\n" unless index($next_line, "DllExport") == -1;
		$next_file = $next_line;
	}
}

sub testFunctionInCPP
{
	my $line = shift;
	my $class = shift;

	if (exists $classes{$class}{$line})
	{
		$classes{$class}{$line} = 0;
		return 1;
	}
	return 0;
}

sub truncateOnStr
{
	my $str = shift;
	my $endChar = shift;

	my $index = index($str, $endChar);

	return $str if $index == -1;
	return substr($str, 0, $index);
}

sub getFirstWord
{
	my $str = shift;
	chomp($str);

	$str = truncateOnStr($str, " ");
	$str = truncateOnStr($str, "\t");
	$str = truncateOnStr($str, ":");
	return $str;
}

sub readCPP
{
	my $file = 'DLLSources/EXE_interface.cpp';
	open my $info, $file or die "Could not open $file: $!";

	my $class;

	my $i = 0;

	while( my $line = <$info>)
	{
		$i = $i + 1;
		if (substr($line, 0, 5) eq "class")
		{
			chomp($line);
			$class = substr($line, rindex($line, " ")+1);
			$classes{$class} = ();
			$function_names{$class} = ();
		}
		elsif (substr($line, 0, 10) eq "\tDllExport")
		{
			chomp($line);
			$line .= ";";
			$classes{$class}{$line} = $i;

			my $func = substr($line, 0, index($line, "("));
			$func = substr($func, rindex($func, " ")+1);
			$function_names{$class}{$func} = 1;
		}
		elsif (index($line, "DllExport") != -1)
		{
			next if (substr($line, 0, 2) eq "//"); # no need to scan for DllExport in comments
			die "\nDLLSources/EXE_interface.cpp(" . $i . "): DllExport has wrong indentation\n\n";
		}
	}

	close $info;
}

sub testAllFunctionsFound
{
	foreach my $class (keys %classes)
	{
		foreach my $func (keys %{$classes{$class}})
		{
			if ($classes{$class}{$func} != 0)
			{
				die "\nDLLSources/EXE_interface.cpp(" . $classes{$class}{$func} . "): DllExport function present, which isn't listed in the vanilla interface\n\n";
			}
		}
	}
}

sub testDEF
{
	my $file = 'DLLSources/EXE_interface.cpp';
	open my $info, $file or die "Could not open $file: $!";

	my $i = 0;

	while( my $line = <$info>)
	{
		$i = $i + 1;
		if (substr($line, 0, 35) eq "	#pragma comment(linker, \"/EXPORT:?")
		{
			my $index = index($line, "@");
			my $func = substr($line, 35, $index-35);
			my $class = substr($line, $index+1);
			$class = substr($class, 0, index($class, "@"));

			if (exists $function_names{$class}{$func})
			{
				if ($function_names{$class}{$func} == 1)
				{
					$function_names{$class}{$func} = 2;
				}
				else
				{
				# disabled for now as it will complain for redirects of overloaded functions (if multiple overloads are redirected)
				# @TODO: improve the check to allow overloads
				#	die "\nDLLSources/EXE_interface.cpp(" . $i . "): " . $class . "::" . $func . " is redirected more than once\n\n";
				}
			}
			else
			{
				die "\nDLLSources/EXE_interface.cpp(" . $i . "): " . $class . "::" . $func . " redirected, but isn't present in EXE_interface.cpp\n\n";
			}
		}
	}

	close $info;

	foreach my $class (keys %function_names)
	{
		foreach my $func (keys %{$function_names{$class}})
		{
			if ($function_names{$class}{$func} == 1)
			{
				die "\nDLLSources/EXE_interface.cpp(1): " . $class . "::" . $func . " isn't present, but must be redirected due to being present in EXE_interface.cpp\n\n";
			}
		}
	}
}
