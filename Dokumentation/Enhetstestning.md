För att köra alla enhetstester på linux x86 och solaris x86 används "make test". 
Vi har totalt 182 tester vilka alla passerar.  

För att köra de 109 header testerna används "make test_header" 

För att köra de 68 gc testerna används "make test_gc"

För att köra de 3 alloc map testerna används "make test_alloc_map" 

För att köra de 2 stack search testerna används "make test_stack_search" 

På solaris SPARC används "make test_sparc" och 3 test fallerar.

För att se code covarage används "make coverage". Detta fungerar dock bara för stack search och alloc map modulerna, vilka båda har hundra procent coverage.