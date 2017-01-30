//
// Class to parse and provide information about instructions
//
#pragma once

// The elements of an instruction.
class Instruction {

public:

	// Codes to indicate the type of instruction we are processing.  Why is this inside the
	// class?
	enum InstructionType {
		ST_MachineLanguage,  // A machine language instruction.
		ST_AssemblerInstr,   // Assembler Language instruction.
		ST_Comment,          // Comment or blank line
		ST_End,              // End instruction.
		ST_NotInstr			 // Not a valid instruction.
	};

	// Parse the Instruction.
	InstructionType ParseInstruction(string &a_buff);

	// Compute the location of the next instruction.
	int LocationNextInstruction(int a_loc);

	// To access the label
	inline string &GetLabel() {

		return m_Label;
	};
	// To determine if a label is blank.
	inline bool IsLabel() {

		return !m_Label.empty();
	};

	// To access the original statement.
	inline string &GetOriginalStatement()
	{
		return m_instruction;
	}

	// To get the numerical op code.
	inline int &GetNumOpCode()
	{
		return m_NumOpCode;
	}

	// To get the operand.
	inline string &GetOperand()
	{
		return m_Operand;
	}

	// To get the op code.
	inline string &GetOpCode()
	{
		return m_OpCode;
	}

	// To determine if an operand is numeric or not.
	inline bool &IsNumOperand()
	{
		return m_IsNumericOperand;
	}

	// To get the value of the operand.
	inline int &GetNumOperand()
	{
		return m_OperandValue;
	}

	// To determine if there is an extra operand.
	inline bool &HasExtraOperand()
	{
		return m_hasExtraOps;
	}
private:


	// The elements of an instruction
	string m_Label;        // The label.
	string m_OpCode;       // The symbolic op code.
	string m_Operand;      // The operand.


	string m_instruction;    // The original instruction.

							 // Derived values.
	int m_NumOpCode;     // The numerical value of the op code.
	InstructionType m_type; // The type of instruction.

	bool m_IsNumericOperand;// == true if the operand is numeric.
	int m_OperandValue;   // The value of the operand if it is numeric.
	bool m_hasExtraOps;	  // == true if the instruction has extra operands.

	// Initializes member variables.
	void InitMemVariables();

	// Determines if the line is a comment.
	bool IsComment(string a_parsedWord);

	// Deletes comments from a line.
	void DeleteComments(string & a_instruction);

	// Converts to lowercase.
	void ConvertToLower(string & a_input);

	// Converts to an integer.
	bool ConvertToInt(string a_parsedWord);

	// Determines if the line is an end statement.
	bool IsEnd(string a_parsedWord);

	// Determines if the line is a machine language instruction.
	bool IsMachineLanguage(string a_parsedWord);

	// Determines if the line is an assembly language instruction.
	bool IsAssemblyLanguage(string a_parsedWord);

	// Finds the numeric op code of the operation.
	int FindNumericOpCode(string a_parsedWord);
};