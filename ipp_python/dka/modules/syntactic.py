#!/usr/bin/python3
# -*- coding: utf-8 -*-
#DKA:xdlapa01
import sys
import re

def syntError():
    sys.stderr.write("Syntax error")
    sys.exit(40)
    #raise SyntaxError


class Syntactic():
    ''' Class for syntactic analysis of automata '''
    
    def __init__(self, auto, lex):
        self.automata = auto
        self.lex = lex
        self.idregex = re.compile(r"^\w([\w\d_]*[\d\w]$|$)", re.UNICODE)
        
    def run(self):  
        self.specialExpected('(')
        self._setAllSets()
        self.specialExpected(')')
        self.checkRest()
        
    def checkRest(self):
        if not self.nextChar():
            pass
        else:
            syntError()
        
    def nextSet(self):
        self.specialExpected(',')
        self.specialExpected('{')
        
        
    def _setAllSets(self):
        self.specialExpected('{')
        self.setSet('sSETSTATES')
        self.nextSet()
        self.setSet('sSETSYMBOLS')
        self.nextSet()
        self.setSet('sSETRULES')     
        self.specialExpected(',')
        self.nextChar()   
        if self.lex.hasId():
            self.automata.addStart(self.lex.idStr)
        else:
            syntError()
        self.nextSet()
        self.setSet('sSETTERMSTATES')     
                          
                    
      
    def setSet(self, which):
        ''' will set all the sets of automata following the syntax '''
        state = 'sSTART'
        
        while True:
            self.nextChar()
            
            if state == 'sSTART':
                if self.lex.hasSpecial('}'):
                    return        
                elif self.lex.hasId():
                    self._addToSet(which)
                    state = 'sELEMENT'        
                else:
                    syntError()
            
            elif state == 'sELEMENT':
                if self.lex.hasSpecial('}'):
                    return          
                elif self.lex.hasSpecial(','):
                    state = 'sCOMMA'
                    continue
                else:
                    syntError()   
                    
            elif state == 'sCOMMA':
                if self.lex.hasId():
                    self._addToSet(which)
                    state = 'sELEMENT'
                else:
                    syntError()   
            else:
                raise Exception('This should never gonna happen')
      
      
    def _addToSet(self, which):
        ''' will add which to set actually in progress '''
        if which == 'sSETSTATES' and self.idregex.match(self.lex.idStr):
            self.automata.addState(self.lex.idStr)
        elif which == 'sSETSYMBOLS':
            if self.lex.hasSymbol() and self.lex.idStr != '':
                self.automata.addSymbol(self.lex.idStr)
            else:
                syntError()    
        elif which == 'sSETRULES':
            self._saveRule()     
        elif which == 'sSETTERMSTATES' and re.match(self.idregex, self.lex.idStr):
            self.automata.addTerminal(self.lex.idStr)
        else:
            syntError()    
                   
            
            
    def _saveRule(self):
        ''' for setting the rules '''
        startState = self.lex.idStr
        bySymbol = ''
        toState = ''
        
        self.nextChar()
        
        if self.lex.hasSymbol():
            bySymbol = self.lex.idStr
            self.nextChar()
            
        if self.lex.hasArrow():
            self.nextChar()
            if self.lex.hasId():
                toState = self.lex.idStr
                self.automata.addRule(startState, toState, bySymbol)
            else:
                syntError()
        else:
            syntError()   

                     
                
            
    def specialExpected(self, char):
        ''' will exit program with syntax error
         if next lexem is not what expected  '''
        if self.nextChar() == char:
            return True
        else:
            syntError()
       
        
    def nextChar(self):
        ''' syntactic sugar for getting next token '''
        return self.lex.next().char
        
    
                
        
            
        
            
            
                

                
        
        
    