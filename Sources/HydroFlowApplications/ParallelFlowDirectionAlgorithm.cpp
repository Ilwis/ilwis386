#include "ParallelFlowDirectionAlgorithm.h"


int FlowDirectionAlgorithm::noflow = 0;

FlowDirectionAlgorithm::FlowDirectionAlgorithm(Tranquilizer* trq) :
	m_trq(trq),
		flatcell(9), flag(10), 
		increment(1)
{
	//	Location number				Order in m_vDirection
	//	-------						-------	 looping order of the neighbors 	
	//	|6|7|8|						|0|1|2|
	//	-------						-------
	//	|5| |1|						|3|4|5|
	//	-------						-------
	//	|4|3|2|						|6|7|8|
	//	-------						-------
	//

	Location[0] = 6;
	Location[1] = 7;
	Location[2] = 8;
	Location[3] = 5;
	Location[4] = 1;
	Location[5] = 4;
	Location[6] = 3;
	Location[7] = 2;

}

FlowDirectionAlgorithm::Method FlowDirectionAlgorithm::methodValueOf(String val) {
	if (fCIStrEqual(val, "slope")) {
		return FlowDirectionAlgorithm::Method::slope;
	}
	return FlowDirectionAlgorithm::Method::height;
}

/**
 * The approach is formed by the Deterministic-8 flow direction procedure with improvement 
 * for flat surface treatment to prevent occurrence of parallel drainage lines 
 * using the raster digital elevation model.
 * @param dem Two dimensional array holding the raster digital elevation model
 * @param method a string parameter "slope" or "height" to indicate how flow direction should be calculated; slope calculates flow direction as the run off to a neighbor pixel according to steepest downhill slope; height calculates flow direction as the run off to the neighbor pixel which has the lowest height.
 * @return Two dimensional array with flow directions    
 */
vector<ByteBuf> FlowDirectionAlgorithm::calculate(const vector<RealBuf>& demInput, String methodInput) {
		String sl = methodInput.toLower();
		dem = demInput;
		method = methodValueOf(sl);
		
		FlowDirection fd;
		lines = dem.size();
		columns = dem[0].iSize();
		flowdir.resize(lines);
		for (int row = 0; row < lines; ++row) {
			flowdir[row].Size(columns);
		}
		double max;
		m_trq->SetText("Flow direction assignment");
		for (long row=0; row<lines; row++){
			m_trq->fUpdate(row, lines - 1);
			for(int col=0; col<columns; col++){

				if (isEdge(row, col) || dem[row][col] == rUNDEF){
					flowdir[row][col] = noflow; 
				}
				else{
					//Get maximum slope for the specified cell with it's eight neighboring cells
					double listVal[8];
					//vector<double> listVal;// = new vector<double>(8);
					max = maxAdj(row, col, listVal);
					if (max > 0){
						//Finds thw elements with maximum value in the list
						vector<FlowDirection> listPos;
						findDirection(listVal, max, listPos);
						fd = getFlowDirection(listPos);
						flowdir[row][col] = Location[(byte)fd];
					}
					else if (max == rUNDEF)
						flowdir[row][col] = noflow;
					else{  
						flowdir[row][col] = flatcell;
					}	
				}
			}
		}
		
		//Flat surface treatment
		vector<IntBuf> gradient1(lines);
		vector<IntBuf> gradient2(lines);
		for (int v = 0; v < lines; ++v) {
			gradient1[v].Size(columns); 
			gradient2[v].Size(columns); 
			for (int c = 0; c < columns; ++c) {
				gradient1[v][c] = 0;
				gradient2[v][c] = 0;
			}
		}
		m_trq->SetText("Flat surface treatment");
		for (long row=0; row<lines; row++){
			m_trq->fUpdate(row,lines-1);
			for(int col=0; col<columns; col++){
				if (flowdir[row][col] == flatcell){
					vector<Cell> flatList;
					vector<Cell> outletList;
					locateOutlet(row,col, flatList, outletList);
					if (outletList.size() > 0){

				    	imposeGradient2LowerElevation(outletList, flatList, gradient1);
						imposeGradientFromElevation(flatList, gradient2);
						combineGradient(gradient1,gradient2, flatList);
				        assignFlowInFlat(flatList, gradient2);
				        Cell cell;
						for (vector<Cell>::iterator pos = outletList.begin(); 
								 pos < outletList.end(); ++pos)
						{
				        	cell = *pos;
				        	flowdir[cell.row][cell.col] = cell.val;
				        }
				        
				        //Apply flow assignment procedure to those exceptional cells based on gradient1
				        assignFlowInFlat(flatList, gradient1);
				        iniGradient(gradient1, gradient2, flatList);
				    }   
				}
		    }
		}
		for (long row=0; row<lines; row++){
			for(int col=0; col<columns; col++){
				if (flowdir[row][col] > 8)
					flowdir[row][col] = 0;
			}
		}
		m_trq->Stop();
		return flowdir;
	}
	
	bool FlowDirectionAlgorithm::isEven(int elem)
	{
			return elem % 2 == 0;
	}
	
	bool FlowDirectionAlgorithm::isEdge(int row, int col){
		if ((row == 0) || (row == lines-1)||(col == 0 ) || (col == columns - 1)){
			return true;
		}
		return false;
	}
