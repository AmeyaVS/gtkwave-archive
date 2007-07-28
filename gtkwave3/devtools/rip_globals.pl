use FileHandle;   
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

open(INFH, $ARGV[0]) || die("\nCan't open $ARGV[0] for reading: $!\n");
open(OUT, ">out.txt");

$var_count = 0;
$bss_count = 0;


# Initialize rename here, so that we can put the globals into this structure as we discover them.  If we choose we
# may rename all statics, but not the globals (or file processing order will matter)
%rename = {};

while(<INFH>)
{
  @line = split(/\s+/,$_);
  # Split indices could be empty...
  
  for($i = 0; $i < scalar(@line); $i = $i + 1)
  {
     if((@line[$i]) ne "")
     {
	 last;
     }
  }
  
  if(@line[$i] eq ".data" || @line[$i] eq "COMMON" || @line[$i] eq ".bss") 
  { 
  
    #We should extract the source file name so as to locate the source file 
    #containing the original data.
    @objsplit = split(/\./, @line[$i+3]);
    $objfile =  @objsplit[0];
    print "foo @objsplit[0] \n";
    if( @line[$i] eq ".bss")
    {
      @bss_files[$bss_count] = $objfile;
      $bss_count = $bss_count +1;
    }

   while(<INFH>)
   {
     #print $_;
     @line = split(/\s+/,$_);

     for($i = 0; $i < scalar(@line); $i = $i + 1)
     {
       if((@line[$i]) ne "")
       {
	 last;
       }
     }
  
     if(@line[$i]=~ /0x[\dabcdef]+/)
     {
	 print $_; # this one ought to go to the outfile. 
         if(scalar(@line) < 4)
         {
           # just find the second value here.
           if(@line[$i+1] =~ /^[a-zA-Z]/)
           {
             print OUT "@line[$i+1]\n";
           
             @vars[$var_count] = @line[$i+1];
             @var_file[$var_count] = $objfile;
             $var_count = $var_count + 1;
             $rename{@vars[$var_count]} = 1;      
      	   }
         }
     }
     else
     { 
         # check to see if we're still in a section of interest
         if(@line[$i] ne ".data" && @line[$i] ne "COMMON" && @line[$i] ne ".bss") 
         {          
	   last;  # jump to the end, we're done with the section
         }
         else 
         {
           
           # we should collect the .o file name.  this will give us the place to look for definitions ...
           @objsplit = split(/\./, @line[$i+3]);
           $objfile =  @objsplit[0];
	   print "foo @objsplit[0]  \n";
           # need to keep tabs on the bss thingies they'll be necessary later 
           if( @line[$i] eq ".bss")
           {
	       @bss_files[$bss_count] = $objfile;
               $bss_count = $bss_count +1;
	   }
         }
     }
   }
 }
}

close(OUT);
close(INFH);



@localhfiles = `ls *.h`;
@helperhfiles = `ls helpers/*.h`;
#dummy single entry c file
@cfile[0] = "junk";
@hfiles = (@cfile,@localhfiles,@helperhfiles);


#We have everything now....  
#we should now auto gen the struct header, modding files as we go.  This 
# means stripping things out.

#grab list of the .o files

@object=`ls *.o`;
open(OUT, ">globals.h");
open(INCL, ">includes.txt");
open(DATA, ">data.txt"); # this will hold the data structure
print OUT "#ifndef GLOBALS_H\n";
print OUT "#define GLOBALS_H\n";
for($j = 1; $j < scalar(@hfiles); $j = $j + 1)
{
  chomp(@hfiles[$j]);
  print OUT "#include\"@hfiles[$j]\"\n";
}
print OUT "struct Global{ \n";
%includes = ();


