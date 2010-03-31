[Params]
Param0=DEM
Type0=rasmap
Ext0=Yes
Param1=Flow Accumulation Map
Type1=rasmap
Ext1=Yes
Param2=Output Wetness Index Map
Type2=filename
Ext2=No
Param3=Output Stream Power Index Map
Type3=filename
Ext3=Yes
Param4=Output Sediment Transport Index Map
Type4=filename
Ext4=No
DefaultValue2=wetness_index
DefaultValue3=power_index
DefaultValue4=sediment_index
[Ilwis]
Description=Compound Index Calculation
Time=1118746080
Version=3.1
Class=Script
Type=Script
[Script]
ScriptFile=Compound_Parameter_Extraction.isf
NrParams=5
