from distutils.core import setup, Extension


ilwis = Extension('_ilwis',
                           sources=['..\\Frameworks\\ilwis_wrap.cpp','..\\Frameworks\\PBoundingBox.cpp',
                           '..\\Frameworks\\PColumn.cpp','..\\Frameworks\\PCoordinate.cpp',
                           '..\\Frameworks\\PLatLon.cpp','..\\Frameworks\\PEllipsoid.cpp',
                           '..\\Frameworks\\PCoordinateSystem.cpp','..\\Frameworks\\PCoverage.cpp',
                           '..\\Frameworks\\PCoverageCollection.cpp','..\\Frameworks\\PCoverageList.cpp',
                           '..\\Frameworks\\PDataType.cpp','..\\Frameworks\\PDatum.cpp',
                           '..\\Frameworks\\PFeature.cpp','..\\Frameworks\\PFeatureCoverage.cpp',
                           '..\\Frameworks\\PGeoReference.cpp','..\\Frameworks\\PGridBoundingBox.cpp',
                           '..\\Frameworks\\PGridCoverage.cpp','..\\Frameworks\\RootObject.cpp',
                           '..\\Frameworks\\PNumericData.cpp','..\\Frameworks\\PPixel.cpp',
                           '..\\Frameworks\\PProjection.cpp','..\\Frameworks\\PSRObject.cpp',
                           '..\\Frameworks\\PCRSBoundingBox.cpp','..\\Frameworks\\PStringData.cpp',
                           '..\\Frameworks\\PTable.cpp','..\\Frameworks\\PThematicData.cpp',
                           '..\\Frameworks\\PValue.cpp','..\\Frameworks\\PValueRange.cpp'],
                           include_dirs=['..\\Frameworks\\headers','..\\Frameworks\\headers\\geos'],
                           library_dirs=['..\\Frameworks\\libs'],
                           libraries=['IlwisEngine']
                           )

setup (name = 'ilwis',
       version = '0.1',
       author      = "Martin Schouwenburg",
       description = """Implementation of ILWIS functionality in Python""",
       ext_modules = [ilwis],
       py_modules = ["ilwis"],
       )