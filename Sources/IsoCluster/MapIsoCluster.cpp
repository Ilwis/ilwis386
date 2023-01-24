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
/* $Log: /ILWIS 3.0/RasterApplication/Mapaggr.cpp $
* 
* 8     22-02-02 16:18 Hendrikse
* removed limitation  if ( iInpMapCols > 8192 ) still needed in 1997
* 
* 7     3-02-00 8:51 Hendrikse
* extended  GroupFactorError to include too large GrpFactor
* made ReadBufLinesReal and  ReadBufLinesRaw both const and also the
* related  ReadLineVal ReadLineRaw 
* Made ComputeLineVal work for value and non-value input
* Made fDomainChangeable for AggregCnt function so that it can count
* class and id pixels
* 
* 6     21/01/00 16:19 Willem
* Added dependencies for GeoRef's, so map gets recalculated when the
* GeoRef is changed
* 
* 5     9/29/99 4:15p Wind
* added case insensitive string comparison
* 
* 4     9/29/99 10:47a Wind
* added case insensitive string comparison
* 
* 3     9/08/99 8:51a Wind
* changed sName( to sNameQuoted( in sExpression()
* 
* 2     3/11/99 12:16p Martin
* Added support for Case insesitive 
// Revision 1.6  1998/03/06 16:59:52  janh
// Two things have been solved:
// - ComputeLineRaw calculated the Values correctly, but then did the
//   calculation on the raw values, resulting in incorrect results.
// - The ReadbufLine function did not correct for the RowOffset and
//   ColOffset values. Both the ReadBufLinesReal and ReadBufLinesRaw
//   were affected.
//
// Revision 1.5  1997/09/17 14:24:23  martin
// Checks if a map is not too large to be calculated.
//
// Revision 1.4  1997/08/20 17:24:36  Wim
// For Standard deviation only positive valuerange
//
// Revision 1.3  1997-08-20 19:22:32+02  Wim
// Changed some default setting for valuerange and domain
//
// Revision 1.2  1997-08-18 17:09:12+02  Wim
// Error message group factor corrected
//
/* MapIsoCluster
Copyright Ilwis System Development ITC
dec. 1996, by Jelle Wind
Last change:  WK   29 Jun 98    3:41 pm
*/
#include "IsoCluster\MapIsoCluster.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Image.h"
//#include "Engine\Domain\dmclass.h"
#include "Point.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapIsoCluster(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapIsoCluster::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapIsoCluster(fn, (MapPtr &)ptr);
}

const char* MapIsoCluster::sSyntax() {
	return "MapIsoCluster(maplist,numdesiredcluster,minsamples,stdev,minlumpdist,maxlumpnumber,numberofiters)";

}

String wpsmetadataMapIsoCluster() {
	WPSMetaData metadata("MapIsoCluster");
	return metadata.toString();
}

ApplicationMetadata metadataMapIsoCluster(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapIsoCluster();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapIsoCluster::sSyntax();

	return md;
}

MapIsoCluster* MapIsoCluster::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if ( iParms < 6) {
		ExpressionError(sExpr, sSyntax());
	}
	MapList mpl(as[0], fn.sPath());
	if ( mpl.fValid()) {
		int minimumLumpDistance = 1;
		double stdDev = 10;
		int maxLumpNumber = 5;
		int numDesiredCluster = 16;
		int minSampleSize = 5;
		int iterations = 20;
		if ( iParms > 1)
			numDesiredCluster = as[1].iVal();
		if ( iParms > 2)
			minSampleSize = as[2].iVal();
		if ( iParms > 3)
			stdDev = as[3].iVal();
		if ( iParms > 4)
			minimumLumpDistance = as[4].rVal();
		if ( iParms > 5)
			maxLumpNumber = as[5].iVal();
		if ( iParms > 6)
			iterations = as[6].iVal();
		return new MapIsoCluster(fn, p, mpl,numDesiredCluster, minSampleSize, iterations, stdDev, minimumLumpDistance, maxLumpNumber) ;
	}
	return 0; 
}

MapIsoCluster::MapIsoCluster(const FileName& fn, MapPtr& p)
: MapFromMapList(fn, p)
{
	ReadElement("MapIsoCluster", "NumberDesiredClusters", numDesiredCluster);
	ReadElement("MapIsoCluster", "MinimumSampleSize", minSampleSize);
	ReadElement("MapIsoCluster", "NumberOfIterations", iterations);
	ReadElement("MapIsoCluster", "StandardDeviation", stdDev);
	ReadElement("MapIsoCluster", "MinimumLumpingDistance", minimumLumpDistance);
	ReadElement("MapIsoCluster", "MaximumLumpNumber", maxLumpNumber);
	fNeedFreeze = true;
	sFreezeTitle = "MapIsoCluster";

}

MapIsoCluster::MapIsoCluster(const FileName& fn, MapPtr& p, const MapList& mpl, int _numDesiredCluster, int _minSampleSize, int _iter, double _std, int _minLDist, int maxLNumber) 
: MapFromMapList(fn, p, mpl, Domain("image")/* just temp, see below*/),
minimumLumpDistance(_minLDist),
stdDev(_std),
maxLumpNumber(maxLNumber),
numDesiredCluster(_numDesiredCluster),
minSampleSize(_minSampleSize),
iterations(_iter)
{

	FileName fndom(fnObj, ".dom");
	Domain dom(fndom, numDesiredCluster, dmtCLASS);
	dom->Store();
	SetDomainValueRangeStruct(dom);
	fNeedFreeze = true;
	sFreezeTitle = "MapIsoCluster";
	if (!fnObj.fValid())
		objtime = objdep.tmNewest();
}

