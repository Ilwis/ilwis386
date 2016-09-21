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
Bas Retsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

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
// LCMFlockViz.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////

#include "Headers\toolspch.h"
#include "LCMFlockViz.h"
#include "Pair.h"
#include "D2Node.h"
#include "key.h"

#include <math.h>
#include <time.h>
#include <list>

LCMFlockViz::LCMFlockViz()
: precision(0.001)
{
	cid = 1;
}

void LCMFlockViz::getGrid(vector<Point2d> & points, map<int, map<int, vector<Point2d>>> & grid, double epsilon) const {
	for (vector<Point2d>::iterator point = points.begin(); point != points.end(); ++point) {
		int i = (int) (point->getX() / epsilon);
		int j = (int) (point->getY() / epsilon);
		grid[i][j].push_back(*point);
	}
}

const DiskCentres LCMFlockViz::calculateDisks(const Point2d & p1, const Point2d & p2, const double r2) const {
	double p1_x = p1.getX();
	double p1_y = p1.getY();
	double p2_x = p2.getX();
	double p2_y = p2.getY();

	double X = p1_x - p2_x;
	double Y = p1_y - p2_y;
	double D2 = X * X + Y * Y;
		// Check if the two points are the same (Measure or resample errors)
	if (D2 == 0)
		return DiskCentres();

	double expression = 4.0 * (r2 / D2) - 1.0;
	double root = sqrt(expression);
	double h_1 = ((X + Y * root) / 2.0) + p2_x;
	double h_2 = ((X - Y * root) / 2.0) + p2_x;
	double k_1 = ((Y - X * root) / 2.0) + p2_y;
	double k_2 = ((Y + X * root) / 2.0) + p2_y;

	return DiskCentres(Point2d(h_1, k_1), Point2d(h_2, k_2));
}

struct ltDisk {
    bool operator () (Disk* const & a, Disk* const & b) {
        return (*a < *b) ;
    }
};

