#!/usr/bin/perl -w


use strict;
use warnings;

#use lib './bin';
use File::Slurp;


my $file = "DLLSources/autogenerated/AutoVariableFunctions.h";

my %var;
my $output = "";
my @comparison_operators = ("==", "!=", "> ", "< ", ">=", "<=");


$var{Achieve}          = {not_strict => 1};
$var{AreaAI}           = {not_strict => 1, JIT => "NO_JIT_ARRAY_TYPE"};
$var{ArtStyle}         = {not_strict => 1};
$var{Bonus}            = {not_strict => 1};
$var{Build}            = {not_strict => 1};
$var{Building}         = {not_strict => 1};
$var{BuildingClass}    = {not_strict => 1};
$var{SpecialBuilding}  = {not_strict => 1, JIT => "JIT_ARRAY_BUILDING_SPECIAL"};
$var{CivCategory}      = {not_strict => 1, JIT => "JIT_ARRAY_CIV_CATEGORY", NUM => "NUM_CIV_CATEGORY_TYPES", COMPILE => "COMPILE_TIME_NUM_CIV_CATEGORY_TYPES"};
$var{CivEffect}        = {not_strict => 1, JIT => "JIT_ARRAY_CIV_EFFECT", NUM => "NUM_CIV_EFFECT_TYPES", COMPILE => "COMPILE_TIME_NUM_CIV_EFFECT_TYPES"};
$var{Civic}            = {not_strict => 1};
$var{CivicOption}      = {not_strict => 1};
$var{Civilization}     = {not_strict => 1};
$var{CityPlot}         = {not_strict => 1, JIT => "NO_JIT_ARRAY_TYPE", NUM => "NUM_CITY_PLOTS", COMPILE => "NUM_CITY_PLOTS_2_PLOTS"};
$var{Climate}          = {not_strict => 1};
$var{Contact}          = {not_strict => 1};
$var{Culture}          = {not_strict => 1, type => "CultureLevelTypes", NUM => "NUM_CULTURELEVEL_TYPES", COMPILE => "COMPILE_TIME_NUM_CULTURELEVEL_TYPES"};
$var{Diplomacy}        = {};
$var{Domain}           = {not_strict => 1};
$var{Emotion}          = {not_strict => 1};
$var{Era}              = {not_strict => 1};
$var{Emphasize}        = {not_strict => 1};
$var{Europe}           = {not_strict => 1};
$var{Event}            = {not_strict => 1};
$var{EventTrigger}     = {not_strict => 1, JIT => "JIT_ARRAY_EVENT_TRIGGER"};
$var{Father}           = {not_strict => 1};
$var{FatherPoint}      = {not_strict => 1, JIT => "JIT_ARRAY_FATHER_POINT", NUM => "NUM_FATHER_POINT_TYPES", COMPILE => "COMPILE_TIME_NUM_FATHER_POINT_TYPES"};
$var{Feat}             = {not_strict => 1};
$var{Feature}          = {not_strict => 1};
$var{GameOption}       = {not_strict => 1, JIT => "JIT_ARRAY_GAME_OPTION"};
$var{GameSpeed}        = {not_strict => 1, JIT => "JIT_ARRAY_GAME_SPEED"};
$var{Goody}            = {not_strict => 1};
$var{Handicap}         = {not_strict => 1};
$var{Hurry}            = {not_strict => 1};
$var{Improvement}      = {not_strict => 1};
$var{Invisible}        = {not_strict => 1};
$var{LeaderHead}       = {not_strict => 1, JIT => "JIT_ARRAY_LEADER", NUM => "NUM_LEADER_TYPES", COMPILE => "COMPILE_TIME_NUM_LEADER_TYPES"};
$var{Memory}           = {not_strict => 1};
$var{Player}           = {not_strict => 1};
$var{PlayerColor}      = {not_strict => 1, JIT => "JIT_ARRAY_PLAYER_COLOR"};
$var{PlayerOption}     = {not_strict => 1, JIT => "JIT_ARRAY_PLAYER_OPTION"};
$var{Plot}             = {not_strict => 1, get => "getPlotType", JIT => "JIT_ARRAY_PLOT_TYPE"};
$var{Profession}       = {not_strict => 1};
$var{Promotion}        = {not_strict => 1};
$var{Route}            = {not_strict => 1};
$var{SeaLevel}         = {not_strict => 1, JIT => "JIT_ARRAY_SEA_LEVEL"};
$var{Strategy}         = {not_strict => 1};
$var{Team}             = {not_strict => 1};
$var{Terrain}          = {not_strict => 1};
$var{Trait}            = {};
$var{Unit}             = {not_strict => 1};
$var{UnitAI}           = {not_strict => 1};
$var{UnitClass}        = {not_strict => 1};
$var{UnitCombat}       = {not_strict => 1};
$var{SpecialUnit}      = {not_strict => 1, JIT => "JIT_ARRAY_UNIT_SPECIAL"};
$var{Victory}          = {not_strict => 1};
$var{WarPlan}          = {not_strict => 1, JIT => "NO_JIT_ARRAY_TYPE"};
$var{WorldSize}        = {not_strict => 1, JIT => "JIT_ARRAY_WORLD_SIZE"};
$var{Yield}            = {not_strict => 1};






