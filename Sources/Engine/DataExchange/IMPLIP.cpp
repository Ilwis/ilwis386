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
/*
  implip.c
  import ILWIS from .LIP format
  By Willem Nieuwenhuis, nov 1996
  ILWIS Department ITC
	Last change:  WN   29 Nov 96    4:50 pm
*/
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Base\DataObjects\ERR.H"

/*struct LIP_Header {

};

class IPL_LIP {
public:
  IPL_LIP(const FileName& fnFile);
  bool SetupMapList(const FileName&, Tranquilizer&);
  long iLinesRead()        { return _iLins; }
  long iNrRows()           { return _iTotRows; }
  long iRowsRead()         { return _iRow; }
  ~IPL_LIP();
  String sReason;
//  bool fIPL_LIPCoord(Coord&);    // read world coordinate from file
  void GetNextLine();            // get line from IPL_LIP list
private:
  File* fileIPL_LIP;
  String sLine;
  long _iLins, _iRow, _iTotRows;
};

// implementation


LIP_Header_Type = RECORD
                      LREC, STF, LEXT, NDAT, NBEL,
                      IA, NKAN, NDUM, LF, LL, JF, JL,
                      LINC, JINC, NLIN, NELS: Integer;
                      KAN: Array[1..32] of Integer;
                      AlfLabel: String(416);
                    END;*/
/*
{$Include : '\Lib\AskFuncs.Int'}
{$Include : '\Lib\Dir.Int'}
{$Include : '\Lib\MapHandl.Int'}
{$Include : '\Lib\FileHand.Int'}
{$Include : '\Lib\GenGraph.Int'}
{$Include : '\Lib\MirrMap.Int'}
{$Include : 'ConvRasI.Int'}

Implementation of Conversion_Raster_Input;
  Uses Ask_Functions, Dir, Map_Handler, File_Handler, General_Graphics,
       Mirr_Rot_Map;


{$Include : '\Lib\Tls.Inc'}
{$Include : '\Lib\IbmMenu.Inc'}
{$Include : '\Lib\IbmMenu1.Inc'}
FUNCTION AllMQQ(w : WORD) : ADSMEM; EXTERN;
Function FreMqq(Buf: AdsMem): Word; extern;
Procedure EndXqq; extern;

Type TypePower2 = Array[0..14] of Integer;
Const Power2 = TypePower2 (#0001,#0002,#0004,#0008,#0010,#0020,#0040,#0080,
                           #0100,#0200,#0400,#0800,#1000,#2000,#4000);
Const Pi = 3.141593;

Procedure ConvRasIn_LIP;
Type
  LIP_Header_Type = RECORD
                      LREC, STF, LEXT, NDAT, NBEL,
                      IA, NKAN, NDUM, LF, LL, JF, JL,
                      LINC, JINC, NLIN, NELS: Integer;
                      KAN: Array[1..32] of Integer;
                      AlfLabel: String(416);
                    END;
CONST
  MAX_NR_BANDS = 32;
VAR
  Inp : INTEGER;
  Out : ARRAY [1..MAX_NR_BANDS] OF INTEGER;
  Number: LString(3);
  Name, Inp_File_Name : LSTRING(80);
  Out_File_Name : ARRAY [1..MAX_NR_BANDS] OF LSTRING(80);
  LIP_Header : LIP_Header_Type;
  Nr_Cols, Nr_Lines, NBands, Size : INTEGER;
  Code, OutType : INTEGER;
  Nr, Err : INTEGER;
  i, j, k, l : INTEGER;
  Buf : ADSMEM;

BEGIN
  REPEAT
    writeln;
    AskFileName('Input LIP file name',
                  Null, '.LIP', True, Name);
    If Name = Null then Return;
    GetFileName(Name, Inp_File_Name, '.LIP');
    Openff(Inp_File_Name, 0, Inp, Err);
    IF Err <> 0
    THEN begin
      Writeln(Chr(7), 'File ', Inp_File_Name, ' not found');
      Ask_Active := False;
    end;
  UNTIL Err = 0;
  Readff(Inp, ADS LIP_Header, 512, Nr, Err);

  Nbands := LIP_Header.NKAN;
  Nr_Lines := LIP_Header.NLIN;
  Nr_Cols := LIP_Header.NELS;
  writeln;
  MdUdl; writeln('Enter output map names:'); MdNormal;
  FOR i := 1 TO Nbands DO
    REPEAT
      WriteLn;
      Write('For band ',i:0,' (Channel ', LIP_Header.KAN[i]:0, '): ');
      AskFilename('Map name', Null,
                  '.MPD', False, Out_File_Name[i]);
      If Out_File_Name[i] = Null then Return;
      Out[i] := Open_Map(Out_File_Name[i], FALSE);
      IF Out[i] < 0
      THEN begin
        write(Chr(7), 'Can''t create map ', Out_File_Name[i]);
        Ask_Active := False;
      end;
    UNTIL Out[i] >= 0;
  FOR i := 1 TO Nbands
  DO BEGIN
    Code := Put_Map_Int_Item(Out[i], It_Lines, Nr_Lines);
    Code := Put_Map_Int_Item(Out[i], It_Cols, Nr_Cols);
    Code := Put_Map_Window(Out[i], 1, Nr_Lines, 1, Nr_Cols);
    IF LIP_Header.NBEL = 1
    THEN BEGIN
      Code := Put_Map_Int_Item(Out[i], It_MpType, 1);
      OutType := Byte_Map;
    END
    ELSE BEGIN
      Code := Put_Map_Int_Item(Out[i], It_MpType, 2);
      OutType := Int_Map;
    END;
  END;
  writeln;

  Write('Converting...');
  If LIP_Header.IA = 0 then   ! Contiguous Format
    If OutType = Int_Map then
      Size := Nr_Cols * 2
    else
      Size := Nr_Cols
  else                        ! Number of Blocks per Channel
    Size := LIP_Header.IA * 512;
  Buf := AllMQQ(Wrd(Size));

  FOR i := 1 TO Nr_Lines
  DO BEGIN
    Write(i:4,chr(27),'[4D');
    FOR j := 1 TO Nbands
    DO BEGIN
      Readff(Inp, Buf, Size, Nr, Err);
      Code := Write_Map_Line(Out[j], i, OutType, Buf);
    END;
  END;

  Eval(FreMqq(Buf));
  Closeff(Inp, Err);
  FOR j := 1 TO Nbands DO
    Code := Close_Map(Out[j]);
end; { ConvRasIn_LIP }
*/

void ImpExp::ImportLIP(File& FileIn, const FileName& fnObject)
{
}


