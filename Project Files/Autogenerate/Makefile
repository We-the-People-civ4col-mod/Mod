


!IFDEF TEMP_FILE_DIR
AUTO_SOURCE=Autogenerate\Source

!ELSE

AUTO_SOURCE=Source

TEMP_FILE_DIR=..\temp_files

!include ..\Makefile.auto
TOOLKIT=..\$(TOOLKIT)
PSDK=..\$(PSDK)

GLOBAL_LIBS=/LIBPATH:"$(TOOLKIT)/lib" /LIBPATH:"$(PSDK)/Lib" winmm.lib user32.lib shell32.lib
CC="$(TOOLKIT)\bin\cl.exe"
CPP="$(TOOLKIT)\bin\cl.exe"
LD="$(TOOLKIT)\bin\link.exe"
RC="$(PSDK)\bin\rc.exe"

GLOBAL_INCS=/I"$(TOOLKIT)/include" /I"$(PSDK)/Include" /I"$(PSDK)/Include/mfc"
!ENDIF

TEMP_AUTO=$(TEMP_FILE_DIR)\Autogenerate


!IFDEF AUTO_DEBUG
# debug flags. Not used when called from the main makefile
TEMP_AUTO_TARGET=$(TEMP_AUTO)\Debug
TEMP_AUTO_Target_PDB=$(TEMP_AUTO_TARGET)\Autogenerate.pdb

AUTO_CFLAGS=/GR /Z7 /Gy /W3 /EHsc /Gd /Gm- /MD /Od /RTC1 /Fd"$(TEMP_AUTO_Target_PDB)"
AUTO_LDFLAGS=/NOLOGO /DEBUG /TLBID:1 /PDB:"$(TEMP_AUTO_Target_PDB)" /SUBSYSTEM:CONSOLE

#  /D_DEBUG
# msvcprt.lib /SUBSYSTEM:CONSOLE

!ELSE
# release flags. Always in use when called from the main makefile even if that makefile makes a debug DLL
TEMP_AUTO_TARGET=$(TEMP_AUTO)\Release
AUTO_CFLAGS=/GR /Z7 /Gy /W3 /EHsc /Gd /Gm- /MD /O2 /Oy /Oi /G7 /GL /DTEXT_ASSERT
AUTO_LDFLAGS=/NOLOGO /TLBID:1 /INCREMENTAL:NO /OPT:REF /OPT:ICF /LTCG /SUBSYSTEM:CONSOLE
!ENDIF

!IFNDEF AUTO_EXE
AUTO_EXE=$(TEMP_AUTO_TARGET)\Autogenerate.exe
!ENDIF

AUTO_ALL: $(AUTO_EXE)


AUTO_HEADER = \
	$(AUTO_SOURCE)\assert.h \
	$(AUTO_SOURCE)\Constants.h \
	$(AUTO_SOURCE)\EnumGen.h \
	$(AUTO_SOURCE)\EnumLengthSetupGen.h \
	$(AUTO_SOURCE)\EnumMap.h \
	$(AUTO_SOURCE)\FileAccess.h \
	$(AUTO_SOURCE)\LineString.h \
	$(AUTO_SOURCE)\OutputHandler.h \
	$(AUTO_SOURCE)\SourceReader.h \
	$(AUTO_SOURCE)\tinyxml2.h \
	Makefile \



AUTO_OBJ = \
	$(TEMP_AUTO_TARGET)\Main.obj \
	$(TEMP_AUTO_TARGET)\EnumGen.obj \
	$(TEMP_AUTO_TARGET)\EnumLengthSetupGen.obj \
	$(TEMP_AUTO_TARGET)\EnumMap.obj \
	$(TEMP_AUTO_TARGET)\FileAccess.obj \
	$(TEMP_AUTO_TARGET)\LineString.obj \
	$(TEMP_AUTO_TARGET)\OutputHandler.obj \
	$(TEMP_AUTO_TARGET)\SourceReader.obj \
	$(TEMP_AUTO_TARGET)\tinyxml2.obj \




$(TEMP_AUTO_TARGET)\Main.obj: $(AUTO_SOURCE)\Main.cpp $(AUTO_HEADER)
$(TEMP_AUTO_TARGET)\EnumGen.obj: $(AUTO_SOURCE)\EnumGen.cpp $(AUTO_HEADER)
$(TEMP_AUTO_TARGET)\EnumGen.obj: $(AUTO_SOURCE)\EnumLengthSetupGen.cpp $(AUTO_HEADER)
$(TEMP_AUTO_TARGET)\EnumMap.obj: $(AUTO_SOURCE)\EnumMap.cpp $(AUTO_HEADER)
$(TEMP_AUTO_TARGET)\FileAccess.obj: $(AUTO_SOURCE)\FileAccess.cpp $(AUTO_HEADER)
$(TEMP_AUTO_TARGET)\LineString.obj: $(AUTO_SOURCE)\LineString.cpp $(AUTO_HEADER)
$(TEMP_AUTO_TARGET)\OutputHandler.obj: $(AUTO_SOURCE)\OutputHandler.cpp $(AUTO_HEADER)
$(TEMP_AUTO_TARGET)\SourceReader.obj: $(AUTO_SOURCE)\SourceReader.cpp $(AUTO_HEADER)
$(TEMP_AUTO_TARGET)\tinyxml2.obj: $(AUTO_SOURCE)\tinyxml2.cpp $(AUTO_HEADER)



$(AUTO_EXE): $(AUTO_OBJ)
	@ECHO Linking Autogenerate.exe
	@$(LD) /out:$(AUTO_EXE) $** $(GLOBAL_LIBS) $(AUTO_LDFLAGS)


{$(AUTO_SOURCE)}.cpp{$(TEMP_AUTO_TARGET)}.obj:
	@FOR %i IN ( "$(TEMP_FILE_DIR)" "$(TEMP_AUTO)" "$(TEMP_AUTO_TARGET)" ) DO @IF NOT EXIST "%i" mkdir "%i"
	@$(CPP) /nologo $(GLOBAL_INCS) $(AUTO_CFLAGS) /Fo$@ /c $<
	


