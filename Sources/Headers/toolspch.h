/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

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

#pragma warning( disable : 4244)

#include "Headers\base.h"

#include "Engine\Base\DataObjects\Dlist.h"
#include "Engine\Base\DataObjects\Slist.h"
#include "Engine\Base\DataObjects\Buf.h"
#include "Engine\Base\DataObjects\ARRAY.H"
#include "Engine\Base\DataObjects\Hash.h"
#include "Engine\Base\DataObjects\strng.h"
#include "Engine\Base\DataObjects\Set.h"
#include "Engine\Base\Msm.h"
#include "Engine\Base\File\FILENAME.H"
#include "Engine\Base\System\Appcont.h"
#include "Engine\Base\DataObjects\DT.H"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Base\File\File.h"
#include "Engine\Base\DataObjects\Dat2.h"
#include "Engine\Base\DataObjects\Tranq.h"
#include "Engine\Base\System\ILWSingleLock.h"




