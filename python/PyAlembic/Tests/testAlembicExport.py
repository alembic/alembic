#Load test dataset
from buildTestData import *

def exportCacheImpl( iTopObject, iTraitsData, iPropertyType, iWriteMeta = True):
    assert iTopObject.valid()

    cp = iTopObject.getProperties()

    for k, v in iTraitsData.iteritems():
        name = k;
        traits = v[0]
        data = v[1:]
        meta = MetaData()
        inter = traits.interpretation()
        if len( inter ) > 0 and iWriteMeta:
            meta.set( "interpretation", traits.interpretation() )
        prop = iPropertyType( cp, name, traits.dataType(), meta )
        for v in data:
            prop.setValue( v )

#------------------------------------------------------------------------------
# Main test function
def exportCache( iFileName, iDataType ):
    if iDataType == 0:
        exportCacheImpl( OArchive( iFileName ).getTop(), ScalarTraitsData, \
                         OScalarProperty )
    elif iDataType == 1:
        exportCacheImpl( OArchive( iFileName ).getTop(), SmallArrayTraitsData, \
                         OScalarProperty, False )
    elif iDataType == 2:
        exportCacheImpl( OArchive( iFileName ).getTop(), ArrayTraitsData, \
                         OArrayProperty )

# Local Variables:
# mode:python
# End:
