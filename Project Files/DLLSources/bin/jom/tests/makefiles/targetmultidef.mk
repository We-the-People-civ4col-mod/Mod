all: foo bar

foo: foo1.cpp
foo: foo3.cpp
foo: foo4.cpp
	echo $**

bar:: foo1.cpp
	echo one
bar:: foo3.cpp
	echo two
bar:: foo4.cpp
	echo three

