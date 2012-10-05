//=== File Prolog ================================================================
//      This code was developed by NASA, Goddard Space Flight Center, Code 588
//      and computer science department of the university of Maryland at College Park.
//--- Contents -------------------------------------------------------------------
//      File 'Image.cc' contains the implementaion of 'Image' class as described in
//      'Image.h' in C++.
//
//--- Description ----------------------------------------------------------------
//  This calss was originally implemented for ISOCLUS algorithm but one may use it for
//  other image processing purposes.
//-- Notes:-----------------------------------------------------------------------
//
//-- Development History:--------------------------------------------------------
//   Date             Author                Reference
//   Description
//
//   May 24, 2002     Nargess Memarsadeghi  NASA GSFC, Code 588
//   Initial implementation
//
//--- DISCLAIMER---------------------------------------------------------------
//
//	This software is provided "as is" without any warranty of any kind, either
//	express, implied, or statutory, including, but not limited to, any
//	warranty that the software will conform to specification, any implied
//	warranties of merchantability, fitness for a particular purpose, and
//	freedom from infringement, and any warranty that the documentation will
//	conform to the program, or any warranty that the software will be error
//	free.
//
//	In no event shall NASA be liable for any damages, including, but not
//	limited to direct, indirect, special or consequential damages, arising out
//	of, resulting from, or in any way connected with this software, whether or
//	not based upon warranty, contract, tort or otherwise, whether or not
//	injury was sustained by persons or property or otherwise, and whether or
//	not loss was sustained from or arose out of the results of, or use of,
//	their software or services provided hereunder.
//--- Warning-------------------------------------------------------------------
//    This software is property of the National Aeronautics and Space
//    Administration.  Unauthorized use or duplication of this software is
//    strictly prohibited.  Authorized users are subject to the following
//    restrictions:
//    *   Neither the author, their corporation, nor NASA is responsible for
//        any consequence of the use of this software.
//    *   The origin of this software must not be misrepresented either by
//        explicit claim or by omission.
//    *   Altered versions of this software must be plainly marked as such.
//    *   This notice may not be removed or altered.
//
//=== End File Prolog=============================================================

//#include <iostream>
//#include <fstream>
//#include <math.h>
//#include <algorithm>  //for 'sort' function call

#include "Headers\toolspch.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "IsoCluster\MapIsoCluster.h"
#include "Engine\Domain\dmsort.h"
#include "Image.h"
#include "Point.h"
#include "KMrand.h"

#define K 0.5
#define  MINCLUS 1
#define  MAXCLUS 255

