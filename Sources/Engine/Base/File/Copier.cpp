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
/* $Log: /ILWIS 3.0/Script_Copier/Copier.cpp $
 * 
 * 21    5-03-01 12:39 Koolhoven
 * in UpdateCatalog() use PostMessage() instead of PostThreadMessage()
 * 
 * 20    9/02/01 15:48 Willem
 * Copier constructor did not properly initialize sDestPath
 * 
 * 19    10/02/00 4:15p Martin
 * Fullpath function is not needed (even harmfull) anymore because the
 * path passed has already its fullpath
 * 
 * 18    17/08/00 10:56 Willem
 * Copying a domain will first always disable the attribute table link in
 * case of quiet copy
 * 
 * 17    18-07-00 1:01p Martin
 * ObjectCollection use AddObjectAndProp instead of AddObject to construct
 * the list
 * 
 * 16    17-07-00 2:59p Martin
 * copier copies object collection correctly
 * 
 * 15    17-07-00 1:43p Martin
 * MoveFile does not overwrite existing files so it did not work for a
 * copy (first delete the target)
 * 
 * 14    17-07-00 9:03a Martin
 * local var exist instead of temporary file. Array can now be safely
 * extended
 * 
 * 13    16-06-00 16:11 Koolhoven
 * add protection in exec() against extra empty items in list
 * 
 * 12    14/04/00 11:30 Willem
 * The single file to file copy (static function) now checks for equality
 * of the entire filenames instead of for inequality of paths
 * 
 * 11    12/04/00 16:29 Willem
 * Copier::Copy(FileName, DestDir) does folder check properly now
 * 
 * 10    4/04/00 17:32 Willem
 * GetFreeDiskSpaceEx is now not loaded anymore; a check is made first
 * 
 * 9     7-03-00 17:08 Wind
 * adjusted check on free disk space to int64
 * 
 * 8     14-01-00 17:19 Koolhoven
 * Post reread Catalog as thread message
 * 
 * 7     3-11-99 13:07 Wind
 * case insensitive path check
 * 
 * 6     29-10-99 12:51 Wind
 * case sensitive stuff
 * 
 * 5     20-10-99 15:54 Wind
 * replaced OpenFile with CreateFile, because OpenFile only accepts file
 * names up to 128 characters
 * 
 * 4     8/25/99 11:27a Wind
 * still comment problem
 * 
 * 3     8/25/99 11:26a Wind
 * comment problem
 * 
 *
 * 2     8/25/99 11:25a Wind
 * remove bug form SetDestinationpath: return value of GetFileAttributes
 * was used incorrectly
// Revision 1.9  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.8  1997/10/02 08:12:22  Wim
// Do not treat .rpr file as data file in Exec(), hack to prevent problems
//
// Revision 1.7  1997-09-25 15:20:12+02  Wim
// Copy also maps with an internal domain properly
//
// Revision 1.6  1997-09-16 21:27:07+02  Wim
// Also when copying to another directory return when Stop is pressed
//
// Revision 1.5  1997-09-08 15:20:18+02  Wim
// AddObjectAndProp() now knows about 2dim tables
//
// Revision 1.4  1997-08-28 17:04:47+02  Wim
// In AddObjectAndProp() add dependencies of columns as well
//
// Revision 1.3  1997-08-06 19:25:56+02  Wim
// When copying a representation to a new name, make sure that also
// the Data entry of TableStore is the new name.
//
// Revision 1.2  1997-07-31 16:05:08+02  Wim
// When copying a domain to a new domain, and it has a representation,
// the correct updating is being done, so that the resulting domain has
// a representation with the correct domain.
//
  Copier
   Copyright Ilwis System Development ITC
   may 1996, Jelle Wind
	Last change:  WK   13 Aug 98   11:43 am
*/

#include "Headers\constant.h"
#include "Engine\Base\File\COPIER.H"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Table\Col.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Base\objdepen.h"
#include "Engine\Table\tbl2dim.h"
#include "Headers\Hs\DAT.hs"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

static void UpdateCatalog()
{
	AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG); 
}

Copier::Copier()
{
  init(String(), false, false);
}

Copier::Copier(const FileName& fnObj, const String& sDestPath, 
               bool fBreakDep)
{
  AddObject(fnObj);
  init(sDestPath, true, fBreakDep);
}
  
