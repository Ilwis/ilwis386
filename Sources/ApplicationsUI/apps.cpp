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
along with this program (see gnu-gpl v2.txt)); if not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA or visit the web page of the Free
Software Foundation, http://www.fsf.org.

Created on: 2007-02-8
***************************************************************/
/* Forms for Interactive "Applications"
Copyright Ilwis System Development ITC
september 1996, by Wim Koolhoven
Last change:  WK   17 Sep 98    2:14 pm
*/


#include "Client\Headers\AppFormsPCH.h"
#include "ApplicationsUI\frmmapap.h"
#include "ApplicationsUI\frmmapcl.h"
#include "ApplicationsUI\frmmapip.h"
#include "ApplicationsUI\frmmapip.h"
#include "ApplicationsUI\frmmatap.h"
#include "ApplicationsUI\frmtblap.h"
#include "ApplicationsUI\frmUniqueId.h"
#include "ApplicationsUI\frmappia.h"
#include "ApplicationsUI\frmstereopaircr.h"
#include "ApplicationsUI\FormMaplistGraph.h"
#include "ApplicationsUI\FormMapAggregateMapList.h"
#include "ApplicationsUI\FormMapListChangeDetection.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Table\tblinfo.h"
#include "Client\TableWindow\TableDoc.h"
#include "ApplicationsUI\Tblforms.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "ApplicationsUI\TableCreateTimeColumn.h"
#include "Client\ilwis.h"
#include "Headers\Hs\Applications.hs"

