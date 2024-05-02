//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: MyString.h
// Description  : Prototypes for manipulation of strings
// Dependency 	: None
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 10-01-2017 		Initial version
//*****************************************************************************************

#ifndef MYSTRING_H
#define MYSTRING_H

#define STRCMP_SUCCESS 	1
#define STRCMP_FAILED	0
#define CASE_SENSITIVE		1
#define CASE_INSENSITIVE	0
#define LEADING_WS	1
#define TRAILING_WS	0
#define UPPER_CASE	1
#define LOWER_CASE	0

int mystrcmp(const char *s1, const char *s2, int EnableCase); // case sensitive compare
char *mystrcat(char *dest, const char *src); // join src and dest strings
char *mystrcpy(char *dest, const char *src); // copy src to dest
int mystrlen(const char *s); //return length of the string
int mystrncmp(const char *s1, const char *s2, int n); // comp first n char in s1 and s2
char *mystrncpy(char *dest, const char *src, int n); // copy 'n' characters from src to dest
void mystrdelws (char *s, int flag_leading_trailing); //delete leading or trailing white spaces in the given string
void mystrcase (char *s, int flag_upper_lower); //convert string from upper to lower case and vice versa

#endif // MYSTRING_H