//The slope is calculated by subscribing the neighbor's elevation value from the center
//distance 1.14 is concerned for diagonal cells
//Parameters
//h1 - the elevation of the center cell
//h2 - the elevation of the neighboring cell
//pos - the location number according to flow direction definition in ILWIS 	
//Returns - slope value of the center cell	
	double FlowDirectionAlgorithm::computeSlope(double h1,double h2,int pos){
		double val;
		if (isEven(pos))
			val = h1 - h2;
		else
			val = (h1 - h2)/1.41;
		return val;
	}
	double FlowDirectionAlgorithm::computeHeightDifference(double h1,double h2){
		return h1 - h2;
	}

//Finds the elements with the specified value in the given list
//Parameters
//	listA - the list filled with the slopes with the neighboring cells
//	val - the specified value to be searched in the given list
//  Returns - the list filled with the index of the specified element 	
	void FlowDirectionAlgorithm::findDirection(double listA[], double val,
			vector<FlowDirectionAlgorithm::FlowDirection>& listPos) {
		for (int index = 0; index < 8; ++index) {
			double value = listA[index];
			if (value == val)
				listPos.push_back(mapFlowLocation(index));
		}
	}
	
//Get the maximum value of the adjcent cells. if the elements of a list with the slope values with the eight neighboring cells
//Parameters
//		row - row number of the given cell
//		col - column number of the given cell
//		listVal - the list filled with the slop values against the eight neighboring cells	
//Returns
//		the maximum value     	
	double FlowDirectionAlgorithm::maxAdj(int row, int col, double listVal[]){
		int nrow = 0, ncol = 0, pos=0;
		int index = 0;
		double val=0, rheight, nheight;
		rheight = dem[row][col];
		double max = -1; 
		for(int i=-1; i<=1; ++i){
			nrow = row + i;
			for(int j=-1; j<=1; ++j){
				ncol = col + j;
				pos = pos +1;  
				if (pos != 5){
					if (dem[nrow][ncol] == rUNDEF)
						return rUNDEF;
					nheight = dem[nrow][ncol];
					switch(method){
						case slope:
							val = computeSlope(rheight, nheight,pos);
							break;
						case height:
							val = computeHeightDifference(rheight,nheight);
							break;
					}
					listVal[index] = val;
					if ( val > max)
						max = val;
					index++;
				}
			}	
		}
		return max;
	}	

	double FlowDirectionAlgorithm::maxAdj(int row, int col, vector<IntBuf>& gradient, double listVal[]){
		int nrow = 0, ncol = 0, pos=0;
		int index = 0;
		double val=0, height, nheight;
		height = gradient[row][col];
		double max = -1; 
		for(int i=-1; i<=1; ++i){
			nrow = row + i;
			for(int j=-1; j<=1; ++j){
				ncol = col + j;
				pos = pos +1;  
				if (pos != 5){
					if (dem[nrow][ncol] > dem[row][col])
						val = rUNDEF;
					else{
						nheight = gradient[nrow][ncol];
						val = computeHeightDifference(height,nheight);
					}	
					listVal[index] = val;
					if (val > max)
						max = val;
					index++;
				}	
			}
		}
		return max;
	}	
	