extern vector<vector<int> > clusters;
//extern vector<KMpoint> kcCenters; 
/**********************************************************************/
/* Constructor                                                        */
/**********************************************************************/
Image::Image(const MapList& _mpl , int NumClus, int SAMPRM, Tranquilizer& _trq) : mpl(_mpl), trq(_trq)
{
	int i;
	Map mp = mpl[mpl->iLower()];
	NumRow= mp->rcSize().Row;
	NumCol=mp->rcSize().Col;
	NumBands=mpl->iSize();
	NumClusters=NumClus;
	NumSamples=SAMPRM;
	Deleted=false;
	ImageSizeInByte=NumRow*NumCol;
	STDVector=NULL;
	Vmax=NULL;
	Vmax_index=NULL;
	io_image=NULL;
	is_rand = false;


	if (!is_rand)
	{
		io_image=new unsigned char* [NumBands];
		if (!io_image)
		{
			throw ErrorObject(TR("Memory allocation failed in Image"));
		}

		for (i=0; i <NumBands; i++)
		{	
			io_image[i]=new unsigned char[ImageSizeInByte];
			if (!io_image[i])
			{
				throw ErrorObject(TR("Memory allocation failed in Image"));
			}

		}


		/* Initialize */
		for (i=0; i<NumBands; i++)
			for (int j=0; j< ImageSizeInByte; j++)
				io_image[i][j]=0;
	}

	allPoints=new IsoCluster::Point*[ImageSizeInByte];
	if (!allPoints)
	{
		throw ErrorObject(TR("Memory allocation failed in Image"));
	}


	for (i=0; i < ImageSizeInByte; i++)
	{
		allPoints[i]=NULL;
	}


}
/******************************************************************/
/* Destructor							  */
/*****************************************************************/
Image::~Image()
{
	int i;


	if (io_image!=NULL)
	{
		for (i=0; i < NumBands; i++)
			delete [] io_image[i];

		delete [] io_image;
	}

	trq.SetText("Cleaning up");
	for (i=0; i < ImageSizeInByte && !is_rand ; i++)
	{  
		if ( i % 1000)
			trq.fUpdate(i, ImageSizeInByte);
		delete allPoints[i];

	} 

	DeleteCenters();

	delete [] allPoints;
	delete [] average_distances;

	io_image=NULL;
	allPoints=NULL;
	samples.clear();

}
/*****************************************************************/
void Image::DeleteCenters()
{
	int c=centers.size(),i;

	for (i=0; i < c; i++)
		delete centers[i];

	centers.clear();

}
/**********************************************************************/
/* If points have been generated already, just set them to all points */
/**********************************************************************/
void Image:: setPoints(KMpointArray all )
{

	for (int i=0; i< ImageSizeInByte; i++)
	{
		allPoints[i]=new IsoCluster::Point(NumBands, all[i]);


	}
}
/*****************************************************************/
/* readImage:							 */
/* This file will read each bands information from its file, and */
/* stores it in a row of the 'image' member			 */
/*****************************************************************/
void Image::readImages()
{
	int i;

	for (i=0; i<mpl->iSize(); i++)
	{
		Map mp = mpl[i];

		FileName fnData(mp->fnObj, ".mp#");
		ifstream file(fnData.sPhysicalPath().c_str(),ios_base::in | ios_base::binary);
		if ( file.is_open()) {
			file.read((char *)io_image[i], ImageSizeInByte);
		}
		file.close();
	}

}
/**********************************************************************/
/* Builds the 'filter; field based on the current points and centers. */
/**********************************************************************/ 
void Image::BuildKMfilterCenters()
{
	int i, s=samples.size();


	KMpointArray all=new KMpoint[s];

	for (i=0 ; i <s; i++)
	{ 
		trq.fUpdate(i,s);

		all[i]=kmAllocCopyPt(NumBands, allPoints[samples[i]]->getPoint());
		//      all[i]=(allPoints[samples[i]])->getPoint();

	}

	int c=centers.size();
	vector<KMpoint> cnts;

	for (i=0; i< c; i++)
	{
		trq.fUpdate(i,c);

		KMpoint P=new KMcoord[NumBands];
		kmCopyPt(NumBands, centers[i]->getPoint(), P);
		cnts.push_back(P);
	}

	data=new KMdata(NumBands, s, all);
	filter=new KMfilterCenters(c, *data, cnts);
	DeleteCenters();

}
/**********************************************************************/
//copies the image centers to filter centers.
void Image::SetFilterCenters()
{


	filter->SetFilterCenters(centers);
	DeleteCenters();


}
/***********************************************************************/
// copies the filter centers to the image centers.
void Image::SetImageCenters()
{


	vector<KMpoint> v=filter->getCtrPts();
	int num=v.size(),i;


	DeleteCenters();


	for (i=0; i <num; i++)
	{
		IsoCluster::Point* p=new IsoCluster::Point();
		p=p->AllocPoint(NumBands, v[i]);
		centers.push_back(p);
	}


}
/*************************************************************************/
/* outputs the classified data in one row of the 'io_image' array        */
/* and outputs the result into a file whose name is passed as parameter. */
/*************************************************************************/
void Image::writeClassifiedImage(MapPtr& ptr, Domain& dm )
{
	int i,j;


	int num_clusters=clusters.size();
	int size;

	for (i=0; i < num_clusters; i++)
	{
		size=clusters[i].size();
		for (j=0; j < size; j++) 
			io_image[0][clusters[i][j]]=i+1;
	}

	for(int row = 0; row < ptr.rcSize().Row; ++row) {
		LongBuf buf(ptr.rcSize().Col);
		for(int col = 0; col < ptr.rcSize().Col; ++col) {
			byte v = *(io_image[0]+ row * ptr.rcSize().Col + col);
			buf[col] = v;
		}
		ptr.PutLineRaw(row, buf);


	}
	for(int i = 1 ; i <= NumClusters; ++i)
		dm->pdsrt()->SetVal(i, String("class %d", i));
}
/********************************************************************************/
/* This function looks at the io_image (which its dimensions are number of      */
/* points in image times NumBands), and returns a IsoCluster::Point object whose coordinate */
/* are that of the 'PointCount' column in io_image.				*/
/********************************************************************************/
IsoCluster::Point* Image::points_helper(int PointCount)
{
	IsoCluster::Point* to_return;
	double * p=new double[NumBands];
	if (!p)
	{
		throw ErrorObject(TR("Memory Allocation Failed"));
		exit(1);

	}
	for (int i=0; i< NumBands; i++)
	{
		p[i]=io_image[i][PointCount];	

	}

	to_return=new IsoCluster::Point(NumBands, p);
	if (!to_return)
	{
		throw ErrorObject(TR("Memory Allocation Failed"));
		exit(1);
	}
	return to_return;

}
/*******************************************************************/
IsoCluster::Point* Image::getPoint(int row, int col)
{

	int  index=row*NumCol + col ;

	if (index>-1 && index <ImageSizeInByte)
		return allPoints[index];

	return NULL;

}
/*******************************************************************/
IsoCluster::Point* Image::getPoint(int index)
{

	if (index>-1 && index <ImageSizeInByte)
		return allPoints[index];

	return NULL; 

}
/*******************************************************************/
IsoCluster::Point** Image::getAllPoints()
{
	return allPoints;

}
/******************************************************************/
KMfilterCenters* Image::getFilter()
{

	return filter;
}
/******************************************************************/
vector<IsoCluster::Point*> Image::getCenters()
{

	return centers;

}
/*******************************************************************/
int Image::size()
{
	return ImageSizeInByte;

}
/*******************************************************************/
int Image::SampleSize()
{

	return samples.size();
}
/*******************************************************************/
int Image::getDimension()
{
	return NumBands;

}
/*******************************************************************/
int  Image::getNumCenters()
{
	// return kcCenters.size();
	return centers.size();
}

