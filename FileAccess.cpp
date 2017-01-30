//
//  Implementation of file access class.
//
#include "stdafx.h"
#include "FileAccess.h"

/*
FileAccess::FileAccess()

NAME

FileAccess::FileAccess - accesses the file being passed into the program

SYNOPSIS

void FileAccess::FileAccess(int argc, char *argv[]);
	argc -> argument count from main
	argv[] -> current argument being visited from main

DESCRIPTION

This function accesses the file that is being passed into the program through the
command line argument. It receives this information through the arguments that the
main function receives when the assembler is started up. This allows the program to
parse through each line and "assemble" the program.
*/
FileAccess::FileAccess(int argc, char *argv[])
{
	// Check that there is exactly one run time parameter.
	if (argc != 2) {
		cerr << "Usage: Assem <FileName>" << endl;
		exit(1);
	}
	// Open the file.  One might question if this is the best place to open the file.
	// One might also question whether we need a file access class.
	m_sfile.open(argv[1], ios::in);

	// If the open failed, report the error and terminate.
	if (!m_sfile) {
		cerr << "Source file could not be opened, assembler terminated."
			<< endl;
		exit(1);
	}
}

// The destructor just closes the file.
FileAccess::~FileAccess()
{
	m_sfile.close();
}

// Get the next line from the file.
bool FileAccess::GetNextLine(string &a_buff)
{
	if (m_sfile.eof()) return false;

	getline(m_sfile, a_buff);

	// Return indicating success.
	return true;
}

// "Rewinds" back to the beginning of the file.
void FileAccess::Rewind()
{
	// Clean the end of file flag and go back to the beginning of the file.
	m_sfile.clear();
	m_sfile.seekg(0, ios::beg);
}