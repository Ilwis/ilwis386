#pragma once

#include "map.h"

namespace ILWIS {
template <class T> class Grid {
public:
	Grid(int ySize, int xSize, unsigned long maxBSize=1e8) : maxBlockSize(maxBSize) : valid(false){
		elementSize = sizeof(T);
		setSize(ySize,xSize);
	}

	Grid(const Map& mp) : valid(false) {
		RowCol rc = mp->rcSize();
		setSize(rc.Row, rc.Col);
		FileName fnData(mp->fnObj,".mp#");
		ifstream infile(fnData.sPhysicalPath());
		int noOfBunsigned long sz = rc.Row * rc.Col * elementSize;
		int count;
		while(true && infFile.is_open() && inFile.isGood()) {
			if ( sz > blockSize)
				infile.read((char *)values[count],blockSize);
			else {
				infile.read((char *)values[count],sz);
				valid = true;
				break;
			}
			++count;
			sz -= blockSize;
		}
	}

	virtual ~Grid() {
		clear();
	}

	void setSize(int y, int x) {
		unsigned long sz = xSize * ySize * elementSize;
		int linesPerBlock = maxBlockSize / ( xSize * elementSize);
		unsigned long blockSize = linesPerBlock * xSize * elementSize;
		noOfBlocks = sz / blockSize + 1;
		values = new T*[noOfBlocks];
		int count = 0;
		while(true) {
			if ( sz > blockSize)
				values[count] =  new T[blockSize];
			else {
				values[count]  = new T[sz];
				valid = true;
				break;
			}
			++count;
			sz -= blockSize;
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

	T& at(int y, int x){
		values[ y / linesPerBlock] + elementSize * x;		
	}

	T* at(int y) {
		values[ y / linesPerBlock];
	}

private:
	const long maxBlockSize;
	T **values;
	int linesPerBlock;
	size_t elementSize;
	int noOfBlocks;
	bool valid;


};
}