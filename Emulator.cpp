//
//		Implementation of the Emulator class.
//
#include "stdafx.h"
#include "Emulator.h"
#include "Errors.h"
#include <iomanip>

// Inserts the contents into the location in the VC3600 memory, if possible.
bool Emulator::InsertMemory(int a_location, int a_contents)
{
	// Check to see if the location and contents are small enough to be put into memory.

	if (a_contents > 999999)
	{
		return false;
	}
	if (a_location >= MEMSZ)
	{
		return false;
	}

	// Now check to see if there is already something stored in memory.

	if (m_memory[a_location] != NULL)
	{
		return false;
	}

	m_memory[a_location] = a_contents;

	return true;
}

/*
Emulator::RunProgram()

NAME

Emulator::RunProgram - runs the final result of all the translation from the first
two passes

SYNOPSIS

bool Emulator::RunProgram();

DESCRIPTION

This function runs the program that is stored in the memory of the VC3600 emulator. It goes
through any possible addresses with values/instructions stored in them until an "end" value is hit,
in which case it will end. Hence the class name, it is emulating how a VC3600 would run a program stored
in its memory.

RETURNS

Returns true if the program ran successfully without any errors being encountered. Returns false
if there was an error that caused the program to stop running.
*/
bool Emulator::RunProgram()
{
	InitEmulator();

	while (m_currentAddress < MEMSZ)
	{
		if (m_memory[m_currentAddress] != NULL)
		{
			// First, check to see what kind of data is stored here.
			// If it is greater than 9999, it most likey has an op code.
			// If not, it is just data.
			if (m_memory[m_currentAddress] > 9999)
			{
				int data = m_memory[m_currentAddress];
				int opCode = data / 10000;
				int address = data - (opCode * 10000);
				if (!PerformAction(opCode, address))
				{
					return false;
				}
			}
		}

		m_currentAddress++;
	}

	return true;
}

// Initializes the accumulator and current address in the emulator.
void Emulator::InitEmulator()
{
	m_accumulator = 0;
	m_currentAddress = 0;
}

/*
Emulator::PerformAction()

NAME

Emulator::PerformAction - performs the action found through parsing the value at the
current address being visited in the emulator's memory

SYNOPSIS

bool Emulator::PerformAction(int a_opCode, int a_address);
	a_opCode -> op code that contains which operation to perform
	a_address -> address that will receive the operation

DESCRIPTION

This function goes through switch statements to see which case matches with the op code
passed into the function, and if the op code and case values match, performs the operation
stored in that case on the value stored at the address that is passed into the function.

RETURNS

Returns true upon a successful operation, returns false is there was some sort of error
during the operation (accumulator/value in memory becoming too large to be stored in memory).
*/
bool Emulator::PerformAction(int a_opCode, int a_address)
{
	switch (a_opCode)
	{
	case 1: // ADD
		m_accumulator = m_accumulator + m_memory[a_address];
		break;
	case 2: // SUB
		m_accumulator = m_accumulator - m_memory[a_address];
		break;
	case 3: // MULT
		m_accumulator = m_accumulator * m_memory[a_address];
		break;
	case 4: // DIV
		m_accumulator = m_accumulator / m_memory[a_address];
		break;
	case 5: // LOAD
		m_accumulator = m_memory[a_address];
		break;
	case 6: // STORE
		if (m_accumulator > 999999)
		{
			// The accumulator's value is too big to store in memory.
			Errors::RecordError("ERROR: Accumulator's value is too big to store in memory!");
			return false;
		}
		else
		{
			m_memory[a_address] = m_accumulator;
		}
		break;
	case 7: // READ
		int input;
		cout << "? ";
		cin >> setw(6) >> input;
		m_memory[a_address] = input;
		break;
	case 8: // WRITE
		cout << m_memory[a_address] << endl;
		break;
	case 9: // BRANCH
		m_currentAddress = a_address - 1;
		break;
	case 10: // BRANCH MINUS
		if (m_accumulator < 0)
		{
			m_currentAddress = a_address - 1;
		}
		break;
	case 11: // BRANCH ZERO
		if (m_accumulator == 0)
		{
			m_currentAddress = a_address - 1;
		}
		break;
	case 12: // BRANCH POSITIVE
		if (m_accumulator > 0)
		{
			m_currentAddress = a_address - 1;
		}
		break;
	case 13: // HALT
		m_currentAddress = MEMSZ - 1;
		break;
	default: // This is just regular data, do nothing.
		break;
	}
	return true;
}
