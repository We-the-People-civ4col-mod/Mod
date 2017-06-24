all : one two three dollarSigns

DEPENDENT_C=c

one: a b $(DEPENDENT_C)
    @echo one
two:; @echo two
three : "a;b" ;@echo three; @echo end of three
four: ; @echo four=4

a :
b :
c :
"a;b":
    @echo X

DIRECTORYNAME=.
$(DIRECTORYNAME):
    @echo directory $(DIRECTORYNAME) doesn't exist. That's strange.
"$(DIRECTORYNAME)":
    @echo directory "$(DIRECTORYNAME)" doesn't exist. That's strange.

DIRECTORYNAME=..
$(DIRECTORYNAME):
    @echo directory $(DIRECTORYNAME) doesn't exist. That's strange.
"$(DIRECTORYNAME)":
    @echo directory "$(DIRECTORYNAME)" doesn't exist. That's strange.

dollarSigns:
    @echo ($$dollar-signs$$)
    @echo $$(dollar-signs)$$

TARGETNAME=XTargetName
$(TARGETNAME:X=Substituted): dep1 dep2 dep3
    @echo target name with macro substitution