//Examine the flow direction location to perform one of the following:
//---1. if there are only two elements in the given list (maxium slop occurs in two neiboring cells):	
//---2. if there are only two elements in the given list (maxium slop occurs in two neiboring cells):
//make flow direction with S or W or E or N, if such a element exists in the given list
//otherwise, make the flow direction with the first element in the list.
//---3. if there are more than two cells with maximum slop, perform one of the following:
//if three cells located in one edge, make the flow direction value with the middle cell of the edge, otherwise,
//make flow direction with S or W or E or N, if such a element exists in the given list, otherwise
//make the flow direction with the first element in the list 	
//Parameters
//	listPos - the list filled with flow directions
//Returns - flow direction 	
	FlowDirectionAlgorithm::FlowDirection FlowDirectionAlgorithm::getFlowDirection(const vector<FlowDirection>& listPos){
		//vector<FlowDirection>::iterator pos = listPos.iterator();
		if (listPos.size() == 1){ 
			return listPos[0];
		}
		else if (listPos.size() == 2){ //sets it flow witho the center element, if it exists
			int index = 0;
			while(index < 2){
				FlowDirection fd = listPos[index];
				if ((fd == FlowDirection::E) || (fd == FlowDirection::S) || 
					(fd == FlowDirection::W) || (fd == FlowDirection::N)){
					return fd;
				}
				index++;
			}
		}
		else{
			if (isInOneEdge(listPos, FlowDirection::SW, FlowDirection::S, FlowDirection::SE))
				return FlowDirection::S;
			else if (isInOneEdge(listPos, FlowDirection::NW, FlowDirection::W, FlowDirection::SW))
				return FlowDirection::W;
			else if (isInOneEdge(listPos, FlowDirection::NW, FlowDirection::N, FlowDirection::NE))
				return FlowDirection::N;
			else if (isInOneEdge(listPos, FlowDirection::NE, FlowDirection::E, FlowDirection::SE))
				return FlowDirection::E;
			else{
				int index = 0;
				while(index < listPos.size()){
					FlowDirection fd = listPos[index];
					if (fd == FlowDirection::E || fd == FlowDirection::S || 
						fd == FlowDirection::W || fd == FlowDirection::N){
						return fd;
					}
					index++;
				}
			}
		}
		return listPos[0];
	}

//	Examine if the elements of a given list with flow directions are located at one edge
//	Parameters
//		listPos - the list filled with flow directions
//		fd* - the flow direction e.g SW, S, SE represent at one edge 
//	Returns - yes indicates flow directions are in one edge 	
	bool FlowDirectionAlgorithm::isInOneEdge(const vector<FlowDirection>& listPos, FlowDirection fd1, FlowDirection fd2, FlowDirection fd3){

		bool fCondition1 = find(listPos.begin(),listPos.end(), fd1) != listPos.end();
		bool fCondition2 = find(listPos.begin(),listPos.end(), fd2) != listPos.end();
		bool fCondition3 = find(listPos.begin(),listPos.end(), fd3) != listPos.end();
		
		return fCondition1 && fCondition2 && fCondition3;
	}
	
//	Map the flow location number with flow direction
//	Parameters
//		pos - the flow location number as defined in ILWIS
//	Returns - flow direction 	
	FlowDirectionAlgorithm::FlowDirection FlowDirectionAlgorithm::mapFlowLocation(int pos){
		FlowDirection result = FlowDirection::E; 
		switch (pos){
			case 0: result = FlowDirection::NW; 
			  		break;
			case 1: result = FlowDirection::N;
					break;
			case 2:	result = FlowDirection::NE;
					break;
			case 3: result = FlowDirection::W;
					break;	
			case 4: result = FlowDirection::E;
					break;
			case 5: result = FlowDirection::SW;
					break;
			case 6: result = FlowDirection::S;
					break;	
			case 7: result = FlowDirection::SE;
					break;
		}
		return result;
	}
		
