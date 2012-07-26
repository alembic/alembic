# Load test dataset
from buildTestData import *

def compareScalarSamples( iProp ):
    if not ScalarTraitsData.has_key( iProp.getName() ):
        return
    
    inputData = ScalarTraitsData[iProp.getName()][1:]
    compare = TraitsComparison[iProp.getName()]

    assert len( inputData ) == iProp.getNumSamples()

    for i in range( len( inputData ) ):
        assert compare( inputData[i], iProp.samples[i] )

def compareArraySamples( iProp ):
    if not ArrayTraitsData.has_key( iProp.getName() ):
        return

    assert iProp.getNumSamples() == 1

    s = iProp.samples[0]

    inputData = ArrayTraitsData[iProp.getName()][1:]
    compare = TraitsComparison[iProp.getName()]

    assert len( inputData ) == iProp.getNumSamples()

    for i in range( len( inputData ) ):
        arrayData = inputData[i]
        s = iProp.samples[i]

        assert len(arrayData) == len(s)

        for k in range( len( arrayData ) ):
            assert compare( arrayData[i], s[i] )
    
def visitCompoundProperty( iProp ):
    visitProperties( iProp )

def visitSimpleProperty( iProp ):
    if iProp.isScalar():
        compareScalarSamples( iProp )
    else:
        compareArraySamples( iProp )

def visitProperties( iParent ):
    for header in iParent.propertyheaders:
        prop = iParent.getProperty( header.getName() )

        if header.isCompound():
            visitCompoundProperty( prop )
        elif header.isScalar() or header.isArray():
            visitSimpleProperty( prop )

def visitObject( iObj ):
    visitProperties( iObj.getProperties() )

    for child in iObj.children:
        visitObject( child )

#------------------------------------------------------------------------------
# Main test function
def importCache( iFileName ):
    visitObject( IArchive( iFileName ).getTop() )

# Local Variables:
# mode:python
# End:
