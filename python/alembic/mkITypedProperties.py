#!/usr/bin/env python

sprops = \
"""
IBoolProperty
IUcharProperty
ICharProperty
IUInt16Property
IInt16Property
IUInt32Property
IInt32Property
IUInt64Property
IInt64Property
IHalfProperty
IFloatProperty
IDoubleProperty
IStringProperty
IWstringProperty
IV2sProperty
IV2iProperty
IV2fProperty
IV2dProperty
IV3sProperty
IV3iProperty
IV3fProperty
IV3dProperty
IBox2sProperty
IBox2iProperty
IBox2fProperty
IBox2dProperty
IBox3sProperty
IBox3iProperty
IBox3fProperty
IBox3dProperty
IM33fProperty
IM33dProperty
IM44fProperty
IM44dProperty
IQuatfProperty
IQuatdProperty
IC3hProperty
IC3fProperty
IC3cProperty
IC4hProperty
IC4fProperty
IC4cProperty
IN3fProperty
IN3dProperty
"""

def printdefs( p ):
    s = "    //%s\n    //\n"
    #s += 'bool\n( Abc::%s::*matchesMetaData )( const AbcA::MetaData&,\nAbc::SchemaInterpMatching ) = \\\n'
    #s += '&Abc::%s::matches;\n\n'
    #s += 'bool\n( Abc::%s::*matchesHeader )( const AbcA::PropertyHeader&,\nAbc::SchemaInterpMatching ) = \\\n'
    #s += '&Abc::%s::matches;\n\n'
    s += 'class_<Abc::%s>( "%s",\ninit<Abc::ICompoundProperty,\nconst std::string&>() )\n'
    s += '.def( init<>() )\n'
    s += '.def( "getName", &Abc::%s::getName,\nreturn_value_policy<copy_const_reference>() )\n'
    s += '.def( "getHeader", &Abc::%s::getHeader,\nreturn_internal_reference<1>() )\n'
    s += '.def( "isScalar", &Abc::%s::isScalar )\n'
    s += '.def( "isArray", &Abc::%s::isArray )\n'
    s += '.def( "isCompound", &Abc::%s::isCompound )\n'
    s += '.def( "isSimple", &Abc::%s::isSimple )\n'
    s += '.def( "getMetaData", &Abc::%s::getMetaData,\nreturn_internal_reference<1>() )\n'
    s += '.def( "getDataType", &Abc::%s::getDataType,\nreturn_internal_reference<1>() )\n'
    s += '.def( "getTimeSamplingType", &Abc::%s::getTimeSamplingType )\n'
    s += '.def( "getInterpretation", &Abc::%s::getInterpretation,\nreturn_value_policy<copy_const_reference>() )\n'
    #s += '.def( "matches", matchesMetaData )\n'
    #s += '.def( "matches", matchesHeader )\n'
    s += '.def( "getNumSamples", &Abc::%s::getNumSamples )\n'
    #s += '.def( "getValue", &Abc::%s::getValue, %s_overloads() )\n'
    s += '.def( "getObject", &Abc::%s::getObject,\nwith_custodian_and_ward_postcall<0,1>() )\n'
    s += '.def( "reset", &Abc::%s::reset )\n'
    s += '.def( "valid", &Abc::%s::valid )\n'
    s += '.def( "__str__", &Abc::%s::getName,\nreturn_value_policy<copy_const_reference>() )\n'
    s += '.def( "__nonzero__", &Abc::%s::valid )\n'
    s += ';'

    print s % eval( "%s" % ( 'p,' * s.count( r'%s' ) ) )
    print

    return

for i in sprops.split():
    if i == "": pass
    else: printdefs( i )