Copier::Copier(const Array<FileName>& afnObj, const String& sDestPath)
{
  AddObjects(afnObj);  
  init(sDestPath, true, false);
}

Copier::Copier(const FileName& fnObj, const FileName& fnObjCop, 
         const FileName& fnAttTbl, const FileName& fnAttTblCopy, bool fBreakDep)
{
  AddObject(fnObj);
  fnObjCopy = fnObjCop;
  fnObjCopy.sExt = fnObj.sExt;
  fnAttTable = fnAttTbl;
  fnAttTableCopy = fnAttTblCopy;
  sDestPath = fnObjCopy.sPath();
  if (fnAttTable.fValid() && (fnAttTable.sFullName() == fnAttTableCopy.sFullName())) {
    fnAttTable = FileName();
    fnAttTableCopy = FileName();
  }
  init(sDestPath, fnAttTable.fValid(), fBreakDep);
}          

void Copier::init(const String& sDestPath, bool fCopyAttTable, bool fBreakDep)
{
  SetBreakDependency(fBreakDep);
  SetDestinationPath(sDestPath);
  sSystemDir = getEngine()->getContext()->sStdDir();
  if (sSystemDir.sRight(1) != "\\" )
    sSystemDir &= '\\';
  fIncludeSystemFiles = false;
  fObjectListConstructed = false;
  trq.SetTitle(SDATTitleCopyObjects);
  trq.Start();
}

void Copier::AddObject(const FileName& fnObj)
{
  if (!fIncludeSystemFiles)
    if (fCIStrEqual(fnObj.sPath(), sSystemDir))
      return;
  AddFileName(fnObj, afnObj);  
}

void Copier::AddObjects(const Array<FileName>& afnObjct)
{
  for (unsigned int i=0; i < afnObjct.iSize(); ++i) {
    if (!fIncludeSystemFiles)
      if (fCIStrEqual(afnObjct[i].sPath(), sSystemDir))
        continue;
    AddFileName(afnObjct[i], afnObj);
  }  
}

void Copier::AddObject(const Domain& dm)
{
  AddObject(dm->fnObj);
  FileName fnRpr = dm->fnObj;
  fnRpr.sExt = ".rpr";
  if (File::fExist(fnRpr))
    AddObject(fnRpr);
}


void Copier::SetDestinationPath(const String& sPath)
{
  //char *sFullDestPath = new char[MAXPATH];
  //_fullpath(sFullDestPath, sPath.scVal(), MAX_PATH);
	String sPathu = sPath.sUnQuote();

  if (strlen(sPathu.scVal()) != 0) {
    DWORD dwFileAtt;

    if ((dwFileAtt=GetFileAttributes(sPathu.scVal()))==0xFFFFFFFF)
      throw ErrorDirNotFound(sPath);
  }  
  sDestPath = sPath;
  //delete [] sFullDestPath;
}

void Copier::SetCopyAttTable(const FileName& fnAttTblCopy)
{
  if (sSystemDir.length() != 0) // no copy and rename possible, only copy
    return;
  fCopyAttTable = fnAttTblCopy.fValid();
  fnAttTableCopy = fnAttTblCopy;
}

void Copier::SetBreakDependency(bool f)
{
  fBreakDep = f;
  fCopyDep = !fBreakDep;
}

bool Copier::fSingleCopy() const
{
  return afnObj.iSize() == 1;
}

bool Copier::fSameDirCopy() const
{
  return /*fSingleCopy() &&*/ fCIStrEqual(fnObjCopy.sPath(),afnObj[0].sPath());
}

void Copier::CheckExist(Array<FileName>& fnObjExist)
{
  ConstructObjectList();
  if (fSameDirCopy()) {
    if (File::fExist(fnObjCopy))
      fnObjExist &= afnObj[0];
    if (fnAttTableCopy.fValid()) {
      AddObject(fnAttTable);
      if (File::fExist(fnAttTableCopy))
        fnObjExist &= fnAttTable;
    }
    for (unsigned int i=1; i < afnObj.iSize(); ++i) {
      FileName fnObjCop = fnObjCopy;
      fnObjCop.sExt = afnObj[i].sExt;
      if (File::fExist(fnObjCop))
        fnObjExist &= fnObjCop;
    }  
  }
  else {
    for (unsigned int i=0; i < afnObj.iSize(); ++i) {
      FileName fn = afnObj[i];
      fn.Dir(sDestPath);
      if (File::fExist(fn))
        fnObjExist &= afnObj[i];
    }
  }  
}

