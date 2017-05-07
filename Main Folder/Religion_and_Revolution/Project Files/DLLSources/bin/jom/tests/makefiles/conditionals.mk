TEST1=false
!if 1
TEST1=true
!elseif 0
TEST1=false
!elseif 0
TEST1=false
!endif

TEST2=false
!if 0
TEST2=false
!elseif 0
# some comment
# more to say...
NOOP=1
TEST2=false
!elseif 1
NOOP=1
TEST2=true
NOOP=1
!endif

TEST3=false
!if 0 # comment
TEST3=false
!elseif 1#another comment
TEST3=true
!elseif 1 # comment
TEST3=false
!endif

TEST4=false
!IFDEF TEST1
TEST4=true
!ELSEIFDEF NOT_DEFINED
TEST4=false
!ELSE
TEST4=false
!ENDIF

TEST5=false
!IFDEF NOT_DEFINED
TEST5=false
!ELSEIFDEF TEST1
TEST5=true
!ELSE
TEST5=false
!ENDIF

TEST6=false
!IFDEF NOT_DEFINED
TEST6=false
!ELSEIFDEF NOT_DEFINED
TEST6=false
!ELSE
TEST6=true
!ENDIF

TEST7=false
!IFDEF NOT_DEFINED
TEST7=false
!ELSEIFNDEF NOT_DEFINED
TEST7=true
!ELSE
TEST7=false
!ENDIF

TEST8=false
!IFNDEF NOT_DEFINED
TEST8=true
!ELSEIFNDEF NOT_DEFINED
TEST8=false
!ELSE
TEST8=false
!ENDIF

TEST9=foo \
bar \
!IF 1
baz \
!ELSE
boo \
hoo
!ENDIF

TEST10=foo \
bar \
!IF 0
baz \
!ELSE
boo \
hoo
!ENDIF

all:

