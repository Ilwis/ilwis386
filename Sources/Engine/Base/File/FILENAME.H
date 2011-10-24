/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
/* filename.h
// Interface for FileName for ILWIS 2.0
// okt 1993, by Wim Koolhoven
// (c) Computer Department ITC
	Last change:  WK   28 Sep 98    3:58 pm
*/

#ifndef ILWFILENAME_H
#define ILWFILENAME_H

#define MAXPATH _MAX_PATH
#include <queue>

#define FileName ilwFileName

static char* pcIllegalChars ="\\/:*?\"<>|%";

class FilenameIter : public iterator<forward_iterator_tag, string, void>
{
  friend class FilenameIter;

public:
   _export FilenameIter(const string& sFilenameMask, bool fSubdirectories=false);
   _export FilenameIter(const FilenameIter& iter);
   _export ~FilenameIter();

   const string    _export  operator*() const;
   FilenameIter    _export& operator++();
   void _export    Reset(const string sFilenameMask);

  virtual bool    fValid() 
                    { return _fValid; }
  FilenameIter    Invalid() // function is needed to define the end of operations, empty iterator
                    { return FilenameIter(""); }
  bool            operator!=(const FilenameIter& file) // iterator is (un)equal value if it points to the same file
                    { return operator*() != *file; }
  bool            operator==(const FilenameIter& file) 
                    { return operator*() == *file; }

private:
  string          _sMask; // file mask used
  string          _sCurMask; // actual mask, contains path information
  bool            _fValid; // is the state of the object valid
  bool            _fSubdirectories; 
  WIN32_FIND_DATA _findData; // data of find action
  int             _BaseLength; // length of path to cwd, used to strip the base path from the absolute path
  HANDLE          _findHandle; // handle must be stored, care must be taken to release it in time
  queue<string, list<string> >    _dirTree;

  void             MakeDirectoryTree(string s);
};

//! Class FileName encapsulates filename functionality including long file names
class  FileName
{
public:
  _export FileName();
  // empty
  _export FileName(const FileName& fn);  // to prevent inline expansion.
  _export FileName(const char* sName, bool includeSystemDir=false);   // only name no path or extension
  _export FileName(const String& sName, bool includeSystemDir=false);
  _export FileName(const CString& sName, bool includeSystemDir=false);
  
  _export FileName(const char* sName, const char* sExtension,
           bool fReplaceExtension=true);
  _export FileName(const String& sName, const String& sExtension,
           bool fReplaceExtension=true);
  _export FileName(const CString& sName, const CString& sExtension,
           bool fReplaceExtension=true);

  _export FileName(const String& sName, const char* sExtension,
           bool fReplaceExtension=true);
  _export FileName(const FileName& fn,const String& sExtension,
           bool fReplaceExtension=true);
  _export FileName(const FileName& fn, const char* sExtension,
           bool fReplaceExtension=true);
  _export FileName(const String& sName, const FileName& fnDir);

  _export virtual ~FileName();

  bool _export operator == (const FileName& fn) const;
//  friend bool operator != (const FileName& fnA, const FileName& fnB) 
//    { return !(fnA == fnB); }

	bool fExist() const { return _access(sFullPath(true).c_str(), 0)==0 ; }
	bool _export fValid() const;

	static FileName _export fnUnique(const FileName& fnTemplate);
	static FileName _export fnUniqueShort(const FileName& fnTemplate);  // create a 8.3 DOS filename

	String sFullName(bool fExt = true) const
		{ return sFullPath(fExt); }

  String _export virtual sFullPath(bool fExt = true) const;
  String _export sRelative(bool fExt = true, const String& sDirRelative = "") const;
  String _export sShortName(bool fExt = true) const;
  String _export sPath() const;
  // return drive and directory, followed by a '\\'
  void _export Dir(const String& s);
  // set directory ans drive to s

	static const String _export sAllExtensions();
	static bool _export fIsIlwisExtension(const String& fn);
	static bool _export fUsesAbsolutePath(const String& sFileName);
	static bool _export fUsesRelativePath(const String& sFileName);
	static bool _export fUsesNoPath(const String& sFileName);
	static bool _export fIsContainerFor(const FileName& fnCont, const String& sExtensions);
	static bool _export fIsContainer(const FileName& fnCont);
 
  String sFullNameQuoted(bool fExt = true) const
    { return sFullPathQuoted(fExt); }
  // return full file name, if fExt = false excluding extension
  String _export sFullPathQuoted(bool fExt = true) const;
  String _export sRelativeQuoted(bool fExt = true, const String& sDirRelative = "") const;
  String _export sShortNameQuoted(bool fExt = true) const;
  bool fDelete() { return _unlink(sFullPath().c_str()) == 0; }

