#!/usr/bin/perl -w


use strict;
use warnings;

use File::Slurp;

use XML::LibXML;
use lib './bin';
use XMLlists;

my $FILE         = getAutoDir() . "/AutoXmlEnum.h";
my $FILE_TEST    = getAutoDir() . "/AutoXmlTest.h";
my $FILE_DECLARE = getAutoDir() . "/AutoXmlDeclare.h";
my $FILE_INIT    = getAutoDir() . "/AutoXmlInit.h";
my $FILE_PRELOAD = getAutoDir() . "/AutoXmlPreload.h";

my $files = [];
my %varToEnum;

my $output         = "";
my $output_test    = "";
my $output_declare = "";
my $output_init    = "";

my $output_preload_dynamic     = "";
my $output_preload_declaration = "";
my $output_preload_function    = "";
my $output_length_test         = "";

$output .= "#ifndef AUTO_XML_ENUM\n";
$output .= "#define AUTO_XML_ENUM\n";
$output .= "\n";
$output .= "// Autogenerated file. Do not edit!!!\n";
$output .= "\n";
$output .= "// Each of the enums in this file represent an xml file.\n";
$output .= "// Civ4 has always hardcoded some xml data this way and modders have always had issues with dll and xml going out of sync.\n";
$output .= "// \n";
$output .= "// This file will hopefully solve the out of sync problem for good.\n";
$output .= "// The makefile will update this file if needed each time the compiler is used.\n";
$output .= "// As a result, a compiled DLL will always match the xml files at the time of compilation.\n";
$output .= "// \n";
$output .= "// The debugger is aided by this file.\n";
$output .= "// The debugger will use the names given in this file, like UNIT_WAGON_TRAIN(81) instead of just 81.\n";
$output .= "// It would be nice to trick the debugger to assume hardcoded even if the DLL isn't, but for the time being the debugger prefers hardcoded DLL files.\n";
$output .= "// \n";
$output .= "// The file can optionally hardcode everything (it doesn't by default. It has to be enabled)\n";
$output .= "// Hardcoding can help the compiler optimization, but it shouldn't be used if somebody wants to edit xml files.\n";
$output .= "// \n";
$output .= "// All hardcoded data will be assert checked at startup.\n";
$output .= "\n";

$output_test .= "\n";
$output_test .= "// Autogenerated file. Do not edit!!!\n";
$output_test .= "\n";

$output_declare .= "\n";
$output_declare .= "// Autogenerated file. Do not edit!!!\n";
$output_declare .= "\n";
$output_declare .= "#ifndef HARDCODE_XML_VALUES\n";

$output_init .= "\n";
$output_init .= "// Autogenerated file. Do not edit!!!\n";
$output_init .= "\n";
$output_init .= "#ifndef HARDCODE_XML_VALUES\n";
$output_init .= "if (bFirst) {\n";

foreach my $file (getEnumFiles())
{
	processFile($file);
}

$output_init .= "} else {\n";

handleGlobalDefineALT();
handleHardcodedEnumVariables();

$output .= "\n#endif // AUTO_XML_ENUM\n";
$output_declare .= "#endif\n";
$output_init .= "}\n#endif\n";

my $output_preload = "";
$output_preload .= $output_preload_declaration;
$output_preload .= "#ifndef HARDCODE_XML_VALUES\n";
$output_preload .= $output_preload_dynamic;
$output_preload .= "#endif\n";
$output_preload .= "static void setXmlLengthsAuto(const std::string& basePath)\n{\n";
$output_preload .= $output_preload_function;
$output_preload .= "}\n\n";
$output_preload .= "static void testAllXMLLengths()\n{\n";
$output_preload .= $output_length_test;
$output_preload .= "}\n\n";


writeFile($FILE        , \$output        );
writeFile($FILE_TEST   , \$output_test   );
writeFile($FILE_DECLARE, \$output_declare);
writeFile($FILE_INIT   , \$output_init   );
writeFile($FILE_PRELOAD, \$output_preload);

sub getChild
{
	my $parent = shift;
	my $name = shift;
	
	my $element = $parent->firstChild;
	
	while (1)
	{
		return if (ref($element) eq "");
		if (ref($element) eq "XML::LibXML::Element")
		{
			return $element if $name eq "" or $element->nodeName eq $name;
		}
		$element = $element->nextSibling;
	}
}

sub nextSibling
{
	my $element = shift;
	
	$element = $element->nextSibling;
	while (ref($element) ne "XML::LibXML::Element" and ref($element) ne "")
	{
		$element = $element->nextSibling;
	}
	return $element;
}