void Copier::ExcludeObjects(Array<FileName>& afnObjExcl)
{
  if (fBreakDep) {
    afnObjExclSav = afnObjExcl;
    return;
  }  
  for (unsigned int i=0; i < afnObjExcl.iSize(); ++i) {
    for (unsigned int j=0; j < afnObj.iSize(); ++j)
      if (afnObj[j] == afnObjExcl[i]) {
        afnObj.Remove(j, 1);
        --j;
        continue;
      }  
  }
//  DetermineDataFiles(); // superflous, is also done in Exec()
}

void Copier::DetermineDataFiles()
{
  afnData.Resize(0);
  asSection.Resize(0);
  asEntry.Resize(0);
  for (unsigned int i=0; i < afnObj.iSize(); ++i) {
    Array<FileName> afnDat;
    try {
      IlwisObject obj = IlwisObject::obj(afnObj[i]);
      if (trq.fText(String("%S '%S'", SDATMsgFindDataFilesFor, afnObj[i].sFullName())))
        return;
      if (!obj.fValid())
        continue;
      obj->GetDataFiles(afnDat, &asSection, &asEntry);
      for (unsigned int j=0; j < afnDat.iSize(); ++j)
        AddFileName(afnDat[j], afnData, afnObj[i].sPath());
    }
    catch (const ErrorObject& err) {
      err.Show();
      continue;
    }  
  } 
}

