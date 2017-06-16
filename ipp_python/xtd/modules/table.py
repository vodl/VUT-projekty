#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
#XTD:xdlapa01

import re

def getType(mystr):
    ''' Ziskani datoveho typu'''
    matchObj = re.match( r'(^$)|[01]|(True)|(False)', mystr)
    if matchObj:
        return "BIN"
    else:
        matchObj = re.match( r'^\d+$', mystr)
        if matchObj:
            return "INT"
        else:
            matchObj = re.match(r'^[-+]?[0-9]+[.]?[0-9]+([eE][-+]?[0-9]+)?$', mystr)
            if matchObj:
                return "FLOAT"
            else:
                return "TEXT"
    

class Table:
    ''' Trida reprezentujici tabulku'''
    def __init__(self, name, etc=-1):
        self.fkeys = []
        self.name = name
        self.cols = []
        self.attrs = {}
        self.etc = etc
        self.parent = None
        self.fparent = False
        
    def setParent(self, parent):
        self.parent = parent
        if self.fparent == True:
            self.setFkeys(list(parent))
        
        
        
    def setColumns(self, cols):
        self.cols = cols
        
    def setAttrs(self,newattrs,textvalue):
        ''' Nastavi atributy '''
        
        re.sub(r"^\s+", "", textvalue, flags = re.MULTILINE)
        if textvalue is not "":
            newattrs['value'] = textvalue

        for k in newattrs:
            if k in self.attrs:
                if getType(newattrs[k]) == "BIN":
                    pass
                else:
                    tmp = getType(newattrs[k])
                    if tmp == 'TEXT':
                        if (k == 'value'):
                            self.attrs[k] = 'NTEXT'
                        else:
                            self.attrs[k] = 'NVARCHAR'
                    else:
                        self.attrs[k] = tmp
                
            else:
                tmp = getType(newattrs[k])
                if tmp == 'TEXT':
                    if (k == 'value'):
                        self.attrs[k] = 'NTEXT'
                    else:
                        self.attrs[k] = 'NVARCHAR'
                else:
                    self.attrs[k] = tmp
                 
                
    def getAttrs(self):
        return self.attrs
    
    def getAttrsStr(self):
        mystr = ""
        for key in self.attrs.keys():
            mystr = mystr + ",\n   " + key + " " + self.attrs[key]
            
        return mystr
            
        
    def getTableStr(self):
        ''' Generuje vysledny retezec '''
        return "CREATE TABLE " + self.name + "(\n   prk_" + self.name + "_id INT PRIMARY KEY" + self.getFkeysStr() + self.getAttrsStr() + "\n);\n"
        
        
    def setFkeys(self, fkeys):
        ''' Nastavi cizi klice a pritom 
            zkontroluje zda nema mit cizi klic otce'''
        for x in fkeys:
            self.fkeys.append(x.tag.lower())
        #self.fkeys = fkeys
        
        #kontrola
        fkeysDict = {}
        
        # spocitani cetnosti jednotlivych cizich klicu
        for myfkey in self.fkeys:
            if not myfkey in fkeysDict:
                fkeysDict[myfkey] = 1
            else:
                fkeysDict[myfkey] += 1
            
        for key in fkeysDict.keys():
            if (fkeysDict[key] > self.etc) & (self.etc >= 0):
                self.fparent = True #tabulka ma cizi klic otce




        
    def getFkeys(self):
        ''' vrati naformatovane cizi klice jako seznam'''
        fkeysDict = {}
        outlist = []
        
        # spocitani cetnosti jednotlivych cizich klicu
        for myfkey in self.fkeys:
            if not myfkey in fkeysDict:
                fkeysDict[myfkey] = 1
            else:
                fkeysDict[myfkey] += 1
            
        #vygenerovani kyzeneho poctu cizich klicu
        for key in fkeysDict.keys():
            #vrat mensi z dvojice: pocet klicu vs etc(pokud je platne) 
            if (fkeysDict[key] > self.etc) & (self.etc >= 0): 
                continue
            else:
                genCount = fkeysDict[key]
                    
            if genCount == 1:
                outlist.append(key + "_id INT")
            else:
                for i in range(genCount):
                    outlist.append(key + str(i+1) + "_id INT")
                
        return outlist
    


    def getFkeysStr(self):
        ''' udela ze seznamu cizich klicu retezec'''
        mystr = ""
        for x in self.getFkeys():
            mystr += ',\n   ' + x + ""
        return mystr