all: one.txt two three four five six

clean:
	@del one.txt two.txt three.txt four.txt six.txt > NUL 2>&1

one.txt:
	@echo $@
	@echo $@ > $@
#	@ping 127.0.0.1 -n 2 -w 1000 > nul

two: two.txt
	@echo $@

two-without-commands: two.txt

two.txt:
	@echo $@
	@echo $@ > $@
#	@ping 127.0.0.1 -n 2 -w 1000 > nul

three: three.txt
	@echo $@

# Insane nmake behavior:
# three.txt is always built because 'two' has commands.
three.txt: two
	@echo $@
	@type two.txt > $@
#	@ping 127.0.0.1 -n 2 -w 1000 > nul

four: four.txt
	@echo $@

four.txt: two.txt
	@echo $@
	@copy two.txt $@ > NUL
#	@ping 127.0.0.1 -n 2 -w 1000 > nul

five:
	@echo $@

six: six.txt
	@echo $@

# Insane nmake behavior:
# six.txt is only built when two.txt has changed because 'two-without-commands'
# is a real pseudotarget.
six.txt: two-without-commands 
	@echo $@
	@type two.txt > $@

