//
//		Implementation of the symbol table class.
//
#include "stdafx.h"
#include "SymTab.h"

/*
NAME

AddSymbol - adds a new symbol to the symbol table.

SYNOPSIS

void AddSymbol(string &a_symbol, int a_loc);

DESCRIPTION

This function will place the symbol "a_symbol" and its location "a_loc"
in the symbol table.
*/
void
SymbolTable::AddSymbol(string &a_symbol, int a_loc)
{
	// If the symbol is already in the symbol table, record it as multiply defined.
	map<string, int>::iterator st;
	st = m_symbolTable.find(a_symbol);
	if (st != m_symbolTable.end())
	{
		st->second = multiplyDefinedSymbol;
		return;
	}
	// Record the location in the symbol table.
	m_symbolTable[a_symbol] = a_loc;
}

/*
void SymbolTable::DisplaySymbolTable()

NAME

void SymbolTable::DisplaySymbolTable - parses the instruction being passed into
the function

SYNOPSIS

void SymbolTable::DisplaySymbolTable();

DESCRIPTION

This function displays the Symbol Table by iterating through it from beginning to end. It
also places a dotted line to separate it from the machine code translation that will come
later.
*/
void SymbolTable::DisplaySymbolTable()
{
	int numberCount = 0;
	cout << "Symbol Table:" << endl;
	cout << endl;
	cout << "Symbol#\tSymbol\tLocation" << endl;

	for (map<string, int>::iterator it = m_symbolTable.begin(); it != m_symbolTable.end(); it++)
	{
		cout << numberCount << "\t" << it->first << "\t" << it->second << endl;
		numberCount++;
	}

	cout << "---------------------------------------------" << endl;
	system("pause");
}

/*
bool SymbolTable::LookupSymbol()

NAME

void SymbolTable::LookupSymbol - looks up the symbol in the symbol table

SYNOPSIS

bool SymbolTable::LookupSymbol(string &a_symbol, int &a_loc);
	a_symbol -> symbol to be looked up
	a_loc -> location of the symbol

DESCRIPTION

This function looks up the symbol passed into the function in the symbol table. If it
is found, it records the location of the symbol into the a_loc passed into the function.

RETURNS

Returns true if the symbol was successfully found. Returns false if the symbol could not
be found.
*/
bool SymbolTable::LookupSymbol(string &a_symbol, int &a_loc)
{
	map<string, int>::iterator st;
	st = m_symbolTable.find(a_symbol);
	if (st != m_symbolTable.end())
	{
		a_loc = st->second;
		return true;
	}
	else
	{
		return false;
	}
}