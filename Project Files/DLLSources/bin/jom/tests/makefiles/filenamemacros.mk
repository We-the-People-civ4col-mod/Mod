all: Footb$$@ LolCatExtractorManager.tar.gz manyDependents \
     manyDependentsSingleExecution manyDependentsSubstitutedNames \
     manyDependentsInlineFile \
     gen_init generated.txt gen_cleanup macros.mk $(MAKEDIR)\infrules.mk

# $** and $? are undefined here and should be evaluated to empty strings.
Football:
	@echo $$@
	@echo $@ $@
	@echo $$** $**
	@echo $$? $?
	@echo $$(**) $(**)
	@echo $$(?) $(?)

LolCatExtractorManager.tar.gz:
	@echo $*

manyDependents: Timmy Jimmy Kenny Eric Kyle Stan
	@echo $**
	@echo $?

manyDependentsSingleExecution: Timmy Jimmy Kenny Eric Kyle Stan
	!@echo $**
	!@echo $?

manyDependentsSubstitutedNames: Tilly Jilly
    @echo $$(**) $(**)
    @echo $$(?) $(?)
    @echo $$(**:ll=mm) $(**:ll=mm)
    @echo $$(?:ll=mm) $(?:ll=mm)

manyDependentsInlineFile: Timmy Jimmy Kenny Eric Kyle Stan
    @type <<
$$@ $@
$$** $**
<<

manyDependentsWithModifiers: subdir\Timmy.txt subdir\subsubdir\Jimmy.txt Kenny.txt Eric.txt Kyle.txt Stan.txt
	@echo $$(**D) $(**D)
	@echo $$(**B) $(**B)
	@echo $$(**F) $(**F)
	@echo $$(**R) $(**R)
	@echo $$(?D) $(?D)
	@echo $$(?B) $(?B)
	@echo $$(?F) $(?F)
	@echo $$(?R) $(?R)

Timmy:
Jimmy:
Kenny:
Eric:
Kyle:
Stan:
Tilly:
Jilly:

subdir\Timmy.txt:
subdir\subsubdir\Jimmy.txt:
Kenny.txt:
Eric.txt:
Kyle.txt:
Stan.txt:
Tilly.txt:
Jilly.txt:

gen_init:
	@echo x > gen1.txt
	@echo x > generated.txt
	@echo x > gen2.txt
	@echo x > gen3.txt

generated.txt: gen1.txt gen2.txt gen3.txt
	@echo $?

gen_cleanup:
	@del generated.txt gen?.txt

macros.mk:
	@echo $$(@D) $(@D)
	@echo $$(@B) $(@B)
	@echo $$(@F) $(@F)
	@echo $$(@R) $(@R)

$(MAKEDIR)\infrules.mk: force
	@echo $$(@D) $(@D)
	@echo $$(@B) $(@B)
	@echo $$(@F) $(@F)
	@echo $$(@R) $(@R)

force:

root_dir\substitutionWithColon:
    @echo $(@:root_dir=C:\somewhere)
    @echo $(@R:root_dir=C:\somewhere)
