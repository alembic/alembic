// Used for internal python bindings
#include <PY/PY_CPythonAPI.h>
// This file contains functions that will run arbitrary Python code
#include <PY/PY_Python.h>


#include "SOP_AlembicIn.h"

#include <Alembic/AbcCoreHDF5/All.h>

#include <OP/OP_OperatorTable.h>
#include <OP/OP_Director.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_SpareData.h>


#include <GU/GU_Detail.h>
#include <GU/GU_PrimNURBSurf.h>
#include <GU/GU_PrimNURBCurve.h>
#include <GU/GU_PrimPart.h>
#include <GU/GU_PrimPoly.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_DMatrix4.h>

#include <HOM/HOM_Module.h>
#include <boost/tokenizer.hpp>
#include <sstream>
#include <boost/shared_ptr.hpp>

//-*****************************************************************************

namespace
{
    struct ArchiveCacheEntry
    {
        ArchiveCacheEntry()
        : objectPathMenuList(NULL)
        {
        }
        
        ~ArchiveCacheEntry()
        {
            if (objectPathMenuList != NULL)
            {
                PY_Py_DECREF(objectPathMenuList);
            }
        }
        
        Abc::IArchive archive;
        std::string error;
        PY_PyObject * objectPathMenuList;
    };
    
    typedef boost::shared_ptr<ArchiveCacheEntry> ArchiveCacheEntryRcPtr;
    typedef std::map<std::string, ArchiveCacheEntryRcPtr> ArchiveCache;
    
    //-*************************************************************************
    
    size_t g_maxCache = 50;
    //for now, leak the pointer to the archive cache so we don't
    //crash at shutdown
    ArchiveCache * g_archiveCache(new ArchiveCache);
    
    //-*************************************************************************
    
    ArchiveCacheEntryRcPtr LoadArchive(const std::string & path)
    {
        ArchiveCache::iterator I = g_archiveCache->find(path);
        if (I != g_archiveCache->end())
        {
            return (*I).second;
        }
        ArchiveCacheEntryRcPtr entry = ArchiveCacheEntryRcPtr(
                new ArchiveCacheEntry);
        try
        {
            entry->archive = Abc::IArchive(
                    ::Alembic::AbcCoreHDF5::ReadArchive(), path);
        }
        catch (const std::exception & e)
        {
            entry->error = e.what();
        }
        while (g_archiveCache->size() >= g_maxCache)
        {
            long d = static_cast<long>(std::floor(
                    static_cast<double>(std::rand())
                            / RAND_MAX * g_maxCache - 0.5));
            if (d < 0)
            {
                d = 0;
            }
            ArchiveCache::iterator it = g_archiveCache->begin();
            for (; d > 0; --d)
            {
                ++it;
            }
            g_archiveCache->erase(it);
        }
        (*g_archiveCache)[path] = entry;
        return entry;
    }
    
    //-**************************************************************************
    
    void TokenizeObjectPath(const std::string & objectPath,
            SOP_AlembicIn::PathList & pathList)
    {
        typedef boost::char_separator<char> Separator;
        typedef boost::tokenizer<Separator> Tokenizer;
        Tokenizer tokenizer( objectPath, Separator( "/" ) );
        for ( Tokenizer::iterator iter = tokenizer.begin() ;
                iter != tokenizer.end() ; ++iter )
        {
            if ( (*iter).empty() ) { continue; }
            pathList.push_back( *iter );
        }
    }
}

//-*****************************************************************************

OP_Node * SOP_AlembicIn::myConstructor( OP_Network *net, const char *name,
        OP_Operator *op)
{
    return new SOP_AlembicIn( net, name, op );
}

//-*****************************************************************************

static PRM_Name prm_abcAttribName("abcName#", "Alembic Name #");
static PRM_Name prm_hAttribName("hName#", "Houdini Name #");
static PRM_Template prm_AttributeRemapTemplate[] = {
    PRM_Template(PRM_STRING, 1, &prm_abcAttribName),
    PRM_Template(PRM_STRING, 1, &prm_hAttribName),
    PRM_Template()
};

static PRM_Name prm_fileNameName("fileName", "File Name");
static PRM_Name prm_frameName("frame", "Frame");
static PRM_Name prm_fpsName("fps", "Frames Per Second");
static PRM_Name prm_objectPathName("objectPath", "Object Path");
static PRM_Name prm_includeXformName("includeXform", "Include Xform");
static PRM_Name prm_remapAttribName("remapAttributes", "Remap Attributes");

static PRM_Default prm_frameDefault(1, "$F");
static PRM_Default prm_objectPathDefault(0, "");
static PRM_Default prm_fpsDefault(24, "$FPS");
static PRM_Default prm_includeXformDefault(true);

static PRM_ChoiceList	prm_objectPathMenu(PRM_CHOICELIST_TOGGLE,
        "__import__('_alembic_hom_extensions').alembicGetObjectPathListForMenu"
                "(hou.pwd().evalParm('fileName'))[:16380]",
        CH_PYTHON_SCRIPT);

PRM_Template SOP_AlembicIn::myTemplateList[] =
{
    PRM_Template(PRM_FILE, 1, &prm_fileNameName),
    PRM_Template(PRM_FLT_J, 1, &prm_frameName, &prm_frameDefault),
    PRM_Template(PRM_FLT_J, 1, &prm_fpsName, &prm_fpsDefault),
    PRM_Template(PRM_STRING, 1, &prm_objectPathName, &prm_objectPathDefault,
            &prm_objectPathMenu),
    PRM_Template(PRM_TOGGLE, 1, &prm_includeXformName, &prm_includeXformDefault),
    PRM_Template(PRM_MULTITYPE_LIST, prm_AttributeRemapTemplate, 2,
            &prm_remapAttribName, PRMzeroDefaults, 0,
            &PRM_SpareData::multiStartOffsetOne),
    PRM_Template()
};

//-*****************************************************************************

SOP_AlembicIn::SOP_AlembicIn(OP_Network *net, const char *name,
        OP_Operator *op)
: SOP_Node(net, name, op)
, myFileObjectCache(UT_String::ALWAYS_DEEP, "")
, myTopologyConstant(false)
, myEntireSceneIsConstant(false)
{
}

//-*****************************************************************************

SOP_AlembicIn::~SOP_AlembicIn()
{
}

//-*****************************************************************************

bool SOP_AlembicIn::unloadData()
{
    bool unloaded = SOP_Node::unloadData();
    
    if (unloaded)
    {
    }
    
    return unloaded;
}

//-*****************************************************************************

