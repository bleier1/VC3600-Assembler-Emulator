//
//		Emulator class - supports the emulation of VC3600 programs
//
#ifndef _EMULATOR_H      // UNIX way of preventing multiple inclusions.
#define _EMULATOR_H

class Emulator {

public:

	const static int MEMSZ = 10000;	// The size of the memory of the VC3600.
	
	// Allocates 10000 words into the emulator's memory.
	Emulator()
	{
		memset(m_memory, 0, MEMSZ * sizeof(int));
	}
	// Records instructions and data into VC-3600 memory.
	bool InsertMemory(int a_location, int a_contents);

	// Runs the VC-3600 program recorded in memory.
	bool RunProgram();

private:

	int m_memory[MEMSZ]; // The memory of the VC3600.
	int m_accumulator; // The accumulator used for operations.
	int m_currentAddress; // The current address being visited in memory.
	
	// Initializes the emulator's accumulator and starting address.
	void InitEmulator();

	// Performs the action at the address.
	bool PerformAction(int a_opCode, int a_address);
};

#endif