$output .= "#ifndef ENUM_SPECIALIZED_FUNCTIONS_H\n";
$output .= "#define ENUM_SPECIALIZED_FUNCTIONS_H\n";
$output .= "#include \"../CvEnumsFunctions.h\"\n\n";


foreach my $name (sort(keys %var))
{
	$var{$name}{name} = $name;
	my $NAME = uc $name;
	
	$var{$name}{type} = $name . "Types" unless exists $var{$name}{type};
	my $type = $var{$name}{type};
	$var{$name}{get} = "get" . $name unless exists $var{$name}{get};
	$var{$name}{JIT} = "JIT_ARRAY_" . $NAME unless exists $var{$name}{JIT};
	$var{$name}{NUM} = "NUM_" . $NAME . "_TYPES" unless exists $var{$name}{NUM};
	$var{$name}{COMPILE} = "COMPILE_TIME_NUM_" . $NAME . "_TYPES" unless exists $var{$name}{COMPILE};
	$var{$name}{START} = "static_cast<$type>(0)" unless exists $var{$name}{START};
	$var{$name}{END} = $var{$name}{NUM} unless exists $var{$name}{END};

	handleOperators($type);
	handleComparison($name);
	handleStruct($name);
	handleInfoArray($name);
	handleEnumMap($name);
	
	$output .= "\n"; # extra newline between different variables
}

$output .= "#endif // !ENUM_SPECIALIZED_FUNCTIONS_H\n";

my $file_content = "";
$file_content = read_file($file) if -e $file;


exit() if $file_content eq $output;

print "Writing new AutoVariableFunctions.h\n";

open (my $output_file, "> " . $file) or die "Can't open file " . $file . "\n" . $!;
print $output_file $output;
close $output_file;


sub handleOperators
{
	my $type = shift;
	
	operatorAdd($type, "+");
	operatorAdd($type, "-");
	
	operator($type, "++", 0);
	operator($type, "++", 1);
	operator($type, "--", 0);
	operator($type, "--", 1);
}

sub operator
{
	my $type = shift;
	my $operator = shift;
	my $postfix = shift;
	
	$output .= "static inline $type";
	$output .= "&" unless $postfix;
	$output .= " operator" . $operator . "($type& c";
	$output .= ", int" if $postfix;
	$output .= ")\n";
	$output .= "{\n";
	$output .= "\t" . $type . " cache = c;\n" if $postfix;
	$output .= "\tc = static_cast<$type>(c " . substr($operator, 0, 1) . " 1);\n";
	$output .= "\treturn ";
	$output .=  "c" unless $postfix;
	$output .=  "cache" if $postfix;
	$output .= ";\n";
	$output .= "};\n";
}

sub operatorAdd
{
	my $type = shift;
	my $operator = shift;
	
	$output .= "static inline $type operator" . $operator . "(const $type& A, const $type& B)\n";
	$output .= "{\n";
	$output .= "\treturn static_cast<$type>((int)A $operator (int)B);\n";
	$output .= "};\n";
}

sub handleComparison
{
	my $name = shift;
	my $type = $var{$name}{type};
	my $strict = 1;
	$strict = 0 if exists $var{$name}{not_strict};
	
	for my $operator (@comparison_operators)
	{
		singleComparison($type, "T", $operator, $strict);
		singleComparison("T", $type, $operator, $strict);
	}
}

sub singleComparison
{
	my $varA = shift;
	my $varB = shift;
	my $operator = shift;
	my $strict = shift;
	
	$output .= "template <typename T>\n";
	$output .= "static inline bool operator $operator ($varA a, $varB b)\n";
	$output .= "{\n";
	$output .= "\tconst bool bTypeCheck = boost::is_same<$varA, $varB>::value;\n" if $strict;
	$output .= "\tBOOST_STATIC_ASSERT(bTypeCheck);\n" if $strict;
	$output .= "\treturn (int)a $operator (int)b;\n";
	$output .= "};\n";
}