/******************************************************************************/
/* sampleCenters selects a set of initial centers from the set of allPoints.  */
/******************************************************************************/
void Image::sampleCenters()
{
	srand(0);

	int i, to_add;
	for (i=0; i < NumClusters ; i++)
	{
		to_add=rand()%ImageSizeInByte;

		//If the random point is not a duplicate, add it to the list of centers.
		IsoCluster::Point* p;
		if (is_rand)
			p=new IsoCluster::Point(*allPoints[to_add]);
		else
			p=points_helper(to_add);
		if (!p)
		{
			cout<<"Error 7: Memory Allocation Failed"<<endl;
			exit(1);
		}

		if (find_center(p)== -1)
		{
			centers.push_back(p);
		}
		else 
			i--;
	}//for


}
/*******************************************************************************/
/* here we sample points randomly to perform the iterative clustering on them  */
/* for more information on the method used please see:			       */
/* (1) "Programming Pearls", Addison Wesley, 1986,                             */
/* (2) "More Programming Pearls", Addison Wesley, 1988. 		       */
/*******************************************************************************/
void Image::samplePoints(double s)
{

	srand(0);

	int n=ImageSizeInByte;

	double probability=s/n;

	for  (int i=0; (i <ImageSizeInByte) && (s > 0) ; i++)
	{
		//generate a random number between 0 and 1
		double x = ( (double)rand() / (double)(RAND_MAX+1) ); 

		if (x <= probability)
		{
			samples.push_back(i);
			if (!is_rand)
				allPoints[i]=points_helper(i);  	

			s=s-1;
		}

		n=n-1;
		probability=s/n;

	}


}
/*******************************************************************************/
/* we just modify the data points for real data by adding a small number in    */
/* order of 0.001 such that points that lie on cell borders of the kd-tree be  */
/* assigned to on random to its children's cell.                               */ 
/*******************************************************************************/
void Image::addNoise()
{
	int s=samples.size(); 

	for (int i=0; i <s ; i++)
		*(allPoints[samples[i]]) = *(allPoints[samples[i]])+ 0.001*kmRanUnif(-1,1);



}
/*******************************************************************************/
bool Image::WasDeleted()
{
	return Deleted;
}
/***********************************************************************************/
/* Given all the points in the image we take sampled points to determine	   */	
/* cluster means, for having the final clasified image, one can run this function  */
/* with increment=1 to have all the points classified				   */	
/***********************************************************************************/
void Image::PutInCluster()
{
	Deleted=false;

	if (filter!=NULL)
		filter->ComputeDist();
	else
		kmError("In Image.cc :PutInCluster,KMfilterCenter object field is NULL.\nNeed to Build this object first",KMabort);

}
/*******************************************************************************/
/* searches in the 'centers' vector to see if it can find a particular point   */
/* It returns the index of the point in the vector if found, and -1 otherwise  */
/*******************************************************************************/
int Image::find_center(IsoCluster::Point* to_find)
{
	int value=-1;
	int size=centers.size(),i;

	for (i=0; i < size; i++)
	{
		if ( (*centers[i])== (*to_find) )
		{
			value=i;
			break;
		}

	}

	return value;
}
/************************************************************************************/
/* This function checks number of points in each cluster. If number of points in    */ 
/* any cluster is less than NumSamples (desired minimum number of points in each    */
/* cluster), then that cluster and its center gets deleted (Note: only the cluster  */
/* from 'clusters' vector gets deleted, the points in the cluster do not get deleted*/
/* from set of sampled points for further iterative clustering.                     */
/************************************************************************************/

