//
//      Implementation of the Assembler class.
//
#include "stdafx.h"
#include "Assembler.h"
#include "Errors.h"

// Constructor for the assembler.  Note: we are passing argc and argv to the file access constructor.
Assembler::Assembler(int argc, char *argv[])
	: m_facc(argc, argv)
{
	// Nothing else to do here.
}

/*
Assembler::PassI()

NAME

Assembler::PassI - establishes the location of labels.

SYNOPSIS

void Assembler::PassI();

DESCRIPTION

This function parses through the file being passed into the program at every
line, creating symbols for certain lines of code (if necessary) and inserting them
into the Symbol Table.
*/
void Assembler::PassI()
{
	int loc = 0;        // Tracks the location of the instructions to be generated.

						// Successively process each line of source code.
	for (; ; ) {

		// Read the next line from the source file.
		string buff;
		if (!m_facc.GetNextLine(buff)) {

			// If there are no more lines, we are missing an end statement.
			// We will let this error be reported by Pass II.
			return;
		}
		// Parse the line and get the instruction type.
		Instruction::InstructionType st = m_inst.ParseInstruction(buff);

		// If this is an end statement, there is nothing left to do in pass I.
		// Pass II will determine if the end is the last statement.
		if (st == Instruction::ST_End) return;

		// Labels can only be on machine language and assembler language
		// instructions.
		if (st != Instruction::ST_MachineLanguage && st != Instruction::ST_AssemblerInstr) continue;

		// If the instruction has a label, record it and its location in the
		// symbol table.
		if (m_inst.IsLabel()) {

			m_symtab.AddSymbol(m_inst.GetLabel(), loc);
		}
		// Compute the location of the next instruction.
		loc = m_inst.LocationNextInstruction(loc);
	}
}

/*
Assembler::PassII()

NAME

Assembler::PassII - generates machine language translation of code and reports errors,
if any were found.

SYNOPSIS

void Assembler::PassII();

DESCRIPTION

This function parses through the file again, this time creating machine language translation
of the lines of code. It checks for errors and reports if there are any errors. Should there be
no errors, it will make an attempt to insert the code at a given location into the VC3600 emulator.
*/
void Assembler::PassII()
{
	m_facc.Rewind();	// Go back to the start of the file.
	int currentLoc = 0;	// Keeps track of the current location in memory.

	cout << "Translation of Program:" << endl;
	cout << "Location\tContents\tOriginal Statement" << endl;

	for (; ;)
	{
		// Read the next line from the source file.
		Errors::InitErrorReporting(); 
		string buff;
		if (!m_facc.GetNextLine(buff))	// We are missing an end statement, which is an error.
		{
			Errors::RecordError("ERROR: Missing an end statement!");
			Errors::DisplayErrors();
			return;
		}

		// Parse the line and get the instruction type.
		Instruction::InstructionType st = m_inst.ParseInstruction(buff);

		if (st == Instruction::ST_End)
		{
			if (!m_facc.GetNextLine(buff)) // This is actually the end statement.
			{
				TranslateLine(st, currentLoc);
				return;
			}
			else
			{
				Errors::RecordError("ERROR: End statement not actually the last in the program!");
				Errors::DisplayErrors();
				return;
			}
		}

		// Print the line and insert into memory, if possible.
		TranslateLine(st, currentLoc);

		// Location does not get incremented if the line is a comment.
		if (st != Instruction::ST_MachineLanguage && st != Instruction::ST_AssemblerInstr) continue;

		// Compute the location of the next instruction.
		currentLoc = m_inst.LocationNextInstruction(currentLoc);
	}
	system("pause");
	cout << endl;
}

// Runs the VC-3600 emulation (if possible), and displays errors if an error
// occurred during run time.
void Assembler::RunEmulator()
{
	if (m_hasErrors)
	{
		cout << "Cannot run program if it has errors." << endl;
		return;
	}

	Errors::InitErrorReporting();
	cout << "Results from the emulating program:" << endl;
	cout << endl;
	if (!m_emul.RunProgram())
	{
		Errors::DisplayErrors();
	}
	cout << endl;
	cout << "End of emulation." << endl;
}

