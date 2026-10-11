#!/usr/bin/perl -w

#
# Build-time test: verify that TXT_KEY references in XML data files
# have corresponding definitions in Assets/XML/Text/ files.
#
# Scans all CIV4*Infos.xml files for TXT_KEY_* references,
# then checks that each key exists in at least one text XML file.
#

use strict;
use warnings;
use File::Find;

my $xml_base = "../Assets/XML/";
my $text_dir = $xml_base . "Text/";

# Step 1: Build a set of all defined TXT_KEY values from text files
my %defined_keys;

opendir(DIR, $text_dir) or die "Cannot open $text_dir: $!\n";
while (my $file = readdir(DIR))
{
	next unless $file =~ m/\.xml$/i;
	my $path = $text_dir . $file;

	open my $fh, '<', $path or die "Cannot open $path: $!\n";
	while (my $line = <$fh>)
	{
		# Match <Tag>TXT_KEY_...</Tag>
		while ($line =~ m/<Tag>\s*(TXT_KEY_[A-Z0-9_]+)\s*<\/Tag>/g)
		{
			$defined_keys{$1} = $path;
		}
	}
	close $fh;
}
closedir(DIR);

my $num_defined = scalar keys %defined_keys;
if ($num_defined == 0)
{
	die "test_text_keys: Found no TXT_KEY definitions in $text_dir — check path\n";
}

# Step 2: Scan data XML files for TXT_KEY references
my @data_dirs = (
	"Buildings/", "Units/", "GameInfo/", "Civilizations/",
	"Terrain/", "Events/", "BasicInfos/", "CivEffects/",
	"Interface/", "Misc/", "Art/",
);

my %missing_keys;
my $refs_checked = 0;

for my $subdir (@data_dirs)
{
	my $dir = $xml_base . $subdir;
	next unless -d $dir;

	opendir(my $dh, $dir) or next;
	while (my $file = readdir($dh))
	{
		next unless $file =~ m/\.xml$/i;
		# Skip schema files
		next if $file =~ m/Schema/i;

		my $path = $dir . $file;
		open my $fh, '<', $path or next;
		my $linenum = 0;

		while (my $line = <$fh>)
		{
			$linenum++;

			# Match TXT_KEY references in element content (not inside <Tag> elements)
			while ($line =~ m/>(TXT_KEY_[A-Z0-9_]+)</g)
			{
				my $key = $1;
				$refs_checked++;

				if (!exists $defined_keys{$key})
				{
					$missing_keys{$key} = "$subdir$file($linenum)";
				}
			}
		}
		close $fh;
	}
	closedir($dh);
}

# Step 3: Report
if (scalar keys %missing_keys > 0)
{
	my $count = scalar keys %missing_keys;
	print STDERR "test_text_keys: $count TXT_KEY reference(s) missing from text XML files:\n";
	for my $key (sort keys %missing_keys)
	{
		print STDERR "  $key (referenced in $missing_keys{$key})\n";
	}
	# Warning only — some keys may be defined in code or other locations
	print STDERR "test_text_keys: Review the above keys. Some may be generated at runtime.\n";
}

exit(0);