extern "C" _export InfoUIVector* getCommandInfoUI(ILWIS::Module *module) {
	InfoUIVector *infosui = new InfoUIVector();
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String hyrdoDemBase = ilwDir + "Scripts\\Hydro-DEM\\";

(*infosui).push_back(CommandHandlerUI::createCommandInfo("changedetect","",Cmdchangedetection,TR("&Raster Operations..&Change detection"),TR("Change Detection"),".mpl",".mpl",0,TR("Detect changes in a maplist with baseline data")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("aggregatemaplist","",Cmdaggregatemaplist,TR("&Raster Operations..&Aggregate Maplist"),TR("Aggregate Maplist"),"ExeMap16IcoL",".mpl",0,TR("Aggregate a maplist into a single map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("apply3d","",Cmdapply3d, TR("Visuali&zation..&Apply 3D"),TR("Apply 3D"),"ExeMap16IcoL","",4102,TR("Apply a 3D georeference on a map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("mapcalc","",Cmdmapcalc,TR("&Raster Operations..&Map Calculation"),TR("Map Calculation"),"ExeMap16IcoL","",4022,TR("Perform a calculation with raster maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("maplistcalc","",Cmdmaplistcalc,TR("&Raster Operations..&MapList Calculation"),TR("MapList Calculation"),".mpl","",4022,TR("Perform a calculation with MapLists")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("attribras","",Cmdattribras,TR("&Raster Operations..&Attribute Map"),TR("Attribute Map of Raster Map"),"ExeMap16IcoL",".mpr",4002,TR("Create an attribute map from a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("cross","",Cmdcross,TR("&Raster Operations..&Cross"),TR("Cross"),"ExeTbl16IcoL",".mpr",4202,TR("Cross two raster maps: create a cross map and/or a cross table")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("aggregate","",Cmdaggregate,TR("&Raster Operations..&Aggregate Map"),TR("Aggregate Map"),"ExeMap16IcoL",".mpr.mpl",4192,TR("Aggregate a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("distance","",Cmddistance,TR("&Raster Operations..&Distance Calculation"),TR("Distance Calculation"),"ExeMap16IcoL",".mpr",4052,TR("Perform a distance calculation: create a distance map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("effectdistance","",Cmdeffectdistance,TR("&Raster Operations..&Effect Distance"),TR("Effect Distance"),"ExeMap16IcoL",".mpr",4052,TR("Generate effect distance map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("mapiter","",Cmdmapiter,TR("&Raster Operations..&Iteration"),TR("Iteration"),"ExeMap16IcoL",".mpr",4912,TR("Perform a map iteration: repeating the same calculation")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("areanumb","",Cmdareanumb,TR("&Raster Operations..Area &Numbering"),TR("Area Numbering"),"ExeMap16IcoL",".mpr",4012,TR("Perform an area numbering: assign distinct codes")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("subras","",Cmdsubras,TR("&Spatial Reference Operations.&Raster.&Sub Map"),TR("SubMap of Raster Map"),"ExeMap16IcoL",".mpr.mpl",4162,TR("Copy part of a raster map into a new map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("glueras","",Cmdglueras,TR("&Raster Operations..&Glue Maps"),TR("Glue Raster Maps"),"ExeMap16IcoL",".mpr",4072,TR("Glue two raster maps into one raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("mirror","",Cmdmirror,TR("&Spatial Reference Operations.&Raster.Mirror &Rotate"),TR("Mirror Rotate"),"ExeMap16IcoL",".mpr.mpl",4082,TR("Mirror or rotate a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("filter","",Cmdfilter,TR("Image &Processing..&Filter"),TR("Filter"),"ExeMap16IcoL",".mpr.fil.mpl",4062,TR("Filter a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("stretch","",Cmdstretch,TR("Image &Processing..&Stretch"),TR("Stretch"),"ExeMap16IcoL",".mpr.mpl",4152,TR("Stretch a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("slicing","",Cmdslicing,TR("Image &Processing..S&licing"),TR("Slicing"),"ExeMap16IcoL",".mpr",4142,TR("Slice/Classify a raster map according to a domain Group")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("colorsep","",Cmdcolorsep,TR("Image &Processing..Color Separation"),TR("Color Separation"),"ExeMap16IcoL",".mpr",4182,TR("Extract one band from a color or picture map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("mapcolorcomp","clrcmp",OpenMapListColorComp,"Image &Processing..Color Composite","Color Composite","ExeMap16IcoL","",4222,"Create a color composite of three image bands"));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("cluster","",Cmdcluster,TR("Image &Processing..Cluster"),TR("Cluster"),"ExeMap16IcoL",".mpr",4212,TR("Perform automatic classification of values from multiple maps into clusters")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("classify","clsfy",Cmdclassify,TR("Image &Processing..&Classify"),TR("Classify"),"ExeMap16IcoL",".sms.clf",4032,TR("Perform an image classification according to training pixels in a sample set")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("resample","",Cmdresample,TR("&Spatial Reference Operations.&Raster.&Resample"),TR("Resample"),"ExeMap16IcoL",".mpr.grf.mpl",4092,TR("Resample a raster map to another georeference")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("makestpfromdtm","",Cmdmakestpfromdtm,TR("Image &Processing..&Stereo Pair From DTM"),TR("Stereo Pair From DTM"),".stp",".mpr",6102,TR("Create a stereo pair from an image (or photo) and a DTM")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("histogram","hist",Cmdhistogram,TR("&Statistics..&Histogram"),TR("Histogram"),"ExeTbl16IcoL",".mpr.mpa.mps.mpp",5205,TR("Calculate the histogram of a raster or vector map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("autocorr","",Cmdautocorr,TR("&Statistics.&Raster.&Autocorrelation - Semivariance"),TR("Autocorrelation - Semivariance"),"ExeTbl16IcoL",".mpr",5002,TR("Calculate autocorrelation and create a semivariogram of a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("variogramsurface","",Cmdvariogramsurface,TR("&Statistics.&Raster.&Variogram Surface"),TR("Variogram Surface"),"ExeMap16IcoL",".mpr",4372,TR("Calculate the semivariogram values for different lags of a point or raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("princcmp","",Cmdprinccmp,TR("&Statistics.&MapList.&Principal Components"),TR("Principal Components"),"ExeMat16IcoL",".mpl",6002,TR("Perform a principal components analysis on a map list")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("factanal","",Cmdfactanal,TR("&Statistics.&MapList.&Factor Analysis"),TR("Factor Analysis"),"ExeMat16IcoL",".mpl",6012,TR("Perform a factor analysis on a map list")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("matvarcov","",Cmdmatvarcov,TR("&Statistics.&MapList.&Variance-Covariance Matrix"),TR("Variance-Covariance Matrix"),"ExeMat16IcoL",".mpl",6022,TR("Calculate the variance-covariance matrix of a map list")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("matcorr","",Cmdmatcorr,TR("&Statistics.&MapList.&Correlation Matrix"),TR("Correlation Matrix"),"ExeMat16IcoL",".mpl",6032,TR("Calculate the correlation matrix of a map list")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("mapliststatistics","",Cmdmapliststatistics,TR("&Statistics.&MapList.&MapList Statistics"),TR("MapList Statistics"),"ExeMap16IcoL",".mpl",0000,TR("Calculate a map with statistics from a map list")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("histnbpol","",Cmdhistnbpol,TR("&Statistics.&Polygons.&Neighbour Polygons"),TR("Neighbour Polygons"),"ExeTbl16IcoL",".mpa",5022,TR("Determine which polygons are adjacent")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("histsegdir","",Cmdhistsegdir,TR("&Statistics.&Segments.&Direction Histogram"),TR("Histogram Segment Direction"),"ExeTbl16IcoL",".mps",5012,TR("Calculate the total length of segments per direction")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("spatcorr","",Cmdspatcorr,TR("&Statistics.P&oints.&Spatial Correlation"),TR("Spatial Correlation"),"ExeTbl16IcoL",".mpp",5042,TR("Calculate the spatial correlation between points")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("variogramsurface","",Cmdvariogramsurface,TR("&Statistics.P&oints.&Variogram Surface"),TR("Variogram Surface"),"ExeMap16IcoL",".mpp",4372,TR("Calculate the semivariogram values for different lags of a point or raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("crossvariogram","",Cmdcrossvariogram,TR("&Statistics.P&oints.&Cross Variogram"),TR("Cross Variogram"),"ExeTbl16IcoL",".mpp",5072,TR("Calculate the cross variogram of two columns of a point map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("pattanal","",Cmdpattanal,TR("&Statistics.P&oints.P&attern Analysis"),TR("Pattern Analysis"),"ExeTbl16IcoL",".mpp",5032,TR("Perform a pattern analysis on a point map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("densras","",Cmddensras,TR("&Interpolation..&Densify Raster Map"),TR("Densify Raster Map"),"ExeMap16IcoL",".mpr.mpl",4042,TR("Reduce the pixel size of a raster map by interpolation")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("krigingras","",Cmdkrigingras,TR("&Interpolation..&Kriging from Raster"),TR("Kriging from Raster"),"ExeMap16IcoL",".mpr",4112,TR("Kriging from Raster")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("interpolseg","",Cmdinterpolseg,TR("&Interpolation..&Contour Interpolation"),TR("Interpolate Contours "),"ExeMap16IcoL",".mps",4512,TR("Perform an interpolation on contour lines and return a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("nearestpnt","",Cmdnearestpnt,TR("&Interpolation.&Point Interpolation.&Nearest Point"),TR("Nearest Point "),"ExeMap16IcoL",".mpp",4322,TR("Assign to each output pixel the code of the nearest point in a point map Create a Thiessen map from points")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("movaverage","",Cmdmovaverage,TR("&Interpolation.&Point Interpolation.Moving &Average"),TR("Moving Average "),"ExeMap16IcoL",".mpp",4332,TR("Perform a moving average interpolation on point values and return a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("trendsurface","",Cmdtrendsurface,TR("&Interpolation.&Point Interpolation.&Trend Surface"),TR("Trend Surface "),"ExeMap16IcoL",".mpp",4342,TR("Perform a trend surface interpolation on point values and return a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("movsurface","",Cmdmovsurface,TR("&Interpolation.&Point Interpolation.Moving &Surface"),TR("Moving Surface "),"ExeMap16IcoL",".mpp",4352,TR("Perform a moving surface interpolation on point values and return a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("kriging","",Cmdkriging,TR("&Interpolation.&Point Interpolation.&Kriging"),TR("Kriging "),"ExeMap16IcoL",".mpp",4362,TR("Perform a kriging estimation on point values and return a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("anisotrkriging","",Cmdanisotrkriging,TR("&Interpolation.&Point Interpolation.&Anisotropic Kriging"),TR("Anisotropic Kriging "),"ExeMap16IcoL",".mpp",4365,TR("Perform a anisotropic kriging estimation on point values and return a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("univkriging","",Cmdunivkriging,TR("&Interpolation.&Point Interpolation.&Universal Kriging"),TR("Universal Kriging "),"ExeMap16IcoL",".mpp",4366,TR("Perform a universal kriging estimation on point values and return a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("cokriging","",Cmdcokriging,TR("&Interpolation.&Point Interpolation.&CoKriging"),TR("CoKriging "),"ExeMap16IcoL",".mpp",4382,TR("Perform a cokriging estimation on two point  maps and return a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("uniqueid","",Cmduniqueid,TR("&Spatial Reference Operations..&Unique ID"),TR("Unique ID "),"ExeGenMap16Ico",".mpp.mps.mpa",5502,TR("Assign a unique ID to every element in a map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transformheight","",Cmdtransformheight,TR("&Spatial Reference Operations.&Coordinates.Transform Coordinate &Heights"),TR("Transform Coordinate Heights "),".csy",".csy",4773,TR("Transform individual map coordinates and heights from one system to another")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transform","",Cmdtransform,TR("&Spatial Reference Operations.&Coordinates.Transform &Coordinates"),TR("Transform Coordinates "),".csy",".csy",4772,TR("Transform individual map coordinates from one system to another")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transformtogeocentric","",Cmdtransformtogeocentric,TR("&Spatial Reference Operations.&Coordinates.Transform to &Geocentric"),TR("Transform to Geocentric XYZ,"),".csy",".csy",4774,TR("Transform individual map coordinates and heights to geocentric co-ordinates")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transformfromgeocentric","",Cmdtransformfromgeocentric,TR("&Spatial Reference Operations.&Coordinates.Transform &from Geocentric"),TR("Transform from Geocentric XYZ,"),".csy",".csy",4775,TR("Transform individual map coordinates and heights from geocentric co-ordinates")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transformviageocentric","",Cmdtransformviageocentric,TR("&Spatial Reference Operations.&Coordinates.Transform &via Geocentric"),TR("Transform via Geocentric XYZ"),".csy",".csy",4776,TR("Transform (projected) coordinates and heights via geocentric co-ordinates")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("polras","",Cmdpolras,TR("R&asterize..&Polygon to Raster"),TR("Polygon to Raster "),"ExeMap16IcoL",".mpa.grf",4402,TR("Rasterize a polygon map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("segras","",Cmdsegras,TR("R&asterize..&Segment to Raster"),TR("Segment to Raster "),"ExeMap16IcoL",".mps.grf",4502,TR("Rasterize a segment map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("segdensity","",Cmdsegdensity,TR("R&asterize..Segment &Density"),TR("Segment Density "),"ExeMap16IcoL",".mps.grf",4522,TR("Calculate the segment length in every pixel of a rasterized segment map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("pntras","",Cmdpntras,TR("R&asterize..P&oint to Raster"),TR("Point to Raster "),"ExeMap16IcoL",".mpp.grf",4302,TR("Rasterize a point map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("pntdensity","",Cmdpntdensity,TR("R&asterize..Point &Density"),TR("Point Density "),"ExeMap16IcoL",".mpp.grf",4312,TR("Count the number of points per pixel and return a raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transpose","",Cmdtranspose,TR("&Table Operations..&Transpose Table"),TR("Transpose Table"),"ExeTbl16IcoL",".tbt",5052,TR("Transpose a table, records become columns and vice versa")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("tblchdom","",Cmdtblchdom,TR("&Table Operations..&Change Domain"),TR("Table Change Domain "),"ExeTbl16IcoL",".tbt",5062,TR("Change the domain of a table")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("tblglue","",Cmdtblglue,TR("&Table Operations..&Glue Tables"),TR("Table Glue "),"ExeTbl16IcoL",".tbt",5082,TR("Glue multiple tables into one table")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("bursawolf","", Cmdbursawolf,TR("&Spatial Reference Operations.&Coordinates.Bursa-Wolf"),TR("Bursa-Wolf parameters"),".csy",".csy",-1,""));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("maplistgraph","",Cmdmaplistgraph,TR("&Statistics.&MapList.&MapList Graph"),TR("MapList Graph"),".grh",".mpl",0000,TR("Show a z-graph of a map list")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmjoin","",Cmdjoin,"","",".grh",".mpl",0000,"Joins column(s) into a table", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmaggregate","",Cmdclmaggregate,"","",".grh",".mpl",0000,"Calculate the aggregate of a column", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmsemivariogram","",Cmdclmsemivar,"","",".grh",".mpl",0000,"Calculate the semivariogram of a column", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmstatistics","",Cmdclmstatistiscs,"","",".grh",".mpl",0000,"Calculate the statistics of a column", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmcumulative","",Cmdclmcumulative,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("confmat","",Cmdclmconfusionmatrix,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmleastsquares","",Cmdclmleastsquares,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmsort","",Cmdclmsort,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmslicing","",Cmdclmslicing,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clminvmoldensky","",Cmdclminvmoldensky,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmfindmoldensky","",Cmdclmfindmoldensky,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmfind7parameters","",Cmdclmfind7parameters,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmmoledensky","",Cmdclmmoledensky,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmhelmert7parms","",Cmdclmhelmert7parms,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmfindazimuthdistance","",Cmdclmfindazimuthdistance,"","Find Azimuth Distance",".grh",".mpl",0000,"Find Azimuth Distance", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("timecolumn","",Cmdtimecolumn,TR("&Table Operations..&Create time column"),TR("Create time column from otther columns"),"ExeTbl16IcoL",".col",0,TR("Create time column from otther columns")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("timefromcolumns","",Cmdtimefromcolumns,"","",".grh",".mpl",0,TR("Create time column from maplist name information")));

(*infosui).push_back(CommandHandlerUI::createCommandInfo("updateallcolumns","",CmdUpdateAllColumns,"","",".grh",".mpl",0000,"", false));

return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Rastermap and table applications UI", fnModule,ILWIS::Module::mi37,"1.3");
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getCommandInfoUI);  

	return module;
}
