#!/usr/bin/perl -w

#
# Script frontend to bin/Compile.bat
#
# Adds a menu to pick which DLL file to compile.
#

use strict;
use warnings;

# add the ability to clear screen
use Win32::Console;
my $OUTPUT_SCREEN = Win32::Console->new(STD_OUTPUT_HANDLE);


menu();

sub menu
{
	$OUTPUT_SCREEN->Cls;

	print "Pick type of DLL to compile.\n";
	print "1 Dynamic Assert\n";
	print "2 Dynamic Release\n";
	print "3 Dynamic Final Release\n";
	print "4 Hardcoded Assert\n";
	print "5 Hardcoded Release\n";
	print "6 Hardcoded Final Release\n";
	print "0 Help\n";
	
	print "\n";
	print "Pick an option and press enter\n";
	
	my $input = <STDIN>;
	chomp $input;
	
	if ($input eq "1")
	{
		system("bin\\compile.bat", "Assert");
	}
	elsif ($input eq "2")
	{
		system("bin\\compile.bat", "Release");
	}
	elsif ($input eq "3")
	{
		system("bin\\compile.bat", "FinalRelease");
	}
	elsif ($input eq "4")
	{
		system("bin\\compile.bat", "Assert", "_HARDCODED", "-DHARDCODE_XML_VALUES");
	}
	elsif ($input eq "5")
	{
		system("bin\\compile.bat", "Release", "_HARDCODED", "-DHARDCODE_XML_VALUES");
	}
	elsif ($input eq "6")
	{
		system("bin\\compile.bat", "FinalRelease", "_HARDCODED", "-DHARDCODE_XML_VALUES");
	}
	elsif ($input eq "0")
	{
		help();
		menu();
	}
	else
	{
		print "Not valid input\n\n";
		print "\n\nPress enter to try again\n";
		my $input = <STDIN>;
		menu();
	}
	exit();
}

sub help
{
	$OUTPUT_SCREEN->Cls;
	
	print "There are 3 types of DLL files.\n";
	print "1 Assert\n";
	print "   This will enable assert checks. An assert is a condition the programmer assumes to always be true.\n";
	print "   The game will then make a popup assert failure each time an assert check fails.\n";
	print "   Assert checks are useful for finding bugs. Sadly we do have a bunch of known assert failures.\n";
	print "   Runs 30% slower than release due to spending time on all the extra assert checks/calculations.\n";
	print "\n";
	print "2 Release\n";
	print "   This is the defualt DLL. Fast to play with and fast to compile.\n";
	print "\n";
	print "3 Final Release\n";
	print "   A better optimized version of Release. Takes 4-5 times longer to compile.\n";
	print "   Runs 10% faster than Release.\n";
	print "\n";
	print "Debug and Profile DLL files can only be compiled from within Visual Studio.\n";
	print "   The nature of those two makes them useless for anybody who isn't writing C++ code.\n";
	
	print "\n\nPress enter to continue\n";
	my $input = <STDIN>;
	
	$OUTPUT_SCREEN->Cls;
	
	print "All DLL files comes in two versions, dynamic and hardcoded\n\n";
	print "Dynamic\n";
	print "   Vanilla behavior. Will load xml data at startup of game.\n";
	print "\n";
	print "Hardcoded\n";
	print "   This will read the xml files at compile time and inform the compiler of the values.\n";
	print "   The compiler can then skip a lot of what if scenarios and generate code specific to our data.\n";
	print "   Also having the values directly in the program code unlocks CPU tricks for runtime optimization.\n";
	print "   The results is a faster DLL file with the tradeoff that you will need to compile again when changing xml.\n";
	print "\n";
	print "Which one to pick depends entirely on what you are trying to do.\n";
	print "If you plan on changing xml data and then playtest it, then adding the need to recompile would be a hassle.\n";
	print "If you just want to play or playtest and know you won't touch the xml files, then maybe hardcoding will work.\n";
	print "\n";
	print "When you pull updates from GitHub, always compile afterwards because maybe somebody changed the C++ code.\n";
	print "Not compiling might still go wrong even if you use a dynamic DLL file.\n";
	
	print "\n\nPress enter to continue\n";
	my $input = <STDIN>;
}