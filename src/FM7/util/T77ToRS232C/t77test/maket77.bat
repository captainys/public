cl makebin.cpp
.\makebin.exe
del makebin.obj
del makebin.exe
asm6809 csum.asm -S -o csum.srec
asm6809 csum1.asm -S -o csum1.srec
asm6809 csum2.asm -S -o csum2.srec
asm6809 fnd10inc.asm -S -o fnd10inc.srec
asm6809 fnd1inc.asm -S -o fnd1inc.srec
asm6809 fndnot4e.asm -S -o fndnot4e.srec
t77save t77test.t77 -new
t77save t77test.t77 -srecwrite csum1.srec CSUM1 
t77save t77test.t77 -srecwrite csum2.srec CSUM2 
t77save t77test.t77 -savem random.bin RANDOM 0x2000 0x1400 
t77save t77test.t77 -srecwrite csum.srec CSUM 
t77save t77test.t77 -savem all4e.bin ALL4E 0x2000 0x1400 
t77save t77test.t77 -srecwrite csum.srec CSUM 
t77save t77test.t77 -srecwrite fndnot4e.srec FNDNOT4E
t77save t77test.t77 -savem inc01.bin INC01 0x2000 0x1400 
t77save t77test.t77 -srecwrite csum.srec CSUM 
t77save t77test.t77 -srecwrite fnd1inc.srec FND1INC
t77save t77test.t77 -savem inc10.bin INC10 0x2000 0x1400 
t77save t77test.t77 -srecwrite csum.srec CSUM
t77save t77test.t77 -srecwrite fnd10inc.srec FND10INC
