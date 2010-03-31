[Ilwis]
Description=Calculate slope map using contourline map and georef as input.
Time=956074552
Version=3.0
Type=Script

[Script]
ScriptFile=slope.isf
NrParams=4

[Params]
Param0=Input Contour map
Type0=segmap
Param1=Georef to be used
Type1=georef
Param2=Output slopemap in percentages
Type2=filename
DefaultValue2=Slope in Percent
Param3=Output slopemap in degrees
Type3=filename
DefaultValue3=Slope in Degrees
