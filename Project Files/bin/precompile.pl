#!/usr/bin/perl -w

#
# Setup of makefile settings, such as which cpp files to compile.
# This replaces the unstable fastdep.exe, which is prone to crashing.
# It is also more configurable so we now support source files being in nested directories rather than all files in a single location.
#


use strict;
use warnings;
use File::Slurp;

use lib './bin';
use XMLlists;

my $temp_dir = shift;
my $source_dir = shift;
my $depends_file = shift;
my $hardcoded = index($temp_dir, "HARDCODED") != -1;

my $source_dir_length = length($source_dir) + 1; # +1 to include the trailing slash

my $lib = "$source_dir/lib";
my $lib_length = length($lib);

my ($temp1, $temp2) = split(/\\/, $temp_dir);

mkdir $temp1;
mkdir "$temp1/$temp2";


my @cpp_files;
my %includes;
my $compile_targets = "";
my %using_precompiled_header;

# First build a list of cpp files as well as each included header (recursively, so also header included by headers)

# handle the main source dir
processdir($source_dir);

my $source = "SOURCES= \\\n";
my $objs = "OBJS= \\\n";
my $prefix_length = length($source_dir) + 1;

my %included_headers;



# generate the output. Multiple steps are done in this loop
# building source list of cpp files
# building list of objects
# setting up dependencies for each cpp file and precompiled header
# setting up makefile targets to handle each cpp file explicitly (no wildcards)

my $output = "";

foreach my $file (@cpp_files)
{
	my $file_orig = $file;
	%included_headers = ();
	applyHeader($file);
	$file =~ s/\//\\/g;
	
	my $file_obj = "$temp1\\$temp2\\" . substr($file, $prefix_length);
	$file_obj = substr($file_obj, 0, -3) . "obj";

	$source .= "  $file \\\n";
	$objs .= "  $file_obj \\\n";

	# building dependencies
	$output .= "$file_obj: $file \\\n";
	foreach my $header (sort keys %included_headers)
	{
		$output .= "\t$header \\\n";
	}
	$output .= "\n";

	if ($file eq "$source_dir\\_precompile.cpp")
	{
		# write this file twice, both for obj and pch (precompiled header)
		$output .= "$temp1\\$temp2\\CvGameCoreDLL.pch: $file \\\n";
		foreach my $header (sort keys %included_headers)
		{
			$output .= "\t$header \\\n";
		}
		$output .= "\n";
	}

	if ($file_obj ne "$temp1\\$temp2\\_precompile.obj")
	{
		# skip compiling the precompiled header as the makefile itself handles that part

		# setting up compile target
		$compile_targets .= "$file_obj: $file";
		$compile_targets .= " \$(Target_PCH)" if exists $using_precompiled_header{$file_orig}; # add PCH as dependency, but only if the cpp file actually includes it
		$compile_targets .= "\n";
		$compile_targets .= "\t\$(AT)\$(CPP) /nologo \$(CFLAGS) ";
		$compile_targets .= setPrecompilePath($file_orig);
		$compile_targets .= " \$(INCS) /Fo$file_obj /c $file\n\n";
	}
}

# Add lists and compile targets
$output .= $source;
$output .= "\n";
$output .= $objs;
$output .= "\n";
$output .= $compile_targets;

# write the resulting output file, but only if it differs from the existing file
writeFile($depends_file, \$output);

exit();

# adds a list of headers to %included_headers. This one is reset for each file, so it ends up telling which headers are included by the current cpp File
# called recursively to add dependencies on headers included in headers
sub applyHeader
{
	my $file = shift;

	foreach my $key (keys %{$includes{$file}})
	{
		next if exists $included_headers{$key};
		$included_headers{$key} = 1;
		applyHeader($key);
	}
}

# find all cpp files in the given directory and call processfile on each
sub processdir
{
	my $dir = shift;

	opendir my $working_dir, $dir or die "Cannot open directory: $!";
	my @files = readdir $working_dir;
	closedir $working_dir;

	for my $file (@files)
	{
		next if substr($file, 0, 1) eq ".";

		my $full_path = "$dir/$file";
		if (-d $full_path)
		{
			processdir($full_path);
		}
		else
		{
			next unless -f $full_path;
			next unless $file =~ m/\.cpp$/;
			
			createTempDir($dir);
			push(@cpp_files, $full_path);
			processfile($full_path);
		}
	}
}

