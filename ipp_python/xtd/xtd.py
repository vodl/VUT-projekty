#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
#XTD:xdlapa01

import argparse
#import getopt
import sys
#import xml.dom.minidom
import xml.etree.ElementTree
#import re
import codecs

from modules import table
                


def parse_argv():
    '''Zpracovani parametru '''
    
    xc = 0
    hf = False
    for x in sys.argv:
        xc = xc + 1
        if x == '--help':
            hf = True
            if xc > 1:
                sys.stderr.write("help musi byt samostatne")
                sys.exit(1)
            else:
                continue
                
        if hf ==True:
            sys.stderr.write("help musi byt samostatne")
            sys.exit(1)
            
        

    parser = argparse.ArgumentParser(description='Short sample app', conflict_handler='resolve')#, add_help=False)
    
    parser.add_argument('-a', action="store_true", default=False)
    parser.add_argument('-b', action="store_true", default=False)
    parser.add_argument('-g', action="store_true", default=False)
    parser.add_argument('-i','--input', action='store', help='Vstupni soubor')
    parser.add_argument('-o','--output', action='store', help='Vystupni soubor')
    parser.add_argument('-h','--header', action='store', help='Hlavicka')
    parser.add_argument('-e','--etc', action='store', type=int, default=-1, help='sloupce')
    

    try:
        options = parser.parse_args()
    except():
        sys.stderr.write("chyba argumentu prikazove radky")
        sys.exit(1)
        
            
    return options



######################### MAIN ##########################
if __name__ == "__main__":

    params = parse_argv()

    ''' Otevirani vstupnich a vystupnich souboru '''
    if params.input is None:
        params.input = sys.stdin = codecs.getreader('utf-8')(sys.stdin)
    else:
        try:
            params.input = open(params.input, 'r', encoding='utf-8');
        except:
            sys.stderr.write("chyba souboru pro cteni")
            sys.exit(2)
        
    if params.output is None:
        params.output = sys.stdout# = codecs.getwriter('utf-8')(sys.stdout)
    else:
        try:
            params.output= open(params.output, 'w', encoding='utf-8');
        except:
            sys.stderr.write("chyba souboru pro zapis")
            sys.exit(3)


    if params.b:
        params.etc=1
        
    ''' Zpracovani vstupniho souboru'''
    doc = xml.etree.ElementTree.parse(params.input)#,parser)
    
    myDict = {}# slovni rodic: seznam potomku
    pomDict = {}# pomocny slovnik
    
    root = doc.getroot()
    
    #pruchod stromem
    for parent in root.iter():
        if parent == root:
            continue

        if parent.tag.lower() not in myDict: 
            myDict[parent.tag.lower()] = []
            pomDict[parent.tag.lower()] = []
        actCountDict = {}

        #pro kazdy element pridej potomky 
        for child in parent:
                     
            if not child.tag.lower() in actCountDict:
                actCountDict[child.tag.lower()] = 1
            else:
                actCountDict[child.tag.lower()] += 1
                
            if actCountDict[child.tag.lower()] > pomDict[parent.tag.lower()].count(child.tag.lower()):
                pomDict[parent.tag.lower()].append(child.tag.lower())
                myDict[parent.tag.lower()].append(child)
            

    tableObjDict = {}
           
    # vytvoreni tabulek        
    for key in myDict.keys():
        mytable = table.Table(key, params.etc)
        tableObjDict[key] = mytable
        
    #a cizich klicu v nich
    for key in myDict.keys():
        tableObjDict[key].setFkeys(myDict[key])

        
    # vytvoreni sloupcu tabulek
    for parent in root.iter():
        for child in parent:
            #tableobj = tableObjDict[child.tag.lower()]
            if parent is not root:
                tableObjDict[child.tag.lower()].setParent(parent)

            if child.text is None:
                child.text = ""
            child.text = child.text.lstrip() #odstraneni zarovnavacich bilich znaku
            if params.a == False:
                myAttrDic = {}
                for k in child.attrib.keys():
                    myAttrDic[k.lower()] = child.attrib[k] 
                tableObjDict[child.tag.lower()].setAttrs(myAttrDic,child.text)


    ''' Vypis hlavicky'''            
    if params.header is not None:
        params.output.write("--" + params.header + "\n\n")
        
    ''' Tisk tabulek '''
    for t in tableObjDict.values():
        params.output.write(str(t.getTableStr()))