void Image::PostAnalyzeClusters()
{
	int i, num_clus, index,count;

	num_clus=clusters.size();

	vector< vector <int> >:: iterator clusters_it=clusters.begin();

	count=0;
	for (i=0, index=0; index < num_clus; i++, clusters_it++, index++)
	{
		if (clusters[i].size() < NumSamples)
		{
			clusters.erase(clusters_it);
			Deleted=true;
			clusters_it--; i--;
			count++;
		}
		else
			centers_to_keep.push_back(index);
	}


	if (Deleted)
	{
		filter->UpdateCenters(centers_to_keep);

	}

}
/****************************************************************************************/
/* Update each cluster center by setting it to the sample mean of its corresponding set */
/****************************************************************************************/
void Image::UpdateCenters()
{
	//moves each center to the centroid of the cluster
	filter->lloyd1Stage(centers_to_keep);
	centers_to_keep.clear();


}
/************************************************************************************/
/* we need to calculate the average of squared distances of all points in a cluster */
/* from that cluster center.  							    */
/***********************************************************************************/	
void Image::CalculateAverageDistances()
{
	int num_centers = clusters.size(),i;

	average_distances=filter->getDists(true);

	for (i=0; i< num_centers; i++)
	{
		// here we are taking square root of average of squared distances
		// in order to make this algorithm's result as close as possible to
		// the standard ISOCLUS. 
		average_distances[i]=sqrt(average_distances[i]/clusters[i].size());
	}
}
/************************************************************************************/
/* This function retunrs:                                                           */
/* over all average of squared distances of all points in the sample set form	    */
/* their closest center.		                                            */
/************************************************************************************/

