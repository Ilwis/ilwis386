#pragma once

#include "map.h"

namespace ILWIS {
template <class T> class Grid {
public:
	Grid(int ySize, int xSize, unsigned long maxBSize=1e7) : maxBlockSize(maxBSize),valid(false){
		elementSize = sizeof(T);
		setSize(ySize,xSize);
	}

	Grid(const Map& mp, unsigned long maxBSize=1e7) : valid(false), maxBlockSize(maxBSize) {
		elementSize = sizeof(T);
		RowCol rc = mp->rcSize();
		setSize(rc.Row, rc.Col);
		FileName fnData(mp->fnObj,".mp#");
		ifstream infile(fnData.sPhysicalPath().c_str(),ifstream::in | ifstream::binary);
		unsigned long blockSize = linesPerBlock * rc.Col * elementSize;
		int count = 0;
		unsigned long szLeft = sz;
		while(true && infile.is_open() && infile.good()) {
			if ( szLeft > blockSize)
				infile.read((char *)values[count],blockSize);
			else {
				infile.read((char *)values[count],szLeft);
				valid = true;
				break;
			}
			++count;
			szLeft -= blockSize;
		}
	}

	virtual ~Grid() {
		clear();
	}

	void store(const FileName& fnObj) {
		FileName fnData fnObj.sExt != ".mp#" ? FileName(fnObj,".mp#") : fnObj;
		ofstream outfile(fnData.sPhysicalPath(), ios::out | ios::binary);
		unsigned long blockSize = sz / noOfBlocks;
		unsigned long szLeft = sz;
		if ( outfile.is_open()) {
			for(int i = 0; i < noOfBlocks; ++i) {
				if ( i == noOfBlocks - 1) {
					outfile.write((char *)values[i], szLeft);
				} else {
					otfile.write((char *)values[count],blockSize);
				}
				szLeft -= blockSize;
			}
		}

	}
	void setSize(int y, int x) {
		sz = y * x * elementSize;
		lineSize = x;
		unsigned long szLeft = sz;
		linesPerBlock = maxBlockSize / ( x * elementSize);
		unsigned long blockSize = linesPerBlock * x * elementSize;
		noOfBlocks = sz / blockSize + 1;
		values = new T*[noOfBlocks];
		int count = 0;
		while(true) {
			if ( szLeft > blockSize)
				values[count] =  new T[blockSize];
			else {
				values[count]  = new T[szLeft];
				valid = true;
				break;
			}
			++count;
			szLeft -= blockSize;
		}
	}

	bool isValid() {
		return valid;
	}

	void clear() {
		for(int i = 0; i < noOfBlocks; ++i)
			delete [] values[i];
		delete [] values;
	}

	T& at(int y, int x) const{
		int block = y / linesPerBlock;
		int rest = y % linesPerBlock;
		return values[block][ rest * lineSize +  x];		
	}

	T* at(int y) {
		return values[ y / linesPerBlock];
	}

private:
	const long maxBlockSize;
	long lineSize;
	T **values;
	int linesPerBlock;
	size_t elementSize;
	int noOfBlocks;
	bool valid;
	unsigned long sz;


};
}