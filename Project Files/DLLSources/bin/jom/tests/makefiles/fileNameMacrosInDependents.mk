all:
DEPS=$@ $$@ $* $(@D) $$(@D) $(*D) $(@B) $$(@B) $(*B) $(@F) $$(@F) $(*F) $(@R) $$(@R) $(*R)
foo: $(DEPS)
foo.obj: $(DEPS)
C:\MyProject\tmp\foo.obj: $(DEPS)
"C:\My Project\tmp\foo.obj": $(DEPS)
