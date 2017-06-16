#!/usr/bin/python3
# -*- coding: utf-8 -*-
#DKA:xdlapa01
class Lex:
    ''' Do the lexical analysys '''
    
    STATES = set(['sNORMAL', 'sESCAPE', 'sIDENTIFIER', 'sERROR', 'sSPECIAL'])
    
    def __init__(self, myInput):
        self.char = ''
        self.idStr = ''
        self.state = 'sNORMAL'
        self.input = myInput
        self.SPECIALS = set(['(', ')', '{', '}', ',', '.'])
        self.ALLSPECIALS = self.SPECIALS.union(set(['-', '>', '#', '\'']))
        
    def _nextChar(self):
        self.char = self.input.getChar()
        return self.char
    
    def hasSymbol(self):
        # empty string should be accepted
        if len(self.idStr) <= 1 and self.state == 'sIDENTIFIER':
            return True
        else:
            return False
        
    def hasArrow(self):
        if self.state == 'sSPECIAL' and self.char == '>':
            return True
        else:
            return False

    def hasId(self):
        if self.state == 'sIDENTIFIER':
            return True
        else:
            return False  
        
    def hasSpecial(self, char):    
        if self.state == 'sSPECIAL' and self.char == char:
            return True
        else:
            return False    
    
    def isCharApostrophe(self):
        if self.char == '\'':
            return True
        else:
            return False
        
    def skipWhite(self):
        while True:
            self._nextChar()
            if self.char == '':
                break
            elif self.char.strip() == '':
                continue
            else:
                break
                     
    
    def _nextInit(self):
        self.idStr = '' # will make id empty for next run
        
    def next(self):        
        self._nextInit()
        self.skipWhite()                
        
        if self.char == '#':
            self.input.getLine()
            return self.next() #recursion 
        
        elif self.char in self.SPECIALS:
            self.state = 'sSPECIAL'
            return self
        
        elif self.char == '-':
            return self._arrow()
            
        elif self.isCharApostrophe():
            self.state = 'sESCAPE'
            return self._escape()
            
        else:
            self.state = 'sIDENTIFIER'
            self.idStr += self.char
            return self._identifier()
                      
            
    def _arrow(self):
        self._nextChar()
        if self.char == '>':
            self.state = 'sSPECIAL'
            return self
        else:
            self.state = 'sERROR'
            return self
        
        
    def _identifier(self):
        while self._nextChar() not in self.ALLSPECIALS and self.char.strip() != '':
            self.idStr += self.char
        self.state = 'sIDENTIFIER'
        self.input.pushBack()
        return self            
        
        
    def _escape(self):
        self._nextChar()
        if self.isCharApostrophe():
            return self.escape2()
        
        self.idStr += self.char # anything but ' is identifier
        self._nextChar()
        
        if self.isCharApostrophe():
            return self._identifier() 
        else:
            self.state = 'sERROR'  
            return self
            
            
    def escape2(self): # for '''' or ''
        self.char = self.input.getChar()
        if self.isCharApostrophe():
            self.char = self.input.getChar()
            if self.isCharApostrophe():
                self.idStr += self.char
                self.state = 'sIDENTIFIER'
                return self._identifier()
            else:
                self.state = 'sERROR'
                return self
        else:
            self.state = 'sIDENTIFIER' # it's '' what is epsilon
            self.input.pushBack()
            return self


class LexWch(Lex):
    ''' modification of Lex for WhiteChar extension '''
    
    def __init__(self,myInput):
        super(LexWch, self).__init__(myInput)
        self.wasWhite = True
        
          
    def hasSpecial(self, char):   
        if char == ',':
            if self.wasWhite:
                return True
             
        if self.state == 'sSPECIAL' and self.char == char:
            return True
        else:
            return False
        
    def skipWhite(self):
        while True:
            self._nextChar()
            if self.char == '':
                break
            elif self.char.strip() == '':
                self.wasWhite = True
                continue
            else:
                break
        
    def _nextInit(self):
        self.idStr = ''
        self.wasWhite = False
    