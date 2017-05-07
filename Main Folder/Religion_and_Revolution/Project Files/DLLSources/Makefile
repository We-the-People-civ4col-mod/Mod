#### Civilization 4 SDK Makefile 2.3 ####
####  Copyright 2013 Nightinggale    ####
#########################################
#### Civilization 4 SDK Makefile 2.0 ####
####  Copyright 2013 Nightinggale    ####
#########################################
#### Civilization 4 SDK Makefile 1.0 ####
####  Copyright 2010 Danny Daemonic  ####
#########################################

## User setup
##
## Any setting here can be overwritten by assigning a new value to the variables in Makefile.settings
##  Makefile.settings is made automatically if it is missing whenever the project is build or cleaned
##
## Not touching the makefile itself can be beneficial if the makefile is committed to svn/git
## Makefile.settings can also accept CUSTOM_CFLAGS and CUSTOM_LDFLAGS if you want to ADD additional flags
##   the custom flags will not overwrite any flags the makefile sets itself
##   An example could be you want to define ENABLE_MY_DEBUG_OUTPUT in C+, then you write
##     CUSTOM_CFLAGS -DENABLE_MY_DEBUG_OUTPUT
##     in Makefile.settings and it will be defined for you without the risk of committing active debug output to other people
##
##     You can write whatever you want after -D and it will work like you write #DEFINE in C++
## 


#### Paths ####
#
# Note: $(PROGRAMFILES) converts to "C:\Program Files", "C:\Program Files (x86)" or whatever fits your system.
# run "nmake.exe /P" in cmd.exe to see what it is on your system
TOOLKIT=D:\Games and Patches\Colonization\Programmieren\Microsoft Visual C++ Toolkit 2003
PSDK=D:\Games and Patches\Colonization\Programmieren\WindowsSDK
## Uncomment to have newly compiled dlls copied to your mod's Assets directory
YOURMOD=D:\Games and Patches\Colonization\Programmieren\fertig kompiliert

## Civ install path
## Path to the directory where boost and python is stored
## Overwritten by enviroment variable CIV4_LIB_INSTALL_PATH
CIV4_PATH=C:\Spiele\Firaxis Games\Sid Meier's Civilization IV Colonization\CvGameCoreDLL

#### Tools ####
CC="$(TOOLKIT)\bin\cl.exe"
CPP="$(TOOLKIT)\bin\cl.exe"
LD="$(TOOLKIT)\bin\link.exe"
RC="$(PSDK)\bin\rc.exe"
## Uncomment to build dependencies using fastdep
## Overwritten by enviroment variable FASTDEP_FULL_PATH
FD="$(MAKEDIR)\bin\fastdep.exe"

#### BLACKLIST ####
## Uncomment to block CvTextScreen (accidentally included by Firaxis)
BLACKLIST=CvTextScreens

#### DEFINES ####
## copy the following lines into "Additional options" in NMAKE properties
## This will make VC++ aware of what is defined and what isn't, which affects the help popup and red lines
## Use Release_DEFINES for all targets except debug
## Assert adds /DFASSERT_ENABLE, which should be added to additional options too, if you care.
## However you will only be able to tell a difference if you do something with asserts other than FAssert() or FAssertMsg()

Debug_DEFINES=/DWIN32 /D_WINDOWS /D_USRDLL /DCVGAMECOREDLL_EXPORTS /D_DEBUG
Release_DEFINES=/DWIN32 /D_WINDOWS /D_USRDLL /DCVGAMECOREDLL_EXPORTS /DNDEBUG /DFINAL_RELEASE

#### You shouldn't need to modify anything beyond this point ####
#################################################################

TEMP_FILE_DIR = temp_files
CUSTOM_CFLAGS  =
CUSTOM_LDFLAGS =

!IF [IF NOT EXIST Makefile.settings EXIT 1] != 0
!IF [ECHO CUSTOM_CFLAGS = > Makefile.settings]
!ENDIF
!ENDIF
include Makefile.settings

TEMP_TARGET_FILES = $(TEMP_FILE_DIR)\$(TARGET)

## Environmental path overwrite
!IFDEF CIV4_LIB_INSTALL_PATH
CIV4_PATH=$(CIV4_LIB_INSTALL_PATH)
!ENDIF

!IFDEF FASTDEP_FULL_PATH
FD=$(FASTDEP_FULL_PATH)
!ENDIF

#### Target Files ####
Target_BIN=$(TARGET)\CvGameCoreDLL.dll
Target_BIN_TEMP=$(TEMP_TARGET_FILES)\CvGameCoreDLL.dll

!IF [IF NOT EXIST CvGameCoreDLL.rc EXIT 1] == 0
Target_RESOURCE=$(TEMP_TARGET_FILES)\CvGameCoreDLL.res
!ENDIF

Target_STATICLIB=$(TEMP_TARGET_FILES)\CvGameCoreDLL.lib

Target_LIBDEF=$(TEMP_TARGET_FILES)\CvGameCoreDLL.def