  void Split(const char * sFileName);
  static FileName _export fnPathOnly(const String& sPath);
  void _export FileSet(Array<String>& sFileSet);
	String 	_export sFileExt() const;
	String  _export sFileExtQuoted() const;
	bool _export fIsIlwisDataFile() const;
	static _export FileName fnUniqueWithIndex(const FileName& fnTemplate, int iStartIndex=1);
  String sDrive;
  // drive name
  String sDir;
  // path name (without drive)
  String sFile;
  // file name (without extension)
  String sExt;
  // extension
  String sSectionPostFix; // for reading / writing in ODF
	// attrib columns
	String sCol;

private:
  void init(const char* sName, bool includeSystemDir=false);
  void initExt(const char* sName, const char* sExt, bool fReplaceExtension);
  void initCopyFrom(const FileName& fn);
};

bool _export operator != (const FileName& fnA, const FileName& fnB);

//template class  Array<FileName>;

void  _export AddFileName(const FileName& fnObj, Array<FileName>& fna, const String& sPath=String());

#endif

/*!	class FileName
 */
/*!	\fn FileName::FileName();
 *      The empty FileName 
 */
/*!	\fn FileName::FileName(const FileName& fn)
 *       Copy constructor
 */
/*!	\fn FileName::FileName(const char* sName)
		Build the members of the FileName from a character array. The \a sName does not need to
		contain a fully specified file name. 
		If the path is relative then the absolute path is calculated
		If no path is included the current folder of the current thread is added.
	\param sName Contains the file name
 */
/*!	\fn FileName::FileName(const String& sName)
		Build the members of the FileName from an ILWIS String. The \a sName does not need to
		contain a fully specified file name. 
		If the path is relative then the absolute path is calculated
		If no path is included the current folder of the current thread is added. 
	\param sName Contains the file name
 */
/*!	\fn FileName::FileName(const CString& sName)
		Build the members of the FileName from a MFC CString. The \a sName does not need to
		contain a fully specified file name. 
		If the path is relative then the absolute path is calculated
		If no path is included the current folder of the current thread is added. 
	\param sName Contains the file name
 */
/*!  \fn FileName::FileName(const char* sName, const char* sExtension, bool fReplaceExtension=true)
		Build the members of the FileName from character arrays. The \a sName and \a sExtension
		do not need to contain a fully specified file name. \a sExtension has a value that 
		replaces the extension of the file name only in case \a fReplaceExtension is \a true.
		If the path is relative then the absolute path is calculated
		If no path is included the current folder of the current thread is added. 
	\param sName Contains the file name
	\param sExtension Contains a separate extension, replacing the default extension only when 
		\a fReplaceExtension is \a true
	\param fReplaceExtension This boolean is set when the default extension will be replaced by
		\a sExtension.
 */
/*!  \fn FileName::FileName(const String& sName, const String& sExtension, bool fReplaceExtension=true)
		Build the members of the FileName from ILWIS Strings. The \a sName and \a sExtension
		do not need to contain a fully specified file name. \a sExtension has a value that 
		replaces the extension of the file name only in case \a fReplaceExtension is \a true.
		If the path is relative then the absolute path is calculated
		If no path is included the current folder of the current thread is added. 
	\param sName Contains the file name
	\param sExtension Contains a separate extension, replacing the default extension only when 
		\a fReplaceExtension is \a true
	\param fReplaceExtension This boolean is set when the default extension will be replaced by
		\a sExtension.
 */
/*!  \fn FileName::FileName(const CString& sName, const CString& sExtension, bool fReplaceExtension=true)
		Build the members of the FileName from MFC CStrings. The \a sName and \a sExtension
		do not need to contain a fully specified file name. \a sExtension has a value that 
		replaces the extension of the file name only in case \a fReplaceExtension is \a true.
		If the path is relative then the absolute path is calculated
		If no path is included the current folder of the current thread is added. 
	\param sName Contains the file name
	\param sExtension Contains a separate extension, replacing the default extension only when 
		\a fReplaceExtension is \a true
	\param fReplaceExtension This boolean is set when the default extension will be replaced by
		\a sExtension.
 */
/*!  \fn FileName::FileName(const String& sName, const char* sExtension, bool fReplaceExtension=true)
		Build the members of the FileName from a file as ILWIS String and an extension as character array. 
		The \a sName and \a sExtension
		do not need to contain a fully specified file name. \a sExtension has a value that 
		replaces the extension of the file name only in case \a fReplaceExtension is \a true.
		If the path is relative then the absolute path is calculated
		If no path is included the current folder of the current thread is added. 
	\param sName Contains the file name
	\param sExtension Contains a separate extension, replacing the default extension only when 
		\a fReplaceExtension is \a true
	\param fReplaceExtension This boolean is set when the default extension will be replaced by
		\a sExtension.
 */
