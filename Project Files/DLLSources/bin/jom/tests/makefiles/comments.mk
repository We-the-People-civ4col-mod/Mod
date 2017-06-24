# this is just a comment
!IF 1 # - 1
COMPILER = Ada95    # here's a comment after a macro definition
DEF = ^#define  # ignore me

first: second third    # la la la la
	echo I'm Winneone
# foooooo
# baaaaar
	echo I'm Winnetou
# Wie viele Winnes haben wir in gesamt?
second:
third:

.cpp.obj: # and another one
    echo inference rule applied

forth: fifth ; echo # this is no comment
    echo # this neither
fifth: file^#99.txt # This is a comment!
    echo $**
!ENDIF

.SILENT: # comment after dot directive

