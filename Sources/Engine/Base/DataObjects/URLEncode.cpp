/*****************************************************************************
Module :     URLEncode.cpp
Notices:     Written 2002 by ChandraSekar Vuppalapati
Description: CPP URL Encoder
*****************************************************************************/
#define _CRTDBG_MAP_ALLOC

#include "headers/toolspch.h"
//#include <math.h>
//#include <malloc.h>
//#include <memory.h>
//#include <new.h>
//#include <stdlib.h>
//#include <string.h>
#include <WININET.H>

#include "URLEncode.h"

#define MAX_BUFFER_String 4096
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#include <stdlib.h>
#include <crtdbg.h>
// HEX Values array
char hexVals[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
// UNSAFE String
String CURLEncode::csUnsafeString= "\"<>%\\^[]`+$,@:;/!#?=&";

// PURPOSE OF THIS FUNCTION IS TO CONVERT A GIVEN CHAR TO URL HEX FORM
String CURLEncode::convert(char val) 
{
	String csRet;
	csRet += "%";
	csRet += decToHex(val, 16);	
	return  csRet;
}

// THIS IS A HELPER FUNCTION.
// PURPOSE OF THIS FUNCTION IS TO GENERATE A HEX REPRESENTATION OF GIVEN CHARACTER
String CURLEncode::decToHex(char num, int radix)
{	
	int temp=0;	
	String csTmp;
	int num_char;
	num_char = (int) num;
	
	// ISO-8859-1 
	// IF THE IF LOOP IS COMMENTED, THE CODE WILL FAIL TO GENERATE A 
	// PROPER URL ENCODE FOR THE CHARACTERS WHOSE RANGE IN 127-255(DECIMAL)
	if (num_char < 0)
		num_char = 256 + num_char;

	char cTemp=0;
	while (num_char >= radix)
    {
		temp = num_char % radix;
		num_char = (int)floor((double)num_char / radix);
		cTemp = hexVals[temp];
    }
	csTmp += cTemp;
	
	csTmp += hexVals[num_char];

	if(csTmp.size() < 2)
	{
		csTmp += '0';
	}

	String strdecToHex(csTmp);
	// Reverse the String
	String tmp;
	for(int i=strdecToHex.size() - 1; i >= 0; --i)
		tmp += strdecToHex[i];
	strdecToHex = tmp;
	
	return strdecToHex;
}

// PURPOSE OF THIS FUNCTION IS TO CHECK TO SEE IF A CHAR IS URL UNSAFE.
// TRUE = UNSAFE, FALSE = SAFE
bool CURLEncode::isUnsafe(char compareChar)
{
	bool bcharfound = false;
	char tmpsafeChar;
	int m_strLen = 0;
	
	m_strLen = csUnsafeString.size();
	for(int ichar_pos = 0; ichar_pos < m_strLen ;ichar_pos++)
	{
		tmpsafeChar = csUnsafeString[ichar_pos]; 
		if(tmpsafeChar == compareChar)
		{ 
			bcharfound = true;
			break;
		} 
	}
	int char_ascii_value = 0;
	//char_ascii_value = __toascii(compareChar);
	char_ascii_value = (int) compareChar;

	if(bcharfound == false &&  char_ascii_value > 32 && char_ascii_value < 123)
	{
		return false;
	}
	// found no unsafe chars, return false		
	else
	{
		return true;
	}
	
	return true;
}
// PURPOSE OF THIS FUNCTION IS TO CONVERT A STRING 
// TO URL ENCODE FORM.
String CURLEncode::URLEncode(String pcsEncode)
{	
	int ichar_pos;
	String csEncode;
	String csEncoded;	
	int m_length;

	csEncode = pcsEncode;
	m_length = csEncode.size();
	
	for(ichar_pos = 0; ichar_pos < m_length; ichar_pos++)
	{
		char ch = csEncode[ichar_pos];
		if (ch < ' ') 
		{
			ch = ch;
		}		
		if(!isUnsafe(ch))
		{
			// Safe Character				
			csEncoded += String(ch);
		}
		else
		{
			// get Hex Value of the Character
			csEncoded += convert(ch);
		}
	}
	

	return csEncoded;

}