/*!  \fn FileName::FileName(const FileName& fn, const String& sExtension, bool fReplaceExtension=true)
		Build the members of the FileName from another FileName and an ILWIS String. \a sExtension has a value that 
		replaces the extension of the file name only in case \a fReplaceExtension is \a true.
	\param fn Contains the other file name
	\param sExtension Contains a separate extension, replacing the default extension only when 
		\a fReplaceExtension is \a true
	\param fReplaceExtension This boolean is set when the default extension will be replaced by
		\a sExtension.
 */
/*!  \fn FileName::FileName(const FileName& fn, const char* sExtension, bool fReplaceExtension=true)
		Build the members of the FileName from another FileName and a character array. \a sExtension has a value that 
		replaces the extension of the file name only in case \a fReplaceExtension is \a true.
	\param fn Contains the other file name
	\param sExtension Contains a separate extension, replacing the default extension only when 
		\a fReplaceExtension is \a true
	\param fReplaceExtension This boolean is set when the default extension will be replaced by
		\a sExtension.
 */
/*!  \fn FileName::FileName(const String& sName, const FileName& fnDir)
		Build the members of the FileName from an ILWIS String. The \a sName does not need to
		contain a fully specified file name. 
		If no path is included in \a sName, the path will be copied from \a fnDir.
	\param sName Contains the file name
	\param fnDir The file name used to copy the path from, but only when no path is
		specified in \a sName
 */
/*!  \fn bool FileName::operator == (const FileName& fn) const
		Compare this file name with another file name \a fn. The comparison is done case insensitive
	\param fn The file name to compare with.
	\return Returns true when the full file names are identical.
 */
/*!  \fn bool FileName::fExist() const
		Test if a file with this file name exists on disk.
	\return Returns true if the file with this file name exists
 */
/*!  \fn bool FileName::fValid() const
		Test the validity of the file name. ILWIS enforces that:
		- the first character is alpha, '_' or '$'
		- all other charcters are alphanumerical, '_' or >= 127
	\return Returns true for a valid file name.
 */
/*!  \fn static FileName FileName::fnUnique(const FileName& fnTemplate)
		Create an unique file name, that is: there exists no file yet carrying this name. The
		\a fnTemplate parameter is used as a base name. If the directory is not set, the current thread directory is used.
		If the \a fnTemplate is for example \c map.mpr the function will try to generate names that
		look like \c map00001.mpr. If this file already exists than the result could become
		\c map00017.mpr.
	\param fnTemplate Mask to create the file name.
	\return Returns the unique file name. 
 */
/*!  \fn String FileName::sFullPath(bool fExt = true) const
	\param fExt If true add the extension to the file name
	\return Returns the full file name, if \a fExt is false the extension is not added. If the
		file name does not contain a directory, the current thread directory is added.
 */
/*!  \fn String FileName::sRelative(bool fExt = true, const String& sDirRelative = "") const
		Return the file name without path when the file exists in the \a sDirRelative directory;
		otherwise include the path. The path is also not added for all ILWIS system objects.
	\param fExt If true add the extension to the file name
	\param sDirRelative The directory to check the file name against.
	\return The file name.
 */
/*!  \fn String FileName::sShortName(bool fExt = true) const
	\param fExt If true add the extension to the file name
	\return Returns only the name of the file without the path. The extension is only included 
		when \a fExt is true
 */
/*!  \fn String FileName::sPath() const
	\return Returns the drive and directory of the file name, followed by a '\'.
 */
/*!  \fn void FileName::Dir(const String& s)
		Set a new drive and directory for the file name
	\param s Contains the new path for the file name
 
 */
/*!  \fn String FileName::sFullNameQuoted(bool fExt = true) const
	\param fExt If true add the extension to the file name
	\return Returns the full file name, if \a fExt false without extension
 */
/*!  \fn String FileName::sFullPathQuoted(bool fExt = true) const
	\param fExt If true add the extension to the file name
	\return Returns the full file name, if \a fExt false without extension
 */
/*!  \fn String FileName::sRelativeQuoted(bool fExt = true, const String& sDirRelative = "") const
	\param fExt If true add the extension to the file name
	\return Returns the full file name, if \a fExt false without extension
 */
/*!  \fn String FileName::sShortNameQuoted(bool fExt = true) const
	\param fExt If true add the extension to the file name
	\return Returns the full file name, if \a fExt false without extension
 */
/*!  \fn static FileName FileName::fnPathOnly(const String& sPath)
		Build a file name consisting of only a path. The path is passed in \a sPath
	\param sPath The name of the path
	\return Return the path only file name
 */
/*!  \fn void FileName::FileSet(Array<String>& sFileSet)
 */