void Copier::AddObjectAndProp(const FileName& fnObj)
{
  AddObject(fnObj);
  if (trq.fText(String("%S '%S'", SDATMsgCreateObjectListFor, fnObj.sFullName())))
    return;
  if (fCIStrEqual(fnObj.sExt, ".sms")) { // also copy sample map under new name
    FileName fn;
    ObjectInfo::ReadElement("SampleSet", "SampleMap", fnObj, fn);
    if (fn.fValid())
      AddObject(fn);
    if (!fSameDirCopy()) {
      ObjectInfo::ReadElement("SampleSet", "BackGround", fnObj, fn);
      if (fn.fValid())
        AddObject(fn);
    }
  }
  IlwisObject obj = IlwisObject::obj(fnObj);
  if (!obj.fValid())
    return;
  if (obj->fChanged)
    obj->Store();
    
  BaseMapPtr* bmap = dynamic_cast<BaseMapPtr*>(obj.pointer());
  if (0 != bmap) {
    if (!fSameDirCopy()) {
      if (!bmap->dm()->fSystemObject())
        AddObject(bmap->dm());
      if (!bmap->cs()->fSystemObject())
        AddObject(bmap->cs()->fnObj);
    }
//      if (fCopyAttTable && bmap->fTblAtt()) {
    if (!fSameDirCopy() && bmap->fTblAtt()) {
      Table tblAtt = bmap->tblAtt();
      if (tblAtt.fValid())
        AddObject(tblAtt->fnObj);
    }
    MapPtr* map = dynamic_cast<MapPtr*>(obj.pointer());
    if (0 != map)
      if (!fSameDirCopy()) {
        AddObject(map->gr()->fnObj);
        FileName fnHis(map->fnObj, ".his", true);
        if (File::fExist(fnHis))
          AddObject(fnHis);
      }
    SegmentMapPtr* smap = dynamic_cast<SegmentMapPtr*>(obj.pointer());
    if (0 != smap)
      if (!fSameDirCopy()) {
        FileName fnHis(smap->fnObj, ".hss", true);
        if (File::fExist(fnHis))
          AddObject(fnHis);
      }
    PolygonMapPtr* pmap = dynamic_cast<PolygonMapPtr*>(obj.pointer());
    if (0 != pmap)
      if (!fSameDirCopy()) {
        FileName fnHis(pmap->fnObj, ".hsa", true);
        if (File::fExist(fnHis))
          AddObject(fnHis);
      }
    PointMapPtr* pntmap = dynamic_cast<PointMapPtr*>(obj.pointer());
    if (0 != pntmap)
      if (!fSameDirCopy()) {
        FileName fnHis(pntmap->fnObj, ".hsp", true);
        if (File::fExist(fnHis))
          AddObject(fnHis);
      }
  }
    
  TablePtr* tbl = dynamic_cast<TablePtr*>(obj.pointer());
  if (0 != tbl) {
    if (!fSameDirCopy())
      if (!tbl->dm()->fSystemObject())
        AddObject(tbl->dm());
    for (int j=0; j < tbl->iCols(); ++j) {
      Column col = tbl->col(j);
      if (col.fValid() && !fSameDirCopy()) {
        AddObject(col->dm());
        Array<FileName> afnObjDep;
        Array<String> asNames;
        ObjectDependency::Read(col->sSection(), col->fnObj, afnObj, asNames);
        for (unsigned int j=0; j < afnObjDep.iSize(); ++j)
          AddObject(afnObjDep[j]);
        for (unsigned int j=0; j < asNames.iSize(); ++j) {
          Column c(asNames[j]);
          if (c.fValid())
            AddObject(c->fnObj);
        }
      }
    }
  }

  Table2DimPtr* t2d = dynamic_cast<Table2DimPtr*>(obj.pointer());
  if (0 != t2d) {
    if (!fSameDirCopy()) {
      if (!t2d->dm1()->fSystemObject())
        AddObject(t2d->dm1());
      if (!t2d->dm2()->fSystemObject())
        AddObject(t2d->dm2());
    }
  }

	ObjectCollectionPtr *obc = dynamic_cast<ObjectCollectionPtr*>(obj.pointer());
	if ( obc != 0 )
	{
		for(int i=0; i < obc->iNrObjects(); ++i)
			AddObjectAndProp(obc->fnObject(i));
	}
    
  DomainPtr* dom = dynamic_cast<DomainPtr*>(obj.pointer());
  if (0 != dom) {
    Representation rpr = dom->rpr();
    if (rpr.fValid())
      if (rpr->prc() || (!fSameDirCopy() && !rpr->fSystemObject()))
        AddObject(rpr->fnObj);
    DomainSort* ds = dynamic_cast<DomainSort*>(obj.pointer());
    if ((0 != ds) && (ds->fTblAtt()) && !fSameDirCopy()) {
      Table tblAtt = ds->tblAtt();
      if (tblAtt.fValid())
        AddObject(tblAtt->fnObj);
    }
  }
    
  RepresentationPtr* rpr = dynamic_cast<RepresentationPtr*>(obj.pointer());
  if (0 != rpr) {
    if (!fSameDirCopy())
      AddObject(rpr->dm()->fnObj);
  }
    
  GeoRefPtr* grf = dynamic_cast<GeoRefPtr*>(obj.pointer());
  if (0 != grf) {
    if (!fSameDirCopy())
      AddObject(grf->cs()->fnObj);
  }
      
  if (fCopyDep && !fSameDirCopy()) {
    Array<FileName> afnObjDep;
    ObjectDependency::Read(obj->fnObj, afnObjDep);
/*        Array<String> asColNames;
    ObjectDependency::Read(obj->fnObj, afnObjDep, asColNames);
    for (int k=0; k < asColNames.iSize(); ++k) {
      char* p = strrchr((char*)asColNames[k], '.');
      if (p != 0) {
        int iTblNameSize = p-(char*)asColNames[k];
        FileName fnTbl(asColNames[k].sLeft(iTblNameSize), ".tbt", false);
        AddFileName(fnTbl, afnObjDep);
      }
    }    */
    for (unsigned int j=0; j < afnObjDep.iSize(); ++j )
      AddObjectAndProp(afnObjDep[j]);
  }
}

void Copier::ConstructObjectList()
{
  for (unsigned int i=0; i < afnObj.iSize(); ++i) {
    try {
			FileName fn = afnObj[i]; // do not pass array elements that are to be extend
			                         // as reference member. They will be deallocated
      AddObjectAndProp(fn);
    }
    catch (const ErrorObject& err) {
      err.Show();
      continue;
    }
  }
  fObjectListConstructed = true;
}
#include <sys/types.h>

#include <sys/stat.h>

typedef BOOL (__stdcall * GetDiskFreeSpaceExFunction)(
	LPCTSTR lpDirectoryName,                 // directory name
	PULARGE_INTEGER lpFreeBytesAvailable,    // bytes available to caller
	PULARGE_INTEGER lpTotalNumberOfBytes,    // bytes on disk
	PULARGE_INTEGER lpTotalNumberOfFreeBytes // free bytes on disk
);