sub handleStruct
{
	my $name = shift;
	my $type = $var{$name}{type};
	
	$output .= "template <> struct VARINFO<$type>\n";
	$output .= "{\n";
	$output .= "\tstatic const JITarrayTypes JIT = " . $var{$name}{JIT} . ";\n";
	$output .= "\tstatic const int TYPE = (int)" . $var{$name}{COMPILE} . " < 128 ? 1 : 2;\n";
	#$output .= "\tstatic const int TYPE = (int)" . $var{$name}{COMPILE} . " < 128 ? VARIABLE_TYPE_CHAR : VARIABLE_TYPE_SHORT;\n";
	#$output .= "\tstatic const int TYPE = SIZE;\n";
	#$output .= "\tstatic const int MAX_STATIC = TYPE == VARIABLE_TYPE_CHAR ? 4 : 2;\n";
	$output .= "\tstatic const int LENGTH = " . $var{$name}{COMPILE} . ";\n";
	#$output .= "\tstatic $type start() { return " . $var{$name}{START} . ";}\n";
	#$output .= "\tstatic $type end() { return " . $var{$name}{END} . ";}\n";
	#$output .= "\tstatic bool isInRange($type eIndex) { return eIndex >= start() && eIndex < end();}\n";
	#$output .= "\ttemplate <int T> struct STATIC {\n";
	#$output .= "\t\tstatic const int VAL = T * (TYPE == VARIABLE_TYPE_CHAR ? 1 : 2) <= 4 ? VARIABLE_TYPE_STATIC : VARIABLE_TYPE_DYNAMIC;\n";
	#$output .= "\t};\n";
	$output .= "};\n";
}

sub handleInfoArray
{
	my $name = shift;
	
	return if $var{$name}{JIT} eq "NO_JIT_ARRAY_TYPE";
	
	handleInfoArraySingle($name, 1);
	handleInfoArraySingle($name, 2);
	handleInfoArraySingle($name, 3);
	handleInfoArraySingle($name, 4);

}sub handleInfoArraySingle
{
	my $name = shift;
	my $id = shift;
	my $type = $var{$name}{type};
	my $get = $var{$name}{get};
	my $index = $id - 1;
	
	$output .= "template<" . addtemplates("typename T", $id, 0) . ">\nclass InfoArray$id<" . addtemplates("typename T", $id, 1) . $type . ">\n\t: ";
	$output .= "public InfoArray$index<" . addtemplates("T", $id, 0) . ">\n" unless $id == 1;
	$output .= "protected InfoArrayBase\n\t, public boost::noncopyable\n" if $id == 1;
	$output .= "{\n";
	$output .= "public:\n";
	$output .= "\tint getLength() const\n\t{\n\t\treturn InfoArrayBase::getLength();\n\t}\n" if $id == 1;
	$output .= "\t$type get$index(int iIndex) const\n";
	$output .= "\t{\n";
	$output .= "\t\treturn static_cast<$type>(getInternal(iIndex, $index));\n";
	$output .= "\t}\n";
	$output .= "\t$type $get(int iIndex) const\n";
	$output .= "\t{\n";
	$output .= "\t\treturn static_cast<$type>(getInternal(iIndex, $index));\n";
	$output .= "\t}\n";
	$output .= "\tint getIndexOf($type eValue) const\n";
	$output .= "\t{\n";
	$output .= "\t\treturn _getIndexOf(eValue, $index);\n";
	$output .= "\t}\n";
	$output .= "protected:\n";
	$output .= "friend class CvCity;\n" if $id == 1;
	$output .= "friend class CvGlobals;\n" if $id == 1;
	$output .= "friend class CivEffectInfo;\n" if $id == 1;
	$output .= "friend class CvPlayerCivEffect;\n" if $id == 1;
	$output .= "friend class CvInfoBase;\n" if $id == 1;
	$output .= "\tInfoArray$id(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3)\n";
	$output .= "\t\t: InfoArray" . ($id - 1) . "<" . addtemplates("T", $id, 0) . ">(eType0, eType1, eType2, eType3) {}\n" unless $id == 1;
	$output .= "\t\t: InfoArrayBase(eType0, eType1, eType2, eType3) {}\n" if $id == 1;
	$output .= "};\n";
}

sub handleEnumMap
{
	my $name = shift;
	$output .= "SET_ARRAY_XML_ENUM(";
	$output .= $var{$name}{type};
	$output .= ", ";
	$output .= $var{$name}{NUM};
	$output .= ", ";
	$output .= $var{$name}{JIT};
	$output .= ", ";
	$output .= $var{$name}{COMPILE};
	$output .= ")\n";
}

sub addtemplates
{
	my $str = shift;
	my $id = shift;
	my $append_comma = shift;
	
	my $return = "";
	
	my $i = 1;
	
	return $return if $i == $id;
	$return .=  $str . "0";
	$i = 2;
	
	while ($i < $id)
	{
		$return .=  ", " . $str . ($i - 1);
		$i += 1;
	}
	
	$return .= ", " if $append_comma;
	return $return;
}
