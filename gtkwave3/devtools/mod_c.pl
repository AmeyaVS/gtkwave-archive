# takes string and determines whether there's been a function declaration  
# this will mess up on function pointers
sub is_function
{
  local($local) = $_[0]; # The current string;
  @strs = split(/{/,$local);
  $local = @strs[0]; 
  if(($local =~ /\([\w\*\s,]*\)/) && !($local =~ /=/))
  {
    print "rejecting $local";
    $ret = 1;
  }
  else 
  {
    $ret = 0;
  }
}

# read in data from file
open(INDATA, "data.txt");
  $i = 0;
while(<INDATA>)
{
  
  $i = $i + 1;
}


#We have everything now....  
#we should now auto gen the struct header, modding files as we go.  This 
# means stripping things out.


for($i = 0; $i < $var_count; $i = $i + 1) 
{    
   if($i == 0) 
   {
      open(INFH, "@var_file[$i].c");
   }
   else 
   {
      open(INFH, "@var_file[$i].c.new"); 
   }

   if((open(NEWF, ">@var_file[$i].c.new.temp")))
   {
     # throw in the "prelude include" if this is the first iteration
     if($i == 0)
     {
       print NEWF "#include\"globals.h\"\n";
     }

     if($i == 0 || (@var_file[$i - 1] ne  @var_file[$i]))
     {
       print OUT "//@var_file[$i].c\n";
     }
 
     while(<INFH>)
     {      
      if($_ =~ /#include/)
      {
	print NEWF $_;
        $includes{$_} = $_; 
      }

      # this one we don't print to the new file.
      if($_ =~ /@vars[$i]/ && !($_ =~ /extern/))
      {
          #we need to verify that the given line is not a function declaration.
          if(is_function($_) == 0)
          {
	    print OUT $_;
            last;
	  }
          # we may have the tricky case wherein 

      }
 
     }

     # Now, we've found the global and should replace its sucessors with pointer based structures. This could be ugly,
     # and will require iterative file munging. 
     while(<INFH>)
     {    
       if($_ =~ /@vars[$i]/)
      {
          # we should replace the variable with 
      }
     }            
    
 
   }


   close(INFH);
   close(NEWF);

   if(open(INFH, "@var_file[$i].h"))
   { 
     while(<INFH>)
     {      
      if($_ =~ /#include/)
      {
        $includes{$_} = $_; 
      }

      if($_ =~ /@vars[$i]/&& !($_ =~ /extern/))
      {
        if(is_function($_) == 0)
        {
	  print OUT $_;
          last;
        }
      }

     }
   }
}

for $key ( keys %includes ) 
{
  $value = $includes{$key};
  print INCL $value;
}



print OUT "} globals;\n";
print OUT "#endif";
close(OUT);
close(INCL);

`cat includes.txt globals.txt > globals.h`;

# need to change one file at a time and recompile.  bail on failure








