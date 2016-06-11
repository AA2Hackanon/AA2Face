#pragma once
class Parser
{
public:
	Parser();
	~Parser();

protected:
	/*
	* Modifies str so that it points to a single line, comments (;) or newlines filtered out.
	* if nextLine != NULL, *nextLine becomes the start of the next line, or NULL if end was reached
	*/
	void GetLine(char* str,char** nextLine);
	/*
	* Gets next whitespace seperated token from str, nullterminates it,
	* sets nextPart (if != NULL) to point to after the \0 and then
	* returns a pointer to the beginning of the token (skipping whitespaces),
	* or NULL if no token was found (only whitespaces and \0)
	*/
	char* GetToken(char* str,char** nextLine);
	/*
	* Checks if the string at str starts with word
	*/
	bool StartsWith(const char* str,const char* word);
};

