#!/usr/bin/python3
# -*- coding: utf-8 -*-
#DKA:xdlapa01

import sys
class Automata:
    ''' Class representing the automata to be determinized, deepsilonized or whatever '''

    def __init__(self):
        self.sStates = set()  # stavy
        self.aSymbols = set()  # symboly
        self.aRules = {}
        self.aStart = set()
        self.aTerminals = set()
        self.deepsilonized = False
    
    def addState(self, s):
        self.sStates.add(s)
        
    def addSymbol(self, s):
        self.aSymbols.add(s)
        
    def addRule(self, fromState, to, by):
        if fromState in self.aRules:
            self.aRules[fromState].add((by,to))
        else:
            self.aRules[fromState] = set()
            self.aRules[fromState].add((by,to))
            
        
    def addStart(self, s):
        self.aStart = s
        
    def addTerminal(self, t):
        self.aTerminals.add(t)
        
    def getEscapedSymbolsSorted(self):
        escapedSet = set()
        for sym in self.aSymbols:
            if sym == "'":
                sym = "''"
            escapedSet.add("'"+ sym + "'")
            
        return sorted(escapedSet)
    
    def symbolCount(self):
        return len([i for i in self.aSymbols])
        
        
    def checkSemantics(self):
        ''' will check if semantics is allright '''
        error = False
        if self.symbolCount() == 0:
            error = True
            sys.stderr.write("Semantics error: empty symbol set\n")
        elif self.aStart not in self.sStates:
            error = True
            sys.stderr.write("Starting state is not in set of states\n")
        elif not set(self.aTerminals).issubset(set(self.sStates)):
            sys.stderr.write("Terminal states is not subset of set of states\n")
            error = True
            
        #rules check
        self.addSymbol('')
        for fromState in self.aRules:
            for rule in sorted(self.aRules[fromState]) :
                if fromState not in self.sStates:
                    error = True
                elif rule[0] not in self.aSymbols:
                    error = True
                elif rule[1] not in self.sStates:
                    error = True
                if error:
                    sys.stderr.write("Invalid rule: " + fromState + " " + rule[0] + " -> " + rule[1] + "\n")
        if error:
            sys.exit(41)
        
        self.aSymbols.remove('')
                    
    
    
    
    
    def deepsilonize(self):
        ''' will make this automata free of epsilon rules '''
        self.checkSemantics()
        new_automata = Automata() #R' is empty set()
        
        for p in self.sStates:
            #New rules
            for p_new in getEpsClosure(p, self.aRules):
                if p_new in self.aRules:             
                    for target in self.aRules[p_new]:
                        if target[0] != '':
                            new_automata.addRule(p, target[1], target[0])

            #End states    
            for f_new in set(getEpsClosure(p,self.aRules)).intersection(self.aTerminals):
                new_automata.addTerminal(f_new)
                
        new_automata.aStart = self.aStart
        new_automata.aSymbols = self.aSymbols
        new_automata.sStates = self.sStates
        
        self.aRules = new_automata.aRules
        self.aTerminals = new_automata.aTerminals
        self.deepsilonized = True
        
        return new_automata
    
    
    def determinize(self):
        ''' determinization as is in IFJ lecture,
            symbolics came from IFJ slides
            Q_ii means Q'' set etc.              '''
        if self.deepsilonized == False:
            self.deepsilonize()
        
        aut_d = Automata()
        aut_d.aStart = self.aStart #sd := {s};
        # rest of sets is empty
        
        Q_new = []
        Q_new.append([aut_d.aStart])

        while True:
            Q_i = Q_new.pop()
            aut_d.addState("_".join(sorted(Q_i)))
            
            for a in self.aSymbols:
                Q_ii = set()
                for q_i in Q_i:
                    if q_i in self.aRules:
                        Q_ii.update([i[1] for i in self.aRules[q_i] if a == i[0]])
                        #while computing with sets it's represented as sets or lists      
                if len(Q_ii) != 0:
                    aut_d.addRule("_".join(sorted(Q_i)), "_".join(sorted(Q_ii)), a)  
                    # for outpu purpose is set transfered to it's text representation                
                    if "_".join(sorted(Q_ii)) not in aut_d.sStates:
                        Q_new.append(Q_ii)           
                                    
            if len(self.aTerminals.intersection(set(Q_i))) != 0:
                aut_d.addTerminal("_".join(sorted(Q_i)))
            if len(Q_new) == 0:
                break
            
        self.sStates = aut_d.sStates
        self.aStart = aut_d.aStart
        self.aRules = aut_d.aRules
        self.aTerminals = aut_d.aTerminals
            
    def printMe(self, out):
        ''' will print this automata into out stream '''
        #states
        out.writeLn("(")
        out.printChar("{")
        out.write(", ".join(sorted(self.sStates)))
        out.writeLn("},")
        
        #symbols
        out.printChar("{")
        out.write(", ".join(self.getEscapedSymbolsSorted()))
        out.writeLn("},")
        
        rulesCount = 0 # because of last comma
        
        #rules
        for key in self.aRules:
            for rule in self.aRules[key]:
                rulesCount += 1
        
        rulesCount2 = 0
        
        out.writeLn("{")
        for fromState in sorted(self.aRules):
            for rule in sorted(self.aRules[fromState]) :
                rulesCount2 += 1
                
                out.printChar(fromState)
                out.printChar(" ")
                out.printChar("'")
                if rule[0] == "'":
                    out.printChar("'") # double ' character
                out.printChar(rule[0])
                out.printChar("'")
                out.write(" -> ")
                out.printChar(rule[1])
                
                if rulesCount == rulesCount2:
                    out.writeLn("")
                    break
                else:
                    out.writeLn(",")
                    
        #end of rules
        out.writeLn("},")
             
        #start state
        out.writeChar(self.aStart)
        out.writeLn(",")
                
            
        # terminals
        out.printChar("{")
        out.write(", ".join(sorted(self.aTerminals)))
        out.writeLn("}")
        
        out.write(")")
        
        
          

def getEpsClosure(p, rules):
    closure  = set()
    closure.add(p) #Q0 := {p}; #state itself is always in it's eps closure
    
    if p not in rules:
        return closure
    
    statesrules = list(rules[p])#it's more efective to iterate through the list than through whole set in each pass
    for myrule in statesrules:
        if myrule[0] == '': # if element of tuple is empty string then it's eps rule
            closure.add(myrule[1])
            for newrule in rules[myrule[1]]:
                if newrule not in statesrules:
                    statesrules.append(newrule)
    return closure

            

    
        
    
    