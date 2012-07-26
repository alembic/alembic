from imath import *

def FillArray( iArray, iList ):
    a = iArray( len( iList ) )
    for i in range( len( iList ) ):
        a[i] = iList[i]
    return a   

nu = 4
nv = 4
uOrder = 4
vOrder = 4

uKnot = FillArray( FloatArray, [0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0] )
vKnot = FillArray( FloatArray, [0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0] )

P = FillArray( V3fArray,
                     [  V3f( 0.0, 0.0, -3.0 ),
                                        V3f( 0.0, 1.0, -3.0 ),
                                        V3f( 0.0, 2.0, -3.0 ),
                                        V3f( 0.0, 3.0, -3.0 ),
                                        V3f( 1.0, 0.0, -3.0 ),
                                        V3f( 1.0, 1.0, 3.0 ),
                                        V3f( 1.0, 2.0, 3.0 ),
                                        V3f( 1.0, 3.0, -3.0 ),
                                        V3f( 2.0, 0.0, -3.0 ),
                                        V3f( 2.0, 1.0, 3.0 ),
                                        V3f( 2.0, 2.0, 3.0 ),
                                        V3f( 2.0, 3.0, -3.0 ),
                                        V3f( 3.0, 0.0, -3.0 ),
                                        V3f( 3.0, 1.0, -3.0 ),
                                        V3f( 3.0, 2.0, -3.0 ),
                                        V3f( 3.0, 3.0, -3.0 ) ] )

Pw = FillArray( FloatArray, [ 1.0, 0.5, 1.0, 0.5, 1.0, 0.5, 1.0, 0.5,
                           1.0, 0.5, 1.0, 0.5, 1.0, 0.5, 1.0, 0.5 ] )
trim_nLoops = 1
trim_nCurves = FillArray( IntArray, [ 1 ] )
trim_order = FillArray( IntArray, [ 3 ] )
trim_knot = FillArray( FloatArray, [ 0.0, 0.0, 0.0, 1.0, 1.0, 2.0, 2.0,
                                        3.0, 3.0, 4.0, 4.0, 4.0 ] )
trim_min = FillArray( FloatArray, [ 0.0 ] )
trim_max = FillArray( FloatArray, [ 1.0 ] )
trim_n = FillArray( IntArray, [ 9 ] )
trim_u = FillArray( FloatArray, [ 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0 ] )
trim_v = FillArray( FloatArray, [ 0.5, 1.0, 2.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.5] )
trim_w = FillArray( FloatArray, [ 1.0, 1.0, 2.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0 ] )
