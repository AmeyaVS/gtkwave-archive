$start = 0;

while(<STDIN>)
{
  if($_ =~ "START:")
  {
    if($start != 0)
    {
	close(INFH);
    }
    else
    {
      $start = 1;
    }
    @toks = split(/:/,$_);
    print "Opening @toks[1]";
    chomp(@toks[1]);
    open(INFH,">@toks[1].stack");
  }
  
  print INFH $_;
}
