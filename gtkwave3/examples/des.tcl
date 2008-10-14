#
# simple example tcl script usage from gtkwave...
# gtkwave des.vzt des.sav -S des.tcl
#

puts "printing the screen to a postscript file..."
gtkwave::/File/Print_To_File PS {Letter (8.5" x 11")} Full des.ps

puts "now exiting!"
gtkwave::/File/Quit/Yes,_Quit