#
open(PRJ_FILES,"files.dat") || die("Failed to open files");
%file_moddable = ();
while(<PRJ_FILES>)
{
  if($_ =~ /~#/)
  {
    # skip comments
    next;
  }
  else 
  {
    chomp($_);
    $file_moddable{$_} = $_; 
  }
}

open (GLOBALSC, ">globals.c"); 
print GLOBALSC "#include\"globals.h\"\n";
print GLOBALSC "struct Global GLOBALS = {\n";

%processed_files = ();
%global_decl_index = (); #pointer into the following arrays.  Indexed by rename. Need to handle this
@global_declarations = (); #The actual declaration.

foreach $object (@object)
{
  chomp($object);
  print "*******\n* Processing $object\n********\n";
  @objsplits = split(/\./,$object);
  #@objsplits[1] contains the object stem.  use this to open the .stack file
  #create a list of filehandles for the local .h and .c files.
  @hfiles[0] =  "@objsplits[0]\.c"; # the base is the .c file
  # clear out the previously opened files
  %opened_files = ();
  @hfiles = ();
  @hfilehandle = ();
  @hfilehandlenew = ();
  @hfilehandleindex = ();
  @hfilemoddedlineindex = ();

  #setup the stack frame  
  $stackframe = 0;
  @stacksplit[0] = 0;

  if(!open(INSTACK, "@objsplits[0].c.stack"))
  {
    open(INSTACK, "helpers/@objsplits[0].c.stack") || die("unable to open @objsplits[0].c.stack"); 
  }
  print "Opened @objsplits[0].c.stack\n";   
  $file = <INSTACK>;
  chomp($file);
  @filesplit = split(/:/,$file);
  if(!(@filesplit[0] =~ /START/))
  {
      die("@objsplits[0].c.stack began incorrectly...");
  }  
  $symbolbase = 0;
 
  
  # Not really sure if this code is needed.  Indeed it looks like it should be deleted
  @objsplits2 = split(/\./,@filesplit[1]); 
  # we should split the file split to pick up the helpers designation...
  if(open(CFILE, "@objsplits2[0].c"))
  {
    open(CFILENEW, ">@objsplits2[0].c.new") || die("Couldn't open @objsplits2[0].c.new");
    $use_c = 1;
  }
  else
  {
    print "Warning, unable to find @objsplits2[0].c\n";
    $use_c = 0;
  }
  
  while(<INSTACK>)
  { 
  
    chomp($_); 
    $base_str = $_;
    @stacksplit = split(/\:/,$_);
 
    # Set up opened files if necessary.  Files can only be opened on DECL or IDENT
    if((@stacksplit[0] eq "DECL") || (@stacksplit[0] eq "IDENT"))
    {
      #check for both non-existance and that the file is in fact moddable.
      if(!(exists $opened_files{@stacksplit[2]}) && (exists $file_moddable{@stacksplit[2]}))
      {
        # we should now open a file.
	$opened_files{@stacksplit[2]} = 0;
        if(!exists $processed_files{@stacksplit[2]}) {
          # still, we'll only process a file once.
          $processed_files{@stacksplit[2]} = 0;
	}
         
        $next_index = scalar(@hfiles);
        print "Adding source @stacksplit[2] at $next_index\n";
        @hfiles[$next_index] = @stacksplit[2];
        if($processed_files{@stacksplit[2]} == 0) 
        {
          # bad news here due to the existance of lib includes... eep.,
          @hfilehandle[$next_index] = IO::File->new(@stacksplit[2]);
          @hfilehandlenew[$next_index] = IO::File->new(">@stacksplit[2].new"); 
          @hfilehandlenew[$next_index]->print("#include\"globals.h\"");         
          @hfilehandleindex[$next_index] = 0; # file numbering starts at 1. This variable is the last line read out.  
          @hfilecurrentline[$next_index] = ""; # clean copy of current line
          @hfilemoddedlineindex[$next_index] = -1; #used to track substitutions in the ident substitution string
	  @hfilelineindex[$nextindex] = -1;  #used when consuming decls.
          @hfileline[$nextindex] = "";
	}
        else
        {
          @hfilehandleindex[$next_index] = -1;
	}
      }
    }

    #handle pops
    if(@stacksplit[0] eq "POP")
    {
      # decrement symbol base if necessary...
      # can get strange wraparound condition
      if(@stacksplit[1] ==  @symbolstacknum[$symbolbase - 1] && ($symbolbase > 0)) 
      {
	print "Popped @stacksplit[1]  @symbolstacknum[$symbolbase - 1]  (@symbolstackname[$symbolbase - 1])\n";
        $symbolbase = $symbolbase - 1;
      }
    }    
    
  
    #basically, we handle the global scope specially.  Redeclarations here 
    #are allowed and result in a single push. Other scopes 
    if(@stacksplit[0] eq "STACKPOP")
    {
      $stackframe = $stackframe - 1;     
    }

    if(@stacksplit[0] eq "STACKPUSH")
    {      
      $stackframe = $stackframe + 1;  
      $stacksplit[$stackframe] = $symbolbase;         
    }

    # finally, handle the IDENTS
    if(@stacksplit[0] eq "IDENT")
    {  
      for($i = 0; $i < scalar(@hfiles); $i = $i + 1)
      {
	#  print "@hfiles[0]\n
        # Do not check if file has been processed.  We must always insert symbol defs.  
        # processed only affects dumping symbols.
        if(@stacksplit[2] =~ /^@hfiles[$i]/) 
        {     
         
          #roll down the symbol stack until we find a match.
          for($k = $symbolbase - 1; $k > -1; $k = $k-1)
          {
            if($symbolstacknum[$k] == @stacksplit[1])
            {               
              if((@symbolstackglobalflag[$k] == 1) ||
                 (@symbolstackexternflag[$k] == 1) ||  
                 (@symbolstackstaticflag[$k] == 1))
              {
                print "Processing $_\n"; 
                # got a global match here.  need to advance appropriately.  XXX fix the cut and paste
                # the problem here is that we will replace named variables incorrectly.  The replacement needs to 
                # make sure that only non A-Za-z0-9_ are on the sides of the replacement target. 
                if((@hfilehandleindex[$i] <  @stacksplit[3]) && ($processed_files{@hfiles[$i]} == 0))
                {
                  &advance_line($i, @stacksplit[3]);
 
                  @hfilemoddedline[$i] = &advance_one_line($i);#@hfilehandle[$i]->getline;
                  if(index(@hfilemoddedline[$i],$stacksplit[4]) < 0)
                  {
			die("A $stacksplit[4] not found in @hfilemoddedline[$i]at line @hfilehandleindex[$i] in @hfiles[$i]\n");
		  }
                  @hfilemoddedlineindex[$i] = index(@hfilemoddedline[$i],$stacksplit[4]) + length("GLOBALS.$symbolstackname[$k]");
                  #split the base string at the original symbol
                  $base = substr(@hfilemoddedline[$i], 0, index(@hfilemoddedline[$i],$stacksplit[4]));
                  $subs = substr(@hfilemoddedline[$i], index(@hfilemoddedline[$i],$stacksplit[4]));
                  $subs =~  s/\b$stacksplit[4]\b/GLOBALS.$symbolstackname[$k]/;
                  @hfilemoddedline[$i] = $base . $subs;                  
	        }
                elsif(@hfilehandleindex[$i] == @stacksplit[3]) # shouldn't assume that things are correctly setup
                { 
                  # this sort of generic substitution likely won't work.  we should really track position.
                  # need to crack at the index point
                  if(@hfilemoddedlineindex[$i] > -1) 
                  {
		    $base = substr(@hfilemoddedline[$i], 0, @hfilemoddedlineindex[$i]);
                    $subs = substr(@hfilemoddedline[$i], @hfilemoddedlineindex[$i]);
                    if(index($subs,$stacksplit[4]) < 0)
                    { 
			die("B $stacksplit[4] not found in @hfilemoddedline[$i] at line @hfilehandleindex[$i] in @hfiles[$i]\n");
		    }
                    @hfilemoddedlineindex[$i] = @hfilemoddedlineindex[$i] + index($subs,$stacksplit[4]) + length("GLOBALS.$symbolstackname[$k]");
                    #split the base string at the original symbol
                    $subs =~  s/\b$stacksplit[4]\b/GLOBALS.$symbolstackname[$k]/;
                    @hfilemoddedline[$i] = $base . $subs;
		  }
                  else # in this case we haven't begun to mod the line...
                  {
                    @hfilemoddedline[$i] = @hfilecurrentline[$i];
                    if(index(@hfilemoddedline[$i],$stacksplit[4]) < 0)
                    {
			die("C $stacksplit[4] not found in @hfilemoddedline[$i] at line @hfilehandleindex[$i] in @hfiles[$i]\n");
		    }
                    @hfilemoddedlineindex[$i] = index(@hfilemoddedline[$i],$stacksplit[4]) + length("GLOBALS.$symbolstackname[$k]");
                    #split the base string at the original symbol
                    $base = substr(@hfilemoddedline[$i], 0, index(@hfilemoddedline[$i],$stacksplit[4]));
                    $subs = substr(@hfilemoddedline[$i], index(@hfilemoddedline[$i],$stacksplit[4]));
                    $subs =~  s/\b$stacksplit[4]\b/GLOBALS.$symbolstackname[$k]/;
                    @hfilemoddedline[$i] = $base . $subs;       
		  }
                }
                else 
                {
                  print "Warning advanced too far while looking for a variable name A"
		}
	      }
              # Got a match, go no farther
              last;
	    }
          }   
	}
      }
    }


    #handle declarations
    if(@stacksplit[0] eq "DECL")
    {
      
      #handle decl in .h - no function insertions
      # loop over the hfiles
      # print " May place @stacksplit[1] @stacksplit[5] at $symbolbase\n";
      for($i = 0; $i < scalar(@hfiles); $i = $i + 1)
      {
	#  print "@hfiles[0]\n
        # Do not check if file has been processed.  We must always insert symbol defs.  
        # processed only affects dumping symbols.
        if((@stacksplit[2] =~ /^@hfiles[$i]/) && !(@stacksplit[4] eq "FUNC") && !(@stacksplit[4] eq "TYPEDEF")) 
        { 
	  # always place in symbol stack
	  chomp(@stacksplit[5]);
          @symbolstacknum[$symbolbase] = @stacksplit[1];
          @symbolstackname[$symbolbase] = @stacksplit[5];
          @symbolstackglobalflag[$symbolbase] = 0; # Don't assume things are global
          @symbolstackexternflag[$symbolbase] = 0; # Externs don't count as a definition  
          @symbolstackstaticflag[$symbolbase] = 0; # Externs don't count as a definition          

          if(@stacksplit[4] eq "EXTERN")
          {
             # DO NOT redefine externs?

	     @symbolstackexternflag[$symbolbase] = 1;

	     for($k = $symbolbase - 1; $k > -1; $k = $k-1)
             {
               # globals don't count against the declaration count.
               if((@symbolstacknum[$symbolbase] == @symbolstacknum[$k]))
               {
		 print "rejecting @stacksplit[5] $symbolbase at  due to  @symbolstackname[$k] at $k\n";
	         last;
               }  
	     }
             # no match... define the extern.
             if($k == -1)
             { 
		 print "Adding extern def @stacksplit[5] from @hfiles[$i] at $symbolbase \n";
	       $updatedbase = 1;
               $symbolbase = $symbolbase + 1;               
             }

             #externs must be dropped.
             if((@hfilehandleindex[$i] <  @stacksplit[3]) && ($processed_files{@hfiles[$i]} == 0))
             {
             
               # advance throught the H file
	       print "in file @hfiles[$i] at line @hfilehandleindex[$i]\n";
               &advance_line($i,@stacksplit[3]);
	       
               $str = &advance_one_line($i);
               
               if(!($str =~ @stacksplit[5]))
               {
	 	 die("in file @hfiles[$i] at line @hfilehandleindex[$i] dropping $str, which did not contain @stacksplit[5]");
	       } 
	     }
             elsif(@hfilehandleindex[$i] >  @stacksplit[3])
             { 
	       print "Warning, the hfile has advanced too far B\n";
             }
          }

          if(@stacksplit[4] eq "LOCAL")
          { 	     
	    $updatedbase = 0;
                     
            #for($j = 0; $j < $var_count; $j = $j + 1)
            #{ 
              #check to see if the variable is defined in this object file
              #should check for global scope
              if($stackframe == 0) 
              {                     
                #print "@var_file[$j] @vars[$j]  Processing local $_\n";      
                # this global can exist in this file check for shadowing...
                # see if the names match
                #if(@vars[$j] eq @stacksplit[5])
	        #{
                  print "Processing local @stacksplit[5]\n"; 
                  # this global can exist in this file check for shadowing...
		  for($k = $symbolbase - 1; $k > -1; $k = $k-1)
                  {
                    # globals don't count against the declaration count.
                    if(@symbolstacknum[$symbolbase] == @symbolstacknum[$k])
                    {
			print "rejecting @stacksplit[5] $symbolbase at  due to  @symbolstackname[$k] at $k\n";
		      last;
                    }  
	   	  }
                  # a match... dump a decl to the struct
                  # we can also dump if we're defining a previously declared extern
                  
                  if(($k == -1) || (@symbolstackexternflag[$k] == 1))
                  {    
                    # we should deal with renaming here. 
                    # there should be no renaming of locals only statics will be renamed                    
                    #dump if file has not been previously processed.
		    $emit_decl = 0;
                    if($processed_files{@hfiles[$i]} == 0)
                    {                                        
                      chomp(@stacksplit[6]);
                      print "New global: $base_str\n";
                      print OUT "@stacksplit[6];//from @hfiles[$i] \n";
                      $emit_decl = 1;
		    }

                    # only advance the symbol stack if we are not dealing with an extern
                    @symbolstackglobalflag[$symbolbase] = 1;
                    if(@symbolstackexternflag[$k] == 0)
                    {
                      $symbolbase = $symbolbase + 1;
                      $updatedbase = 1;
		    }
                    else
                    {
                      @symbolstackglobalflag[$k] = 1;
		    }
                 
                    # Should advance the global immediately.  This way the identifier can be dropped.
                    # advance the .h file to the present marker, dumping out the previous h file
                    if((@hfilehandleindex[$i] <  @stacksplit[3]) && ($processed_files{@hfiles[$i]} == 0))  
                    {
                      # advance throught the H file
                  
                      #if(@hfilemoddedlineindex[$i] > -1)
                      #{
                      #  @hfilehandlenew[$i]->print(@hfilemoddedline[$i]);
                      #  print "Dumping @hfilemoddedline[$i]";
                      #  @hfilemoddedlineindex[$i] = -1;
                      #  @hfilehandleindex[$i] = @hfilehandleindex[$i] + 1; 
		      #}

                      &advance_line($i, @stacksplit[3]);
		                           
                      #it's possible that we could be stuck one line before for some reason...
                      if(@hfilelineindex[$i] == -1 || (@hfilehandleindex[$i] < @stacksplit[3])) {
                        $str = &advance_one_line($i);
                        @hfilelineindex[$i] = 0; 
                        @hfileline[$i] = $str;
                        print "In the grep clause: Got $str : @hfilehandleindex[$i], @hfileline[$i], @hfilelineindex[$i], @hfiles[$i]\n";
		      }
                      else {
			  print "Why have we not advanced: hindexfileindex: @hfilehandleindex[$i] hfileline: @hfileline[$i] hfilelineindex: @hfilelineindex[$i] hfiles: @hfiles[$i]\n";
		      }

                     
                      # sanity check
                      $substring = substr(@hfileline[$i], 0); 
                      
                      if(!($substring =~ @stacksplit[5]))
                      {
		        die("in file @hfiles[$i] at line @hfilehandleindex[$i] dropping $substring, which did not contain @stacksplit[5]");
 	              }                       

                      if($emit_decl == 1) {
                        &rip_decl($i,@stacksplit[5],@stacksplit[5]);
		      }
                   
                    }
                    elsif(@hfilehandleindex[$i]+ 1 >  @stacksplit[3])
                    { 
	              print "Warning, the hfile has advanced too far C\n";
                    }
                  }                   
	        #}
	      }
            #} 



            # if we didn't find a global, insert the local variable.
            # nasty symbol base incrment bug was here
            if(($updatedbase == 0) && @symbolstackglobalflag[$symbolbase] == 0) 
            {
              $symbolbase = $symbolbase + 1;
            }
          }        

        if(@stacksplit[4] eq "FUNDEF")
          {
            print "got a FUNDEF\n"; 	     
	    $updatedbase = 1;
            $symbolbase = $symbolbase + 1;
          }

        if(@stacksplit[4] eq "STATIC")
          { 	            
	    @symbolstackstaticflag[$symbolbase] = 1;
            @symbolstackglobalflag[$symbolbase] = 1;   
            $redefined = 0;
            #Globally defined static should not have multiple entries. -> is this bogus?
            for($k = $symbolbase - 1; $k > -1; $k = $k-1)
            {
              # globals don't count against the declaration count.
              if((@symbolstacknum[$symbolbase] == @symbolstacknum[$k]))
              {
		$redefined = 1;
                $static_rename = @symbolstackname[$k];
	        print "rejecting @stacksplit[5] $symbolbase at  due to  @symbolstackname[$k] at $k\n";
	        last;
              }  
	    }
                        
            # Should always emit static decl
            # Statics are always global.
            # we should deal with renaming here. 
            # check out the renaming array to see if we need to rename this value.
            # global variable resolution is done by inserting the globals initially
            # Uncommenting the following line is likely wrong XXX
            if(($redefined == 0)  || ($stackframe > 0))
            { 
              # should probably keep the original symbol name around, since we are going to hammer it
              # XXX for completeness should insert the rename into the hash
              if(exists $rename{@symbolstackname[$symbolbase]})
              {
                $rename{@symbolstackname[$symbolbase]}++;
		print "Incrementing to @symbolstackname[$symbolbase] $rename{@symbolstackname[$symbolbase]}\n";
              } 
              else
              {   
		print "Setting @symbolstackname[$symbolbase]\n";
		$rename{@symbolstackname[$symbolbase]} = 1;
	      }

              #We must always rename due to conflicts with other globals 
              # need to cook the . out of @stacksplit[2]
              $filename = @stacksplit[2];
              $filename =~ s/\./_/;
	      print "Changed  @stacksplit[2] to $filename\n";
	      @symbolstackname[$symbolbase] = "@symbolstackname[$symbolbase]\_$filename\_$rename{@symbolstackname[$symbolbase]}";
              $static_rename = @symbolstackname[$symbolbase];
              print "Overloaded: @symbolstackname[$symbolbase]\n"; 
              @stacksplit[6] =~ s/@stacksplit[5]/@symbolstackname[$symbolbase]/;

              print "Renaming  @symbolstackname[$symbolbase] as @stacksplit[6]\n";               
              
              @symbolstackglobalflag[$symbolbase] = 1;
              $symbolbase = $symbolbase + 1; # statics always get inserted 
	    
                
              #dump if file has not been previously processed.
              if($processed_files{@hfiles[$i]} == 0)
              {                   
                chomp(@stacksplit[6]);
                print "New global: $base_str\n";
                print OUT "@stacksplit[6];// from @hfiles[$i]\n";
	      }

             
              
	    }

            #dump if file has not been previously processed.
            #it's worth noting that we will consume input even on a redefinition
            # Should advance the global immediately.  This way the identifier can be dropped.
            # advance the .h file to the present marker, dumping out the previous h file
            if((@hfilehandleindex[$i] <  @stacksplit[3]) && ($processed_files{@hfiles[$i]} == 0))
            {
          	      
              &advance_line($i, @stacksplit[3]); 
              #drop the declaration on the floor
              if(@hfilelineindex[$i] == -1 || (@hfilehandleindex[$i] < @stacksplit[3])) {
                $str = &advance_one_line($i);
                @hfilelineindex[$i] = 0; 
                @hfileline[$i] = $str;
                print "In the grep clause: Got $str : @hfilehandleindex[$i], @hfileline[$i], @hfilelineindex[$i], @hfiles[$i]\n";
	      }
              else {
	        print "Why have we not advanced: hindexfileindex: @hfilehandleindex[$i] hfileline: @hfileline[$i] hfilelineindex: @hfilelineindex[$i] hfiles: @hfiles[$i]\n";
	      }
              #Check before ripping globals, since globals may advance the line.
              if(!($str =~ @stacksplit[5]))
              {
		die("in file @hfiles[$i] at line @hfilehandleindex[$i] dropping $str, which did not contain @stacksplit[5]");
	      } 
              print "Rename is @symbolstackname[$symbolbase] rename of @stacksplit[5]\n";  
              &rip_decl($i,@stacksplit[5],$static_rename);

	    }
            elsif(@hfilehandleindex[$i]+ 1 >  @stacksplit[3])
            { 
	      print "Warning, the hfile has advanced too far D\n";
            }
            
          }                   
	  

          # advance the .h file to the present marker, dumping out the previous h file
          if((@hfilehandleindex[$i] <  @stacksplit[3]) && ($processed_files{@hfiles[$i]} == 0))
          {
            # advance throught the H file
            &advance_line($i, @stacksplit[3]);
             
	  }
          elsif(@hfilehandleindex[$i] >  @stacksplit[3])
          { 
	    print "Warning, the hfile has advanced too far E\n";
          }


        }
        else 
        {
          
	}
      }
    }
  }

  # Make sure everything popped off the stack
  for($j = $symbolbase ; $j > -1; $j = $j -1)
  {
      print "\$symbolbase[$j]: @symbolstacknum[$j] (@symbolstackname[$j])\n";
  }   

  ($symbolbase == 0) || die("failed to pop all symbols had $symbolbase left and @symbolstacknum[$symbolbase - 1] at top... @objsplits[0].c.stack");


  # we must now clean up all the hfiles, etc.
  for($j = 0; $j < scalar(@hfiles); $j = $j + 1)
  {
    print "Cleaning up @hfiles[$j]\n";
    if(@hfilehandleindex[$j] >= 0) # we touched this file. Don't touch it again. 
    {
      $processed_files{@hfiles[$j]} = 1;
      print "Processed @hfiles[$j]\n";
      if(@hfilemoddedlineindex[$j] > 0)
      {
        @hfilehandlenew[$j]->print(@hfilemoddedline[$j]);
        print "Dumping @hfilemoddedline[$i]";
        @hfilemoddedlineindex[$j] = -1;
        @hfilehandleindex[$j] = @hfilehandleindex[$j] + 1; 
      }
      while($str = @hfilehandle[$j]->getline())
      {
        @hfilehandlenew[$j]->print($str);
      } 
      @hfilehandlenew[$j]->close();
      @hfilehandle[$j]->close();
    }  
  }
}



# print out the global decls
for($i = 0; $i < scalar(@global_declarations); $i = $i + 1) {
  while( my ($name, $index) = each %global_decl_index ) {
    if($index == $i) {
      print "{@global_declarations[$index]},//$name\n";
      print GLOBALSC "{@global_declarations[$index]},//$name\n";
    }
  }
}



print GLOBALSC "};\n";

print OUT "};\nextern struct Global GLOBALS;\n";
print OUT "#endif";
close(DATA);
close(OUT);
close(INCL);



close (GLOBALSC);  


sub rip_decl {
    my ($hindex,$name,$rename) = @_;
    my $decl_index = -1;
    my $i = 0;
    my $stackcount = 0;

    print("Calling rip_decl\n"); 

    #setup the decl index;
    if(!(exists $global_decl_index{$rename})) {
       print "rip_decl New decl index\n";
       $decl_index = scalar(@global_declarations);
       $global_decl_index{$rename} = $decl_index;
       @global_declarations[$decl_index] = "";
    }
    else {
      print "rip_decl Old decl index\n";
      #We've seen this decl before...  Make sure that it isn't already defined.
      $decl_index = $global_decl_index{$rename};
      if(!(@global_declarations[$decl_index] eq "")) {
        die("When parsing decl for $name in @hfiles[$hindex] at @hfilehandleindex[$hindex] previously initialized to @global_declarations[$decl_index]\n"); 
      }
    }
    # this simplistic matching is horribly broken.  We could have comments, etc.
    my $substr = substr(@hfileline[$hindex],@hfilelineindex[$hindex]);
    print "rip_decl decl_index: $decl_index\n";
    # this search must be carried out with the original name, as substitution has not yet occured.
    if(($baseindex = index($substr,$name)) < 0) {
      die("Couldn't find $name in $substr, was:  @hfilelineindex[$hindex]: @hfiles[$hindex] - @hfilehandleindex[$hindex]\n"); 
    }
    
    my $topchar = "";
    my $instring = 0;
    my $incomment = 0;
    my $terminateatlineend = 0;
    my $processing_decl = 0;
    for($i = $baseindex; $i < length($substr); $i = $i+1) {
      $topchar = substr($substr,$i,1);
      print "rip_decl is looping: $topchar:$instring\n";
      #check for entering a string
      if(!$instring && !$incomment && $topchar eq "\""){
	print "rip_decl entering string\n";
        $instring = 1;
	
      }
      
      # are we exiting a string?
      elsif($instring && $topchar eq "\""){
        if(($i == 0) || !(substr($substr,$i - 1,1) eq "\\")) {   
          print "rip_decl exiting string\n";
          $instring = 0;
        }
       
      }

      # are we entering a comment?#/**/
      if(!$instring && !$incomment && (($topchar eq "*") || ($topchar eq "/")) && ($i>0) && ( "/" eq substr($substr,$i-1,1))){
	  $incomment = 1;
          if($topchar eq "/") {
            $terminateatlineend = 1;
	  } 
       
      }     

      # are we exiting a comment?
      elsif($incomment && !$terminateatlineend && ($i>0) && ($topchar eq "/") && ( "*" eq substr($substr,$i-1,1))) {
	$incomment = 0;
        next;
      }

      if($topchar eq "=" && !$instring && !$incomment) {
	  print "rip_decl Processing decl set\n";
        $processing_decl = 1;
        next;	 
      }

      if($processing_decl && !$incomment && !$instring && ($topchar  eq "{")) {
        $stackcount++;
      }
      
      if($processing_decl && !$incomment && !$instring &&($topchar eq "}")) {
        $stackcount--;
        if($stackcount < 0) {
          die("When parsing decl for $name in @hfiles[$hindex] at @hfilehandleindex[$hindex] {} were misbalance\n"); 
	}
      }
    
      # Check for end conditions
      if(!$incomment && !$instring && (($topchar eq ";") || ($topchar eq ",")) && ($stackcount == 0)) { 
        print "rip_decl calling last, setting offset to @hfilelineindex[$hindex] + $i\n";
        if(@hfilelineindex[$hindex] < 0) {
	  @hfilelineindex[$hindex] = 0;
	}
        @hfilelineindex[$hindex] = @hfilelineindex[$hindex] + $i;        
        last;
      } 
      elsif($processing_decl) {
        # should emit character.
        @global_declarations[$decl_index] = @global_declarations[$decl_index] . $topchar; 
        #print "rip_decl is emitting @global_declarations[$decl_index]\n";
      }
      # we should handle the advancing line here, if we have not exhausted the decl.  Since we need to stay within
      # this loop
      if($i + 1 == length($substr)) {
        #we need to advance a line here... 
	$substr = &advance_one_line($hindex);
        $i = -1; # going to have an $i = $i +1;
        if($terminateatlineend == 1) {
          $terminateatlineend = 0;
          $incomment = 0;
	}
      } 
    }
   
    print "rip_decl is emitting @global_declarations[$decl_index]\n";
}

#this one will print out all the lines that it encounters
sub advance_line {
  my ($i, $upto) = @_; 

  if(@hfilemoddedlineindex[$i] > -1)
  {
    @hfilehandlenew[$i]->print(@hfilemoddedline[$i]);
    print "Dumping @hfilemoddedline[$i]";
    @hfilemoddedlineindex[$i] = -1;
  }

  while(@hfilehandleindex[$i] + 1 < $upto)
  {
    @hfilelineindex[$i] = -1; # no line
    $str = @hfilehandle[$i]->getline();
    @hfilecurrentline[$i] = $str;
    @hfilehandleindex[$i] = @hfilehandleindex[$i] + 1; 
    @hfilehandlenew[$i]->print($str);
  } 
  print "in file @hfiles[$i] at line @hfilehandleindex[$i]\n"; 
}


# this will advance one line, and return it.
sub advance_one_line {
  my ($i) = @_;
  if(@hfilemoddedlineindex[$i] > -1)
  {
    @hfilehandlenew[$i]->print(@hfilemoddedline[$i]);
    print "Dumping @hfilemoddedline[$i]";
    @hfilemoddedlineindex[$i] = -1; 
  }
  @hfilehandleindex[$i] = @hfilehandleindex[$i] + 1;
  @hfilelineindex[$i] = -1; # no line  
  $str = @hfilehandle[$i]->getline();
  @hfilecurrentline[$i] = $str;
  $ret_str = $str;
}