void MapIsoCluster::Store()
{
	MapFromMapList::Store();
	WriteElement("MapFromMapList", "Type", "MapIsoCluster");
	WriteElement("MapIsoCluster", "NumberDesiredClusters", numDesiredCluster);
	WriteElement("MapIsoCluster", "MinimumSampleSize", minSampleSize);
	WriteElement("MapIsoCluster", "NumberOfIterations", iterations);
	WriteElement("MapIsoCluster", "StandardDeviation", stdDev);
	WriteElement("MapIsoCluster", "MinimumLumpingDistance", minimumLumpDistance);
	WriteElement("MapIsoCluster", "MaximumLumpNumber", maxLumpNumber);
}

MapIsoCluster::~MapIsoCluster()
{

}

void MapIsoCluster::Init()
{

}

bool MapIsoCluster::fFreezing()
{
	trq.SetTitle("Isocluster");

	maxSampleSize = 100000; // Should this be user-defined?
	long pixels = mpl->rcSize().Col * mpl->rcSize().Row;
	if (maxSampleSize > pixels)
		maxSampleSize = pixels; 

	Image image=Image(mpl, numDesiredCluster, minSampleSize, trq);
	image.readImages();
	image.sampleCenters();

	trq.SetText("Setting Sample points");
	image.samplePoints(maxSampleSize);

	trq.SetText("Setting filter centers");
	image.BuildKMfilterCenters();

	trq.SetText("Clustering");

	clock_t start = clock();                    // start the clock

	int iter=0;
	double exec_time=0;


	for (iter=1; iter<= iterations; iter++)
	{
		if (trq.fUpdate(iter, iterations))
			return false;

		if (iter== iterations)
		{
			//*IsoOut<<"\tPerform the last iterative clustering on all points.\n";
			trq.SetText("Perform iterative clustering on all points");
			image.preFinalClustering();

		}
		else if (iter!=1 && iter!=iterations)
		{

			image.SetFilterCenters();	//copies the image centers to filter centers.

		}
		do
		{
			//STEP 2:
			//*IsoOut<<"\tCalculate distortions and Put points into clusters.\n"; 		
			image.PutInCluster();


			//STEP 3:
			image.PostAnalyzeClusters(); 


			//STEP 4:
			//*IsoOut<<"\tUpdate centers by calculating the average point in each cluster."<<endl;  
			image.UpdateCenters();


		}while (image.WasDeleted());

		//need to update distances since in the last iteration centers have modified.

		//*IsoOut<<"\tCalculate distortions and Put points into clusters.\n"; 		
		image.PutInCluster();

		//set Image's centers to be equal to filter's centers, calculated in KCtree.
		image.SetImageCenters();

		//STEP 5:
		//calculates the average of squared distances
		image.CalculateAverageDistances();

		//STEP 6:
		//calculates the average of values in step 5
		image.OverallAverageDistances();

		//STEP 7:
		int next_step=8;
		if (iter== iterations)
		{	
			minimumLumpDistance=0;
			next_step=11;
		}

		else if ( image.getNumCenters() <= (numDesiredCluster/2))
		{
			next_step=8;
		}
		else if ((iter % 2 ==0) || (image.getNumCenters() >= 2*numDesiredCluster))
		{
			next_step=11; 
		}

		switch (next_step)
		{
		case 8:
			{
				//STEP 8:
				image.CalculateSTDVector();

				//STEP 9:
				image.CalculateVmax();

				//STEP 10:
				// the vector to_split will contain integers that represent the cluster numbers
				// that need to be split.
				vector<int> to_split=image.ShouldSplit(stdDev);

				if (to_split.size()!=0)
				{
					image.Split(to_split);
					//we need to substract one if it was the last iteration because otherwise we
					//we will exit the loop without updating clusters.
					if (iter==iterations)
						iter=iter-1;

					to_split.clear();
					break; //go to step 2
				}	
			} //CASE 8

		case 11:
			{
				//STEP 11:
				image.ComputeCenterDistances();

				//STEP 12:
				vector<PairDistanceNode> to_lump=image.FindLumpCandidates(minimumLumpDistance, maxLumpNumber);

				//STEP 13:
				if (to_lump.size()!=0)
					image.Lump(to_lump);
			} //CASE 11   


		}// SWITCH

	}// FOR LOOP


	exec_time = double(clock() - start)/double(CLOCKS_PER_SEC);            // get elapsed time

	Domain dm(ptr.dm());
	image.writeClassifiedImage(ptr, dm);

	//image.generateReport(IsoOut);
	//*IsoOut<<"Algorithm's run time: "<<exec_time<<" CPU seconds."<<endl;

	//*IsoOut<<"total overall dist "<<image.getDistortions()<<endl;

	//if (is_rand)
	//	kmDeallocPts(all);



	return true;
}

String MapIsoCluster::sExpression() const
{
	return String("MapIsoCluster(%S, %d, %d, %d, %f, %d, %d)", mpl->fnObj.sRelative(), numDesiredCluster, minSampleSize, iterations, stdDev,minimumLumpDistance, maxLumpNumber);
}


bool MapIsoCluster::fDomainChangeable() const
{
	return true;
}

bool MapIsoCluster::fValueRangeChangeable() const
{
	return false;
}




