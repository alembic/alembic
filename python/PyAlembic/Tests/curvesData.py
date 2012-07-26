from imath import *

def FillArray( iArray, iList ):
    a = iArray( len( iList ) )
    for i in range( len( iList ) ):
        a[i] = iList[i]
    return a   

numVerts = FillArray( IntArray, [8, 4] )

verts = FillArray( V3fArray, [
    # first curve
    V3f( -1.0, -1.0, -1.0 ),
    V3f( 1.0, -1.0, -1.0 ),
    V3f( -1.0, 1.0, -1.0 ),
    V3f( 1.0, 1.0, -1.0 ),
    V3f( -1.0, -1.0, 1.0 ),
    V3f( 1.0, -1.0, 1.0 ),
    V3f( -1.0, 1.0, 1.0 ),
    V3f( 1.0, 1.0, 1.0 ),

    # second curve
    V3f( 1.0, 0.0, 1.0 ),
    V3f( 0.5, 0.25, 1.0 ),
    V3f( 0.5, 0.75, 1.0 ),
    V3f( 1.0, 1.0, 1.0 )
] )

uvs = FillArray (V2fArray, [
    V2f( 0.0, 0.0 ),
    V2f( 0.1, 0.0 ),
    V2f( 0.2, 0.0 ),
    V2f( 0.3, 0.0 ),
    V2f( 0.4, 0.0 ),
    V2f( 0.5, 0.0 ),
    V2f( 0.6, 0.0 ),
    V2f( 0.7, 0.0 ),
    V2f( 0.0, 0.0 ),
    V2f( 0.25, 0.0 ),
    V2f( 0.5, 0.0 ),
    V2f( 1.0, 0.0 )
] )

widths = FillArray( FloatArray, [
    # first curve
    0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6,

    # second curve
    0.25, 0.5, 0.75, 1.0
] )