sub processFile
{
	my $filename = shift;

	my ($basename, $enum, $TYPE) = getXMLKeywords($filename);
	
	my $isHardcoded = isAlwaysHardcodedEnum($enum);
	
	my $hardcodedBool = "true";
	$hardcodedBool = "false" unless $isHardcoded;
	
	my $found_type = 0;
	
	my $isYield = $enum eq "YieldTypes";

	$output .= "enum ";
	$output .= $enum . "\n{\n";
	$output .= "\tINVALID_PROFESSION = -2,\n" if $basename eq "Profession";
	$output .= "\t" . getNoType($TYPE) . " = -1,\n\n";
	
	
	my @types = getTypesInFile($filename);
	
	$found_type = 1 if scalar @types > 0;
	
	unless ($found_type)
	{
		$output .= "\n\tCOMPILE_TIME_NUM_" . $TYPE . "_TYPES = MAX_SHORT,\n";
	}
	else
	{
		$output .= "#ifdef HARDCODE_XML_VALUES\n\n" unless $isHardcoded;
		
		foreach my $type (@types)
		{
			$varToEnum{$type} = $enum;
			$output .= "\t" . $type . ",\n";
			$output_test .= "DisplayXMLhardcodingError(strcmp(\"". $type . "\", " . getInfo($basename) . "($type).getType()) == 0, \"$type\", true);\n" if $isHardcoded;
		}

		$output_test .= "DisplayXMLhardcodingError(NUM_" . $TYPE . "_TYPES == (" . $enum . ")" . getNumFunction($basename) . ", \"NUM_" . $TYPE . "_TYPES\", " . $hardcodedBool . ");\n" if $found_type;

		$output .= "\n\tNUM_" . $TYPE . "_TYPES,\n";
		$output .= "\tNUM_CARGO_YIELD_TYPES = YIELD_HAMMERS,\n" if $isYield;
		$output .= "\n\tCOMPILE_TIME_NUM_" . $TYPE . "_TYPES = NUM_" . $TYPE . "_TYPES,\n";
		
		unless ($isHardcoded)
		{
			$output .= "\n#else // HARDCODE_XML_VALUES\n";
			$output .= "\n\tCOMPILE_TIME_NUM_" . $TYPE . "_TYPES = MAX_SHORT,\n";
			$output .= "\n#endif // HARDCODE_XML_VALUES\n";
		}
	}
	
	$output .= "\n\tFIRST_" . $TYPE . " = 0,\n";
	$output .= "};\n\n";
	
	
	$output_preload_declaration .= $enum . " NUM_" . $TYPE . "_TYPES_NON_CONST;\n";
	$output_preload_declaration .= "const " . $enum . "& NUM_" . $TYPE . "_TYPES = NUM_" . $TYPE . "_TYPES_NON_CONST;\n" unless $found_type;
	$output_preload_function    .= "\tsetSingleLength(basePath, NUM_" . $TYPE . "_TYPES_NON_CONST);\n";
	
	unless ($isHardcoded)
	{
		$output .= "#ifndef HARDCODE_XML_VALUES\n";
		$output .= "extern const " . $enum . "& NUM_" . $TYPE . "_TYPES;\n";
		$output .= "#endif\n\n";
		$output_preload_dynamic .= "const " . $enum . "& NUM_" . $TYPE . "_TYPES = NUM_" . $TYPE . "_TYPES_NON_CONST;\n" if $found_type;
	}
	$output .= "#define NUM_" . substr($enum, 0, -5) . "_TYPES NUM_" . $TYPE . "_TYPES\n\n";
	# blindly test the lenght of all enum types. If it is dynamic, the length will be tested against itself, hence always true
	$output_length_test .= "\tStartupCheck::CheckXmlLength(\"" . $enum . "\", NUM_" . $TYPE . "_TYPES, NUM_" . $TYPE . "_TYPES_NON_CONST);\n";
}

