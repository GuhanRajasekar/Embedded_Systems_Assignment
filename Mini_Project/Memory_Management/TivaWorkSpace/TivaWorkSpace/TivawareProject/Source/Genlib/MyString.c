//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: MyString.c
// Description  : Functions for manipulation of strings
// Dependency 	: None
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 10-01-2017 		Initial version
//*****************************************************************************************
#include "MyString.h"

int mystrcmp(const char *s1, const char *s2, int EnableCase) // case sensitive compare
{
	int l1,l2;
	int returnvalue;
	char c1,c2;

	l1 = mystrlen(s1);
	l2 = mystrlen(s2);
	returnvalue = STRCMP_SUCCESS;
	if(l1==l2)
	{
		if(EnableCase)
		{
			while(l1--)
			{
				if(s1[l1] != s2[l1])returnvalue = STRCMP_FAILED;
			}
		}
		else
		{
			while(l1--)
			{
				c1= s1[l1];
				c2=	s2[l1];
				if((c1>0x60) && (c1<0x7B))c1 -= 0x20;
				if((c2>0x60) && (c2<0x7B))c2 -= 0x20;
				if(c1 != c2)returnvalue = STRCMP_FAILED;
			}
		}
	}
	else
		returnvalue = STRCMP_FAILED;

	return returnvalue;
}
char *mystrcat(char *dest, const char *src) // join src and dest strings
{
	int l1,l2,i;

	l1 = mystrlen(dest);
	l2 = mystrlen(src);

	for(i=0;i<=l2;i++)
	{
		dest[l1+i] = src[i];
	}

	return dest;
}
char *mystrcpy(char *dest, const char *src) // copy src to dest
{
	int i;

	i=0;
	while(src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i]='\0';

	return dest;
}
int mystrlen(const char *s) //return length of the string
{
	int i;

	i=0;
	while(s[i] != '\0')i++;

	return i;
}
int mystrncmp(const char *s1, const char *s2, int n) // comp first n char in s1 and s2
{
	int l1,l2;
	int returnvalue;
	char c1,c2;

	l1 = mystrlen(s1);
	l2 = mystrlen(s2);
	returnvalue = STRCMP_SUCCESS;
	if((l1 >= n)&&(l2 >= n))
	{
		while(n--)
		{
			c1= s1[n];
			c2=	s2[n];
			if((c1>0x60) && (c1<0x7B))c1 -= 0x20;
			if((c2>0x60) && (c2<0x7B))c2 -= 0x20;
			if(c1 != c2)returnvalue = STRCMP_FAILED;
		}
	}
	else
		returnvalue = STRCMP_FAILED;

	return returnvalue;
}
char *mystrncpy(char *dest, const char *src,int n) // copy 'n' characters from src to dest
{
	int i;

	for(i=0;i<n;i++)
	{
		dest[i] = src[i];
	}
	dest[i]='\0';

	return dest;
}
void mystrdelws (char *s, int flag_leading_trailing) //delete leading or trailing white spaces in the given string
{
	int l,n,i;

	l = mystrlen(s);
	if(flag_leading_trailing == LEADING_WS)
	{
		n=0;
		while((s[n] == 0x20)||(s[n] == 0x9))n++;
		for(i=0;i<(l-n);i++)
		{
			s[i] = s[n+i];
		}
		s[i]='\0';
	}
	else
	{
		while((s[l-1] == 0x20)||(s[l-1] == 0x9))
		{
			s[l-1] = s[l];
			l--;
		}
	}
}
void mystrcase (char *s, int flag_upper_lower) //convert string from upper to lower case and vice versa
{
	int i;

	i=0;
	if(flag_upper_lower == UPPER_CASE)
	{
		while(s[i] != '\0')
		{
			if((s[i]>0x60) && (s[i]<0x7B))s[i] -= 0x20;
			i++;
		}
	}
	else
	{
		while(s[i] != '\0')
		{
			if((s[i]>0x40) && (s[i]<0x5B))s[i] += 0x20;
			i++;
		}
	}
}
