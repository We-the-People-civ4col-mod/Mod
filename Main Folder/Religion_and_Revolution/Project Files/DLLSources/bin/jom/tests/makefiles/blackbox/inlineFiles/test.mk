# test inline files in make files
#
# http://msdn.microsoft.com/en-us/library/1whxt45w.aspx

all:
	@echo Please specify a target.

tests: test_basic test_fileRemoval test_keepFile test_multipleFiles test_escaping

init:
	@if exist output rmdir /s /q output
	@md output
	@echo ::post test check script > output\post_check.cmd

post_check:
	@output\post_check.cmd

test_basic:
	copy << output\test_basic.txt
line 1
!ifndef UNDEFINED_MACRO
line 2
!endif
line 3
<<

test_fileRemoval:
	echo @if exist << exit /b 1 >> output\post_check.cmd
some random text
<<
	echo @if exist <<namedInlineFile1.txt exit /b 1 >> output\post_check.cmd
some random text
<<
	echo @if exist <<namedInlineFile2.txt exit /b 1 >> output\post_check.cmd
some random text
<<NOKEEP

test_keepFile:
	echo @if not exist <<"output\named inline file3.txt" exit /b 1 >> output\post_check.cmd
some random text
<<KEEP

test_multipleFiles:
	type << << >output\test_multipleFiles.txt
one
two
<<
three
four
<<

test_escaping:
    copy << output\test_escaping.txt
InRoot$$$$Sections
# This line should be there.
<<
