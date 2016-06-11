#include <Windows.h>
#include "Parser.h"



Parser::Parser()
{
}


Parser::~Parser()
{
}

/*
* Checks if the string at str starts with word
*/
bool Parser::StartsWith(const char * str,const char * word)
{
	while (*word && *str && (*str++ == *word++));
	if (*word == '\0') return true;
	return false;
}

/*
* Modifies str so that it points to a single line, comments (;) or newlines filtered out.
* if nextLine != NULL, *nextLine becomes the start of the next line, or NULL if end was reached
*/
void Parser::GetLine(char* str,char** nextLine) {
	char* it = str;
	//iterate to either ; (comment), \r\n (newline), \n (newline - unix style), \r (newline - mac style),
	//so probably just \r or \n, or \0
	while (true) {
		if (*it == ';') {
			*it++ = '\0';
			//skip rest of line
			while (*it && *it != '\r' && *it != '\n') it++;
			while (*it && *it == '\r' || *it == '\n') it++;
			if (nextLine != NULL) {
				if (*it)
					*nextLine = it;
				else
					*nextLine = NULL;
			}
			break;
		}
		else if (*it == '\r' || *it == '\n') {
			*it++ = '\0';
			while (*it && *it == '\r' || *it == '\n') it++;
			if (nextLine != NULL) {
				if (*it)
					*nextLine = it;
				else
					*nextLine = NULL;
			}
			break;
		}
		else if (*it == '\0') {
			if (nextLine != NULL) *nextLine = NULL;
			break;
		}
		it++;
	}
}

/*
* Gets next whitespace seperated token from str, nullterminates it,
* sets nextPart (if != NULL) to point to after the \0 and then
* returns a pointer to the beginning of the token (skipping whitespaces),
* or NULL if no token was found (only whitespaces and \0)
*/
char* Parser::GetToken(char* str,char** nextPart) {
	while (isspace(*str) && *str != '\0') str++;
	if (*str == '\0') {
		if (nextPart != NULL) *nextPart = NULL;
		return NULL;
	}
	char* ret = str;
	while (!isspace(*str) && *str != '\0') str++;
	if (*str == '\0') {
		if (nextPart != NULL) *nextPart = NULL;
		return ret;
	}
	*str = '\0';
	if (nextPart != NULL) {
		if (*(str+1) == '\0') {
			*nextPart = NULL;
		}
		else {
			*nextPart = str+1;
		}
	}
	return ret;
}