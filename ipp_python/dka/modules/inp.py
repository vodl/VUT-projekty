#!/usr/bin/python3
# -*- coding: utf-8 -*-
#DKA:xdlapa01
import sys
import io
import codecs
class InputFactory:
    def makeInput(self, file, ignoreCase = False):
        if ignoreCase:
            return IgnoreCaseInput(file)
        else:
            return Input(file)

class Input:
    ''' for work with input stream '''
    def __init__(self, file):     
        try:   
            if file == None:
                file = sys.stdin
                self.inputString = io.StringIO(file.read())
            else:         
                with codecs.open(file, 'r', encoding='utf-8') as myFile:
                    self.inputString =io.StringIO( myFile.read())

        except:
            sys.stderr.write("Error in file for read\n")
            sys.exit(2) #error in file for reading

    
    def getChar(self):
        return self.inputString.read(1)
        
    def getLine(self):
        return self.inputString.readline()
    
    def pushBack(self):
        self.inputString.seek(self.inputString.tell() - 1)
        
    
class IgnoreCaseInput(Input):
    def getChar(self):
        return self.inputString.read(1).lower()
        
    def getLine(self):
        return self.inputString.readline().lower()
    
    def pushBack(self):
        self.inputString.seek(self.inputString.tell() - 1)
        

        
    

class Output:
    ''' for work with output stream '''
    def __init__(self, file):     
        try:   
            if file == None:
                self.file = sys.stdout
            else: 
                self.file = codecs.open( file, "w", "utf-8" )      

        except:
            sys.stderr.write("Error in file for write\n")
            sys.exit(3) #error in file for reading
        
    def write(self, mystr):
        self.file.write(mystr)
        
    def writeLn(self,mystr):
        self.file.write(mystr + '\n')
        
    def printChar(self, char):
        self.file.write(char)
        
    def writeChar(self,char):
        self.printChar(char)
        
    def close(self):
        self.file.close()
        
    
        
    
    def getChar(self):
        return self.inputString.read(1)
        
    def getLine(self):
        return self.inputString.readline()