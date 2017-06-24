first: dep1 dep2
    @echo silent command
    -echo no error checking
    -5echo maximum exit code is 5
    -15echo maximum exit code is 15
    !echo command for each dependent $**

dep1:
dep2:
