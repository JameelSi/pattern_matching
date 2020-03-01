# pattern_matching
a simple pattering matching machine written in C where you insert the desired set of strings call it (Dictionary) and then use it to check
if a string X or its substrings matches it.
basically a Deterministic Finite Automaton (DFA) will be bulit of the given set of strings using a linked list structure ,it could be
imagined as a tree where every node corresponds to one symbol when patterns share a common prefix, they also share the corresponding set 
of states in the DFA. then when searching for X it travels along the tree matching multiple patterns simultaneously if it got any matches
it prints them 

included files:
slist.c -> a linked list structure which will be used to build the dictionary
pattern_matching.c -> implements a finite automata that searches if a pattern matches within a given text
