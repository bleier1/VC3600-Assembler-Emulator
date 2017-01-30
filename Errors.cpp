//
//		Implementation of the Errors class.
//

#include "stdafx.h"
#include "Errors.h"
vector<string> Errors::m_ErrorMsgs;

// Initializes error reports.
void Errors::InitErrorReporting()
{
	Errors::m_ErrorMsgs.clear();
}

// Records an error message.
void Errors::RecordError(string a_emsg)
{
	m_ErrorMsgs.push_back(a_emsg);
}

// Displays all the collected error messages.
void Errors::DisplayErrors()
{
	for (vector<string>::const_iterator it = m_ErrorMsgs.begin(); it != m_ErrorMsgs.end(); it++)
	{
		cout << *it << endl;
	}
}