OP_ERROR SOP_AlembicIn::cookMySop(OP_Context &context)
{
    Args args;
    std::map<std::string,std::string> nameMap;
    
    const float now = context.myTime;
    
    args.includeXform = evalInt("includeXform", 0, now);
    
    std::string fileName;
    {
        UT_String str;
        evalString(str, "fileName", 0, now);
        
        fileName = str.toStdString();
    }
    
    PathList pathList;
    
    std::string objectPath;
    {
        UT_String str;
        evalString(str, "objectPath", 0, now);
        
        objectPath = str.toStdString();
    }
    // If the file or object parameter has changed, we need to reset our
    // primitive cache.
    UT_WorkBuffer fileobjecthash;
    fileobjecthash.sprintf("%s:%s:%d", fileName.c_str(), objectPath.c_str(),
            args.includeXform);
    
    
    int nmapSize = evalInt("remapAttributes", 0, now);
    // Entries are one based (not zero based)
    for (int i = 1; i <= nmapSize; ++i)
    {
        UT_String abcName, hName;
        evalStringInst("abcName#", &i, abcName, 0, now);
        evalStringInst("hName#", &i, hName, 0, now);
        if (abcName.isstring() && hName.isstring())
        {
            nameMap[abcName.toStdString()] = hName.toStdString();
            
            // Incorporate attribute remapping values into the fileobjecthash
            // so that recooks reflect the new state even for constant data
            fileobjecthash.sprintf(":%s->%s",
                    (const char *)abcName, (const char *)hName);
        }
    }
    
    if (strcmp(myFileObjectCache, fileobjecthash.buffer()) != 0)
    {
        myFileObjectCache.harden(fileobjecthash.buffer());
        myTopologyConstant = false;
        myEntireSceneIsConstant = false;
        myPrimitiveCountCache.clear();
    }
    
    // Exit early if the entire scene (within this SOP) was recognized
    // as constant on a previous cook. Note that we haven't yet evaluated
    // the "frame" parm so we won't be called for subsequent cooks.
    if (myEntireSceneIsConstant)
    {
        return error();
    }
    
    if ( !objectPath.empty() )
    {
        TokenizeObjectPath(objectPath, pathList);
    }
    
    M44d xform;
    
    if (!myTopologyConstant)
    {
        gdp->clearAndDestroy();
    }
    else
    {
#if UT_MAJOR_VERSION_INT >= 12
        gdp->destroyInternalNormalAttribute();
#else
        gdp->destroyPointAttrib("internalN", sizeof(UT_Vector3), GB_ATTRIB_MIXED);
#endif
    }
    
    
    double fps = evalFloat("fps", 0, now);
    args.abcTime = evalFloat("frame", 0, now) / fps;
    args.isConstant = true;
    args.isTopologyConstant = true;
    args.reusePrimitives = myTopologyConstant && gdp->primitives().entries()>0;
    args.pointCount = 0;
    args.primCount = 0;
    args.nameMap = &nameMap;
    args.boss = UTgetInterrupt();
    
    
    if (!args.boss->opStart("Loading and walking Alembic data"))
    {
        args.boss->opEnd();
        return error();
    }
    
    try
    {
        ArchiveCacheEntryRcPtr cacheEntry = LoadArchive(fileName);
        
        if (!cacheEntry->archive.valid() )
        {
            std::ostringstream buffer;
            buffer << "Alembic exception: ";
            buffer << cacheEntry->error;
            addWarning(SOP_MESSAGE, buffer.str().c_str());
            args.boss->opEnd();
            return error();
        }
        
        attachDetailStringData("abcFileName", fileName);
        
        IObject root = cacheEntry->archive.getTop();
        
        if ( pathList.empty() ) //walk the entire scene
        {
            for ( size_t i = 0; i < root.getNumChildren(); ++i )
            {
                walkObject( args, root, root.getChildHeader(i),
                            pathList.end(), pathList.end(), xform, true);
            }
        }
        else //walk to a location + its children
        {
            PathList::const_iterator I = pathList.begin();
            
            const ObjectHeader *nextChildHeader =
                    root.getChildHeader( *I );
            if ( nextChildHeader != NULL )
            {
                walkObject( args, root, *nextChildHeader, I+1, pathList.end(),
                        xform, true);
            }
        }
        
        // If after doing our traversal nothing has changed args.isConstant,
        // then we copy that state back to our member variables.
        if (args.isConstant)
        {
            myEntireSceneIsConstant = true;
        }
        myTopologyConstant = args.isTopologyConstant;
    }
    catch ( const InterruptedException & e )
    {
        //currently thrown by WalkObject
        myEntireSceneIsConstant = false;
        myTopologyConstant = false;
        myPrimitiveCountCache.clear();
    }
    catch ( const std::exception &e )
    {
        std::ostringstream buffer;
        buffer << "Alembic exception: ";
        buffer << e.what();
        addWarning(SOP_MESSAGE, buffer.str().c_str());
    }
    
    args.boss->opEnd();
    return error();
}

//-*****************************************************************************

void SOP_AlembicIn::nodeUnlocked()
{
}

//-*****************************************************************************

#if UT_MAJOR_VERSION_INT >= 12
    static GA_RWAttributeRef
    findStringTuple(GU_Detail &gdp, GEO_AttributeOwner owner,
            const char *name, int min_size=1)
    {
        return gdp.findStringTuple(owner, name, min_size);
    }
    static GA_RWAttributeRef
    addStringTuple(GU_Detail &gdp, GEO_AttributeOwner owner,
            const char *name, int size)
    {
        return gdp.addStringTuple(owner, name, size);
    }
    static GA_RWAttributeRef
    findIntTuple(GU_Detail &gdp, GEO_AttributeOwner owner,
            const char *name, int min_size)
    {
        return gdp.findIntTuple(owner, name, min_size);
    }
    static GA_RWAttributeRef
    addIntTuple(GU_Detail &gdp, GEO_AttributeOwner owner,
            const char *name, int size)
    {
        return gdp.addIntTuple(owner, name, size);
    }
    static GA_RWAttributeRef
    findFloatTuple(GU_Detail &gdp, GEO_AttributeOwner owner,
            const char *name, int min_size)
    {
        return gdp.findFloatTuple(owner, name, min_size);
    }
    static GA_RWAttributeRef
    addFloatTuple(GU_Detail &gdp, GEO_AttributeOwner owner,
            const char *name, int size, bool normal)
    {
        GA_RWAttributeRef h = gdp.addFloatTuple(owner, name, size);
        if (h.isValid() && normal)
        {
            h.getAttribute()->setTypeInfo(GA_TYPE_NORMAL);
        }
        return h;
    }
    static void
    setTypeInfo(GA_RWAttributeRef &aref, GA_TypeInfo tinfo)
    {
        if (aref.isValid() && tinfo != GA_TYPE_VOID)
        {
            aref.getAttribute()->setTypeInfo(tinfo);
        }
    }
    static void
    addToGroup(GA_PrimitiveGroup *group, GEO_Primitive *gb)
    {
        group->add(gb);
    }
#else
    static GA_RWAttributeRef
    findAttribute(GU_Detail &gdp, GEO_AttributeOwner owner, const char *name,
            GB_AttribType type, size_t bytes)
    {
        switch (owner)
        {
        case GEO_POINT_DICT:
            return gdp.findPointAttrib(name, bytes, type);
        case GEO_VERTEX_DICT:
            return gdp.findVertexAttrib(name, bytes, type);
        case GEO_PRIMITIVE_DICT:
            return gdp.findPrimAttrib(name, bytes, type);
        case GEO_DETAIL_DICT:
            return gdp.findAttrib(name, bytes, type);
        default:
            return GA_RWAttributeRef();
        }
    }
    
    static GA_RWAttributeRef
    addAttribute(GU_Detail &gdp, GEO_AttributeOwner owner, const char *name,
        GB_AttribType type, size_t bytes, const void *def=NULL)
    {
        switch (owner)
        {
        case GEO_POINT_DICT:
            return gdp.addPointAttrib(name, bytes, type, def);
        case GEO_VERTEX_DICT:
            return gdp.addVertexAttrib(name, bytes, type, def);
        case GEO_PRIMITIVE_DICT:
            return gdp.addPrimAttrib(name, bytes, type, def);
        case GEO_DETAIL_DICT:
            return gdp.addAttrib(name, bytes, type, def);
        default:
            return GA_RWAttributeRef();
        }
    }
    static GA_RWAttributeRef
    findStringTuple(GU_Detail &gdp, GEO_AttributeOwner owner,
            const char *name, int min_size=1)
    {
        return findAttribute(gdp, owner, name, GB_ATTRIB_INDEX, sizeof(int));
    }
    static GA_RWAttributeRef
    addStringTuple(GU_Detail &gdp, GEO_AttributeOwner owner, const char *name,
            int)
    {
        return addAttribute(gdp, owner, name, GB_ATTRIB_INDEX, sizeof(int));
    }
    static GA_RWAttributeRef
    findIntTuple(GU_Detail &gdp, GEO_AttributeOwner owner,
        const char *name, int size)
    {
        return findAttribute(gdp, owner, name, GB_ATTRIB_INT, sizeof(int)*size);
    }
    static GA_RWAttributeRef
    addIntTuple(GU_Detail &gdp, GEO_AttributeOwner owner,
            const char *name, int size)
    {
        return addAttribute(gdp, owner, name, GB_ATTRIB_INT, sizeof(int)*size);
    }
    static GA_RWAttributeRef
    findFloatTuple(GU_Detail &gdp, GEO_AttributeOwner owner,
            const char *name, int size)
    {
        GA_RWAttributeRef h;
        h = findAttribute(gdp, owner, name, GB_ATTRIB_FLOAT,
                sizeof(float)*size);
        if (!h.isValid() && size == 3)
        {
            h = findAttribute(gdp, owner, name, GB_ATTRIB_VECTOR,
                    sizeof(float)*size);
        }
        return h;
    }
    static GA_RWAttributeRef
    addFloatTuple(GU_Detail &gdp, GEO_AttributeOwner owner,
            const char *name, int size, bool normal)
    {
        if (normal)
        {
            return addAttribute(gdp, owner, name, GB_ATTRIB_VECTOR,
                    sizeof(float)*size);
        }
        return addAttribute(gdp, owner, name, GB_ATTRIB_FLOAT,
                sizeof(float)*size);
    }
    static void
    setTypeInfo(GA_RWAttributeRef &, GA_TypeInfo)
    {
    }
    static void
    addToGroup(GB_PrimitiveGroup *group, GEO_Primitive *gb)
    {
        group->add(gb, 0); // Un-ordered group
    }