//	Locates cells in the flat adjacent to a outlet and add this cell to a list
//	Parameters
//		row - row number of the given cell in a flat
//		col - column number of the given cell in a flat
//		flatList - the list filled with the cells over a flat
//	Returns - a list filled with cells over the flat adjacent to an outlet cell 	
	void FlowDirectionAlgorithm::locateOutlet(int row, int col, vector<Cell>& flatList, vector<Cell>& outList){
		int nrow, ncol;
		Cell cell(row, col);
		vector<Cell> srcList;
		vector<Cell> desList;
		flowdir[row][col] = flag;
		double flatCellElevation = dem[row][col];
		srcList.push_back(cell);
		flatList.push_back(cell);
		do{
			desList.resize(0);
			for (vector<Cell>::iterator pos = srcList.begin();pos < srcList.end(); ++pos)
			{
				cell = *pos;
				row = cell.row; col = cell.col;
				for(int i=-1; i<=1; ++i){
					nrow = row + i;
					for(int j=-1; j<=1; ++j){
						ncol = col + j;
						if (flowdir[nrow][ncol] != flag && !(isEdge(nrow, ncol)) ){
							if (dem[nrow][ncol] == flatCellElevation && hasFlow(flowdir[nrow][ncol])){
								cell.setRC(nrow, ncol);
								cell.val = flowdir[nrow][ncol];
								flowdir[nrow][ncol] = flag;
								outList.push_back(cell);
								desList.push_back(cell);
							}
							else if (dem[nrow][ncol] == flatCellElevation){
								cell.setRC(nrow, ncol);
								flowdir[nrow][ncol] = flag;
								desList.push_back(cell);
								flatList.push_back(cell);
							}
						}
					}
				}
			}
			srcList.swap(desList);
		} while(srcList.size() > 0);	
	}
//	Impose gradient in flat to lower elevation. This incrementation starts with adjacent cells to existing gradient,   
//	by imposing a downslope gradient. The procedure then is re-applied to all the remainder cells.
//	This is repeated untill all cells have a downstream gradient. 
//	Parameters
//		flow - an array with flow direction
//		outletList - the list filled with the cells adjacent to the outlets
//		flatList - the list filled with the cells in a flat surface	
//	Returns - an array filled with gradient to lower elevation in a flat 	
	void FlowDirectionAlgorithm::imposeGradient2LowerElevation(vector<Cell>& outletList, vector<Cell>& flatList, vector<IntBuf>& gradient){
		int nrow, ncol, row, col;
		Cell cell(0,0);
		vector<Cell> srcList;
		for (vector<Cell>::iterator pos = outletList.begin();pos < outletList.end(); ++pos) {
			srcList.push_back((*pos));
		}
		vector<Cell> desList;
		do{
			for (vector<Cell>::iterator pos = srcList.begin();pos < srcList.end(); ++pos) {
				cell = (*pos);
				row = cell.row; col = cell.col;
				flowdir[row][col] = flatcell;
			}
			
			for (vector<Cell>::iterator pos = flatList.begin();pos < flatList.end(); ++pos) {
				//int index = li.nextIndex();
				cell = (*pos);
				if (flowdir[cell.row][cell.col] == flag){
					gradient[cell.row][cell.col] += increment;
					//gradient[index] += increment;
				}
			}
			for (vector<Cell>::iterator pos = srcList.begin(); pos < srcList.end(); ++pos) {
				cell = (*pos);
				row = cell.row; col = cell.col;
				for(int i=-1; i<=1; ++i){
					nrow = row + i;
					for(int j=-1; j<=1; ++j){
						ncol = col + j;
						if (flowdir[nrow][ncol] == flag ){
							cell.setRC(nrow,ncol);
							desList.push_back(cell);
							flowdir[nrow][ncol] = flatcell;
						}
					}
				}
			}
			srcList.swap(desList);
			desList.resize(0);
		}while(srcList.size() > 0);	
		srcList.resize(0);
	}