bool Copier::fSufficientDiskSpace(unsigned __int64 & iSpaceRequired, unsigned __int64& iSpaceAvailable)
{
  DetermineDataFiles();
  iSpaceRequired = 0;
  for (unsigned int i=0; i < afnData.iSize(); ++i) {
    struct _stati64 st; 
    if (0 != _stati64(afnData[i].sFullName().scVal(), &st))
      continue;
    iSpaceRequired += st.st_size; 
  }
  char cDrive[3];
  if (sDestPath.length() > 0)
    cDrive[0] = sDestPath[0];
  else
    cDrive[0] = fnObjCopy.sDrive[0];
  cDrive[1]=':';
  cDrive[2]=0;

	GetDiskFreeSpaceExFunction pGetDiskFreeSpaceEx = (GetDiskFreeSpaceExFunction)GetProcAddress( GetModuleHandle("kernel32.dll"),
													 "GetDiskFreeSpaceExA");

  ULARGE_INTEGER iAvail, dummy1, dummy2;
	bool fResult;
	if (pGetDiskFreeSpaceEx)
	{
		fResult = pGetDiskFreeSpaceEx (cDrive, &iAvail, &dummy1, &dummy2) > 0;
	  iSpaceAvailable = iAvail.QuadPart;
	}
	else 
	{
		DWORD dwSectPerClust, dwBytesPerSect, dwFreeClusters, dwTotalClusters;
		fResult = GetDiskFreeSpace (cDrive, 
									&dwSectPerClust, 
									&dwBytesPerSect,
									&dwFreeClusters, 
									&dwTotalClusters) > 0;
		iSpaceAvailable = dwFreeClusters * dwSectPerClust * dwBytesPerSect;
	}
  return iSpaceRequired < iSpaceAvailable;
}

