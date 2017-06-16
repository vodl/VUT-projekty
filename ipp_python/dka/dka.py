#!/usr/bin/python3
# -*- coding: utf-8 -*-
#DKA:xdlapa01

import argparse
import sys
from modules.inp import Output, InputFactory
from modules.lexical import Lex
from modules.lexical import LexWch
from modules.automata import Automata
from modules.syntactic import Syntactic


def parseParams():
    ''' Argument parsing '''
    parser = argparse.ArgumentParser(description='Script for fine state automata determinization')
    
    parser.add_argument('--input', action='store', help='Input file')
    parser.add_argument('--output', action='store', help='Output file')

    group = parser.add_mutually_exclusive_group()  # -e and -d cannot be combined
    group.add_argument('-e', '--no-epsilon-rules', action="store_true", default=False, dest="e", help='will make automata free of epsilon rules')
    group.add_argument('-d', '--determinization', action="store_true", default=False, help='this will run the determinization') 
    
    parser.add_argument('-i', '--case-insensitive', action="store_true", default=False, dest="i", help='Case insensitive input')
    parser.add_argument('-w', '--white-char', action="store_true", default=False, dest="w", help='White characters accepted instead of commas')
    
    try:
        options = parser.parse_args()
    except SystemExit:
        sys.stderr.write("argument error: \n")
        sys.exit(1)
                    
    return options
    


        
    
    
#-------- Main function ----------------------------------
if __name__ == '__main__':
    ''' I hope it's kindof selfdescriptive... '''
    options = parseParams()

    automata = Automata()
    inp = InputFactory().makeInput(options.input, options.i)
 
    if options.w:
        lex = LexWch(inp)
    else:
        lex = Lex(inp)
    
    syntaxer = Syntactic(automata, lex)
    syntaxer.run()
    
    automata.checkSemantics()
    if options.e:
        automata.deepsilonize()     
    if options.determinization:
        automata.determinize()

    out = Output(options.output)
    automata.printMe(out)
    out.close()
    #the end, thank you!
    