double Image::OverallAverageDistances()
{
	double sum=0;
	int i,size=clusters.size();

	for (i=0; i < size; i++)
		sum+=(clusters[i].size())*average_distances[i];

	OverallD=sum/samples.size();


	return OverallD;
}
/*************************************************************************************/
/* given the position of a point in the image array, we find its coordinate, assuming*/
/* the first points coordinate is (1,1).					     */
/*************************************************************************************/ 
void Image::printCoordinates(int pos)
{
	throw ErrorObject("TODO 1123");
	//*IsoOut<<"( ";
	//if ( (pos % NumCol) !=0)
	//	*IsoOut<<(pos/NumCol)+1;
	//else
	//	*IsoOut<<pos/NumCol;

	//*IsoOut<<" ,";

	//if ( (pos % NumCol) !=0)
	//	*IsoOut<<pos%NumCol;
	//else
	//	*IsoOut<<NumCol; 

	//*IsoOut<<" )"<<endl; 



}
/**************************************************************************************/
/* This function calculated average of sum of *squared* distances of all points from  */
/* their cluster centers.  This function gets called as a basis for comaprison of     */
/* results of this algorithm verses other clustering algorithms that use squared      */
/* distances . This, function will be called at the end of the clustering, and so     */
/* modifications to arrays should not affect performance of the clustering.           */
/**************************************************************************************/
double Image::getDistortions()
{

	return filter->getAvgDist();

}
/*****************************************************************************************/
/* This function calculates a vector for each cluster (a point for each cluster).        */
/* Each coordinate D of a particular  cluster's standard deviation vector is the         */
/* standard deviation of all coordinate D-s of all points in that cluster from coordinate*/  
/* D of the same cluster's center.                                                       */
/*****************************************************************************************/
void Image::CalculateSTDVector()
{

	if (filter!= NULL)
	{
		STDVector=filter->getStdv(true);
	}
	else
	{
		kmError("Image::CalculateSTDVector-Cannot get the standard deviation vector, filter is null",
			KMabort);	
	}

	//int n=clusters.size();
	//cout<<"Printing ******STDV********"<<endl;
	//for (int i=0; i< n; i++)
	//{
	//  cout<<"[ ";
	//  for (int j=0; j< NumBands; j++)
	//    { cout<<STDVector[i][j]<<"  ";}
	//  cout<<endl;
	//}

}
/***************************************************************************/
/* calculates the maximum element in each column of STDVector. Since after */
/* this step we do not need values of STDVector, and each IsoClus iteration*/
/* needs to recalculate STDVector, we will delete the allocated memory for */
/* STDVector at this function as well.					   */
/**************************************************************************/
void Image::CalculateVmax()
{
	int c,b;
	int num_clusters=clusters.size(); 

	if (Vmax !=NULL)
	{
		delete [] Vmax;
		Vmax=NULL;


	}

	if (Vmax_index !=NULL)
	{
		delete [] Vmax_index;
		Vmax_index=NULL;


	}



	Vmax=new double[num_clusters];
	if (!Vmax)
	{
		throw ErrorObject(TR("Memory Allocation for 'Vmax_index' Failed"));
	}

	Vmax_index=new int[num_clusters];
	if (!Vmax_index)
	{
		throw ErrorObject(TR("Memory Allocation for 'Vmax_index' Failed"));
	}


	for (c=0; c < num_clusters; c++)
	{
		Vmax[c]=STDVector[c][0];
		Vmax_index[c]=0;

		for (b=1; b < NumBands; b++)
		{
			if (STDVector[c][b]	> Vmax[c])
			{
				Vmax[c]=STDVector[c][b];
				Vmax_index[c]=b;
			}
		}		


	}

}
/**************************************************************************************/
/* This function evaluates 3 conditions X, A, and B and evaluates the value of:	      */
/* (X and (A or B)) for each cluster.						      */
/* These conditions are described in step 10 of IsoClus algorithm.  See top of        */
/* IsoClus.cc for references.							      */
/* It adds the cluster numbers of all those clusters that satisfied (X and (A or B))  */
/**************************************************************************************/
vector<int> Image::ShouldSplit(double stdv)
{

	vector<int> to_return;  
	bool X=false, A=false, B=false;
	int num_clusters=clusters.size();


	//evaluateing X
	for (int i=0; i < num_clusters; i++)
	{
		if (Vmax[i] > stdv)
		{

			X=true;

		}
		//the goal is to evaluate [ X and (A or B) ]
		if (X)
		{
			//evaluate A
			if( (average_distances[i] > OverallD) && (clusters[i].size() > 2*(NumSamples+1)) )
			{

				A=true;
			}
			if (A)
				to_return.push_back(i);

			else 
			{
				//evaluate B: compares the actual number of clusters with the desired number of
				//clusters.	
				if (num_clusters <= (NumClusters/2))	
				{

					B=true;
				}
				if (B)
					to_return.push_back(i);

			}//else

		} //if (X)

		// reset the booleans for next iteration
		X=false; A=false; B=false;
	} //for loop
	return to_return;
}
/****************************************************************************************/
/* It splits all those clusters whose index appears in 'to_split' parameter.  It does   */
/* so by calculating two new centers.  It assigns one of the centers to the same cluster*/
/* that is being split, and add the other new center to the end of 'centers' array.     */
/****************************************************************************************/
void Image::Split(vector<int> to_split)
{
	int i,size=to_split.size(), j, index, num_centers=centers.size();
	double Gj, current;
	IsoCluster::Point Zminus, Zplus;
	try
	{
		for (j=0; j < size &&  num_centers <MAXCLUS; j++, num_centers++)
		{
			index=to_split[j];
			Gj=K*Vmax[index];

			//Zminus=(*centers[index])-Gj;
			//Zplus=(*centers[index])+Gj;

			Zminus=(*centers[index]);
			Zplus=(*centers[index]);

			//i would be the coordinate of STDV vector which had the maximum 
			//component.

			i=Vmax_index[index];
			current=Zminus.getCoordinate(i);

			Zminus.setCoordinate(i, current-Gj);
			Zplus.setCoordinate (i, current+Gj);


			// this erases the previous center value and updates it by what is called
			// Zplus in in ISOCLUS algorithm step 10, see references in IsoClus.cc

			(*centers[index])=Zminus;
			centers.push_back(new IsoCluster::Point(Zplus));
			//*IsoOut<<"\tSplit cluster "<<index+1<<"."<<endl; 

		} // for

	} //try
	catch ( bad_alloc exception)
	{
	}


}
/********************************************************************************************/
void Image::ComputeCenterDistances()
{
	unsigned int i, j, size, count;
	double dist;
	bool Emptied=false;

	size=centers.size();

	if (size != CenterDistances.size())
	{
		CenterDistances.clear();
		Emptied=true;
	}

	if (size==0 || Emptied)
	{ 
		for (i=0; i < size-1; i++)
			for (j=i+1; j < size ; j++)
			{
				dist=sqrt(centers[i]->Norm2DistanceSquared(centers[j]));
				PairDistanceNode n={dist , i, j};
				CenterDistances.push_back(n);

			}
	}

	else  //just need to update "dist" values rather than allocating all structure nodes
	{
		//note that 'count' is suppose to be the index number of the next element in
		//CenterDistances array. 
		for (i=0, count=0; i < size-1; i++)
			for (j=i+1; j < size ; j++, count++)
			{
				dist=sqrt(centers[i]->Norm2DistanceSquared(centers[j]));
				CenterDistances[count].dist=dist;

			}
	}


	//cout<<" in pair wise distances size was "<<size<<endl;
	//cout<<"number of iterations was "<<count<<endl;

	//for (i=0; i < count; i++)
	//{
	//cout<<i<<": dist= "<<CenterDistances[i].dist<<"  c1: "<<CenterDistances[i].c1<<" c2: 
	//"<<CenterDistances[i].c2<<endl; 


	//}

}