sub getTypesInFile
{
	my $filename = shift;
	
	return () if hasNoTypeTag($filename);
	
	my $fileWithPath = getFileWithPath($filename);
	
	if ($fileWithPath)
	{
		my $dom = XML::LibXML->load_xml(location => $fileWithPath);
		
		my $loopElement = getChild($dom, "");
		$loopElement = getChild($loopElement, "");
		$loopElement = getChild($loopElement, "") unless isTwoLevelFile($filename);
		
		my @types = ();
		
		while (ref ($loopElement) ne "")
		{
			my $child = getChild($loopElement, "Type");
			die $filename . " failed to read elements" unless $child;
			push(@types, $child->textContent);
			$loopElement = nextSibling($loopElement);
		}
		return @types;
	}
	
	# the file isn't present in the mod. Add the vanilla values
	# no need to look up vanilla. That can cause issues and we know the values even without looking
	return ("ATTITUDE_FURIOUS", "ATTITUDE_ANNOYED", "ATTITUDE_CAUTIOUS", "ATTITUDE_PLEASED", "ATTITUDE_FRIENDLY") if $filename eq "BasicInfos/CIV4AttitudeInfos.xml";
	return ("CALENDAR_DEFAULT", "CALENDAR_BI_YEARLY", "CALENDAR_YEARS", "CALENDAR_TURNS", "CALENDAR_SEASONS", "CALENDAR_MONTHS", "CALENDAR_WEEKS") if $filename eq "BasicInfos/CIV4CalendarInfos.xml";
	return ("DENIAL_UNKNOWN", "DENIAL_NEVER", "DENIAL_TOO_MUCH", "DENIAL_MYSTERY", "DENIAL_JOKING", "DENIAL_CONTACT_THEM", "DENIAL_VICTORY", "DENIAL_ATTITUDE", "DENIAL_ATTITUDE_THEM", "DENIAL_POWER_US", "DENIAL_POWER_YOU", "DENIAL_POWER_THEM", "DENIAL_TOO_MANY_WARS", "DENIAL_NO_GAIN", "DENIAL_NOT_INTERESTED", "DENIAL_NOT_ALLIED", "DENIAL_RECENT_CANCEL", "DENIAL_WORST_ENEMY", "DENIAL_POWER_YOUR_ENEMIES", "DENIAL_TOO_FAR", "DENIAL_WAR_NOT_POSSIBLE_US", "DENIAL_WAR_NOT_POSSIBLE_YOU", "DENIAL_PEACE_NOT_POSSIBLE_US", "DENIAL_PEACE_NOT_POSSIBLE_YOU") if $filename eq "BasicInfos/CIV4DenialInfos.xml";
	return ("DOMAIN_SEA", "DOMAIN_LAND", "DOMAIN_IMMOBILE") if $filename eq "BasicInfos/CIV4DomainInfos.xml";
	return ("FATHERCATEGORY_EXPLORATION", "FATHERCATEGORY_RELIGION", "FATHERCATEGORY_TRADE", "FATHERCATEGORY_MILITARY", "FATHERCATEGORY_POLITICS") if $filename eq "BasicInfos/CIV4FatherCategoryInfos.xml";
	return ("MEMORY_DECLARED_WAR", "MEMORY_DECLARED_WAR_ON_FRIEND", "MEMORY_HIRED_WAR_ALLY", "MEMORY_RAZED_CITY", "MEMORY_GIVE_HELP", "MEMORY_REFUSED_HELP", "MEMORY_ACCEPT_DEMAND", "MEMORY_REJECTED_DEMAND", "MEMORY_ACCEPTED_JOIN_WAR", "MEMORY_DENIED_JOIN_WAR", "MEMORY_ACCEPTED_STOP_TRADING", "MEMORY_DENIED_STOP_TRADING", "MEMORY_STOPPED_TRADING", "MEMORY_STOPPED_TRADING_RECENT", "MEMORY_HIRED_TRADE_EMBARGO", "MEMORY_MADE_DEMAND", "MEMORY_MADE_DEMAND_RECENT", "MEMORY_CANCELLED_OPEN_BORDERS", "MEMORY_EVENT_GOOD_TO_US", "MEMORY_EVENT_BAD_TO_US", "MEMORY_LIBERATED_CITIES", "MEMORY_REFUSED_TAX", "MEMORY_REVENGE_TAKEN", "MEMORY_MISSIONARY_FAIL") if $filename eq "BasicInfos/CIV4MemoryInfos.xml";
	return ("MONTH_JANUARY", "MONTH_FEBRUARY", "MONTH_MARCH", "MONTH_APRIL", "MONTH_MAY", "MONTH_JUNE", "MONTH_JULY", "MONTH_AUGUST", "MONTH_SEPTEMBER", "MONTH_OCTOBER", "MONTH_NOVEMBER", "MONTH_DECEMBER") if $filename eq "BasicInfos/CIV4MonthInfos.xml";
	return ("SEASON_WINTER", "SEASON_SPRING", "SEASON_SUMMER", "SEASON_FALL") if $filename eq "BasicInfos/CIV4SeasonInfos.xml";
	
	return ("ALARM_DEFAULT") if $filename eq "Civilizations/CIV4AlarmInfos.xml";
	
	return ("CIVICOPTION_SLAVERY", "CIVICOPTION_ELECTION", "CIVICOPTION_NATIVES", "CIVICOPTION_RELIGION", "CIVICOPTION_SECURITY") if $filename eq "GameInfo/CIV4CivicOptionInfos.xml";
	return ("CLIMATE_TEMPERATE", "CLIMATE_TROPICAL", "CLIMATE_ROCKY") if $filename eq "GameInfo/CIV4ClimateInfo.xml";
	return ("CURSOR_DEFAULT", "CURSOR_PING", "CURSOR_GO_TO", "CURSOR_ROUTE_TO", "CURSOR_BUSY", "CURSOR_MOVE", "CURSOR_SPLITV", "CURSOR_SPLITH", "CURSOR_SIZEV", "CURSOR_SIZEH", "CURSOR_SIZENE", "CURSOR_SIZENW", "CURSOR_SIZEALL", "CURSOR_LINK", "CURSOR_GRIP", "CURSOR_EDIT") if $filename eq "GameInfo/CIV4CursorInfo.xml";
	return ("EUROPE_EAST", "EUROPE_WEST", "EUROPE_NORTH", "EUROPE_SOUTH") if $filename eq "GameInfo/CIV4EuropeInfo.xml";
	return ("FORCECONTROL_SPEED", "FORCECONTROL_HANDICAP", "FORCECONTROL_OPTIONS", "FORCECONTROL_VICTORIES", "FORCECONTROL_MAX_TURNS", "FORCECONTROL_MAX_CITY_ELIMINATION", "FORCECONTROL_ADVANCED_START") if $filename eq "GameInfo/CIV4ForceControlInfos.xml";
	return ("GRAPHICOPTION_HEALTH_BARS", "GRAPHICOPTION_NO_COMBAT_ZOOM", "GRAPHICOPTION_NO_ENEMY_GLOW", "GRAPHICOPTION_FROZEN_ANIMATIONS", "GRAPHICOPTION_EFFECTS_DISABLED", "GRAPHICOPTION_GLOBE_VIEW_BUILDINGS_DISABLED", "GRAPHICOPTION_FULLSCREEN", "GRAPHICOPTION_LOWRES_TEXTURES", "GRAPHICOPTION_HIRES_TERRAIN", "GRAPHICOPTION_NO_MOVIES") if $filename eq "GameInfo/CIV4GraphicOptionInfos.xml";
	return ("HURRY_GOLD", "HURRY_IMMIGRANT") if $filename eq "GameInfo/CIV4HurryInfo.xml";
	return ("MPOPTION_SIMULTANEOUS_TURNS", "MPOPTION_TAKEOVER_AI", "MPOPTION_SHUFFLE_TEAMS", "MPOPTION_ANONYMOUS", "MPOPTION_TURN_TIMER") if $filename eq "GameInfo/CIV4MPOptionInfos.xml";
	return ("SEALEVEL_LOW", "SEALEVEL_MEDIUM", "SEALEVEL_HIGH") if $filename eq "GameInfo/CIV4SeaLevelInfo.xml";
	return ("TURNTIMER_STATIC", "TURNTIMER_SNAIL", "TURNTIMER_SLOW", "TURNTIMER_MEDIUM", "TURNTIMER_FAST", "TURNTIMER_BLAZING") if $filename eq "GameInfo/CIV4TurnTimerInfo.xml";
	
	return ("ANIMCAT_NONE", "ANIMCAT_IDLE", "ANIMCAT_MOVE", "ANIMCAT_FIDGET", "ANIMCAT_MELEE_FORTIFY", "ANIMCAT_MELEE_FORTIFIED", "ANIMCAT_MELEE_STRIKE", "ANIMCAT_MELEE_HURT", "ANIMCAT_MELEE_DIE", "ANIMCAT_MELEE_DIE_FADE", "ANIMCAT_MELEE_FLEE", "ANIMCAT_RUNDIE", "ANIMCAT_RANGED_FORTIFY", "ANIMCAT_RANGED_FORTIFIED", "ANIMCAT_RANGED_STRIKE", "ANIMCAT_RANGED_DIE", "ANIMCAT_RANGED_DIE_FADE", "ANIMCAT_FOUND", "ANIMCAT_IRRIGATE_BEGIN", "ANIMCAT_IRRIGATE", "ANIMCAT_IRRIGATE_PAUSE", "ANIMCAT_BUILD_BEGIN", "ANIMCAT_BUILD", "ANIMCAT_BUILD_PAUSE", "ANIMCAT_MINE_BEGIN", "ANIMCAT_MINE", "ANIMCAT_MINE_PAUSE", "ANIMCAT_CHOP_BEGIN", "ANIMCAT_CHOP", "ANIMCAT_CHOP_PAUSE", "ANIMCAT_SHOVEL_BEGIN", "ANIMCAT_SHOVEL", "ANIMCAT_SHOVEL_PAUSE", "ANIMCAT_RAILROAD_BEGIN", "ANIMCAT_RAILROAD", "ANIMCAT_RAILROAD_PAUSE", "ANIMCAT_GREAT_EVENT", "ANIMCAT_SURRENDER", "ANIMCAT_LEADER_COMMAND", "ANIMCAT_LEADER_COMMAND_IDLE", "ANIMCAT_DAMAGE_0", "ANIMCAT_DAMAGE_1", "ANIMCAT_DAMAGE_2", "ANIMCAT_DAMAGE_3") if $filename eq "Units/Civ4AnimationInfos.xml";
	return ("ANIMATIONPATH_IDLE", "ANIMATIONPATH_MOVE", "ANIMATIONPATH_DAMAGE", "ANIMATIONPATH_RANDOMIZE_ANIMATION_SET", "ANIMATIONPATH_MELEE_STRIKE", "ANIMATIONPATH_MELEE_HURT", "ANIMATIONPATH_MELEE_DIE", "ANIMATIONPATH_MELEE_FORTIFIED", "ANIMATIONPATH_MELEE_DIE_FADE", "ANIMATIONPATH_MELEE_FLEE", "ANIMATIONPATH_RANGED_STRIKE", "ANIMATIONPATH_RANGED_DIE", "ANIMATIONPATH_RANGED_FORTIFIED", "ANIMATIONPATH_RANGED_RUNHIT", "ANIMATIONPATH_RANGED_RUNDIE", "ANIMATIONPATH_RANGED_DIE_FADE", "ANIMATIONPATH_LEADER_COMMAND", "ANIMATIONPATH_HEAL", "ANIMATIONPATH_SLEEP", "ANIMATIONPATH_FORTIFY", "ANIMATIONPATH_MELEE_FORTIFY", "ANIMATIONPATH_PILLAGE", "ANIMATIONPATH_SENTRY", "ANIMATIONPATH_FOUND", "ANIMATIONPATH_IRRIGATE", "ANIMATIONPATH_BUILD", "ANIMATIONPATH_MINE", "ANIMATIONPATH_CHOP", "ANIMATIONPATH_SHOVEL", "ANIMATIONPATH_RAILROAD", "ANIMATIONPATH_GREAT_EVENT", "ANIMATIONPATH_SURRENDER") if $filename eq "Units/Civ4AnimationPathInfos.xml";
	return ("ENTITY_EVENT_IDLE", "ENTITY_EVENT_DIE", "ENTITY_EVENT_DAMAGE", "ENTITY_EVENT_BEGIN_COMBAT", "ENTITY_EVENT_END_COMBAT", "ENTITY_EVENT_SURRENDER", "ENTITY_EVENT_CAPTURED", "ENTITY_EVENT_MULTI_SELECT", "ENTITY_EVENT_MULTI_DESELECT", "ENTITY_EVENT_GREAT_EVENT", "ENTITY_EVENT_PILLAGE", "ENTITY_EVENT_BOMBARD", "ENTITY_EVENT_FOUND", "ENTITY_EVENT_FORTIFY", "ENTITY_EVENT_SENTRY", "ENTITY_EVENT_HEAL", "ENTITY_EVENT_SLEEP", "ENTITY_EVENT_SHOVEL", "ENTITY_EVENT_BUILD", "ENTITY_EVENT_IRRIGATE", "ENTITY_EVENT_MINE", "ENTITY_EVENT_CHOP", "ENTITY_EVENT_RAILROAD") if $filename eq "Units/Civ4EntityEventInfos.xml";
	
	die "getTypesInFile: " . $filename . " not supported\n";
}