Target_PCH=$(TEMP_TARGET_FILES)\CvGameCoreDLL.pch

Target_PDB=$(TEMP_TARGET_FILES)\CvGameCoreDLL.pdb

Target_OTHER=$(TEMP_TARGET_FILES)\CvGameCoreDLL.exp

!IFDEF IGNORE_CUSTOM
CUSTOM_CFLAGS  =
CUSTOM_LDFLAGS =
!ENDIF

#### CFLAGS ####
GLOBAL_CFLAGS=/GR /Gy /W3 /EHsc /Gd /Gm- /Yu"CvGameCoreDLL.h" /Fp"$(Target_PCH)" /Fd"$(Target_PDB)" $(CUSTOM_CFLAGS)
Debug_CFLAGS=/MD /Zi /Od /RTC1 $(Debug_DEFINES) $(GLOBAL_CFLAGS)
Release_CFLAGS= /MD /O2 /Oy /Oi /G7 $(Release_DEFINES) $(GLOBAL_CFLAGS)

#### LDFLAGS ####
GLOBAL_LDFLAGS=/DLL /NOLOGO /SUBSYSTEM:WINDOWS /LARGEADDRESSAWARE /TLBID:1 /PDB:"$(Target_PDB)" $(CUSTOM_LDFLAGS)
Debug_LDFLAGS=/INCREMENTAL /DEBUG /IMPLIB:"$(Target_STATICLIB)" $(GLOBAL_LDFLAGS)
Release_LDFLAGS=/INCREMENTAL:NO /OPT:REF /OPT:ICF $(GLOBAL_LDFLAGS)

#### INCLUDES ####
GLOBAL_INCS=/I"$(TOOLKIT)/include" /I"$(PSDK)/Include" /I"$(PSDK)/Include/mfc"

#### boost and python ####

BOOST_LIB_PATH =
!IF [IF NOT EXIST "$(BOOST_LIB_PATH)Boost-1.32.0\libs\boost_python-vc71-mt-gd-1_32.lib" EXIT 1] != 0
BOOST_LIB_PATH=$(CIV4_PATH)/
!IF [IF NOT EXIST "$(BOOST_LIB_PATH)Boost-1.32.0\libs\boost_python-vc71-mt-gd-1_32.lib" EXIT 1] != 0
BOOST_LIB_PATH=$(CIV4_LIB_INSTALL_PATH)/
!IF [IF NOT EXIST "$(BOOST_LIB_PATH)Boost-1.32.0\libs\boost_python-vc71-mt-gd-1_32.lib" EXIT 1] != 0
!MESSAGE FATAL ERROR: Failed to locate boost and python
!ENDIF
!ENDIF
!ENDIF

PROJECT_INCS=/I"$(BOOST_LIB_PATH)Boost-1.32.0/include" /I"$(BOOST_LIB_PATH)Python24/include"
PROJECT_LIBS=/LIBPATH:"$(BOOST_LIB_PATH)Python24/libs" /LIBPATH:"$(BOOST_LIB_PATH)boost-1.32.0/libs/" boost_python-vc71-mt-1_32.lib

INCS = $(PROJECT_INCS) $(GLOBAL_INCS)

#### LIBS ####
GLOBAL_LIBS=/LIBPATH:"$(TOOLKIT)/lib" /LIBPATH:"$(PSDK)/Lib" winmm.lib user32.lib
LIBS = $(PROJECT_LIBS) $(GLOBAL_LIBS)

#### Auto SOURCES/OBJS ####
!IF [ECHO SOURCES= \> sources.mk] == 0 && \
    [FOR %i IN (*.cpp) DO @ECHO. "%i" \>> sources.mk] == 0 && \
    [ECHO.>> sources.mk] == 0 && \
	[ECHO OBJS= \>> sources.mk] == 0 && \
    [FOR /F "delims=." %i IN ('dir /b *.cpp') DO @ECHO. $(TEMP_TARGET_FILES)\%i.obj \>> sources.mk] == 0 && \
    [ECHO.>> sources.mk] == 0
!INCLUDE sources.mk
!IF [DEL sources.mk]
!ENDIF
!ENDIF

!IF "$(TARGET)" == "Debug"
!IFNDEF _NMAKE_VER
!ERROR Jom can't be used on Debug builds
!ENDIF
CFLAGS  = $(Debug_CFLAGS)
LDFLAGS = $(Debug_LDFLAGS)
LIBS    = $(LIBS) msvcprt.lib
!ENDIF
!IF "$(TARGET)" == "Release"
CFLAGS  = $(Release_CFLAGS)
LDFLAGS = $(Release_LDFLAGS)
!ENDIF
!IF "$(TARGET)" == "Assert"
CFLAGS  = $(Release_CFLAGS) /DFASSERT_ENABLE
LDFLAGS = $(Release_LDFLAGS)
!ENDIF
!IF "$(TARGET)" == "Profile"
!IFNDEF _NMAKE_VER
!ERROR Jom can't be used on profile builds
!ENDIF
CFLAGS  = $(Release_CFLAGS) /Zi
LDFLAGS = $(Debug_LDFLAGS)
!ENDIF

