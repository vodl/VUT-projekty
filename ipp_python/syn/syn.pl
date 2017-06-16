#!/usr/bin/perl
#@author: xdlapa01@stud.fit.vutbr.cz
#SYN:xdlapa01


#knihovny pro podporu vyvoje 
#use strict;
#use warnings;
#use common::sense;

#dalsi knihovny
use Getopt::Long qw(:config no_auto_abbrev pass_through);
use utf8;
use encoding "utf-8";
use open ':encoding(utf8)';


#deklarace/definice globalnich promennych
my $format = '';
my $help = '';
my $br = '';
my $input = '';
my $output = '';

my %format_hash; #klic = RV , hodnota = retezec pravidel pro znacky
my @re_prior;    #pole na regularni vyrazy, kdy index urcuje poradi
my $inText;      #retezec vstupniho textu
my ($endTags,$startTags);



# Vytvori ze slov oddelenych carkami ve formatovacim souboru
# retezec odpovidajicich oteviracich a uzaviracich <> znacek
# @param: řetězec s pravidly pro vytvoreni znacek 
sub parseTags ( $ )
{
    my $argStr = shift;
    $startTags = "";
    $endTags = "";
    
    #jednotliva pravidla oddelena mezerami a tabelatory
    my @parsedTags = split /,[\t ]*/, $argStr;
    my $tmpstr;
    
    #generovani znacek dle pravidel
    foreach (@parsedTags)
    {
      if ($_ =~ /bold/) 
      {
          $startTags =  "$startTags" . "<b>";
          $endTags = "</b>"."$endTags";
      }
      elsif($_ =~ /italic/)
      {
          $startTags =  "$startTags" . "<i>";
          $endTags = "</i>"."$endTags";
      }
      elsif($_ =~ /underline/)
      {
          $startTags =  "$startTags" . "<u>";
          $endTags = "</u>"."$endTags";
      }
      elsif($_ =~ /teletype/)
      {
          $startTags =  "$startTags" . "<tt>";
          $endTags = "</tt>"."$endTags";
      }
      elsif($_ =~ /size:([1234567])/)
      {
          $startTags =  "$startTags" . "<font size=$1>"; 
          $endTags = "</font>"."$endTags";
      }
      elsif($_ =~ /color:([0-9A-F]{6})/)
      {
          $startTags =  "$startTags" . "<font color=#$1>"; 
          $endTags = "</font>"."$endTags";
      }
      else
      {
          print STDERR "Chyba formátu vstupního souboru s formátem";
          exit(4);
      }
    }   
}

# vypise retezec s napovedou na <STDOUT>
sub printHelp()
{
    print "  Napoveda: program SYN\n" .
          "  zvyraznovani syntaxe\n" .
          "  autor: xdlapa01\@stud.fit.vutbr.cz\n" .
          "\n  Paramatry\n" . 
          "    --br      prida pred kazdy konec radku tag </ br>\n" .
          "    --format=file         vstupni soubor s formatovanim\n" . 
          "    --input=file          vstupni soubor s textem pro naformatovani\n". 
          "    --output=file         zformatovany vystupni soubor\n";
}

#obslouzi chybu parametru
sub badoptHandle
{
    print STDERR "Chyba parametru! Pro více informací zadejte --help.";
    exit(1);
}

