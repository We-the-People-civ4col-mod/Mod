use strict;
use warnings;

use File::Slurp;

use lib './bin';
use XMLlists;


my $FILE_INFO = $ARGV[0] // (getAutoDir() . "/AutoInfoArray.h");

my $output_info = "#include \"CvEnums.h\"\n\n";


my @types = ( {}, {}, {}, {});

my @TypesToIgnore;

push(@TypesToIgnore, "JIT_NoneTypes");
push(@TypesToIgnore, "T");
push(@TypesToIgnore, "Ta");
push(@TypesToIgnore, "Tb");
push(@TypesToIgnore, "T0");
push(@TypesToIgnore, "T1");
push(@TypesToIgnore, "T2");
push(@TypesToIgnore, "T3");
push(@TypesToIgnore, "IndexType");

my $dir = "DLLSources/";

{
	opendir my($dirhandle), $dir;
	for( readdir $dirhandle ){ # sets $_
	next if substr($_, 0, 1) eq ".";

	my $ext = substr($_, rindex($_, "."));
	handleFile($_) if $ext eq ".cpp" or $ext eq ".h";
  }
  # $dirhandle is automatically closed here
}

for my $i (0 .. $#types )
{
	# sort is very important here. Without it the order will be random, hence force recompiling each time
    for my $type (sort keys %{ $types[$i] } )
	{
		next if (grep( /^$type$/, @TypesToIgnore ));
		handleInfoArraySingle($type, $i);
    }
}
writeFile($FILE_INFO, \$output_info);

sub handleFile
{
	my $file = shift;

	my $path = $dir . $file;

	open my $fileHandle, $path or die "Could not open $path: $!";

	while( my $line = <$fileHandle>)
	{
		my $index = index($line, "InfoArray<");
		next if $index == -1;

		my $info = substr($line, $index);
		$info = substr($info, 0, index($info, ">"));
		$info = substr($info, index($info, "<")+1);

		my $i = 0;
		foreach my $token (split(', ', $info))
		{
			handleToken($token, $i);
			$i += 1;
		}
	}

	close $fileHandle;
}

sub handleToken
{
	my $token = shift;
	my $index = shift;

	# remove leading and trailing whitespace
	$token=~ s/^\s+|\s+$//g;

	$types[$index]{$token} = 1;
}

sub handleInfoArraySingle
{
	my $type = shift;
	my $index = shift;
	my $id = $index + 1;
	my $get = "get" . removeType($type);

	$get = "getFontSymbol" if $get eq "getFontSymbols";

	$output_info .= "template<" . addtemplates("typename T", $id, 0) . ">\nclass InfoArray$id<" . addtemplates("typename T", $id, 1) . $type . ">\n\t: ";
	$output_info .= "public InfoArray$index<" . addtemplates("T", $id, 0) . ">\n" unless $id == 1;
	$output_info .= "protected InfoArrayBase\n\t, public boost::noncopyable\n" if $id == 1;
	$output_info .= "{\n";
	$output_info .= "\tfriend class CyInfoArray;\n";
	$output_info .= "public:\n";
	if ($id == 1)
	{
		$output_info .= "\tint getLength() const\n\t{\n\t\treturn InfoArrayBase::getLength();\n\t}\n";
		$output_info .= "\t$type get(int iIndex) const\n";
		$output_info .= "\t{\n";
		$output_info .= "\t\treturn static_cast<$type>(getInternal(iIndex, $index));\n";
		$output_info .= "\t}\n";
	}
	$output_info .= "\t$type get$index(int iIndex) const\n";
	$output_info .= "\t{\n";
	$output_info .= "\t\treturn static_cast<$type>(getInternal(iIndex, $index));\n";
	$output_info .= "\t}\n";
	$output_info .= "\t$type $get(int iIndex) const\n";
	$output_info .= "\t{\n";
	$output_info .= "\t\treturn static_cast<$type>(getInternal(iIndex, $index));\n";
	$output_info .= "\t}\n";
	$output_info .= "\tint getIndexOf($type eValue) const\n";
	$output_info .= "\t{\n";
	$output_info .= "\t\treturn _getIndexOf(eValue, $index);\n";
	$output_info .= "\t}\n";
	$output_info .= "protected:\n";
	$output_info .= "friend class CvCity;\n" if $id == 1;
	$output_info .= "friend class CvGlobals;\n" if $id == 1;
	$output_info .= "friend class CivEffectInfo;\n" if $id == 1;
	$output_info .= "friend class CvPlayerCivEffect;\n" if $id == 1;
	$output_info .= "friend class CvInfoBase;\n" if $id == 1;
	$output_info .= "\tInfoArray$id(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3)\n";
	$output_info .= "\t\t: InfoArray" . ($id - 1) . "<" . addtemplates("T", $id, 0) . ">(eType0, eType1, eType2, eType3) {}\n" unless $id == 1;
	$output_info .= "\t\t: InfoArrayBase(eType0, eType1, eType2, eType3) {}\n" if $id == 1;
	$output_info .= "};\n";
}

sub removeType
{
	my $type = shift;

	return "Int" if $type eq "int";

	if (substr($type, -5) eq "Types")
	{
		$type = substr($type, 0, -5);
	}
	return $type;
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
