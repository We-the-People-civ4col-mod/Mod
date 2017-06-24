C:\foo.txt:
	@echo $@

C:/bar.txt:
	@echo $@

"C:\three.txt"::
	@echo $@

C:\three.txt::
	@echo ...

S ::
	@echo $@

"S" ::
	@echo ...