#endif

GA_ROAttributeRef  SOP_AlembicIn::attachDetailStringData(
        const std::string &attrName, const std::string &value)
{
    GA_RWAttributeRef attrIdx = findStringTuple(*gdp, GEO_DETAIL_DICT,
            attrName.c_str());
    if (attrIdx.isInvalid())
    {
        attrIdx = addStringTuple(*gdp, GEO_DETAIL_DICT, attrName.c_str(), 1);
        
        if (error() >= UT_ERROR_ABORT || attrIdx.isInvalid())
        {
            std::ostringstream buffer;
            buffer << "Could not create detail string attribute: ";
            buffer << attrName;
            addWarning(SOP_MESSAGE, buffer.str().c_str());
        }
    }
#if UT_MAJOR_VERSION_INT >= 12
    const GA_AIFStringTuple *aifstring = attrIdx.getAIFStringTuple();
    if (aifstring)
    {
        aifstring->setString(attrIdx.getAttribute(), GA_Offset(0),
                value.c_str(), 0);
    }
    else
    {
       return GA_ROAttributeRef();
    }
#else
    GB_Attribute *atr = gdp->attribs().findByOffset(attrIdx);
    if (atr)
    {
        gdp->attribs().getElement().setValue<int>(
                attrIdx, atr->addIndex(value.c_str()));
    }
    else
    {
       return GBmakeInvalidAttributeRef();
    }
#endif
    
    return attrIdx;
}

//-*****************************************************************************

void SOP_AlembicIn::walkObject( Args & args, IObject parent, const ObjectHeader &ohead,
            PathList::const_iterator I, PathList::const_iterator E,
                    M44d parentXform, bool parentXformIsConstant)
{
    if ( args.boss->opInterrupt() )
    {
        throw InterruptedException(
                parent.getFullName() + ohead.getName() );
    }
    
    //set this if we should continue traversing
    IObject nextParentObject;
    
    if ( IXform::matches( ohead ) )
    {
        if (args.includeXform)
        {
            
            IXform xform( parent, ohead.getName() );
            IXformSchema &xs = xform.getSchema();
            if (!xs.isConstant())
            {
                args.isConstant = false;
                parentXformIsConstant = false;
            }
            
            XformSample xformSample = xs.getValue(
                    ISampleSelector( args.abcTime ));
            
            M44d m = xformSample.getMatrix();
            
            if (xformSample.getInheritsXforms())
            {
                parentXform = m * parentXform;
            }
            else
            {
                parentXform = m;
            }
            
            nextParentObject = xform;
        }
        else
        {
            //if we're not processing transforms, just grab the child object
            //and move on
            nextParentObject = IObject(parent, ohead.getName());
        }
    }
    else if ( ISubD::matches( ohead ) )
    {
        ISubD subd( parent, ohead.getName() );
        ISubDSchema &ss = subd.getSchema();
        if (ss.getTopologyVariance() == kHeterogenousTopology)
        {
            args.isTopologyConstant = false;
        }
        
        buildSubD( args, subd, parentXform, parentXformIsConstant );
        
        nextParentObject = subd;
    }
    else if ( IPolyMesh::matches( ohead ) )
    {
        IPolyMesh polymesh( parent, ohead.getName() );
        IPolyMeshSchema &ps = polymesh.getSchema();
        if (ps.getTopologyVariance() == kHeterogenousTopology)
        {
            args.isTopologyConstant = false;
        }
        
        buildPolyMesh( args, polymesh, parentXform, parentXformIsConstant );
        
        nextParentObject = polymesh;
    }
    else
    {
        //For now, silently skip types we don't recognize
    }
    
    
    
    if ( nextParentObject.valid() )
    {
        if ( I == E )
        {
            for ( size_t i = 0; i < nextParentObject.getNumChildren(); ++i )
            {
                walkObject( args, nextParentObject,
                        nextParentObject.getChildHeader( i ), I, E,
                                parentXform, parentXformIsConstant);
            }
        }
        else
        {
            const ObjectHeader *nextChildHeader =
                nextParentObject.getChildHeader( *I );
            
            if ( nextChildHeader != NULL )
            {
                walkObject( args, nextParentObject,
                        *nextChildHeader, I+1, E,
                                parentXform, parentXformIsConstant);
            }
        }
    }
}

//-*****************************************************************************

std::string SOP_AlembicIn::getFullName( IObject object )
{
    std::string result = object.getFullName();
    
    if (result.size() > 4 && result.substr(0, 5) == "/ABC/")
    {
        return result.substr( 4, result.size() - 1 );
    }
    
    return result;
}

//-*****************************************************************************

void SOP_AlembicIn::addUVs(Args & args, IV2fGeomParam param,
        size_t startPointIdx, size_t endPointIdx,
        size_t startPrimIdx, size_t endPrimIdx)
{
    if (!param.valid()) { return; }
    
    GA_RWAttributeRef uvAttrIndex;
    switch (param.getScope())
    {
    case kVaryingScope:
    case kVertexScope:
    {
        addOrFindTextureAttribute(GEO_POINT_DICT, uvAttrIndex);
        break;
    }
    case kFacevaryingScope:
    {
        addOrFindTextureAttribute(GEO_VERTEX_DICT, uvAttrIndex);
        break;
    }
    default:
        break;
    }
    
    if (uvAttrIndex.isValid())
    {
        processArbitraryGeomParam<IV2fGeomParam, float>(
            args,
            param.getParent(),
            param.getHeader(),
            GA_STORE_REAL32,
            GA_TYPE_VOID,
            uvAttrIndex,
            startPointIdx, endPointIdx,
            startPrimIdx, endPrimIdx,
            false);
    }
}

//-*****************************************************************************