/************************************************************************************/
/* Searches the list of pair distance nodes and selects those pairs whose distances */
/* from each other are less than parameter LUMP.  Orders the list in ascending order*/
/* and selects the first MAXPAIR pairs as candid pairs for lumping.		    */
/************************************************************************************/
vector<PairDistanceNode> Image::FindLumpCandidates(double lump, int MAXPAIR)
{
	int count=0;
	int size=CenterDistances.size();
	vector<PairDistanceNode> lump_candidates;

	// sort the list of center pairs based on '<' operator which is overloaded to compare
	// 'dist' field of each node. (sort based on distances) 
	sort(CenterDistances.begin(), CenterDistances.end());

	for (int i=0; i < size  && lump!=0 && count < MAXPAIR; i++)
	{
		if (CenterDistances[i] <lump)
		{ 

			lump_candidates.push_back(CenterDistances[i]);
			count+=1;
		}
	}

	//If there are more candidates to lump, than the MAXPAIR, select the first MAXPAIR candidates

	if (count > MAXPAIR)
		lump_candidates.erase(lump_candidates.begin()+MAXPAIR, lump_candidates.end());


	return lump_candidates;


}
/**********************************************************************************/
/* Eligible clusters among to_lump vector will be lumped.                         */
/* Each cluster/center can be lumped only once, and thus not all the centers      */
/* associated with elements of to_lump vector will be lumped.                     */ 
/**********************************************************************************/
void Image::Lump(const vector<PairDistanceNode>& to_lump)
{
	int to_lump_size=to_lump.size();
	int orig_centers_size=centers.size();
	int i, clus1_size, clus2_size, clus1, clus2, used_index, count=0;

	try
	{
		bool* used_centers=new bool[orig_centers_size];

		for (i=0; i < orig_centers_size ; i++)
			used_centers[i]=0;


		for (i=0 ; i < to_lump_size ; i++)
		{
			clus1=to_lump[i].c1;
			clus2=to_lump[i].c2;


			// only go about lumping the two clusters if neither of the centers had been used before.
			if (!used_centers[clus1] && !used_centers[clus2] &&  (orig_centers_size-count)> MINCLUS )
			{
				//calculate the new center
				clus1_size=clusters[clus1].size(); 
				clus2_size=clusters[clus2].size();

				IsoCluster::Point new_center=( ( *centers[clus1]) * clus1_size )+ ( (*centers[clus2]) * clus2_size) ;
				new_center= new_center/ (clus1_size + clus2_size);
				centers.push_back(new IsoCluster::Point(new_center));

				//merge the two clusters into a new one, and add the new cluster to the end of vector of clusters.
				vector<int> new_cluster;
				new_cluster.insert(new_cluster.end(), clusters[clus1].begin(), clusters[clus1].end());
				new_cluster.insert(new_cluster.end(), clusters[clus2].begin(), clusters[clus2].end());
				clusters.push_back(new_cluster);

				used_centers[clus1]=1;
				used_centers[clus2]=1;
				//*IsoOut<<"\tLumped clusters "<<clus1+1<<" and "<<clus2+1<<"."<<endl;
				count++;
			} // if could be lumped


		} // for loop



		vector<IsoCluster::Point*>:: iterator centers_it=centers.begin();
		vector< vector <int> >:: iterator clusters_it=clusters.begin();

		for (i=0, used_index=0; used_index < orig_centers_size; i++, centers_it++, clusters_it++, used_index++)
		{
			//if this cluster has been lumped remove it and its center.
			if (used_centers[used_index])   
			{
				clusters.erase(clusters_it);
				delete centers[i];
				centers.erase(centers_it);
				i--; centers_it--; clusters_it--;

			} // if
		} // for
	} //try
	catch ( bad_alloc exception)
	{
		//*IsoErr<<"Exception occured in Image::Lump function: "<<exception.what() <<endl;
		exit(1);
	}


}
/***************************************************************************************/
/* Select all points for the last iterative clustering.                                */
/***************************************************************************************/
void Image::preFinalClustering()
{
	//since it is going to be the last round, we need to classify every single point

	samples.clear();
	for (int i=0; i<ImageSizeInByte; i++)
	{
		samples.push_back(i);
		if (allPoints[i]==NULL)
		{
			allPoints[i]=points_helper(i);
		}
	}
	// since it is the last iteration, build a new KMfilterCenters to consider all points
	// and the latest centers.
	if (filter!=NULL)
	{
		delete filter;
		filter=NULL;
	}
	if (data!=NULL)
	{
		delete data;
		data=NULL;
	}
	BuildKMfilterCenters();
}  
/****************************************************************************************/
/* Print a report of current clustering results. (Number of clusters, their, sizes, etc */
/****************************************************************************************/
void Image::generateReport(ostream* out)
{

	int i, num_clusters=clusters.size();
	*out<<"\n\tNumber of Clusters: "<<clusters.size()<<endl;
	*out<<"\t===============================================================================\n";
	for (i=0; i< num_clusters; i++)
	{
		*out<<"\tCluster "<<i+1<<":\n\tSize: "<<clusters[i].size();
		*out<<"\n\tAverage of squared distances: ";
		*out<<average_distances[i]<<endl;
		*out<<"\tCenter:  ";
		centers[i]->print(out);
		*out<<"\t===============================================================================\n";

	}
	*out<<"\tOverall average of squared distances of all points from their cluster center: "<<OverallD<<endl;

}
/******************************************************************************************/