#zpracuje parametry prikazove radky
sub getParams
{
    my @aformat = ('');
    my @ainput = ('');
    my @aoutput = ('');

    #zpracovani parametru - funkce z Getopt::Long
    GetOptions(
         "help" => \$help,
         "br" => \$br,
         "format=s" => \@aformat,
         "input=s" => \@ainput,
         "output=s" => \@aoutput,
         '<>' => sub { badoptHandle() }
    );


    my $count;
    # pocet prvku pole do promene count - v kazdem uz je prvek prazdny retezec
    # parametr nesmi byt zadan vicenasobne
    if (($count = @aformat) > 2 or ($count = @ainput) > 2 or ($count = @aoutput) > 2)
    {
        badoptHandle();
    }

    # $# - nejvyssi index  - je vzdy [1] nebo [0]
    $format = $aformat[$#aformat];
    $input = $ainput[$#ainput];
    $output = $aoutput[$#aoutput];


    # osetreni parametru help
    if ($help)
    {
      if ($br or $format or $input or $output)
      { # --help muze byt pouze samostatne
        print STDERR "S parametrem --help nemůže být použit žádný další parametr";
        exit(1);
      }
      printHelp();
      exit(0);
    }

#    if($format eq '')
#    { # dle zadani se ma nyni formatovaci soubor chovat jako prazdny
#        print STDERR "Není specifikován soubor s formátem!";
#        exit(1);
#    }
}



# otevre formatovaci soubor a vytvori slovnik regexp:formatovani
sub parseFormat
{
   if ($format eq "")
   { #zda se mi to jako nesmysl, ale bylo to tak receno ve wis foru
       return;
   }

   open (FORMAT, $format) or do
   {
       print STDERR "Chyba souboru s formátem!";
       exit(2);
   };
   
   while (<FORMAT>)
   {
       chomp;
       my ($regExp, $formParams) = split(/\t+/,$_,2);
       if ((not defined $regExp) or (not defined $formParams))
       {
           print STDERR "Chyba struktury formátovacího souboru";
           	exit(4);
       }

       $regExp =~ s/([{}?^\$\\\[\]])/\\$1/g; #perl specialni znaky
       
       $regExp =~ s/([^%!|.(]|%%|%[.])[.]([^.|+*)])/$1$2/g; #konkatenace        
       
       $regExp =~ s/%%/&&037prc&&/g; #procento jako znak za specialni sekvenci
       $regExp =~ s/%s/[\\013\\s]/g;    # bile znaky
       $regExp =~ s/%a/[\\w\\W]/g;     #jeden libovolny znak
       $regExp =~ s/%d/[0-9]/g;  #jeden libovolny znak
       
       $regExp =~ s/%l/[a-z]/g;        #mala pismena od a do z
       $regExp =~ s/%L/[A-Z]/g;        #velka pismena od A do Z
       $regExp =~ s/%w/[A-Za-z]/g;     #mala a velka pismena
       $regExp =~ s/%W/\[A-Za-z0-9]/g; #%w + cisla
       
       $regExp =~ s/%t/\\t/g;  #tab 
       $regExp =~ s/%n/\\n/g;  # odradkovani
       
       $regExp =~ s/%([|*+()])/\\$1/g; #escapovane specialni symboly => % na \

       # [ je zrejme v nahrazovacim retezci treba escapovat
       $regExp =~ s/(^|[^%!]|%!)!(\\[stn\[\]])/$1\[^$2]/g;    #negace s escape \s \t \n ] [  
       $regExp =~ s/(^|[^%!]|%!)!\[/$1\[^$2/g;                  #skupiny znaku ![] --> [^ ]
       $regExp =~ s/(^|[^%!]|%!)!\\([.|*+()])/$1\[^$2]/g;     #negace escapovany znak       
       $regExp =~ s/(^|[^%!]|%!)!(&&037prc&&|%!|[^%!|)(.+*])/$1\[^$2]/g;    #negace znak
                      # [^%]    [^!]  [^znak]      
       
       # kontrola spravnosti regexp
       eval { qr/$regExp/ };
       if ($@ or $regExp =~ /[^%][.]/ or $regExp =~ /%[^.!]/
              or $regExp =~ /[^%]![.|+*)(!]/ or $regExp =~ /[^\\][(][|]/
              or $regExp =~ /[|][)|]/)
       {
             print STDERR "Neplatný regulární výraz!";
             exit(4);
       }
       
       $regExp =~ s/%!/!/g; #neescapovane specialni symboly (neovlivni integritu vyrazu)
       $regExp =~ s/%./\\./g; #tecka znamenajici tecku
       $regExp =~ s/&&037prc&&/%/g;
       
       
       push(@re_prior, $regExp); #vlozeni vyrazu do pole priorit
       
       # vlozeni regularniho vyrazu do hashe jako klic
       # a k nemu nalezici retezec pro formatovani jako hodnotu 
       if (exists $format_hash{$regExp})
       {  #resi kdyz  bude jeden regexp vicekrat
            $format_hash{$regExp} = "$format_hash{$regExp}".", $formParams";
       }    #zde este nejsou formatovaci znacky rozparsovany na tagy proto ", "
       else
       {
            $format_hash{$regExp} = "$formParams";
       }

   }
   close FORMAT;
}




# nacte vstupni soubor do retezce inText
sub readInput
{
  my @lines;
  if ($input eq '')
  { #neni urcen vstupni soubor - vstup = STDIN
      @lines = <>; 
  }
  else
  {
      open INPUT, $input or do
      {
          print STDERR "Chyba souboru se vstupem!";
          exit(2);
      };
      
      @lines = <INPUT>; #naceteni do pole po radkach
      close INPUT;
  }
  $inText = join('',@lines); # spojeni radku do retezce
}




#--------------------------------------------------------------------
#----------------- HLAVNI TELO SKRIPTU  -----------------------------
# -------------------------------------------------------------------

getParams();
parseFormat();
readInput();


my $len = length( $inText );
my @tagsOnIndexes;

for( my $ii = 0; $ii <= $len; $ii++)
{ #inicializuji si pole tagu
  $tagsOnIndexes[$ii] = ""; 
}


my @endPos;
my @startPos;
my $mypos = 0;


foreach(@re_prior)
{
  my $regExp = $_;
  my $tags = $format_hash{$regExp};
  $format_hash{$regExp} = ""; # pokud by byl regularni vyraz duplicitni,
                              #  nebudou se duplikovat tagy
  parseTags($tags); #naplni endTags a startTags
  
  #vymazani obsahu pole pro ulozeni matches noveho regexp
  @endPos = (); 
  @startPos = ();

  while ($inText =~ /($regExp)/g)
  {
      my $match =  $1;
      my $matchLen = length($match);
      $mypos = pos($inText);
      
      if ($matchLen > 0)
       {  #neoznacovani prazdnych retezcu                  
          push(@endPos, $mypos);
          push(@startPos, $mypos - $matchLen);
       }
  }
  
  foreach (@endPos)
  {
     $tagsOnIndexes[$_] =  "$endTags"."$tagsOnIndexes[$_]";
  }
  
  foreach (@startPos)
  {
     $tagsOnIndexes[$_] = "$tagsOnIndexes[$_]"."$startTags";
  }      
}# konec foreach(@re_prior)



if ($output eq '')
{
  select STDOUT;
}
else
{
  open(OUTPUT, ">$output") or do
  {  
      print STDERR "Nelze zapisovat do souboru!"; # melo by vracet 3!
      exit(3);
  };
      
  select OUTPUT;
}

my $char;
for( my $i = 0; $i <= $len; $i++)
{
  if($tagsOnIndexes[$i] ne "")
  {
      print $tagsOnIndexes[$i];
  }
  
  $char = substr( $inText, $i , 1 );
  
  #pri parametru --br znacka pred kazdy konec radku vstupniho souboru
  if ($br and ($char eq "\n"))
  {
      print "<br />";
  }
  print $char;
  
}

close OUTPUT;
 
# ----------------------------------------------------------------------------
# ------------------- **** KONEC SKRIPTU **** --------------------------------
# ----------------------------------------------------------------------------