void SOP_AlembicIn::addNormals(Args & args, IN3fGeomParam param,
        size_t startPointIdx, size_t endPointIdx,
        size_t startPrimIdx, size_t endPrimIdx,
        bool parentXformIsConstant)
{
    if (!param.valid()) { return; }
    
    GA_RWAttributeRef nAttrIndex;
    switch (param.getScope())
    {
    case kVaryingScope:
    case kVertexScope:
    {
        addOrFindNormalAttribute(GEO_POINT_DICT, nAttrIndex);
        break;
    }
    case kFacevaryingScope:
    {
        addOrFindNormalAttribute(GEO_VERTEX_DICT, nAttrIndex);
        break;
    }
    default:
        break;
    }
    
    if (nAttrIndex.isValid())
    {
        processArbitraryGeomParam<IN3fGeomParam, float>(
                args,
                param.getParent(),
                param.getHeader(),
                GA_STORE_REAL32,
                GA_TYPE_NORMAL,
                nAttrIndex,
                startPointIdx, endPointIdx,
                startPrimIdx, endPrimIdx,
                parentXformIsConstant);
    }
    
}

//-*****************************************************************************

bool SOP_AlembicIn::addOrFindTextureAttribute(GEO_AttributeOwner owner,
        GA_RWAttributeRef & attrIdx)
{
    attrIdx = gdp->findTextureAttribute(owner);
    
    if (!attrIdx.isValid())
    {
        attrIdx = gdp->addTextureAttribute(owner);
        
        if (error() >= UT_ERROR_ABORT || !attrIdx.isValid())
        {
            addError(SOP_MESSAGE, "could not create texture attribute.");
            return false;
        }
    }
    return true;
}

//-*****************************************************************************

bool SOP_AlembicIn::addOrFindNormalAttribute(GEO_AttributeOwner owner,
        GA_RWAttributeRef & attrIdx)
{
    attrIdx = gdp->findNormalAttribute(owner);
    
    if (!attrIdx.isValid())
    {
        attrIdx = gdp->addNormalAttribute(owner);
 
        if (error() >= UT_ERROR_ABORT || !attrIdx.isValid())
        {
            addError(SOP_MESSAGE, "could not create Normal attribute.");
            return false;
        }
    }
    return true;
}

//-*****************************************************************************
void SOP_AlembicIn::addArbitraryGeomParams(Args & args,
        ICompoundProperty parent,
        size_t startPointIdx, size_t endPointIdx,
        size_t startPrimIdx, size_t endPrimIdx,
        bool parentXformIsConstant)
{
    if (!parent.valid())
    {
        return;
    }
    
    for (size_t i = 0; i < parent.getNumProperties(); ++i)
    {
        const PropertyHeader &propHeader = parent.getPropertyHeader(i);
        
        if (IFloatGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IFloatGeomParam, float>(
                    args, parent, propHeader,
                    GA_STORE_REAL32,
                    GA_TYPE_VOID,
                    GA_RWAttributeRef(),
                    startPointIdx, endPointIdx,
                    startPrimIdx, endPrimIdx,
                    false);
        }
        else if (IInt32GeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IInt32GeomParam, int>(
                    args, parent, propHeader,
                    GA_STORE_INT32,
                    GA_TYPE_VOID,
                    GA_RWAttributeRef(),
                    startPointIdx, endPointIdx,
                    startPrimIdx, endPrimIdx,
                    false);
        }
        else if (IStringGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IStringGeomParam, std::string>(
                    args, parent, propHeader,
                    GA_STORE_STRING,
                    GA_TYPE_VOID,
                    GA_RWAttributeRef(),
                    startPointIdx, endPointIdx,
                    startPrimIdx, endPrimIdx,
                    false);
        }
        else if (IV2fGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IV2fGeomParam, float>(
                    args, parent, propHeader,
                    GA_STORE_REAL32,
                    GA_TYPE_VOID,
                    GA_RWAttributeRef(),
                    startPointIdx, endPointIdx,
                    startPrimIdx, endPrimIdx,
                    false);
        }
        else if (IV3fGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IV3fGeomParam, float>(
                    args, parent, propHeader,
                    GA_STORE_REAL32,
                    GA_TYPE_VECTOR,
                    GA_RWAttributeRef(),
                    startPointIdx, endPointIdx,
                    startPrimIdx, endPrimIdx,
                    parentXformIsConstant);
        }
        else if (IN3fGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IN3fGeomParam, float>(
                    args, parent, propHeader,
                    GA_STORE_REAL32,
                    GA_TYPE_NORMAL,
                    GA_RWAttributeRef(),
                    startPointIdx, endPointIdx,
                    startPrimIdx, endPrimIdx,
                    parentXformIsConstant);
        }
        else if (IC3fGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IC3fGeomParam, float>(
                    args, parent, propHeader,
                    GA_STORE_REAL32,
                    GA_TYPE_COLOR,
                    GA_RWAttributeRef(),
                    startPointIdx, endPointIdx,
                    startPrimIdx, endPrimIdx,
                    false);
        }
        else if (IP3fGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IP3fGeomParam, float>(
                    args, parent, propHeader,
                    GA_STORE_REAL32,
                    GA_TYPE_POINT,
                    GA_RWAttributeRef(),
                    startPointIdx, endPointIdx,
                    startPrimIdx, endPrimIdx,
                    false);
        }
    }
}

//-*****************************************************************************

template <typename geomParamT, typename podT>
void SOP_AlembicIn::processArbitraryGeomParam(
        Args & args,
        ICompoundProperty parent,
        const PropertyHeader & propHeader,
        GA_Storage attrStorage,
        GA_TypeInfo attrTypeInfo,
        const GA_RWAttributeRef & existingAttr,
        size_t startPointIdx, size_t endPointIdx,
        size_t startPrimIdx, size_t endPrimIdx,
        bool parentXformIsConstant
)
{
    GA_RWAttributeRef attrIdx = existingAttr;
    
    geomParamT param(parent, propHeader.getName());
    
    // If the param isn't changing and isn't affected by animation in inherited
    // transformations, exit early
    if (param.isConstant())
    {
        if (args.reusePrimitives &&
                (!args.includeXform || parentXformIsConstant))
        {
            return;
        }
    }
    
    ISampleSelector sampleSelector( args.abcTime );
    typename geomParamT::sample_type paramSample;
    param.getExpanded(paramSample, sampleSelector);
    
    size_t extent = geomParamT::prop_type::traits_type::dataType().getExtent();
    size_t arrayExtent = param.getArrayExtent();
    size_t totalExtent = extent*arrayExtent;
    
    GEO_AttributeOwner owner = GEO_POINT_DICT;
    switch (paramSample.getScope())
    {
    case kVaryingScope:
    case kVertexScope:
        // TODO: For NURBS, kVertexScope should use GEO_VERTEX_DICT
        owner = GEO_POINT_DICT;
        break;
    case kFacevaryingScope:
        owner = GEO_VERTEX_DICT;
        break;
    case kUniformScope:
    case kConstantScope:
    case kUnknownScope:
        owner = GEO_PRIMITIVE_DICT;
    }
    
    if (!attrIdx.isValid())
    {
        std::string aname = propHeader.getName();
        std::map<std::string,std::string>::const_iterator it =
                args.nameMap->find(aname);
        if (it != args.nameMap->end())
        {
            aname = it->second;
        }
        if (GAisIntStorage(attrStorage))
        {
            attrIdx = findIntTuple(*gdp, owner, aname.c_str(), totalExtent);
            if (!attrIdx.isValid())
            {
                attrIdx = addIntTuple(*gdp, owner,
                        aname.c_str(), totalExtent);
            }
        }
        else if (GAisFloatStorage(attrStorage))
        {
            attrIdx = findFloatTuple(*gdp, owner, aname.c_str(),
                    totalExtent);
            if (!attrIdx.isValid())
            {
                attrIdx = addFloatTuple(*gdp, owner,
                        aname.c_str(), totalExtent,
                        attrTypeInfo == GA_TYPE_NORMAL);
            }
        }
        else
        {
            UT_ASSERT(attrStorage == GA_STORE_STRING);
            attrIdx = findStringTuple(*gdp, owner, aname.c_str(),
                    totalExtent);
            if (!attrIdx.isValid())
            {
                attrIdx = addStringTuple(*gdp, owner,
                        aname.c_str(), totalExtent);
            }
        }
    }
    
    if (!attrIdx.isValid())
    {
        //TODO, indicate as error, for now just skip it
        return;
    }
    
    setTypeInfo(attrIdx, attrTypeInfo);
    applyArbitraryGeomParamSample<typename geomParamT::sample_type, podT>(
            paramSample,
            attrIdx,
            totalExtent,
            startPointIdx, endPointIdx,
            startPrimIdx, endPrimIdx);
}