!IFNDEF CFLAGS
!ERROR Target "$(TARGET)" not supported. Supported targets: Debug Release Assert Profile
!ENDIF

#### Targets ####
#################

.PHONY: all clean build precompile Make_Dir

all: Precompile build

dll: all

clean:
	@FOR %i IN ($(Target_BIN) $(Target_BIN_TEMP) $(Target_STATICLIB) $(Target_LIBDEF) \
		$(TEMP_TARGET_FILES)\*.obj $(TEMP_TARGET_FILES)\*.@ $(Target_RESOURCE) \
		$(Target_PCH) $(Target_PDB) $(Target_OTHER)) DO @IF EXIST "%i" DEL "%i"

$(Target_BIN): $(Target_BIN_TEMP)
	@-COPY "$(Target_BIN_TEMP)" "$(Target_BIN)"

		
build: $(Target_BIN)
!IFDEF YOURMOD
	-COPY "$(Target_BIN)" "$(YOURMOD)\Assets\."
!ENDIF

precompile: Make_Dir Target_unfinished $(Target_PCH)

Make_Dir:
	-@IF NOT EXIST "$(TEMP_TARGET_FILES)\." MKDIR "$(TEMP_TARGET_FILES)"
	-@IF NOT EXIST "$(TARGET)\." MKDIR "$(TARGET)"

Target_unfinished:
	@ECHO.>$(TEMP_TARGET_FILES)\unfinished.@
	@FOR /F "delims=@" %i IN ('dir /b $(TEMP_TARGET_FILES)\*.@') DO \
		@IF EXIST "$(TEMP_TARGET_FILES)\%i" DEL "$(TEMP_TARGET_FILES)\%i"
	@FOR /F %i IN ('dir /b $(TEMP_TARGET_FILES)\*.@') DO \
		@IF EXIST "$(TEMP_TARGET_FILES)\%i" DEL "$(TEMP_TARGET_FILES)\%i"

$(Target_BIN_TEMP): $(OBJS) $(Target_RESOURCE)
	@ECHO.Linking DLL
	@$(LD) /out:$(Target_BIN_TEMP) $(LDFLAGS) $(LIBS) $(OBJS) $(Target_RESOURCE)

.cpp{$(TEMP_TARGET_FILES)}.obj:
	@ECHO.>"$*.obj.@"
    @$(CPP) /nologo $(CFLAGS) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

$(Target_PCH) $(TEMP_TARGET_FILES)\_precompile.obj:
	@ECHO.>"$(Target_PCH).@"
	@ECHO.>"$(TEMP_TARGET_FILES)\_precompile.obj.@"
    @$(CPP) /nologo $(CFLAGS) $(INCS) /YcCvGameCoreDLL.h /Fo"$(TEMP_TARGET_FILES)\_precompile.obj" /c _precompile.cpp
	@DEL "$(Target_PCH).@"
	@DEL "$(TEMP_TARGET_FILES)\_precompile.obj.@"

.rc{$(TEMP_TARGET_FILES)}.res:
	@ECHO.>"$*.res.@"
	$(RC) /Fo$@ $(INCS) $<
	@DEL "$*.res.@"

!IFDEF BLACKLIST
$(TEMP_TARGET_FILES)\$(BLACKLIST).obj: $(BLACKLIST).cpp
	@ECHO.>"$*.obj.@"
	@ECHO.>"$*-dummy.cpp"
	@$(CPP) /nologo $(Release_CFLAGS) $(Release_INCS) /Y- /Fo$@ /c "$*-dummy.cpp"
	@DEL "$*-dummy.cpp"
	@DEL "$*.obj.@"

!ENDIF


!IFDEF FD

# jom can't handle FD
# make sure only NMAKE will try
# jom only executes right after nmake generated the needed file anyway
!IFDEF _NMAKE_VER

!IF [IF NOT EXIST $(FD) EXIT 1] == 0
!IF [$(FD) --objectextension=pch -q -O $(TEMP_TARGET_FILES) CvGameCoreDLL.cpp > $(TEMP_TARGET_FILES)\depends] != 0 || \
    [$(FD) --objectextension=obj -q -O $(TEMP_TARGET_FILES) $(SOURCES) >> $(TEMP_TARGET_FILES)\depends] != 0
!MESSAGE Error running fastdep.
!MESSAGE $(TARGET)-fast causes this issue if $(TARGET) or $(TARGET)-fast has never been compiled before.
!MESSAGE It is safe to ignore this error in that case, but only in that case!
!ENDIF
!ELSE
!IF [ECHO "fastdep.exe" NOT FOUND! && \
     ECHO Please edit Makefile to reflect the correct path of fastdep. && \
     ECHO. ]
!ENDIF
!ENDIF

!ENDIF

!ENDIF

!IF EXIST($(TEMP_TARGET_FILES)\depends)
!INCLUDE $(TEMP_TARGET_FILES)\depends
!ENDIF
