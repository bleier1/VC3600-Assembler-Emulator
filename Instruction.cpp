//
//		Implementation of the Instruction class.
//
#include "stdafx.h"
#include "Instruction.h"

/*
Instruction::InstructionType Instruction::ParseInstruction()

NAME

Instruction::InstructionType Instruction::ParseInstruction - parses the instruction being
passed into the function

SYNOPSIS

Instruction::InstructionType Instruction::ParseInstruction(string &a_buff);
	a_buff -> current line being visited in the file passed into program

DESCRIPTION

This function parses the instruction that gets passed into it. When being parsed, various
variables in the Instruction class will be filled in, made easier through the usage of private
functions to assist in the parsing. The function aims to fill in as many variables in the Instruction
class as possible to make creating the Symbol Table and the translations easier.

RETURNS

Returns the type of the instruction that is passed in.
*/
Instruction::InstructionType Instruction::ParseInstruction(string &a_buff)
{
	// Initialize the member variables.
	InitMemVariables();

	m_instruction = a_buff;
	string a, b, c, d;

	// If the first character is a semicolon, then the entire line is a comment and should
	// be indicated as such.
	if (IsComment(a_buff))
	{
		return Instruction::ST_Comment;
	}
	
	// If there any comments after the first character, remove them.
	DeleteComments(a_buff);

	istringstream parser(a_buff);
	parser >> a >> b >> c >> d;

	// "d" should be empty. If it isn't, we're going to have an error later.
	if (!d.empty())
	{
		m_hasExtraOps = true;
	}

	// If "c" is empty, then there should be no label, and the op code should be in "a."
	if (c.empty())
	{
		m_Label = "";
		m_OpCode = a;
		// Check to see if "b" is a numeric operand.
		if (ConvertToInt(b))
		{
			m_IsNumericOperand = true;
			m_OperandValue = stoi(b);
		}
		m_Operand = b;

		// Check to see if "a" is op code. Converting to lowercase makes comparison easier.
		ConvertToLower(a);
		if (IsEnd(a))
		{
			return InstructionType::ST_End;
		}
		if (IsMachineLanguage(a))
		{
			// Find the numeric op code for the machine language instruction.
			m_NumOpCode = FindNumericOpCode(a);
			m_type = ST_MachineLanguage;

			return InstructionType::ST_MachineLanguage;
		}
		if (IsAssemblyLanguage(a))
		{
			return InstructionType::ST_AssemblerInstr;
		}
		if (!IsAssemblyLanguage(a) && !IsMachineLanguage(a))
		{
			return InstructionType::ST_NotInstr;
		}
	}
	// If "c" is not empty, then there should be a label.
	else
	{
		m_Label = a;
		m_OpCode = b;
		// Check to see if "c" is a numeric operand.
		if (ConvertToInt(c))
		{
			m_IsNumericOperand = true;
			m_OperandValue = stoi(c);
		}
		m_Operand = c;

		// Check to see if "b" really is op code. Converting to lowercase makes comparison easier.
		ConvertToLower(b);
		if (IsEnd(b))
		{
			return InstructionType::ST_End;
		}
		if (IsMachineLanguage(b))
		{
			// Find the numeric op code for the machine language instruction.
			m_NumOpCode = FindNumericOpCode(b);
			m_type = ST_MachineLanguage;

			return InstructionType::ST_MachineLanguage;
		}
		if (IsAssemblyLanguage(b))
		{
			return InstructionType::ST_AssemblerInstr;
		}
		if (!IsMachineLanguage(b) && !IsMachineLanguage(b))
		{
			return InstructionType::ST_NotInstr;
		}
	}
}

// Computes the location of the next instruction.
int Instruction::LocationNextInstruction(int a_loc)
{
	//Easier to compare and to keep original op code intact.
	string opCodeCheck = m_OpCode;
	ConvertToLower(opCodeCheck);
	if (opCodeCheck == "org")
	{
		return m_OperandValue;
	}
	else if (opCodeCheck == "ds")
	{
		return a_loc + m_OperandValue;
	}
	else
	{
		return a_loc + 1;
	}
}