//	Impose gradient away from higher elevation. 
//	Parameters
//		flow - an array with flow direction
//		outletList - the list filled with the cells adjacent to the outlets
//	Returns - an array filled with gradient to lower elevation in a flat 	
	void FlowDirectionAlgorithm::imposeGradientFromElevation(vector<Cell>& flatList, vector<IntBuf>& gradient){
		int row, col;
		Cell cell(0,0);
		boolean condition1 = false, condition2 = false;
		do{
			condition2 = false;
			vector<Cell> flagList;
			for (vector<Cell>::iterator pos = flatList.begin(); 
					pos < flatList.end(); ++pos) {
				cell = (*pos);
				row = cell.row; col = cell.col;
				if (flowdir[row][col] != flag){
					condition1 = true;
					condition2 = true;
					for(int i=-1; i<=1; ++i){
						int nrow = row + i;
						for(int j=-1; j<=1; ++j){
							int ncol = col + j;
							//if (((flowdir[nrow][ncol] != flatcell) && (dem[row][col] != dem[nrow][ncol])) || 
							//	(flowdir[nrow][ncol] == flag)){
							if ((flowdir[nrow][ncol] != flatcell) || (flowdir[nrow][ncol] == flag)){
								flagList.push_back(cell);
								condition1 = false;
								break;
							}
						}
						if (condition1 == false) 
							break;
					}
				}	
			}
			
			for (vector<Cell>::iterator pos = flagList.begin(); 
					pos < flagList.end(); ++pos) {
				cell = (*pos);
				flowdir[cell.row][cell.col] = flag;
			}
			
			if (flagList.size() > 0){
				for ( vector<Cell>::iterator pos = flatList.begin(); 
						pos < flatList.end(); ++pos) {
					cell = (*pos);
					if(flowdir[cell.row][cell.col] == flag){
						gradient[cell.row][cell.col] += increment; 
					}
				}
			}
			condition2 = flagList.size() > 0;
		}while(condition2);	
		//return gradient;
	}
//	Combine gradient in the previous two steps, 
//	imposeGradient2LowerElevation and imposeGradientFromElevation
//	Parameters
//		grds - an array resulting from the previous step - imposeGradientFromElevation
//		flatList - the list filled with the cells in a flat surface 
	void FlowDirectionAlgorithm::combineGradient(const vector<IntBuf>& grd1, vector<IntBuf>& grd2, vector<Cell>& flatList){
		Cell cell;
		for (vector<Cell>::iterator pos = flatList.begin(); 
				pos < flatList.end(); ++pos) {
			cell = (*pos);
			grd2[cell.row][cell.col] += grd1[cell.row][cell.col]; 
		}
	}

	/*Assign flow direction based on D-8 approach using the result from the previous step combineGradient   
	* @param flatList - the list filled with the cells in a flat surface
	*/
	void FlowDirectionAlgorithm::assignFlowInFlat(vector<Cell>& flatList, vector<IntBuf>& gradient){
		Cell cell;
		for (vector<Cell>::iterator pos = flatList.begin(); 
				pos < flatList.end(); ++pos) {
			cell = (*pos);
			if (hasFlow(flowdir[cell.row][cell.col]) != true){
				double listVal[8];
				double max = maxAdj(cell.row, cell.col, gradient, listVal);
				//double max = Collections.max(listVal);
				if(max > 0){
					vector<FlowDirection> listPos;
					findDirection(listVal, max, listPos);
					FlowDirection fd = getFlowDirection(listPos);
					flowdir[cell.row][cell.col] = Location[(byte)fd];
				}
			}	
		}
	}
	
	bool FlowDirectionAlgorithm::hasFlow(byte flowdirection){
		if (flowdirection >= 1 && flowdirection <= 8)
			return true;
		else
			return false;
	}
	
	void FlowDirectionAlgorithm::iniGradient(vector<IntBuf>& grd1, vector<IntBuf>& grd2, vector<Cell>& flatList){
		Cell cell;
		for (vector<Cell>::iterator pos = flatList.begin(); 
				pos < flatList.end(); ++pos) {
			cell = (*pos);
			grd1[cell.row][cell.col] = 0;
			grd2[cell.row][cell.col] = 0;
		}	
	}
	