/*
Assembler::TranslateLine()

NAME

Assembler::TranslateLine - translates the current line that the program is reading
in the text file.

SYNOPSIS

void Assembler::TranslateLine(Instruction::InstructionType a_statement, int a_location);
	a_statement -> the line currently being read in memory
	a_location -> location in the VC3600 memory

DESCRIPTION

This function parses the line that is being visited in the file that is passed
into the program. It uses the type of instruction that is passed into the
function to make the proper parsing of the line of code, in additon to the proper
location that is passed into the function. It also reports errors, should they be
encountered.
*/
void Assembler::TranslateLine(Instruction::InstructionType a_statement, int a_location)
{
	int contents = 0;  // This is the contents in memory that will be stored at a given address.
	string printContents = ""; // The same contents as above, but stored as a string.

	// End statements and comments have no location or contents.
	if (a_statement == Instruction::ST_Comment || a_statement == Instruction::ST_End)
	{
		cout << "\t\t\t\t" << m_inst.GetOriginalStatement() << endl;
		return;
	}

	// If it is not a valid instruction, report an error and return.
	if (a_statement == Instruction::ST_NotInstr)
	{
		cout << a_location << "\t\t" << printContents << "\t\t" << m_inst.GetOriginalStatement() << endl;
		Errors::RecordError("ERROR: Not a valid instruction!");
		Errors::DisplayErrors();
		m_hasErrors = true;
		return;
	}

	// Machine language and assembler language need to be translated and parsed a little
	// differently.
	if (a_statement == Instruction::ST_MachineLanguage)
	{
		cout << a_location << "\t\t";

		TranslateMachineLanguage(printContents);
		cout << printContents << "\t\t" << m_inst.GetOriginalStatement() << endl;

		// If we have errors, list them. If not, put the word into memory at the given address.
		if (m_hasErrors)
		{
			Errors::DisplayErrors();
		}
		else
		{
			contents = stoi(printContents);
			if (!m_emul.InsertMemory(a_location, contents))
			{
				Errors::RecordError("ERROR: Error inserting into memory!");
				Errors::DisplayErrors();
			}
		}
		return;
	}

	if (a_statement == Instruction::ST_AssemblerInstr)
	{
		cout << a_location << "\t\t";

		TranslateAssemblerLanguage(printContents, a_location);
		cout << printContents << "\t\t" << m_inst.GetOriginalStatement() << endl;

		// If we have errors, list them. If not, put the word into memory at the given address.
		if (m_hasErrors)
		{
			Errors::DisplayErrors();
		}
		else
		{	
			if (m_inst.GetOpCode() == "ds" ||
				m_inst.GetOpCode() == "org")
			{
				// Do not insert into memory.
				return;
			}
			else
			{
				contents = stoi(printContents);
				if (!m_emul.InsertMemory(a_location, contents))
				{
					Errors::RecordError("ERROR: Error inserting into memory!");
					Errors::DisplayErrors();
				}
			}
		}
	}
}

