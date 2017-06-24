# test ignore exit codes in make files
#
# http://msdn.microsoft.com/en-us/library/1whxt45w.aspx

all: test_ignoreExitCode
	@echo ---SUCCESS---

test_ignoreExitCode:
	-nonexistent_command
	@echo Failing command was properly ignored