void LCMFlockViz::getDisks(map<int, map<int, vector<Point2d>>> & grid, unsigned int mu, double epsilon, int time, map<int, vector<int>> & database, vector<DiskInfo> & dbdisks) {
	const double r = (epsilon / 2.0) + precision;
	const double r2 = (epsilon / 2.0) * (epsilon / 2.0);
	set<Pair> computedPairs;
	D2Node * kdtree = 0;
	vector<Key> diskCoordinates;
	for (map<int, map<int, vector<Point2d>>>::iterator gridXIterator = grid.begin(); gridXIterator != grid.end(); ++gridXIterator) {
		int gridXPos = gridXIterator->first;
		map<int, vector<Point2d>> & gridX = gridXIterator->second;
		for (map<int, vector<Point2d>>::iterator gridYIterator = gridX.begin(); gridYIterator != gridX.end(); ++gridYIterator) {
			int gridYPos = gridYIterator->first;

			vector<Point2d> pointsInSubgrid;
			int xMin = gridXPos - 1;
			int xMax = gridXPos + 1;
			int yMin = gridYPos - 1;
			int yMax = gridYPos + 1;
			for (int x = xMin; x <= xMax; ++x) {
				map<int, map<int, vector<Point2d>>>::iterator & gridXLoop = grid.find(x);
				if (gridXLoop != grid.end()) {
					for (int y = yMin; y <= yMax; ++y) {
						map<int, vector<Point2d>>::iterator & gridYLoop = gridXLoop->second.find(y);
						if (gridYLoop != gridXLoop->second.end())
							pointsInSubgrid.insert(pointsInSubgrid.end(), gridYLoop->second.begin(), gridYLoop->second.end());
					}
				}
			}
			if (pointsInSubgrid.size() < mu)
				continue;
			vector<Point2d> & pointsInCell = gridYIterator->second;
			for (vector<Point2d>::iterator & pointInCell = pointsInCell.begin(); pointInCell != pointsInCell.end(); ++pointInCell) {
				vector<Point2d> range;
				for (vector<Point2d>::iterator & pointInSubgrid = pointsInSubgrid.begin(); pointInSubgrid != pointsInSubgrid.end(); ++pointInSubgrid) {
					// Do not compare with the same point
					// pointInCell will not be part of range
					if (pointInCell->getUserData() == pointInSubgrid->getUserData())
						continue;
					if (pointInCell->isWithinDistance(*pointInSubgrid, epsilon))
						range.push_back(*pointInSubgrid);
				}
				// Points in range plus pointInCell
				if ((range.size() + 1) >= mu) {
					int p1 = pointInCell->getUserData();
					for (vector<Point2d>::iterator & pointInSubgrid = range.begin(); pointInSubgrid != range.end(); ++pointInSubgrid) {
						int p2 = pointInSubgrid->getUserData();
						if (computedPairs.insert(Pair(p1, p2)).second) { // insert(Pair(p1, p2)).second == true if the element is inserted, false if it was already inside
							DiskCentres centres (calculateDisks(*pointInCell, *pointInSubgrid, r2));
							if (!centres.fValid)
								continue;
							// Counting points in each disk
							Disk * disk1 = new Disk(centres.diskCentre1);
							disk1->addPoint(*pointInCell);
							Disk * disk2 = new Disk(centres.diskCentre2);
							disk2->addPoint(*pointInCell);
							for (vector<Point2d>::iterator & pointInRange = range.begin(); pointInRange != range.end(); ++pointInRange) {
								if (disk1->isWithinDistance(*pointInRange, r))
									disk1->addPoint(*pointInRange);
								if (disk2->isWithinDistance(*pointInRange, r))
									disk2->addPoint(*pointInRange);
							}
							if (disk1->count >= mu) {
								Key key (disk1->getX(), disk1->getY());
								if (0 == kdtree)
									kdtree = new D2Node(key, disk1);
								else if (kdtree->insert(key, disk1, 0)) // Prune identical centres
									diskCoordinates.push_back(key);
								else
									delete disk1;
							} else
								delete disk1;
							if (disk2->count >= mu) {
								Key key (disk2->getX(), disk2->getY());
								if (0 == kdtree)
									kdtree = new D2Node(key, disk2);
								else if (kdtree->insert(key, disk2, 0)) // Prune identical centres
									diskCoordinates.push_back(key);
								else
									delete disk2;
							} else
								delete disk2;
						}
					}
				}
			}
		}
	}

	set<Disk*, ltDisk> maximalDisks;

	for (vector<Key>::iterator & key = diskCoordinates.begin(); key != diskCoordinates.end(); ++key) {
		// get Points in range of each key
		Key lo (key->keys[0] - epsilon, key->keys[1] - epsilon );
		Key hi (key->keys[0] + epsilon, key->keys[1] + epsilon );

		vector<Disk*> disks;
		if (kdtree !=0)
			kdtree->search(lo, hi, 0, disks);

		int size = disks.size();
		for (int i = 0; i < size - 1; i++) {
			Disk *& disk1 = disks[i];
			if (disk1->isSubset())
				continue;
			set<int> & members1 = disk1->pointsIDs;
			for (int j = i + 1; j < size; ++j) {
				Disk *& disk2 = disks[j];
				if (disk2->isSubset())
					continue;
				set<int> & members2 = disk2->pointsIDs;
				if (std::includes(members1.begin(), members1.end(), members2.begin(), members2.end())) {
					disk2->setSubset(true);
					maximalDisks.erase(disk2);
					//Key rkey (disk2->getX(), disk2->getY());
					//diskCoordinates.erase(find(diskCoordinates.begin(), diskCoordinates.end(), rkey));
					if (kdtree != 0)
						kdtree->del(Key(disk2->getX(), disk2->getY()), 0);
				} else if (std::includes(members2.begin(), members2.end(), members1.begin(), members1.end())) {
					disk1->setSubset(true);
					maximalDisks.erase(disk1);
					//Key rkey (disk1->getX(), disk1->getY());
					//diskCoordinates.erase(find(diskCoordinates.begin(), diskCoordinates.end(), rkey));
					if (kdtree != 0)
						kdtree->del(Key(disk1->getX(), disk1->getY()), 0);							
					break;
				}
			}
		}
		for (vector<Disk *>::iterator & disk = disks.begin(); disk != disks.end(); ++disk) {
			if (!(*disk)->isSubset())
				maximalDisks.insert(*disk);
		}
	}
	for (set<Disk*, ltDisk>::iterator & disk = maximalDisks.begin(); disk != maximalDisks.end(); ++disk) {
		DiskInfo di ((*disk)->points, time);
		dbdisks.push_back(di);
		for (vector<Point2d>::iterator & point = (*disk)->points.begin(); point != (*disk)->points.end(); ++point) {
			int id = point->getUserData();
			database[id].push_back(cid);
		}
		++cid;
	}
	delete kdtree;
}

Point2d getCentroid(vector<Point2d> & points) {
	double x = points[0].getX();
	double y = points[0].getY();
	for (unsigned int i = 1; i < points.size(); ++i) {
		x += points[i].getX();
		y += points[i].getY();
	}
	x /= (double) points.size();
	y /= (double) points.size();
	return Point2d(x, y);
}

std::string getPointsString(vector<int> & pointIDs) {
	char buf [256];
	std::string ret = "[";
	for (unsigned int i = 0; i < pointIDs.size() - 1; ++i) {
		sprintf(buf, "%d, ", pointIDs[i]);
		ret += buf;
	}
	sprintf(buf, "%d]", pointIDs[pointIDs.size() - 1]);
	ret += buf;
	return ret;
}

