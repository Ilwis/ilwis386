/*****************************************************************************
Module :     URLEncode.H
Notices:     Written 2002 by ChandraSekar Vuppalapati
Description: H URL Encoder
*****************************************************************************/
#ifndef __CURLENCODE_H_
#define __CURLENCODE_H_

#include "stdafx.h"

class _export CURLEncode
{
private:
	static String csUnsafeString;
	String decToHex(char num, int radix);
	bool isUnsafe(char compareChar);
	String convert(char val);

public:
	CURLEncode() { };
	virtual ~CURLEncode() { };
	String URLEncode(String vData);
};

#endif //__CURLENCODE_H_