void Copier::Exec()
{
  if (!fSameDirCopy() && fBreakDep) {
    // copy to temp object in current dir and break dependency
    FileName fnTemp = FileName::fnUnique(afnObj[0]);
    Copier copTemp(afnObj[0], fnTemp, FileName(), FileName(), true);
    copTemp.ConstructObjectList();
    copTemp.Exec();
    // copy temp object to other dir
    Copier cop(fnTemp, sDestPath);
    cop.ExcludeObjects(afnObjExclSav);
    cop.ConstructObjectList();
    cop.Exec();
    // delete temp object
    { IlwisObject obj = IlwisObject::obj(fnTemp);
      obj->fErase = true;
    }
    // rename copied object and its data files
    FileName fnCopy = fnTemp;
    fnCopy.Dir(sDestPath);
    Array<FileName> afnData;
    Array<String> asSection, asEntry;
    { 
			IlwisObject obj = IlwisObject::obj(fnCopy);      
      obj->GetDataFiles(afnData, &asSection, &asEntry);
    }  
    for (int j=afnData.iSize()-1; j >= 1; --j) 
		{
			if (asSection[j] == "") 
				continue;
      FileName fnDataCopy = afnData[j];
      fnDataCopy.sFile = afnObj[0].sFile;
			ObjectInfo::WriteElement(asSection[j].scVal(), asEntry[j].scVal(), fnCopy, fnDataCopy);
			MoveFile(afnData[j].sFullName().scVal(), fnDataCopy.sFullName().scVal());
    }
    FileName fnDataCopy = afnData[0];
    fnDataCopy.sFile = afnObj[0].sFile;
		if ( fnDataCopy.fExist())
			DeleteFile(fnDataCopy.sFullName().scVal());
    MoveFile(afnData[0].sFullName().scVal(), fnDataCopy.sFullName().scVal());
    return;
  }
  if (!fObjectListConstructed)
    ConstructObjectList();
  DetermineDataFiles();
  if (!fSameDirCopy()) {
    for (unsigned int i=0; i < afnData.iSize(); ++i) {
      FileName fnDest = afnData[i];
      fnDest.Dir(sDestPath);
      trq.fText(String("%S %S --> %S", SDATOthCopy, afnData[i].sFullName(), fnDest.sFullName()));
      File::Copy(afnData[i], fnDest);
      if (trq.fAborted()) {
        for (int j=i; j>=0; --j) {
          FileName fnDest = afnData[j];
          fnDest.Dir(sDestPath);
          _unlink(fnDest.sFullName().scVal());
        }
        return;
      } 
    }
    UpdateCatalog();
  }
  else {
    // fnaObj[0] is first object
    // for basemap: fnaObj[1] is attribute table if it needs to be copied 
    // for sampleset: fnaObj[1] is sample map
    int iObj = -1;
    for (unsigned int i=0; i < afnData.iSize(); ++i) {
      if (asSection[i].length() == 0) {
        ++iObj;
        UpdateCatalog();
      }
      FileName fnDest = afnData[i];
      if (iObj == 0)
        fnDest.sFile = fnObjCopy.sFile;
      else if (fnAttTableCopy.fValid()) // attribute table
        fnDest.sFile = fnAttTableCopy.sFile;
      else // sample map
        fnDest.sFile = fnObjCopy.sFile;
      trq.fText(String("%S %S --> %S", SDATOthCopy, afnData[i].sFullName(), fnDest.sFullName()));
      File::Copy(afnData[i], fnDest);
      if (trq.fAborted()) {
        for (int j=i; j>=0; --j) {
          FileName fnDest = afnData[j];
          fnDest.Dir(sDestPath);
          _unlink(fnDest.sFullName().scVal());
        }
        return;
      }
      if (fCIStrEqual(fnDest.sExt,".sms")) {
        FileName fnSmpMap = fnDest;
        fnSmpMap.sExt = ".mpr";
        ObjectInfo::WriteElement("SampleSet", "SampleMap", fnDest, fnSmpMap);
        ObjectInfo::WriteElement("ObjectDependency", "Object2", fnDest, fnSmpMap);
      }  
      if (fCIStrEqual(fnDest.sExt,".mpr") || fCIStrEqual(fnDest.sExt, ".mpa") ||
          fCIStrEqual(fnDest.sExt,".mps") || fCIStrEqual(fnDest.sExt, ".mpp")) {
        if (ObjectInfo::fDomain(fnDest)) {
          FileName fnDom = fnDest;
          ObjectInfo::WriteElement("BaseMap", "Domain", fnDest, fnDom);

          FileName fnRpr;
          ObjectInfo::ReadElement("Domain", "Representation", fnDom, fnRpr);
          if (fnRpr.sFile.length()) {
            fnRpr.sFile = fnDom.sFile;
            ObjectInfo::WriteElement("Domain", "Representation", fnDom, fnRpr);
          }
        }
        if (ObjectInfo::fRepresentation(fnDest)) {
          FileName fnRpr = fnDest;
          FileName fnDom = fnRpr;
          if (fnDom.fExist()) {
            ObjectInfo::ReadElement("Domain", "Representation", fnDom, fnDest);
            if (fnDest == fnRpr) {
              ObjectInfo::WriteElement("Representation", "Domain", fnRpr, fnDom);
              ObjectInfo::WriteElement("Rpr:Table", "Domain", fnRpr, fnDom);
            }
          }
        }
      }
      if (fCIStrEqual(fnDest.sExt, ".dom")) {
        FileName fnDom = fnDest;
        FileName fnRpr;
        ObjectInfo::ReadElement("Domain", "Representation", fnDom, fnRpr);
        if (fnRpr.sFile.length()) {
          fnRpr.sFile = fnDom.sFile;
          ObjectInfo::WriteElement("Domain", "Representation", fnDom, fnRpr);
        }
		ObjectInfo::WriteElement("DomainSort", "AttributeTable", fnDest, (char*)0);  // always remove link to attribute table
      }
      if (fCIStrEqual(fnDest.sExt, ".rpr")) {
        FileName fnRpr = fnDest;
        FileName fnDom = fnRpr;
        fnDom.sExt = ".dom";
        if (fnDom.fExist()) {
          ObjectInfo::ReadElement("Domain", "Representation", fnDom, fnDest);
          if (fnDest == fnRpr) {
            ObjectInfo::WriteElement("Representation", "Domain", fnRpr, fnDom);
            ObjectInfo::WriteElement("Table", "Domain", fnRpr, fnDom);
            FileName fnData = fnRpr;
            fnData.sExt = ".rp#";
            ObjectInfo::WriteElement("TableStore", "Data", fnRpr, fnData);
          }
        }
      }
      if ((iObj==0) && fBreakDep) {
        String sType;
        ObjectInfo::ReadElement("Ilwis", "Type", fnObjCopy, sType);
        if ((sType == "BaseMap") || (sType == "Table")) {
          String s;
          ObjectInfo::ReadElement(sType.scVal(), "Type", fnObjCopy, s);
          sType = s;
          ObjectInfo::ReadElement(sType.scVal(), "Type", fnObjCopy, s);
          String sTypeVirt = sType;
          sTypeVirt &= "Virtual";
          if (s == sTypeVirt) { // virtual object
            s = s.sLeft(s.length()-7); // remove Virtual
            s &= "Store";
            ObjectInfo::WriteElement(sType.scVal(), "Type", fnObjCopy, s);
          }  
          // break dependencies of columns in table
          ObjectInfo::ReadElement("Ilwis", "Type", fnObjCopy, sType);
          if ((sType == "Table") && ObjectInfo::fDependenciesForCopy(fnObjCopy)) {
            Table tbl(fnObjCopy);
            for (long i=0; i < tbl->iCols(); ++i) {
              Column col = tbl->col(i);
              if (col.fValid())
                tbl->col(i)->BreakDependency();
            }  
          }
        }
      }
    }
    iObj=0;
    int iData = 0;
//    for (int j=1; j < afnData.iSize(); ++j) {
    for (unsigned int j=1; j < asSection.iSize(); ++j) {
      if (asSection[j].length() == 0) {
        iObj++;
        continue;
      }
      iData++;
      FileName fnDataCopy = afnData[iData];
      FileName fnObjCop;
      if (iObj == 0 || fnAttTableCopy.sFile.length() == 0) {
        if (fCIStrEqual(fnDataCopy.sExt, ".rpr")) // hack to prevent problems
          continue;
        fnObjCop = afnObj[0];
        fnDataCopy.sFile = fnObjCopy.sFile;
        fnObjCop.sFile = fnObjCopy.sFile;
      }  
      else { // attribute table
        fnObjCop = afnObj[iObj];
        fnDataCopy.sFile = fnAttTableCopy.sFile;
        fnObjCop.sFile = fnAttTableCopy.sFile;
      }  
      ObjectInfo::WriteElement(asSection[j].scVal(), asEntry[j].scVal(), fnObjCop, fnDataCopy);
    }  
    if (fnAttTableCopy.fValid())
      ObjectInfo::WriteElement("BaseMap", "AttributeTable", fnObjCopy, fnAttTableCopy);
    else {
      String s;
      ObjectInfo::ReadElement("BaseMap", "AttributeTable", fnObjCopy, s);
      if (s.length() > 0)
        ObjectInfo::WriteElement("BaseMap", "AttributeTable", fnObjCopy, (const char*)0);
    }
  }
}

