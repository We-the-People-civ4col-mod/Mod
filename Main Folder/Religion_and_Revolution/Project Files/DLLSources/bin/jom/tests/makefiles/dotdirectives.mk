all: silence ignorance preciousness suffixes

silence: silence_one silence_two silence_three
silence_one:
    echo 1
$(NOT_DEFINED).SILENT:
silence_two:
    echo 2
$(NOT_DEFINED)!CMDSWITCHES -S
silence_three:
    echo 3

ignorance: ignorance_one ignorance_two ignorance_three
ignorance_one:
    echo 1
$(NOT_DEFINED).IGNORE   :
ignorance_two:
    echo 2
    cmd /c exit /b 1
$(NOT_DEFINED)!   CMDSWITCHES   -i
ignorance_three:
    echo 3

preciousness: preciousness_one preciousness_two preciousness_three
$(NOT_DEFINED).PRECIOUS : preciousness_one preciousness_two preciousness_three
preciousness_one:
preciousness_two:
preciousness_three:

$(NOT_DEFINED).SUFFIXES: .exe .obj
suffixes:
