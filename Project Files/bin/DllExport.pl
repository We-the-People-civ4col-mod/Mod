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

if (scalar @ARGV == 1)
{
	$dir = $ARGV[0];
	$dir .= "/";
}

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
	
	
	for my $line (@lines)
	{
		if (index($line, "DllExport") != -1)
		{
			if ($line eq $next_line)
			{
				$next_line = shift @txt_lines;
			}
			else
			{
				die "\nDllExport ERROR: $file: Found line, which shouldn't be there: $line\nExpected: $next_line\n\n";
			}
		}
	}
	
	if ($known_file)
	{
		die "\nDllExport ERROR: $file: Failed to locale line: $next_line\n\n\n" unless index($next_line, "DllExport") == -1;
		$next_file = $next_line;
	}
}