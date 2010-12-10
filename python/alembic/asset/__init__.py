import _init

# must init hdf5
_init.Init( True, True )

from _icontext import IContext
import _iasset

# shared IContext for all assets
__ICTX = IContext()


class IAsset( _iasset.IAsset ):
    def __init__( self, fileName, ctx = __ICTX ):
        _iasset.IAsset.__init__( self, fileName, ctx )