//-*****************************************************************************

template <typename geomParamSampleT, typename podT>
void SOP_AlembicIn::applyArbitraryGeomParamSample(
        geomParamSampleT & paramSample,
        const GA_RWAttributeRef & attrIdx,
        size_t totalExtent,
        size_t startPointIdx, size_t endPointIdx,
        size_t startPrimIdx, size_t endPrimIdx
)
{
    switch (paramSample.getScope())
    {
    case kUniformScope:
    {
        const podT * values =reinterpret_cast<const podT *>(
                paramSample.getVals()->get());
        
        size_t i = 0;
        for (size_t primIdx = startPrimIdx;
                primIdx < endPrimIdx; ++primIdx, ++i)
        {
            GEO_Primitive *prim = gdp->primitives()(primIdx);
            prim->set(attrIdx, values+i*totalExtent, totalExtent);
        }
        
        break;
    }
    case kVaryingScope:
    case kVertexScope:
    {
        const podT * values = reinterpret_cast<const podT *>(
                paramSample.getVals()->get());
        
#if UT_MAJOR_VERSION_INT < 12
        size_t i = 0;
        for (size_t pointIdx = startPointIdx;
                pointIdx < endPointIdx; ++pointIdx, ++i)
        {
            GEO_Point *point = gdp->points()(pointIdx);
            point->set(attrIdx, values+i*totalExtent, totalExtent);
        }
#else
        GA_RWHandleTS<podT>	h(attrIdx.getAttribute());
        int	tsize = SYSmin((int)totalExtent, attrIdx.getTupleSize());
        for (size_t i = 0, pointIdx = startPointIdx;
                pointIdx < endPointIdx; ++pointIdx, ++i)
        {
            GA_Offset pt = gdp->pointOffset(GA_Index(pointIdx));
            h.setV(pt, values+i*totalExtent, tsize);
        }
#endif
        
        break;
    }
    case kFacevaryingScope:
    {
        const podT * values = reinterpret_cast<const podT *>(
                    paramSample.getVals()->get());
        
        size_t vertexIdx = 0;
        for (size_t primIdx = startPrimIdx;
                primIdx < endPrimIdx; ++primIdx)
        {
            GEO_Primitive *prim = gdp->primitives()(primIdx);
            
            int vtxCount = prim->getVertexCount();
            for(int i=0; i < vtxCount; ++i, ++vertexIdx)
            {
                prim->getVertex(i).set(attrIdx, values+vertexIdx*totalExtent, totalExtent);
            }
        }
        
        break;
    }
    
    case kConstantScope:
    default:
    {
        const podT * values = reinterpret_cast<const podT *>(
                paramSample.getVals()->get());
        for (size_t primIdx = startPrimIdx;
                primIdx < endPrimIdx; ++primIdx)
        {
            GEO_Primitive *prim = gdp->primitives()(primIdx);
            prim->set(attrIdx, values, totalExtent);
        }
        
        break;
    }
    }
}


//-*****************************************************************************
//specialized for handling the string case
template <>
void SOP_AlembicIn::applyArbitraryGeomParamSample<
            IStringGeomParam::sample_type, std::string>(
        IStringGeomParam::sample_type & paramSample,
        const GA_RWAttributeRef & attrIdx,
        size_t totalExtent,
        size_t startPointIdx, size_t endPointIdx,
        size_t startPrimIdx, size_t endPrimIdx
)
{//TACO
    switch (paramSample.getScope())
    {
    case kUniformScope:
    {
        const std::string * values =reinterpret_cast<const std::string *>(
                paramSample.getVals()->get());
        
#if UT_MAJOR_VERSION_INT < 12
        GB_Attribute *attr = gdp->primitiveAttribs().findByOffset(attrIdx);
#endif
        size_t i = 0;
        for (size_t primIdx = startPrimIdx;
                primIdx < endPrimIdx; ++primIdx, ++i)
        {
            GEO_Primitive *prim = gdp->primitives()(primIdx);
            
            for (size_t j = 0; j < totalExtent; ++j)
            {
#if UT_MAJOR_VERSION_INT >= 12
                prim->setString(attrIdx, values[i*totalExtent+j].c_str(), j);
#else
                prim->setValue<int>(attrIdx,
                        attr->addIndex(values[i*totalExtent+j].c_str()), j);
#endif
            }
        }
        break;
    }
    case kVaryingScope:
    case kVertexScope:
    {
        const std::string * values = reinterpret_cast<const std::string *>(
                paramSample.getVals()->get());
        
        size_t i = 0;
#if UT_MAJOR_VERSION_INT < 12
        GB_Attribute *attr = gdp->pointAttribs().findByOffset(attrIdx);
#else
        const GA_AIFStringTuple	*stuple = attrIdx.getAIFStringTuple();
        GA_Attribute *attr = attrIdx.getAttribute();
#endif
        for (size_t pointIdx = startPointIdx;
                pointIdx < endPointIdx; ++pointIdx, ++i)
        {
#if UT_MAJOR_VERSION_INT >= 12
            GA_Offset pt = gdp->pointOffset(GA_Index(pointIdx));
            for (size_t j = 0; j < totalExtent; ++j)
            {
                stuple->setString(attr, pt, values[i*totalExtent+j].c_str(), j);
            }
#else
            GEO_Point *point = gdp->points()(pointIdx);
            for (size_t j = 0; j < totalExtent; ++j)
            {
                point->setValue<int>(attrIdx,
                        attr->addIndex(values[i*totalExtent+j].c_str()), j);
            }
#endif
        }
        
        break;
    }
    case kFacevaryingScope:
    {
        const std::string * values = reinterpret_cast<const std::string *>(
                    paramSample.getVals()->get());
        
        size_t vertexIdx = 0;
#if UT_MAJOR_VERSION_INT < 12
        GB_Attribute *attr = gdp->pointAttribs().findByOffset(attrIdx);
#endif
        for (size_t primIdx = startPrimIdx;
                primIdx < endPrimIdx; ++primIdx)
        {
            GEO_Primitive *prim = gdp->primitives()(primIdx);
            
            int vtxCount = prim->getVertexCount();
            for(int i=0; i < vtxCount; ++i, ++vertexIdx)
            {
                for (size_t j = 0; j < totalExtent; ++j)
                {
#if UT_MAJOR_VERSION_INT >= 12
                    GEO_Vertex vtx = prim->getVertex(i);
                    vtx.setString(attrIdx,
                            values[vertexIdx*totalExtent+j].c_str(), j);
#else
                    GEO_Vertex &vtx = prim->getVertex(i);
                    vtx.setValue<int>(attrIdx,
                            attr->addIndex(values[i*totalExtent+j].c_str()), j);
#endif
                }
            }
        }
        
        break;
    }
    
    case kConstantScope:
    default:
    {
        const std::string * values = reinterpret_cast<const std::string *>(
                paramSample.getVals()->get());
        
#if UT_MAJOR_VERSION_INT < 12
        GB_Attribute *attr = gdp->primitiveAttribs().findByOffset(attrIdx);
#endif
        for (size_t primIdx = startPrimIdx;
                primIdx < endPrimIdx; ++primIdx)
        {
            GEO_Primitive *prim = gdp->primitives()(primIdx);

            for (size_t j = 0; j < totalExtent; ++j)
            {
#if UT_MAJOR_VERSION_INT >= 12
                prim->setString(attrIdx, values[j].c_str(), j);
#else
                prim->setValue<int>(attrIdx,
                        attr->addIndex(values[j].c_str()), j);
#endif
            }
            
        }
        
        break;
    }
    }
}