/*
Assembler::TranslateMachineLanguage()

NAME

Assembler::TranslateMachineLanguage - properly converts the machine language into
values that can be stored in the VC3600 emulator.

SYNOPSIS

void Assembler::TranslateMachineLanguage(string & a_stringContents);
	a_stringContents -> the value of what will (try) to be stored into the emulator,
	stored as a string in case ?s need to be used

DESCRIPTION

This function translates the machine language code into a value that can be
stored into the VC3600 emulator, where it will determine how to perform various
operations through the op codes that will be given to it through this function.
*/
void Assembler::TranslateMachineLanguage(string & a_stringContents)
{
	// Determine if this is a legal op code.
	if (m_inst.GetNumOpCode() == 0)
	{
		Errors::RecordError("ERROR: Illegal op code!");
		m_hasErrors = true;
		a_stringContents += "??";
	}
	else
	{
		if (m_inst.GetNumOpCode() < 10) // Machine language instructions have op codes.
		{
			a_stringContents += "0";
		}
		a_stringContents += to_string(m_inst.GetNumOpCode());
	}


	// Is this a halt or something with an operand?
	if (m_inst.GetNumOpCode() == 13)
	{
		a_stringContents += "0000";
	}
	// If not, we find the operand's location.
	else
	{
		// But before we do that, the operands here should be symbolic.
		if (m_inst.IsNumOperand())
		{
			Errors::RecordError("ERROR: The operand must be symbolic!");
			a_stringContents += "????";
			m_hasErrors = true;
		}
		else
		{
			int symbolLoc = 0;
			if (!m_symtab.LookupSymbol(m_inst.GetOperand(), symbolLoc))	// Label does not exist.
			{
				Errors::RecordError("ERROR: Label is undefined!");
				a_stringContents += "????";
				m_hasErrors = true;
			}
			else
			{
				if (symbolLoc == -999) // Multiply defined label.
				{
					Errors::RecordError("ERROR: Label is multiply defined!");
					m_hasErrors = true;
				}
				if (symbolLoc >= 10000) // Address is too large to store in the computer.
				{
					Errors::RecordError("ERROR: Address is too large to store in memory!");
					m_hasErrors = true;
				}
				if (symbolLoc < 1000)
				{
					a_stringContents += "0";
				}
				a_stringContents += to_string(symbolLoc);
			}
		}
	}

	// If there are extra operands, report indicating so.
	if (m_inst.HasExtraOperand())
	{
		Errors::RecordError("ERROR: Has extra operands!");
	}
}

/*
Assembler::TranslateAssemblerLanguage()

NAME

Assembler::TranslateAssemblerLanguage - properly converts the assembler language into
values that can be stored in the VC3600 emulator, if possible.

SYNOPSIS

void Assembler::TranslateAssemblerLanguage(string & a_stringContents, int a_location);
	a_stringContents -> the value of what will (try) to be stored into the emulator, stored
	as a string in case ?s need to be used
	a_location -> location value in case the instruction wants to change the location of code in
	the emulator

DESCRIPTION

This function translates the assembler language code into a value that can be
stored into the VC3600 emulator, or if it cannot be stored as a value, to change the
location that the assembler instruction most likely wanted to change.
*/
void Assembler::TranslateAssemblerLanguage(string & a_stringContents, int a_location)
{
	// Determine if this is a legal assembly instruction.

	string origOpCode = m_inst.GetOpCode();
	for (int i = 0; i < origOpCode.length(); i++)
	{
		origOpCode[i] = tolower(origOpCode[i]);
	}

	if (origOpCode == "ds" ||
		origOpCode == "org") // Defining storage and setting the origin does not have contents in memory.
	{
		bool isANumerical = m_inst.IsNumOperand();
		if (!isANumerical)	// The ds and org instructions can only have numerical operands.
		{
			Errors::RecordError("ERROR: Operand must be numeric!");
			m_hasErrors = true;
		}

		if (m_inst.GetNumOperand() > 9999) // Cannot store in memory addresses above 9999.
		{
			Errors::RecordError("ERROR: Operand address value is too large!");
			m_hasErrors = true;
		}

		int newLoc = a_location + m_inst.GetNumOperand();

		if (newLoc > 9999) // Cannot store in memory addresses above 9999.
		{
			Errors::RecordError("ERROR: Operand address value is too large!");
			m_hasErrors = true;
		}
	}
	else
	{
		if (origOpCode != "dc")
		{
			Errors::RecordError("ERROR: Illegal op code!");
			m_hasErrors = true;
			a_stringContents += "??????";
		}

		// Next, determine the value of the operand.

		int value = m_inst.GetNumOperand();

		if (value > 999999) // Value is too big to store in memory.
		{
			Errors::RecordError("ERROR: Value too big to store in memory!");
			a_stringContents += to_string(value);
			m_hasErrors = true;
		}
		else
		{
			// To display the contents properly, we will need to determine how many zeroes to output.

			int digitNum = FindDigits(value);

			for (int i = digitNum; i < 6; i++)
			{
				a_stringContents += "0";
			}
			a_stringContents += to_string(value);
		}

		// If there are extra operands, report indicating so.
		if (m_inst.HasExtraOperand())
		{
			Errors::RecordError("ERROR: Has extra operands!");
		}
	}
}
