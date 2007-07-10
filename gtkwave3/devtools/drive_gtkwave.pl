`rm *.stack`;
`rm *.map`;
`make clean`;
`cd ..;export CC=tcc;./configure`;
`make gtkwave | perl divide_build_dumps.pl`;
`make clean`;
`cd ..;make clean;export LDFLAGS=-Wl,-M;export CC=gcc;./configure;make`;
`make clean`;
`make gtkwave 2> gtkwave.map`;
print "Done building, run rip_globals\n";
`perl rip_globals.pl gtkwave.map`;

# here we need to insert the -Wl,M line into the linker flags