//-*****************************************************************************

void SOP_AlembicIn::buildSubD( Args & args, ISubD &subd, M44d parentXform, bool parentXformIsConstant)
{
    ISampleSelector sampleSelector( args.abcTime );
    ISubDSchema &ss = subd.getSchema();
    if (!ss.isConstant())
    {
        args.isConstant = false;
    }
    // If the schema is constant and we've already cooked once, we can exit
    // early assuming that we're either not baking in tranforms or the inherited
    // transformations are constant.
    else if (args.reusePrimitives)
    {
        if (!args.includeXform || parentXformIsConstant)
        {
            return;
        }
    }
    
    //store the primitive and point start indices
    size_t startPointIdx = args.pointCount;
    size_t startPrimIdx = args.primCount;

    GA_PrimitiveGroup * primGrp;
    if (args.reusePrimitives)
    {
        Abc::P3fArraySamplePtr pSample =
                ss.getPositionsProperty().getValue(sampleSelector);
        
        primGrp = reuseMesh(getFullName(subd), pSample, startPointIdx);
        
        args.pointCount += pSample->size(); // Add # points
        
        
        
        //always present in myPrimitiveCountCache if args.reusePrimitives
        args.primCount += myPrimitiveCountCache[getFullName(subd)];
        
    }
    else
    {
        ISubDSchema::Sample sample = ss.getValue( sampleSelector );
        
        args.pointCount += sample.getPositions()->size(); // Add # points
        args.primCount += sample.getFaceCounts()->size(); // Add # faces
        
        
        primGrp = buildMesh(getFullName(subd),
                sample.getPositions(), sample.getFaceCounts(),
                sample.getFaceIndices(), startPointIdx);
    }
    
    size_t endPointIdx = args.pointCount;
    size_t endPrimIdx = args.primCount;
    
    
    addUVs(args, subd.getSchema().getUVsParam(),
            startPointIdx, endPointIdx, startPrimIdx, endPrimIdx);
    addArbitraryGeomParams(args, subd.getSchema().getArbGeomParams(),
            startPointIdx, endPointIdx, startPrimIdx, endPrimIdx,
                    parentXformIsConstant);
    
    //apply xforms via gdp->transform so that we don't have to think
    //about normals and other affected attributes
    if (args.includeXform && parentXform != M44d())
    {
        UT_DMatrix4 dxform(parentXform.x);
        UT_Matrix4 xform(dxform);
        gdp->transform(xform, primGrp);
    }
    
    //subd tagging?
}

//-*****************************************************************************

void SOP_AlembicIn::buildPolyMesh( Args & args, IPolyMesh & polymesh,
        M44d parentXform, bool parentXformIsConstant)
{
    ISampleSelector sampleSelector( args.abcTime );
    IPolyMeshSchema &schema = polymesh.getSchema();
    if (!schema.isConstant())
    {
        args.isConstant = false;
    }
    // If the schema is constant and we've already cooked once, we can exit
    // early assuming that we're either not baking in tranforms or the inherited
    // transformations are constant.else if (args.reusePrimitives)
    else if (args.reusePrimitives)
    {
        if (!args.includeXform || parentXformIsConstant)
        {
            return;
        }
    }
    
    
    //store the primitive and point start indices
    size_t startPointIdx = args.pointCount;
    size_t startPrimIdx = args.primCount;

    GA_PrimitiveGroup * primGrp;
    if (args.reusePrimitives)
    {
        Abc::P3fArraySamplePtr pSample =
                schema.getPositionsProperty().getValue(sampleSelector);
        
        primGrp = reuseMesh(getFullName(polymesh), pSample, startPointIdx);
        
        args.pointCount += pSample->size();     // Add # points
        
        //always present in myPrimitiveCountCache if args.reusePrimitives
        args.primCount += myPrimitiveCountCache[getFullName(polymesh)];
    }
    else
    {
        IPolyMeshSchema::Sample sample = schema.getValue( sampleSelector );
        
        args.pointCount += sample.getPositions()->size();   // Add # points
        args.primCount += sample.getFaceCounts()->size();   // Add # faces
        
        primGrp = buildMesh(getFullName(polymesh),
                sample.getPositions(), sample.getFaceCounts(),
                sample.getFaceIndices(), startPointIdx);
    }
    
    
    size_t endPointIdx = args.pointCount;
    size_t endPrimIdx = args.primCount;
    
    
    addUVs(args, polymesh.getSchema().getUVsParam(),
            startPointIdx, endPointIdx, startPrimIdx, endPrimIdx);
    addNormals(args, polymesh.getSchema().getNormalsParam(),
            startPointIdx, endPointIdx, startPrimIdx, endPrimIdx,
                    parentXformIsConstant);
    
    addArbitraryGeomParams(args, polymesh.getSchema().getArbGeomParams(),
            startPointIdx, endPointIdx, startPrimIdx, endPrimIdx,
                    parentXformIsConstant);
    
    //apply xforms via gdp->transform so that we don't have to think
    //about normals and other affected attributes
    if (args.includeXform && parentXform != M44d())
    {
        UT_DMatrix4 dxform(parentXform.x);
        UT_Matrix4 xform(dxform);
        gdp->transform(xform, primGrp);
    }
    
}

//-*****************************************************************************

GA_PrimitiveGroup * SOP_AlembicIn::reuseMesh(const std::string & groupName,
        P3fArraySamplePtr positions, size_t startPointIdx)
{
    for (size_t i = 0, e = positions->size(); i < e; ++i)
    {
#if UT_MAJOR_VERSION_INT >= 12
        GA_Offset pt = gdp->pointOffset(GA_Index(startPointIdx+i));
        gdp->setPos3(pt, 
                (*positions)[i][0],
                (*positions)[i][1],
                (*positions)[i][2]);
#else
        GEO_Point *pt = gdp->points()(startPointIdx+i);
        pt->setPos(UT_Vector3(
                (*positions)[i][0],
                (*positions)[i][1],
                (*positions)[i][2]));
#endif
    }
    
    return gdp->findPrimitiveGroup( groupName.c_str() );
}

GA_PrimitiveGroup * SOP_AlembicIn::buildMesh(
        const std::string & groupName, P3fArraySamplePtr positions,
        Int32ArraySamplePtr counts, Int32ArraySamplePtr indicies,
        size_t startPointIdx)
{
    myPrimitiveCountCache[groupName] = counts->size();

    for ( size_t i = 0, e = positions->size(); i < e; ++i )
    {
#if UT_MAJOR_VERSION_INT >= 12
        GA_Offset pt = gdp->appendPointOffset();
        gdp->setPos3(pt, 
                (*positions)[i][0],
                (*positions)[i][1],
                (*positions)[i][2]);
#else
        GEO_Point *pt = gdp->appendPoint();
        pt->setPos(UT_Vector3(
                (*positions)[i][0],
                (*positions)[i][1],
                (*positions)[i][2]));
#endif
    }
    
    GA_PrimitiveGroup *primGrp = 0;
    
    if ( !groupName.empty() )
    {
        primGrp = gdp->newPrimitiveGroup( groupName.c_str() );
    }
    
    size_t npolys = counts->size();
    
    uint32_t currentVtxIndex = 0;
    for ( size_t i = 0; i < npolys; ++i )
    {
        uint32_t numPointsInFace = (*counts)[i];
        
        GU_PrimPoly *poly = GU_PrimPoly::build(gdp,
                numPointsInFace, GU_POLY_CLOSED, 0);
        
        for ( uint32_t ptN = 0; ptN < numPointsInFace;
                ++ptN, ++currentVtxIndex )
        {
#if UT_MAJOR_VERSION_INT >= 12
            GA_Index idx = GA_Index((*indicies)[currentVtxIndex]
                    + startPointIdx);
            poly->setVertexPoint(ptN, gdp->pointOffset(idx));
#else
            GEO_Point *point = gdp->points()(
                    (*indicies)[currentVtxIndex] + startPointIdx);
            poly->getVertex(ptN).setPt(point);
#endif
        }
        
        if ( primGrp )
        {
            addToGroup(primGrp, poly);
        }
    }
    
    return primGrp;
}