sub handleGlobalDefineALT
{
	my %value;
	my %vartype;
	my %hardcoded;
	
	# add all vanilla values
	# should they be present in GlobalDefinesALT.xml, then that will be used instead.
	# vanilla tags not mentioned in the mod will be hardcoded as they are unlikely to change.
	addVanillaValues(\%value, \%vartype, \%hardcoded);
	
	my $declare_hardcoded = "";
	my $declare_dynamic = "";
	
	
	my $filename = "../Assets/XML/GlobalDefinesAlt.xml";
	my $tag = "";
	my $lineno = 0;
	my $state = -1;
	
	open(my $fh, '<:encoding(UTF-8)', $filename)
	  or die "Could not open file '$filename' $!";

	while (my $line = <$fh>)
	{
		$lineno += 1;
		if ($line =~ /\Q<DefineName>\E/)
		{
			$tag = substr($line, index($line, ">")+1);
			$tag = substr($tag, 0, index($tag, "<"));
			$hardcoded{$tag} = 0;
			$state = 0;
		}
		elsif ($state == 0)
		{
			if ($line =~ /\Q<iDefineIntVal>\E/)
			{
				my $number = substr($line, index($line, ">")+1);
				$number = substr($number, 0, index($number, "<"));
				$value{$tag} = $number;
				$vartype{$tag} = "int";
				$state = 1;
			}
			elsif ($line =~ /\Q<DefineTextVal>\E/)
			{
				my $valueStr = substr($line, index($line, ">")+1);
				$valueStr = substr($valueStr, 0, index($valueStr, "<"));
				my $type = $varToEnum{$valueStr} || continue;
				$vartype{$tag} = $type;
				$value{$tag} = $valueStr;
				$state = 1;
			}
		}
		elsif ($state == 1 and $line =~ /\Q<bAlwaysHardcoded>1<\/bAlwaysHardcoded>\E/)
		{
			$state = -1;
			$hardcoded{$tag} = 1;
		}
		
	}
	
	$output .= "// GlobalDefinesAlt\n";
	
	foreach my $tag (sort keys %value)
	{
		my $var = "GLOBAL_DEFINE_" . $tag;
		
		if ($hardcoded{$tag})
		{
			if ($vartype{$tag} eq "int")
			{
				$output .= "#define $var $value{$tag}\n";
			}
			else
			{
				$output .= "const $vartype{$tag} $var = $value{$tag};\n";
			}
			
			if ($vartype{$tag} eq "int")
			{
				$output_test .= "DisplayXMLhardcodingError($var == $value{$tag}, \"$tag\", true);\n";
			}
			else
			{
				$output_test .= "DisplayXMLhardcodingError($var == static_cast<$vartype{$tag}>(GC.getDefineINT(\"$value{$tag}\")), \"$tag\", true);\n";
			}
		}
		else
		{
			$declare_hardcoded .= "const $vartype{$tag} $var = $value{$tag};\n";
			$declare_dynamic .= "extern const $vartype{$tag}& $var;\n";
			$output_declare .= "$vartype{$tag} " . $var . "_NON_CONST;\n";
			$output_declare .= "const $vartype{$tag}& $var = " . $var . "_NON_CONST;\n";
			if ($vartype{$tag} eq "int")
			{
				$output_init .= $var . "_NON_CONST = GC.getDefineINT(\"$tag\");\n";
				$output_test .= "DisplayXMLhardcodingError($var == GC.getDefineINT(\"$tag\"), \"$tag\", false);\n";
			}
			else
			{
				$output_init .= $var . "_NON_CONST = static_cast<$vartype{$tag}>(getIndexForType(VARINFO<$vartype{$tag}>::JIT, \"$value{$tag}\"));\n";
				$output_test .= "DisplayXMLhardcodingError($var == static_cast<$vartype{$tag}>(getIndexForType(VARINFO<$vartype{$tag}>::JIT, \"$value{$tag}\")), \"$tag\", false);\n";
			}
			
		}
	}
	
	$output .= "#ifdef HARDCODE_XML_VALUES\n";
	$output .= $declare_hardcoded;
	$output .= "#else\n";
	$output .= $declare_dynamic;
	$output .= "#endif // hardcoded xml\n";
	
	close($fh);
}

