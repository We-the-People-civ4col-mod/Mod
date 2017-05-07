first:
	@echo Default target does nothing. Specify another one.

provide_subdir:
	@if not exist subdir md subdir

# expected result: this is supposed to output the subdir directory
test1: provide_subdir
	@cd subdir
	@cd

# expected result: this is supposed to output the subdir directory
test2: provide_subdir
	@cd		/D		subdir
	@cd

# expected result: this is supposed to output the current directory (not subdir)
# Why? Because of the && nmake executes this command with cmd and loses the
# current working directory after cmd completed.
test3: provide_subdir
	@cd subdir && cd
	@cd

