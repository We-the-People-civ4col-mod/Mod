.SUFFIXES:
.SUFFIXES: .a .b .c .d

.a.x:
    @echo a -^> x
.b.x:
    @echo b -^> x
.c.x:
    @echo c -^> x

all: one two.x three.x
one: one.x
two.x: one.x
three.x: two.x