void Copier::Copy(const FileName& fnObj, const FileName& fnCopy, bool fBreakDep) 
{
  if (fCIStrEqual(fnObj.sFullPath(), fnCopy.sFullPath()))
    return;
  Copier cop(fnObj, fnCopy, FileName(), FileName(), fBreakDep);
  cop.Exec();
}

void Copier::Copy(const FileName& fnObj, const String& sDestDir, bool fBreakDep)
{
  char *sFullDestPath = new char[MAXPATH];
  _fullpath(sFullDestPath, sDestDir.scVal(), MAX_PATH);
  String sDstDir = sFullDestPath;
  delete [] sFullDestPath;
  if (fCIStrEqual(fnObj.sPath(), sDstDir))
    return;
  Copier cop(fnObj, sDestDir, fBreakDep);
  Array<FileName> afnExist;
  cop.CheckExist(afnExist);
  cop.ExcludeObjects(afnExist);
  cop.Exec();
}

void Copier::Copy(const Array<FileName>& afnObj, const String& sDestDir, bool fBreakDep)
{
  if (afnObj.iSize() == 0)
    return;
  char *sFullDestPath = new char[MAXPATH];
  _fullpath(sFullDestPath, sDestDir.scVal(), 80);
  String sDstDir = sFullDestPath;
  delete [] sFullDestPath;
  if (fCIStrEqual(afnObj[0].sPath(), sDstDir))
    return;
  Copier cop(afnObj, sDestDir);
  cop.SetBreakDependency(fBreakDep);
  Array<FileName> afnExist;
  cop.CheckExist(afnExist);
  cop.ExcludeObjects(afnExist);
  cop.Exec();
}