sub handleHardcodedEnumVariables
{
	my $filename = "DLLSources/HardcodedEnumSetup.h";
	my $lineno = 0;
	my $enum = "";
	
	
	$output .= "\n#ifndef HARDCODE_XML_VALUES\n";
	
	open(my $fh, '<:encoding(UTF-8)', $filename)
	  or die "Could not open file '$filename' $!";
	  
	while (my $line = <$fh>)
	{
		$lineno += 1;
		chomp($line);
		
		if (substr($line, 0, 5) eq "enum ")
		{
			die("$filename($lineno) found new enum without ending the previous one\n") unless $enum eq "";
			$enum = substr($line, 5);
		}
		elsif ($enum ne "")
		{
			if (substr($line, 0, 2) eq "};")
			{
				$enum = "";
			}
			elsif ((my $index = index($line, ",")) != -1)
			{
				my $variableName = substr($line, 0, $index);
				
				# remove whitespace on the left of the string
				$variableName =~ s/^\s+//;
				$output_declare .= $enum . " " . $variableName . "_NON_CONST;\n";
				$output_declare .= "const " . $enum . "& " . $variableName . " = " . $variableName . "_NON_CONST;\n";
				$output_init .= $variableName . "_NON_CONST = getIndexOfType(" . $variableName . "_NON_CONST, \"" . $variableName . "\");\n";
				$output_init .= "DisplayXMLmissingError((int)$variableName != -1, \"$variableName\");\n";
				$output .= "extern const " . $enum . "& " . $variableName . ";\n";
			}
		}
	}
	
	$output .= "#endif // hardcoded xml\n";
	close($fh);
}

sub addVanillaSingleValue
{
	my $valueContainer = shift;
	my $tag = shift;
	my $number = shift;
	
	$$valueContainer{value}{$tag} = $number;
	$$valueContainer{vartype}{$tag} = "int";
	$$valueContainer{hardcoded}{$tag} = 1;
}