# scan file for included header files. Used recursively on header files too
sub processfile
{
	my $file = shift;

	# first bail out of header file has already been processed
	return if exists $includes{$file};

	# don't examine lib for header includes. It won't work correctly anyway
	return if substr($file, 0, $lib_length) eq $lib;

	# use this hash as a list of headers being included. Due to the recursive nature of headers included in headers, the same files might be included many times.
	# A hash will not care if the same is added multiple times, hence why it is beneficial to use it rather than a list.
	$includes{$file} = {};

	my $path = substr($file, 0, rindex($file, "/"));

	open my $handle, '<', $file or die "Failed to open file $file";
	chomp(my @lines = <$handle>);
	close $handle;
	
	my $i = 0;
	for my $line (@lines)
	{
		$i += 1; # keep track of line number in order to produce MSVC compatible error messages

		processLine($file, $line, $i);

		# find file from #include "file" while ignoring whitespace. Place result in $include_file
		$line=~ s/^\s+//;
		next if substr($line, 0, 1) ne "#";
		$line = substr($line, 1);

		$line=~ s/^\s+//;
		next if substr($line, 0, 7) ne "include";
		$line = substr($line, 7);

		$line=~ s/^\s+//;
		next if substr($line, 0, 1) ne "\"";
		$line = substr($line, 1);

		my $rindex = index($line, "\"");
		next if $rindex == -1;
		my $include_file = substr($line, 0, $rindex);

		# test for \ vs /
		die "$file($i) \\ aren't allowed in includes. Use / Instead.\n" if (index($include_file, "\\") != -1);

		#$include_file =~ s/\\/\//g;
		
		my $include_path = createPath($path, $include_file);

		die "$file($i) Included file $include_file doesn't exist\n" unless -e $include_path;

		# mark file as needing the precompiled header if said header is included
		$using_precompiled_header{$file} = $include_file if substr($include_file, -15) eq "CvGameCoreDLL.h";

		# do not include headers inside lib as this script will fail doing that due to no concept of #ifdef and similar C++ code
		next if substr($include_path, 0, $lib_length) eq $lib;

		# add the header file to the hash of headers this file depends on
		$includes{$file}{$include_path} = 1;

		# process the header file just in case that one include includes
		processfile($include_path);
	}
}

# merge file and path. Removes ../ as needed
sub createPath
{
	my $dir = shift;
	my $file = shift;

	return "$dir/$file" if substr($file, 0, 3) ne "../";
	return substr($dir, 0, rindex($dir, "/")) . substr($file, 2);
}

sub createTempDir
{
	my $dir = shift;

	# skip out unless we are in a subdir
	return if $dir eq $source_dir;

	$dir = substr($dir, $source_dir_length);

	# skip out if the dir already exist
	return if -e "$temp_dir/$dir";

	# make the dir recursively in case it's a subdir of a subdir and each level needs to be made explicitly
	my $path = "$temp_dir/";
	foreach my $token (split(/\//, $dir))
	{
		$path .= "$token/";
		# mkdir in perl fails by returning false. Since we aren't reading the return value, it will fail silently if the directory exists already.
		mkdir $path;
	}
}

sub processLine
{
	my $file = shift;
	my $line = shift;
	my $line_number = shift;

	die "$file($line_number) gDLL->getInterfaceIFace->getSymbolID is broken. Use GC.getSymbolID instead.\n" if index($line, "gDLL->getInterfaceIFace()->getSymbolID") != -1;
	die "$file($line_number) gDLL->getSymbolID is broken. Use GC.getSymbolID instead.\n" if index($line, "gDLL->getSymbolID") != -1;
}

sub setPrecompilePath
{
	my $file = shift;

	return "" unless exists $using_precompiled_header{$file};

	my $path = substr($using_precompiled_header{$file}, 0, -15);
	$path =~ s/\//\\/g;
	return "\$(PRECOMPILE_CFLAGS1)$path\$(PRECOMPILE_CFLAGS2) "; # trailing space is intentional 
}