std::string createLineString(vector<Point2d> & points) {
	char buf [256];
	std::string ret = "LINESTRING (";
	for (unsigned int i = 0; i < points.size() - 1; ++i) {
		sprintf(buf, "%lf %lf, ", points[i].getX(), points[i].getY());
		ret += buf;
	}
	sprintf(buf, "%lf %lf)", points[points.size() - 1].getX(), points[points.size() - 1].getY());
	ret += buf;
	return ret;
}

void doLCM(int LCM_th, vector<vector<int>> & items, vector<vector<int>> & solutions, vector<int> & counts);

int LCMFlockViz::checkFlocks(unsigned int mu, unsigned int delta, vector<vector<Coord>> & flocks, map<int, vector<int>> & flockIDs, vector<vector<int>> & trajectoryIDs, Tranquilizer & trq) {
	trq.SetText(TR("Initializing LCM"));
	int numFlock = 0;
	vector<vector<int>> items;
	for (map<int, vector<int>>::iterator & disks = database.begin(); disks != database.end(); ++disks) {
		vector<int> & disk = disks->second;
		if (disk.size() != 1)
			items.push_back(disk);
	}
	vector<vector<int>> solutions;
	vector<int> counts;
	trq.SetText(TR("Performing LCM"));
	doLCM(mu, items, solutions, counts);

	/*
	Database database;
	database.ReadData(items);
	// set LCM parameter
	int min_sup = mu;
	int max_pat = 0;
	Lcm lcm(cout, min_sup, max_pat);
	lcm.RunLcm(database, solutions, counts);
	*/

	trq.SetText(TR("Checking Flocks"));

	int fid = 1;
	unsigned int solutions_size = solutions.size();
	for (unsigned int solution = 0; solution < solutions_size; ++solution) {
		if (solutions[solution].size() < delta) // 3) // ?? what is this condition for? shouldn't 3 be "mu"?
			continue;
		if (fid % 100 == 0)
			if (trq.fUpdate(fid, solutions_size))
				return numFlock;

		int frecuency = counts[solution];
		vector<DiskInfo> aux;
		for (vector<int>::iterator cid2 = solutions[solution].begin(); cid2 != solutions[solution].end(); ++cid2) {
			DiskInfo & di = dbdisks[*cid2 - 1];
			aux.push_back(di);
		}

		sort(aux.begin(), aux.end());

		vector<int> finalPoints = aux[0].getPointIDs();
		vector<vector<Point2d>> arraypoints;
		arraypoints.push_back(aux[0].getPoints());
		int begin = aux[0].getTime();
		int end = begin;
		int limit = aux.size();
		for (int i = 1; i < limit; ++i) {
			int start = aux[i].getTime();
			if ((start == end + 1) || (start == end)) {
				if (start == end + 1)
					arraypoints.push_back(aux[i].getPoints());
				end = start;
				if (finalPoints.size() != frecuency) {
					vector<int> & ptIDs = aux[i].getPointIDs();
					std::sort(finalPoints.begin(), finalPoints.end());
					std::sort(ptIDs.begin(), ptIDs.end());
					vector<int> retained;
					set_intersection(finalPoints.begin(), finalPoints.end(), ptIDs.begin(), ptIDs.end(), inserter(retained, retained.end())); // Java retainAll
					finalPoints = retained;
				}
			} else if ((end - begin) >= (delta - 1)) {
				printf("\n%d(%d):\n", fid, finalPoints.size());
				printf("From time %d to %d\n", begin, end);
				std::string finalPointsString = getPointsString(finalPoints);
				printf("Members: %s\n", finalPointsString.c_str());

				vector<Point2d> centroids;
				vector<Point2d> auxpoints (finalPoints.size(), Point2d(0, 0));
				vector<Coord> flock;
				int time = begin;
				for (vector<vector<Point2d>>::iterator apoints = arraypoints.begin(); apoints != arraypoints.end(); ++apoints) {
					unsigned int n = 0;
					for (vector<int>::iterator id = finalPoints.begin(); id != finalPoints.end(); ++id) {
						for (vector<Point2d>::iterator point = apoints->begin(); point != apoints->end(); ++point) {
							if (point->getUserData() == *id) {
								auxpoints[n++] = (*point);
								break;
							}
						}
					}
					Point2d centroid = getCentroid(auxpoints); // TODO !!
					centroids.push_back(centroid);
					flock.push_back(Coord(centroid.getX(), centroid.getY(), time++));
				}
				std::string flockLine = createLineString(centroids);
				printf("%d\t%d\t%d\t%s\t%d\t%s\n", numFlock, begin, end, finalPointsString.c_str(), finalPoints.size(), flockLine.c_str());
				if (flock.size() > 0) {
					flocks.push_back(flock);
					trajectoryIDs.push_back(finalPoints);
					for (vector<int>::iterator id = finalPoints.begin(); id != finalPoints.end(); ++id)
						flockIDs[*id].push_back(numFlock);
				}

				++numFlock;
				begin = end = start;
				finalPoints = aux[i].getPointIDs();
				arraypoints.clear();
				arraypoints.push_back(aux[i].getPoints());
			} else {
				begin = end = start;
				finalPoints = aux[i].getPointIDs();
				arraypoints.clear();
				arraypoints.push_back(aux[i].getPoints());
			}
		}
		if ((end - begin) >= (delta - 1)) {
			printf("\n%d(%d):\n", fid, finalPoints.size());
			printf("From time %d to %d\n", begin, end);
			std::string finalPointsString = getPointsString(finalPoints);
			printf("Members: %s\n", finalPointsString.c_str());

			vector<Point2d> centroids;
			vector<Point2d> auxpoints (finalPoints.size(), Point2d(0, 0));
			vector<Coord> flock;
			int time = begin;
			for (vector<vector<Point2d>>::iterator apoints = arraypoints.begin(); apoints != arraypoints.end(); ++apoints) {
				unsigned int n = 0;
				for (vector<int>::iterator id = finalPoints.begin(); id != finalPoints.end(); ++id) {
					for (vector<Point2d>::iterator point = apoints->begin(); point != apoints->end(); ++point) {
						if (point->getUserData() == *id) {
							auxpoints[n++] = (*point);
							break;
						}
					}
				}
				Point2d centroid = getCentroid(auxpoints); // TODO !!
				centroids.push_back(centroid);
				flock.push_back(Coord(centroid.getX(), centroid.getY(), time++));
			}
			std::string flockLine = createLineString(centroids);
			printf("%d\t%d\t%d\t%s\t%d\t%s\n", numFlock, begin, end, finalPointsString.c_str(), finalPoints.size(), flockLine.c_str());
			if (flock.size() > 0) {
				flocks.push_back(flock);
				trajectoryIDs.push_back(finalPoints);
				for (vector<int>::iterator id = finalPoints.begin(); id != finalPoints.end(); ++id)
					flockIDs[*id].push_back(numFlock);
			}
			++numFlock;
		}
		++fid;
	}
	trq.fUpdate(solutions_size, solutions_size);
	return numFlock;
}