sub addVanillaValues
{
	my %valueContainer =
	(
		value => shift,
		vartype => shift,
		hardcoded => shift,
	);
	
	addVanillaSingleValue(\%valueContainer, "CIV4_VERSION", 101);
	addVanillaSingleValue(\%valueContainer, "MIN_VERSION", 000);
	addVanillaSingleValue(\%valueContainer, "SAVE_VERSION", 002);
	addVanillaSingleValue(\%valueContainer, "MAX_NUM_LANGUAGES", 100);
	addVanillaSingleValue(\%valueContainer, "MAX_PLOT_LIST_SIZE", 10);
	addVanillaSingleValue(\%valueContainer, "MAX_PLOT_LIST_ROWS", 6);
	addVanillaSingleValue(\%valueContainer, "START_YEAR", 1492);
	addVanillaSingleValue(\%valueContainer, "WEEKS_PER_MONTHS", 4);
	addVanillaSingleValue(\%valueContainer, "PLOT_VISIBILITY_RANGE", 1);
	addVanillaSingleValue(\%valueContainer, "UNIT_VISIBILITY_RANGE", 1);
	addVanillaSingleValue(\%valueContainer, "PEAK_SEE_FROM_CHANGE", 2);
	addVanillaSingleValue(\%valueContainer, "PEAK_SEE_THROUGH_CHANGE", 2);
	addVanillaSingleValue(\%valueContainer, "HILLS_SEE_FROM_CHANGE", 1);
	addVanillaSingleValue(\%valueContainer, "HILLS_SEE_THROUGH_CHANGE", 1);
	addVanillaSingleValue(\%valueContainer, "SEAWATER_SEE_FROM_CHANGE", 0);
	addVanillaSingleValue(\%valueContainer, "MOVE_DENOMINATOR", 60);
	addVanillaSingleValue(\%valueContainer, "STARTING_DISTANCE_PERCENT", 12);
	addVanillaSingleValue(\%valueContainer, "OWN_TEAM_STARTING_MODIFIER", 80);
	addVanillaSingleValue(\%valueContainer, "RIVAL_TEAM_STARTING_MODIFIER", 125);
	addVanillaSingleValue(\%valueContainer, "MIN_CIV_STARTING_DISTANCE", 10);
	addVanillaSingleValue(\%valueContainer, "MIN_CITY_RANGE", 1);
	addVanillaSingleValue(\%valueContainer, "FREE_CITY_CULTURE", 2);
	addVanillaSingleValue(\%valueContainer, "FREE_CITY_ADJACENT_CULTURE", 1);
	addVanillaSingleValue(\%valueContainer, "CITY_FREE_CULTURE_GROWTH_FACTOR", 20);
	addVanillaSingleValue(\%valueContainer, "OWNERSHIP_SCORE_DURATION_THRESHOLD", 20);
	addVanillaSingleValue(\%valueContainer, "INITIAL_CITY_POPULATION", 0);
	addVanillaSingleValue(\%valueContainer, "INITIAL_AI_CITY_PRODUCTION", 10);
	addVanillaSingleValue(\%valueContainer, "BASE_CITY_GROWTH_THRESHOLD", 200);
	addVanillaSingleValue(\%valueContainer, "CITY_GROWTH_MULTIPLIER", 0);
	addVanillaSingleValue(\%valueContainer, "FOOD_CONSUMPTION_PER_POPULATION", 2);
	addVanillaSingleValue(\%valueContainer, "NEW_HURRY_MODIFIER", 50);
	addVanillaSingleValue(\%valueContainer, "MIN_TIMER_UNIT_DOUBLE_MOVES", 32);
	addVanillaSingleValue(\%valueContainer, "COMBAT_DAMAGE", 20);
	addVanillaSingleValue(\%valueContainer, "MAX_HIT_POINTS", 100);
	addVanillaSingleValue(\%valueContainer, "COMBAT_DIE_SIDES", 1000);
	addVanillaSingleValue(\%valueContainer, "HILLS_EXTRA_DEFENSE", 25);
	addVanillaSingleValue(\%valueContainer, "RIVER_ATTACK_MODIFIER", -10);
	addVanillaSingleValue(\%valueContainer, "AMPHIB_ATTACK_MODIFIER", -50);
	addVanillaSingleValue(\%valueContainer, "ENEMY_HEAL_RATE", 5);
	addVanillaSingleValue(\%valueContainer, "NEUTRAL_HEAL_RATE", 10);
	addVanillaSingleValue(\%valueContainer, "FRIENDLY_HEAL_RATE", 15);
	addVanillaSingleValue(\%valueContainer, "CITY_HEAL_RATE", 20);
	addVanillaSingleValue(\%valueContainer, "GREAT_GENERALS_THRESHOLD_INCREASE", 50);
	addVanillaSingleValue(\%valueContainer, "GREAT_GENERALS_THRESHOLD_INCREASE_TEAM", 50);
	addVanillaSingleValue(\%valueContainer, "GREAT_GENERALS_THRESHOLD", 30);
	addVanillaSingleValue(\%valueContainer, "IMMIGRATION_THRESHOLD_INCREASE", 20);
	addVanillaSingleValue(\%valueContainer, "IMMIGRATION_THRESHOLD", 5);
	addVanillaSingleValue(\%valueContainer, "EUROPE_MARKET_CORRELATION_PERCENT", 0);
	addVanillaSingleValue(\%valueContainer, "REVOLUTION_EUROPE_UNIT_THRESHOLD_INCREASE", 25);
	addVanillaSingleValue(\%valueContainer, "REVOLUTION_EUROPE_UNIT_THRESHOLD", 75);
	addVanillaSingleValue(\%valueContainer, "KING_INITIAL_UNIT_INCREASE", 4);
	addVanillaSingleValue(\%valueContainer, "REVOLUTION_EUROPE_UNIT_SHIP_MODIFIER", -50);
	addVanillaSingleValue(\%valueContainer, "EDUCATION_THRESHOLD_INCREASE", 15);
	addVanillaSingleValue(\%valueContainer, "EDUCATION_THRESHOLD", 30);
	addVanillaSingleValue(\%valueContainer, "MISSIONARY_THRESHOLD_INCREASE", 25);
	addVanillaSingleValue(\%valueContainer, "MISSIONARY_THRESHOLD", 1000);
	addVanillaSingleValue(\%valueContainer, "MISSIONARY_RATE_EFFECT_ON_SUCCESS", 50);
	addVanillaSingleValue(\%valueContainer, "NATIVE_TEACH_THRESHOLD_INCREASE", 50);
	addVanillaSingleValue(\%valueContainer, "LAKE_PLOT_RAND", 160);
	addVanillaSingleValue(\%valueContainer, "PLOTS_PER_RIVER_EDGE", 12);
	addVanillaSingleValue(\%valueContainer, "RIVER_SOURCE_MIN_RIVER_RANGE", 4);
	addVanillaSingleValue(\%valueContainer, "RIVER_SOURCE_MIN_SEAWATER_RANGE", 2);
	addVanillaSingleValue(\%valueContainer, "LAKE_MAX_AREA_SIZE", 9);
	addVanillaSingleValue(\%valueContainer, "HILLS_EXTRA_MOVEMENT", 1);
	addVanillaSingleValue(\%valueContainer, "PEAK_EXTRA_MOVEMENT", 2);
	addVanillaSingleValue(\%valueContainer, "FEATURE_GROWTH_MODIFIER", 25);
	addVanillaSingleValue(\%valueContainer, "ROUTE_FEATURE_GROWTH_MODIFIER", -50);
	addVanillaSingleValue(\%valueContainer, "EXTRA_YIELD", 1);
	addVanillaSingleValue(\%valueContainer, "FORTIFY_MODIFIER_PER_TURN", 5);
	addVanillaSingleValue(\%valueContainer, "MAX_FORTIFY_TURNS", 5);
	addVanillaSingleValue(\%valueContainer, "FATHER_COST_EXTRA_TEAM_MEMBER_MODIFIER", 50);
	addVanillaSingleValue(\%valueContainer, "PEACE_TREATY_LENGTH", 10);
	addVanillaSingleValue(\%valueContainer, "COLONIAL_FORCED_PEACE_TURNS", 20);
	addVanillaSingleValue(\%valueContainer, "OCCUPATION_CULTURE_PERCENT_THRESHOLD", 75);
	addVanillaSingleValue(\%valueContainer, "BASE_OCCUPATION_TURNS", 3);
	addVanillaSingleValue(\%valueContainer, "OCCUPATION_TURNS_POPULATION_PERCENT", 50);
	addVanillaSingleValue(\%valueContainer, "DIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT", 67);
	addVanillaSingleValue(\%valueContainer, "UNIT_PRODUCTION_PERCENT", 100);
	addVanillaSingleValue(\%valueContainer, "BUILDING_PRODUCTION_PERCENT", 100);
	addVanillaSingleValue(\%valueContainer, "MAXED_UNIT_GOLD_PERCENT", 100);
	addVanillaSingleValue(\%valueContainer, "MAXED_BUILDING_GOLD_PERCENT", 100);
	addVanillaSingleValue(\%valueContainer, "MAX_CITY_DEFENSE_DAMAGE", 100);
	addVanillaSingleValue(\%valueContainer, "CITY_DEFENSE_DAMAGE_HEAL_RATE", 5);
	addVanillaSingleValue(\%valueContainer, "RAZING_CULTURAL_PERCENT_THRESHOLD", 25);
	addVanillaSingleValue(\%valueContainer, "BASE_CAPTURE_GOLD", 20);
	addVanillaSingleValue(\%valueContainer, "CAPTURE_GOLD_PER_POPULATION", 10);
	addVanillaSingleValue(\%valueContainer, "CAPTURE_GOLD_RAND1", 50);
	addVanillaSingleValue(\%valueContainer, "CAPTURE_GOLD_RAND2", 50);
	addVanillaSingleValue(\%valueContainer, "CAPTURE_GOLD_MAX_TURNS", 50);
	addVanillaSingleValue(\%valueContainer, "MIN_EXPERIENCE_PER_COMBAT", 1);
	addVanillaSingleValue(\%valueContainer, "MAX_EXPERIENCE_PER_COMBAT", 10);
	addVanillaSingleValue(\%valueContainer, "EXPERIENCE_FROM_WITHDRAWL", 1);
	addVanillaSingleValue(\%valueContainer, "MAX_EXPERIENCE_AFTER_UPGRADE", 10);
	addVanillaSingleValue(\%valueContainer, "BUILDING_PRODUCTION_DECAY_TIME", 50);
	addVanillaSingleValue(\%valueContainer, "BUILDING_PRODUCTION_DECAY_PERCENT", 99);
	addVanillaSingleValue(\%valueContainer, "UNIT_PRODUCTION_DECAY_TIME", 10);
	addVanillaSingleValue(\%valueContainer, "UNIT_PRODUCTION_DECAY_PERCENT", 98);
	addVanillaSingleValue(\%valueContainer, "BASE_UNIT_UPGRADE_COST", 20);
	addVanillaSingleValue(\%valueContainer, "UNIT_UPGRADE_COST_PER_PRODUCTION", 3);
	addVanillaSingleValue(\%valueContainer, "WAR_SUCCESS_DEFENDING", 3);
	addVanillaSingleValue(\%valueContainer, "WAR_SUCCESS_ATTACKING", 4);
	addVanillaSingleValue(\%valueContainer, "WAR_SUCCESS_UNIT_CAPTURING", 1);
	addVanillaSingleValue(\%valueContainer, "WAR_SUCCESS_CITY_CAPTURING", 10);
	addVanillaSingleValue(\%valueContainer, "DIPLOMACY_VALUE_REMAINDER", 10);
	addVanillaSingleValue(\%valueContainer, "SCORE_POPULATION_FACTOR", 0);
	addVanillaSingleValue(\%valueContainer, "SCORE_LAND_FACTOR", 4000);
	addVanillaSingleValue(\%valueContainer, "SCORE_FATHER_FACTOR", 4000);
	addVanillaSingleValue(\%valueContainer, "SCORE_FREE_PERCENT", 0);
	addVanillaSingleValue(\%valueContainer, "SCORE_VICTORY_PERCENT", 0);
	addVanillaSingleValue(\%valueContainer, "SCORE_HANDICAP_PERCENT_OFFSET", -60);
	addVanillaSingleValue(\%valueContainer, "SCORE_HANDICAP_PERCENT_PER", 20);
	addVanillaSingleValue(\%valueContainer, "SCORE_TAX_FACTOR", 100);
	addVanillaSingleValue(\%valueContainer, "WARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT", 0);
	addVanillaSingleValue(\%valueContainer, "COMBAT_EXPERIENCE_IN_BORDERS_PERCENT", 100);
	addVanillaSingleValue(\%valueContainer, "VICTORY_SOUNDTRACK_AVAILABLE", 0);
	addVanillaSingleValue(\%valueContainer, "MAX_WITHDRAWAL_PROBABILITY", 90);
	addVanillaSingleValue(\%valueContainer, "PLAYER_ALWAYS_RAZES_CITIES", 0);
	addVanillaSingleValue(\%valueContainer, "MIN_WATER_SIZE_FOR_OCEAN", 10);
	addVanillaSingleValue(\%valueContainer, "AI_CAN_DISBAND_UNITS", 1);
	addVanillaSingleValue(\%valueContainer, "ADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES", 0);
	addVanillaSingleValue(\%valueContainer, "ADVANCED_START_MAX_UNITS_PER_CITY", -1);
	addVanillaSingleValue(\%valueContainer, "ADVANCED_START_SIGHT_RANGE", 4);
	addVanillaSingleValue(\%valueContainer, "ADVANCED_START_CITY_COST", 67);
	addVanillaSingleValue(\%valueContainer, "ADVANCED_START_CITY_COST_INCREASE", 0);
	addVanillaSingleValue(\%valueContainer, "ADVANCED_START_POPULATION_COST", 15);
	addVanillaSingleValue(\%valueContainer, "ADVANCED_START_POPULATION_COST_INCREASE", 0);
	addVanillaSingleValue(\%valueContainer, "ADVANCED_START_CULTURE_COST", 25);
	addVanillaSingleValue(\%valueContainer, "ADVANCED_START_VISIBILITY_COST", 2);
	addVanillaSingleValue(\%valueContainer, "ADVANCED_START_VISIBILITY_COST_INCREASE", 3);
	addVanillaSingleValue(\%valueContainer, "ADVANCED_START_CITY_PLACEMENT_MAX_RANGE", 6);
	addVanillaSingleValue(\%valueContainer, "NEW_CITY_BUILDING_VALUE_MODIFIER", -60);
	addVanillaSingleValue(\%valueContainer, "EVENT_PROBABILITY_ROLL_SIDES", 100);
	addVanillaSingleValue(\%valueContainer, "FIRST_EVENT_DELAY_TURNS", 0);
	addVanillaSingleValue(\%valueContainer, "CIVILOPEDIA_SHOW_ACTIVE_CIVS_ONLY", 0);
	addVanillaSingleValue(\%valueContainer, "USE_MODDERS_PLAYEROPTION_1", 0);
	addVanillaSingleValue(\%valueContainer, "USE_MODDERS_PLAYEROPTION_2", 0);
	addVanillaSingleValue(\%valueContainer, "USE_MODDERS_PLAYEROPTION_3", 0);
	addVanillaSingleValue(\%valueContainer, "CITY_YIELD_CAPACITY", 100);
	addVanillaSingleValue(\%valueContainer, "CITY_YIELD_DECAY_PERCENT", 10);
	addVanillaSingleValue(\%valueContainer, "MIN_CITY_YIELD_DECAY", 5);
	addVanillaSingleValue(\%valueContainer, "BUY_PLOT_MIN_CULTURE", 20);
	addVanillaSingleValue(\%valueContainer, "BUY_PLOT_BASE_CULTURE_COST", 1);
	addVanillaSingleValue(\%valueContainer, "BUY_PLOT_OWNED_COST_MODIFIER", 100);
	addVanillaSingleValue(\%valueContainer, "BUY_PLOT_CULTURE_RANGE", 1);
	addVanillaSingleValue(\%valueContainer, "BUY_PLOT_SELLER_INCOME_PERCENT", 90);
	addVanillaSingleValue(\%valueContainer, "ENABLE_OPEN_BORDERS", 1);
	addVanillaSingleValue(\%valueContainer, "ENABLE_GOLD_TRADING", 1);
	addVanillaSingleValue(\%valueContainer, "ENABLE_MAP_TRADING", 1);
	addVanillaSingleValue(\%valueContainer, "ENABLE_DEFENSIVE_PACT_TRADING", 1);
	addVanillaSingleValue(\%valueContainer, "ENABLE_ALLIANCE_TRADING", 1);
	addVanillaSingleValue(\%valueContainer, "TAX_TRADE_THRESHOLD", 200);
	addVanillaSingleValue(\%valueContainer, "TAX_TRADE_THRESHOLD_TAX_RATE_PERCENT", 1000);
	addVanillaSingleValue(\%valueContainer, "TAX_TRADE_THRESHOLD_ATTITUDE_PERCENT", 100);
	addVanillaSingleValue(\%valueContainer, "TAX_INCREASE_CHANCE", 20);
	addVanillaSingleValue(\%valueContainer, "TAX_RATE_MAX_INCREASE", 5);
	addVanillaSingleValue(\%valueContainer, "NO_BAD_GOODIES_GOLD_PERCENT", 200);
	addVanillaSingleValue(\%valueContainer, "NO_BAD_GOODIES_EXPERIENCE_PERCENT", 150);
	addVanillaSingleValue(\%valueContainer, "REBEL_SENTIMENT_BELLS_FACTOR", 25);
	addVanillaSingleValue(\%valueContainer, "REBEL_SENTIMENT_TURN_WEIGHT", 10);
	addVanillaSingleValue(\%valueContainer, "MAX_REBEL_YIELD_MODIFIER", 50);
	addVanillaSingleValue(\%valueContainer, "MAX_REBEL_COMBAT_MULTIPLER", 100);
	addVanillaSingleValue(\%valueContainer, "REBEL_PERCENT_FOR_REVOLUTION", 50);
	addVanillaSingleValue(\%valueContainer, "SETTLEMENT_TREASURE_YIELD", 100);
	addVanillaSingleValue(\%valueContainer, "KING_BUY_UNIT_PRICE_MODIFIER", 50);
	addVanillaSingleValue(\%valueContainer, "KING_TRANSPORT_TREASURE_COMISSION", 50);
	addVanillaSingleValue(\%valueContainer, "EDUCATION_BASE_TUITION", 1000);
	addVanillaSingleValue(\%valueContainer, "DOCKS_NEXT_UNITS", 3);
	addVanillaSingleValue(\%valueContainer, "CONSUME_EQUIPMENT_ON_FOUND", 0);
	addVanillaSingleValue(\%valueContainer, "FEATURE_PRODUCTION_YIELD_MAX_DISTANCE", 6);
	addVanillaSingleValue(\%valueContainer, "NATIVE_GOODS_RAID_PERCENT", 50);
	addVanillaSingleValue(\%valueContainer, "NATIVE_GROWTH_THRESHOLD_MULTIPLIER", 100);
	addVanillaSingleValue(\%valueContainer, "END_GAME_DISPLAY_WARNING", 100);
}
