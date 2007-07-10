@foo = `ls *.h`;
foreach $file(@foo)
{
    chomp($file);
  `cp $file backup/$file.bak`;
}

@foo = `ls *.c`;
foreach $file(@foo)
{
    chomp($file);
    print "cp $file backup/$file.bak\n";
  `cp $file backup/$file.bak`;
}
 
