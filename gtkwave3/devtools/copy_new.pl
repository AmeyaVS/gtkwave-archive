@files = ();
@dirs = `ls -d */`;
`rm globals.h.new`;

for($i = 0; $i < scalar(@dirs); $i = $i + 1)
{
  chomp(@dirs[$i]);
  print "ls -d @dirs[$i]*/\n";
  @newdirs = `ls -d @dirs[$i]*/`;
  @hfiles =  `ls @dirs[$i]* *.h`;
  @cfiles =  `ls @dirs[$i]* *.c`;
  @files = (@files,@hfiles, @cfiles); 
  @dirs = (@dirs,@newdirs);
}

foreach $file(@files)
  {
    #print $file;
    chomp($file);
    `cp $file.new $file`;
  } 
 