//-*****************************************************************************

void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        "Alembic_In",             // Internal name
        "Alembic_In",            // GUI name
        SOP_AlembicIn::myConstructor,   // Op Constructr
        SOP_AlembicIn::myTemplateList,  // GUI Definition
        0,0,                         // Min,Max # of Inputs
        0,OP_FLAG_GENERATOR)         // Local Variables/Generator
    );
}
namespace
{
    PY_PyObject *
    Py_AlembicGetLocalXform(PY_PyObject *self, PY_PyObject *args)
    {
        const char * archivePath = NULL;
        const char * objectPath = NULL;
        double sampleTime = 0.0;
        bool isConstant = true;
        M44d localXform;
        if (!PY_PyArg_ParseTuple(args, "ssd", &archivePath, &objectPath,
                &sampleTime)) return NULL;
        try
        {
            ArchiveCacheEntryRcPtr cacheEntry = LoadArchive(archivePath);
            if (cacheEntry->archive.valid() )
            {
                IObject root = cacheEntry->archive.getTop();
                SOP_AlembicIn::PathList pathList;
                TokenizeObjectPath(objectPath, pathList);
                IObject currentObject = root;
                for (SOP_AlembicIn::PathList::const_iterator I = pathList.begin();
                        I != pathList.end() && currentObject.valid(); ++I)
                {
                    currentObject = currentObject.getChild(*I);
                }
                if (currentObject.valid() &&
                        IXform::matches( currentObject.getHeader()))
                {
                    IXform xformObject(currentObject, kWrapExisting);
                    isConstant = xformObject.getSchema().isConstant();
                    XformSample xformSample = xformObject.getSchema().getValue(
                            ISampleSelector(sampleTime));
                    localXform = xformSample.getMatrix();
                }
            }
        }
        catch (const std::exception & e)
        {
        }
        PY_PyObject* matrixTuple = PY_PyTuple_New(16);
        for (PY_Py_ssize_t i = 0; i < 16; ++i)
        {
            PY_PyTuple_SET_ITEM(matrixTuple, i,
                    PY_PyFloat_FromDouble(localXform[i / 4][i % 4]));
        }
        PY_PyObject* result = PY_PyTuple_New(2);
        PY_PyTuple_SET_ITEM(result, 0, matrixTuple);
        PY_PyTuple_SET_ITEM(result, 1, PY_PyInt_FromLong(isConstant));
        return result;
    }
    //-*************************************************************************
    PY_PyObject * Py_AlembicWalkNode(IObject obj)
    {
        PY_PyObject* result = PY_PyTuple_New(3);
        PY_PyTuple_SET_ITEM(result, 0,
                    PY_PyString_FromString(obj.getName().c_str()));
        const AbcA::ObjectHeader & header = obj.getHeader();
        if (IXform::matches(header))
        {
            PY_PyTuple_SET_ITEM(result, 1,
                    PY_PyString_FromString("xform"));
        }
        else if (IPolyMesh::matches(header))
        {
            PY_PyTuple_SET_ITEM(result, 1,
                    PY_PyString_FromString("polymesh"));
        }
        else if (ISubD::matches(header))
        {
            PY_PyTuple_SET_ITEM(result, 1,
                    PY_PyString_FromString("subdmesh"));
        }
        else if (ICamera::matches(header))
        {
            PY_PyTuple_SET_ITEM(result, 1,
                    PY_PyString_FromString("camera"));
        }
        else if (IFaceSet::matches(header))
        {
            PY_PyTuple_SET_ITEM(result, 1,
                    PY_PyString_FromString("faceset"));
        }
        else if (ICurves::matches(header))
        {
            PY_PyTuple_SET_ITEM(result, 1,
                    PY_PyString_FromString("curves"));
        }
        else if (INuPatch::matches(header))
        {
            PY_PyTuple_SET_ITEM(result, 1,
                    PY_PyString_FromString("nupatch"));
        }
        else
        {
            PY_PyTuple_SET_ITEM(result, 1,
                    PY_PyString_FromString("unknown"));
        }
        PY_PyObject* childList = PY_PyTuple_New(
                obj.getNumChildren());
        PY_PyTuple_SET_ITEM(result, 2, childList);
        for (size_t i = 0, e = obj.getNumChildren(); i < e; ++i)
        {
            PY_PyTuple_SET_ITEM(childList, i,
                    Py_AlembicWalkNode(obj.getChild(i)));
        }
        return result;
    }
    //-*************************************************************************
    PY_PyObject *
    Py_AlembicGetSceneHierarchy(PY_PyObject *self, PY_PyObject *args)
    {
        const char * archivePath = NULL;
        const char * objectPath = NULL;
        if (!PY_PyArg_ParseTuple(args, "ss", &archivePath, &objectPath
                )) return NULL;
        try
        {
            ArchiveCacheEntryRcPtr cacheEntry = LoadArchive(archivePath);
            
            if (cacheEntry->archive.valid())
            {
                IObject root = cacheEntry->archive.getTop();
            SOP_AlembicIn::PathList pathList;
            TokenizeObjectPath(objectPath, pathList);
            IObject currentObject = root;
            for (SOP_AlembicIn::PathList::const_iterator I = pathList.begin();
                    I != pathList.end() && currentObject.valid(); ++I)
            {
                currentObject = currentObject.getChild(*I);
            }
            return Py_AlembicWalkNode(currentObject);
        }
        }
        catch (const std::exception & e)
        {
            PY_PyErr_SetString(PY_PyExc_RuntimeError(), e.what());
            return NULL;
        }
        PY_Py_RETURN_NONE;
    }
    //-*************************************************************************
    
    PY_PyObject *
    Py_AlembicClearArchiveCache(PY_PyObject *self, PY_PyObject *args)
    {
        g_archiveCache->clear();
        PY_Py_RETURN_NONE;
    }
    
    PY_PyObject *
    Py_AlembicSetArchiveMaxCacheSize(PY_PyObject *self, PY_PyObject *args)
    {
        unsigned int value;
        if (!PY_PyArg_ParseTuple(args, "I", &value)) return NULL;
        
        g_maxCache = value;
        
        PY_Py_RETURN_NONE;
    }
    
    PY_PyObject *
    Py_AlembicGetArchiveMaxCacheSize(PY_PyObject *self, PY_PyObject *args)
    {
        return PY_PyInt_FromLong((long) g_maxCache);
    }
    
    //-*************************************************************************
    
    void Py_AlembicWalkNodeForMenu(IObject obj, PY_PyObject * result,
            bool isRoot=false)
    {
        //For now, depth first. Hopefully a tree later.
        if (!isRoot)
        {
            PY_PyObject * fullName = PY_PyString_FromString(
                    obj.getFullName().c_str());
            //add twice because the menu input wants that.
            PY_PyList_Append(result, fullName);
            PY_PyList_Append(result, fullName);
            PY_Py_DECREF(fullName);
        }
        for (size_t i = 0, e = obj.getNumChildren(); i < e; ++i)
        {
            Py_AlembicWalkNodeForMenu(obj.getChild(i), result);
        }
    }
    
    //-*************************************************************************
    
