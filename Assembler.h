//
//		Assembler class.  This is a container for all the components
//		that make up the assembler.
//
#pragma once 

#include "SymTab.h"
#include "Instruction.h"
#include "FileAccess.h"
#include "Emulator.h"


class Assembler {

public:
	Assembler(int argc, char *argv[]);

	// Pass I - establish the locations of the symbols
	void PassI();

	// Pass II - generate a translation
	void PassII();

	// Displays the symbols in the symbol table.
	void DisplaySymbolTable() { m_symtab.DisplaySymbolTable(); }

	// Run emulator on the translation.
	void RunEmulator();

private:

	// Prints the machine language translation and inserts into memory, if possible.
	void TranslateLine(Instruction::InstructionType a_statement, int a_location);

	// Translates machine language instructions.
	void TranslateMachineLanguage(string & a_stringContents);

	// Translates assembler language instructions.
	void TranslateAssemblerLanguage(string & a_stringContents, int a_location);

	// Determines the amount of digits in a number.
	int FindDigits(int a_value)
	{
		int count = 0;
		while (a_value != 0)
		{
			a_value /= 10;
			count++;
		}
		return count;
	}

	FileAccess m_facc;	    // File Access object
	SymbolTable m_symtab;	// Symbol table object
	Instruction m_inst;	    // Instruction object
	Emulator m_emul;        // Emulator for VC3600
	bool m_hasErrors = false; // Determines if there are errors in Pass II.
};