unsigned int LCMFlockViz::readPoints(char * filename, map<int, vector<Point2d>> & timestamps)
{
  FILE *fp = fopen(filename,"r+");  
  if(!fp)
  {
	  printf("file open error\n");
  }

  unsigned int nrPoints = 0;

  const int BUF_SIZE = 4096;
  char * buf = (char*)malloc (BUF_SIZE);
  char ch;
  char * ptr = buf;
  size_t sz = 0;
  do
  {
	  sz = fread(&ch, 1, 1, fp);
	  if (sz == 0 || ch == '\n')
	  {
		  *ptr = 0;
		  int id;
		  int time;
		  double x;
		  double y;
		  if (4 == sscanf(buf, "%d %d %lf %lf", &id, &time, &x, &y)) {
			Point2d point (x, y);
			point.setUserData(id);
			timestamps[time].push_back(point);
			++nrPoints;
		  }
		  ptr = buf;
	  } else
	  {
		  *ptr = ch;
		  ++ptr;
	  }
  } while (sz > 0);
  fclose ( fp );
  free ( buf );
  return nrPoints;
}

void LCMFlockViz::flockFinder(const map<int, vector<Point2d>> & timestamps, unsigned int mu, double epsilon, unsigned int delta, Tranquilizer & trq) {
	clock_t start = clock();
	clock_t totaltimedisks = 0;
	cid = 1;
	database.clear();
	dbdisks.clear();
	unsigned int jobsize = timestamps.size();
	unsigned int jobnr = 0;
	trq.SetText(TR("Computing Disks"));

	for(map<int, vector<Point2d>>::const_iterator itTimes = timestamps.begin(); itTimes != timestamps.end(); ++itTimes) {
		if (trq.fUpdate(jobnr++, jobsize))
			return;
		int time = itTimes->first;
		vector<Point2d> pointset = itTimes->second;
		map<int, map<int, vector<Point2d>>> grid;
		getGrid(pointset, grid, epsilon);
		start = clock();
		getDisks(grid, mu, epsilon, time, database, dbdisks);
		clock_t end = clock();
		totaltimedisks += end - start;
	}
	start = clock();
	trq.fUpdate(jobsize, jobsize);
}