// Initializes all the member variables.
void Instruction::InitMemVariables()
{
	m_Label = "";
	m_OpCode = "";
	m_Operand = "";
	m_instruction = "";
	m_NumOpCode = 0;
	m_type = ST_Comment;
	m_IsNumericOperand = false;
	m_OperandValue = NULL;
	m_hasExtraOps = false;
}

// Determines if the entire line is a comment.
bool Instruction::IsComment(string a_parsedWord)
{
	char firstChar = a_parsedWord[0];
	if (firstChar == ';')
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Deletes any portions of the string that has a semicolon and whatever follows.
void Instruction::DeleteComments(string & a_instruction)
{
	for (int i = 0; i < a_instruction.length(); i++)
	{
		if (a_instruction[i] == ';')
		{
			a_instruction.erase(i, a_instruction.length() - 1);
			return;
		}
	}
}

// Convert the string to lowercase, since op codes do not need to be case sensitive. This makes for
// easier comparison.
void Instruction::ConvertToLower(string & a_input)
{
	for (int i = 0; i < a_input.length(); i++)
	{
		a_input[i] = tolower(a_input[i]);
	}
}

// Converts the string passed in into an integer, if possible.
bool Instruction::ConvertToInt(string a_parsedWord)
{
	if (!a_parsedWord.empty())
	{
		for (int i = 0; i < a_parsedWord.length(); i++)
		{
			if (a_parsedWord[i] < 48 || a_parsedWord[i] > 57)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

// Determines if the instruction is an end statement.
bool Instruction::IsEnd(string a_parsedWord)
{
	if (a_parsedWord == "end")
	{
		return true;
	}
	return false;
}

// Determines if the instruction is machine language.
bool Instruction::IsMachineLanguage(string a_parsedWord)
{
	if (a_parsedWord == "add" ||
		a_parsedWord == "sub" ||
		a_parsedWord == "mult" ||
		a_parsedWord == "div" ||
		a_parsedWord == "load" ||
		a_parsedWord == "store" ||
		a_parsedWord == "read" ||
		a_parsedWord == "write" ||
		a_parsedWord == "b" ||
		a_parsedWord == "bm" ||
		a_parsedWord == "bz" ||
		a_parsedWord == "bp" ||
		a_parsedWord == "halt")
	{
		return true;
	}
	return false;
}

// Determine if the instruction is assembly language.
bool Instruction::IsAssemblyLanguage(string a_parsedWord)
{
	if (a_parsedWord == "dc" || 
		a_parsedWord == "ds" ||
		a_parsedWord == "org")
	{
		return true;
	}
	return false;
}

/*
int Instruction::FindNumericOpCode()

NAME

int Instruction::FindNumericOpCode() - finds the numerical value of the
op code

SYNOPSIS

int Instruction::FindNumericOpCode(string a_parsedWord);
	a_parsedWord -> operation to be translated into op code

DESCRIPTION

This function determines the numerical value of the operation that is passed into
it. This is done through numerous if statements. Since this function is only called
once ParseInstruction has determined if the line is assembler or machine language,
there is no need to spit out a value to say otherwise.

RETURNS

Returns the numerical value of the op code.
*/
int Instruction::FindNumericOpCode(string a_parsedWord)
{
	if (a_parsedWord == "add")
	{
		return 1;
	}
	if (a_parsedWord == "sub")
	{
		return 2;
	}
	if (a_parsedWord == "mult")
	{
		return 3;
	}
	if (a_parsedWord == "div")
	{
		return 4;
	}
	if (a_parsedWord == "load")
	{
		return 5;
	}
	if (a_parsedWord == "store")
	{
		return 6;
	}
	if (a_parsedWord == "read")
	{
		return 7;
	}
	if (a_parsedWord == "write")
	{
		return 8;
	}
	if (a_parsedWord == "b")
	{
		return 9;
	}
	if (a_parsedWord == "bm")
	{
		return 10;
	}
	if (a_parsedWord == "bz")
	{
		return 11;
	}
	if (a_parsedWord == "bp")
	{
		return 12;
	}
	if (a_parsedWord == "halt")
	{
		return 13;
	}
}