    PY_PyObject *
    Py_AlembicGetObjectPathListForMenu(PY_PyObject *self, PY_PyObject *args)
    {
        const char * archivePath = NULL;
        if (!PY_PyArg_ParseTuple(args, "s", &archivePath)) return NULL;
        PY_PyObject * result = NULL;
        try
        {
            ArchiveCacheEntryRcPtr cacheEntry = LoadArchive(archivePath);
            if (cacheEntry->objectPathMenuList != NULL)
            {
                result = cacheEntry->objectPathMenuList;
                PY_Py_INCREF(result);
            }
            else
            {
                result = PY_PyList_New(0);
                cacheEntry->objectPathMenuList = result;
                PY_Py_INCREF(result);
                if (cacheEntry->archive.valid())
                {
                    IObject root = cacheEntry->archive.getTop();
                    Py_AlembicWalkNodeForMenu(root, result, /*isRoot=*/true);
                }
            }
        }
        catch (const std::exception & e)
        {
            //PY_PyErr_SetString(PY_PyExc_RuntimeError(), e.what());
            //return NULL;
        }
        if (result == NULL)
        {
            result = PY_PyList_New(0);
        }
        return result;
    }
    
    //-*************************************************************************
    
    PY_PyObject *
    Py_AlembicGetCameraDict(PY_PyObject *self, PY_PyObject *args)
    {
        const char * archivePath = NULL;
        const char * objectPath = NULL;
        double sampleTime = 0.0;
        
        if (!PY_PyArg_ParseTuple(args, "ssd", &archivePath, &objectPath,
                &sampleTime)) return NULL;
        
        bool isConstant = true;
        
        PY_PyObject * resultDict = PY_PyDict_New();
        
        try
        {
            ArchiveCacheEntryRcPtr cacheEntry = LoadArchive(archivePath);
            if (cacheEntry->archive.valid() )
            {
                IObject root = cacheEntry->archive.getTop();
                SOP_AlembicIn::PathList pathList;
                TokenizeObjectPath(objectPath, pathList);
                IObject currentObject = root;
                for (SOP_AlembicIn::PathList::const_iterator I = pathList.begin();
                        I != pathList.end() && currentObject.valid(); ++I)
                {
                    currentObject = currentObject.getChild(*I);
                }
                
                if (currentObject.valid() &&
                        ICamera::matches( currentObject.getHeader()))
                {
                    ICamera cameraObject(currentObject, kWrapExisting);
                    isConstant = cameraObject.getSchema().isConstant();
                    
                    CameraSample cameraSample = cameraObject.getSchema().getValue(
                            ISampleSelector(sampleTime));
                    
                    //Express in houdini terms?
                    
                    PY_PyObject * val = NULL;
                    
                    val = PY_PyFloat_FromDouble(cameraSample.getFocalLength());
                    PY_PyDict_SetItemString(resultDict, "focal", val);
                    PY_Py_DECREF(val);
                    
                    
                    
                    val = PY_PyFloat_FromDouble(cameraSample.getNearClippingPlane());
                    PY_PyDict_SetItemString(resultDict, "near", val);
                    PY_Py_DECREF(val);
                    
                    val = PY_PyFloat_FromDouble(cameraSample.getFarClippingPlane());
                    PY_PyDict_SetItemString(resultDict, "far", val);
                    PY_Py_DECREF(val);
                    
                    val = PY_PyFloat_FromDouble(cameraSample.getFocusDistance());
                    PY_PyDict_SetItemString(resultDict, "focus", val);
                    PY_Py_DECREF(val);
                    
                    double top, bottom, left, right;
                    cameraSample.getScreenWindow(top, bottom, left, right);
                    
                    double winx = cameraSample.getHorizontalFilmOffset() *
                            cameraSample.getLensSqueezeRatio() /
                                    cameraSample.getHorizontalAperture();
                    
                    double winy = cameraSample.getVerticalFilmOffset() *
                            cameraSample.getLensSqueezeRatio() /
                                    cameraSample.getVerticalAperture();
                    
                    //TODO, full 2D transformations
                    
                    Abc::V2d postScale(1.0, 1.0);
                    for ( size_t i = 0; i < cameraSample.getNumOps(); ++i )
                    {
                        const FilmBackXformOp & op = cameraSample.getOp(i);
                        
                        if ( op.isScaleOp() )
                        {
                            postScale *= op.getScale();
                        }
                    }
                    
                    //TODO overscan
                    double winsizex =
                            cameraSample.getLensSqueezeRatio() / postScale[0];
                    //TODO overscan
                    double winsizey =
                            cameraSample.getLensSqueezeRatio() / postScale[1];
                    
                    
                    
                    
                    
                    val = PY_PyFloat_FromDouble(winx);
                    PY_PyDict_SetItemString(resultDict, "winx", val);
                    PY_Py_DECREF(val);
                    
                    val = PY_PyFloat_FromDouble(winy);
                    PY_PyDict_SetItemString(resultDict, "winy", val);
                    PY_Py_DECREF(val);
                    
                    val = PY_PyFloat_FromDouble(winsizex);
                    PY_PyDict_SetItemString(resultDict, "winsizex", val);
                    PY_Py_DECREF(val);
                    
                    val = PY_PyFloat_FromDouble(winsizey);
                    PY_PyDict_SetItemString(resultDict, "winsizey", val);
                    PY_Py_DECREF(val);
                    
                    val = PY_PyFloat_FromDouble(
                            cameraSample.getHorizontalAperture()*10.0);
                    PY_PyDict_SetItemString(resultDict, "aperture", val);
                    PY_Py_DECREF(val);
                    
                    
                }
            }
        }
        catch (const std::exception & e)
        {
            PY_PyErr_SetString(PY_PyExc_RuntimeError(), e.what());
            return NULL;
        }
        
        return resultDict;
    }
    
    
    
    
}
void
HOMextendLibrary()
{
    {
    // A PY_InterpreterAutoLock will grab the Python global interpreter
    // lock (GIL).  It's important that we have the GIL before making
    // any calls into the Python API.
    PY_InterpreterAutoLock interpreter_auto_lock;
    // We'll create a new module named "_hom_extensions", and add functions
    // to it.  We don't give a docstring here because it's given in the
    // Python implementation below.
    static PY_PyMethodDef alembic_hom_extension_methods[] = {
        {"alembicGetLocalXform", Py_AlembicGetLocalXform,
                PY_METH_VARARGS(), ""},
        {"alembicGetSceneHierarchy", Py_AlembicGetSceneHierarchy,
                PY_METH_VARARGS(), ""},
        
        {"alembicClearArchiveCache", Py_AlembicClearArchiveCache,
                PY_METH_VARARGS(), ""},
        {"alembicSetArchiveMaxCacheSize", Py_AlembicSetArchiveMaxCacheSize,
                PY_METH_VARARGS(), ""},
        {"alembicGetArchiveMaxCacheSize", Py_AlembicGetArchiveMaxCacheSize,
                PY_METH_VARARGS(), ""},
        
        
        {"alembicGetObjectPathListForMenu", Py_AlembicGetObjectPathListForMenu,
                PY_METH_VARARGS(), ""},
        {"alembicGetCameraDict", Py_AlembicGetCameraDict,
                PY_METH_VARARGS(), ""},
        { NULL, NULL, 0, NULL }
    };
    PY_Py_InitModule("_alembic_hom_extensions", alembic_hom_extension_methods);
    }
    
    
    PYrunPythonStatementsAndExpectNoErrors(
    "def _alembicGetCameraDict(self, archivePath, objectPath, sampleTime):\n"
    "    '''Return camera information.'''\n"
    "    import _alembic_hom_extensions\n"
    "    return _alembic_hom_extensions.alembicGetCameraDict(archivePath, objectPath, sampleTime)\n"
    "__import__('hou').ObjNode.alembicGetCameraDict = _alembicGetCameraDict\n"
    "del _alembicGetCameraDict\n");
}
