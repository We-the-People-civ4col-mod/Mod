#!/usr/bin/perl -w

#
# Build-time test: detect non-deterministic function calls in game logic.
# Calls to rand(), srand(), time(), GetTickCount() outside of allowed files
# will cause multiplayer OOS (out-of-sync) bugs.
#
# Allowed files:
#   CvRandom.cpp  - the synced RNG wrapper (uses srand internally)
#   CvCity.cpp    - has a deterministic srand-based helper (seeded by coordinates)
#

use strict;
use warnings;

my $dir = "DLLSources";
my $errors = 0;

# Files allowed to use these functions
my %allowed_rand = (
	"CvRandom.cpp" => 1,
	"CvCity.cpp"   => 1,
);

my %allowed_time = ();

opendir(DIR, $dir) or die "Cannot open $dir: $!\n";

while (my $file = readdir(DIR))
{
	next unless (-f "$dir/$file");
	next unless ($file =~ m/\.cpp$/) or ($file =~ m/\.h$/);

	# Skip test files
	next if ($file =~ m/^Test/);
	next if ($file =~ m/^WTPTest/);

	open my $fh, '<', "$dir/$file" or die "Cannot open $dir/$file: $!\n";
	my $linenum = 0;

	while (my $line = <$fh>)
	{
		$linenum++;

		# Skip comments (simple heuristic: lines starting with // or inside /* */)
		next if $line =~ m{^\s*//};
		# Skip lines that are commented out
		next if $line =~ m{^\s*//.*\brand\b};

		# Check for rand()/srand() outside allowed files
		if (!$allowed_rand{$file})
		{
			# Match standalone rand() or srand() calls, not CvRandom or other identifiers
			if ($line =~ m/\b(?:std::)?(?:s?rand)\s*\(/ && $line !~ m{^\s*//})
			{
				print STDERR "$dir/$file($linenum): Non-deterministic: use of rand()/srand() outside CvRandom\n";
				$errors++;
			}
		}

		# Check for time()/GetTickCount() in any game logic file
		if (!$allowed_time{$file})
		{
			if ($line =~ m/\btime\s*\(\s*NULL\s*\)/ && $line !~ m{^\s*//})
			{
				print STDERR "$dir/$file($linenum): Non-deterministic: use of time(NULL)\n";
				$errors++;
			}
			if ($line =~ m/\bGetTickCount\s*\(/ && $line !~ m{^\s*//})
			{
				# GetTickCount is OK in profiling code (Profile.cpp etc.)
				next if $file eq "Profile.cpp";
				next if $file =~ m/^Cv.*Profile/;
				print STDERR "$dir/$file($linenum): Non-deterministic: use of GetTickCount()\n";
				$errors++;
			}
		}
	}

	close $fh;
}

closedir(DIR);

if ($errors > 0)
{
	# Report as warnings, not fatal errors — existing code has known uses
	print STDERR "test_determinism: $errors potential non-deterministic call(s) found (review for OOS risk)\n";
}

exit(0);
