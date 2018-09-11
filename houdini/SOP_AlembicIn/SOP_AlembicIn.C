//-*****************************************************************************
//
// Copyright (c) 2009-2013,
//  Sony Pictures Imageworks Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic, nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-*****************************************************************************

// Used for internal python bindings
#include <PY/PY_CPythonAPI.h>
// This file contains functions that will run arbitrary Python code
#include <PY/PY_Python.h>


#include "SOP_AlembicIn.h"

#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreFactory/All.h>

#include <UT/UT_StopWatch.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_Director.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_SpareData.h>


#include <GEO/GEO_PolyCounts.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimNURBSurf.h>
#include <GU/GU_PrimNURBCurve.h>
#include <GU/GU_PrimPart.h>
#include <GU/GU_PrimPoly.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_DMatrix4.h>

#include <UT/UT_CappedCache.h>

#include <HOM/HOM_Module.h>
#include <hboost/tokenizer.hpp>
#include <sstream>
#include <hboost/shared_ptr.hpp>

//-*****************************************************************************

namespace
{
    void TokenizeObjectPath(const std::string & objectPath,
            SOP_AlembicIn::PathList & pathList)
    {
        typedef hboost::char_separator<char> Separator;
        typedef hboost::tokenizer<Separator> Tokenizer;
        Tokenizer tokenizer( objectPath, Separator( "/" ) );
        for ( Tokenizer::iterator iter = tokenizer.begin() ;
                iter != tokenizer.end() ; ++iter )
        {
            if ( (*iter).empty() ) { continue; }
            pathList.push_back( *iter );
        }
    }

    class ArchiveObjectKey : public UT_CappedKey
    {
    public:
	ArchiveObjectKey(const char *key)
	    : UT_CappedKey()
	    , myKey(UT_String::ALWAYS_DEEP, key)
	{
	}
	virtual ~ArchiveObjectKey() {}
	virtual UT_CappedKey	*duplicate() const
				 {
				     return new ArchiveObjectKey(myKey);
				 }
	virtual unsigned int	 getHash() const	{ return myKey.hash(); }
	virtual bool		 isEqual(const UT_CappedKey &cmp) const
				 {
				     UT_ASSERT(dynamic_cast<const ArchiveObjectKey *>(&cmp));
				     const ArchiveObjectKey	*key = static_cast<const ArchiveObjectKey *>(&cmp);
				     return myKey == key->myKey;
				 }
    private:
	UT_String	myKey;
    };

    class ArchiveObjectItem : public UT_CappedItem
    {
    public:
	ArchiveObjectItem()
	    : UT_CappedItem()
	    , myIObject()
	{
	}
	ArchiveObjectItem(const IObject &obj)
	    : UT_CappedItem()
	    , myIObject(obj)
	{
	}
	// Approximate usage
	virtual int64	getMemoryUsage() const	{ return 1024; }
	IObject		getObject() const	{ return myIObject; }
    private:
	IObject		 myIObject;
    };

    struct ArchiveCacheEntry
    {
        ArchiveCacheEntry()
        : objectPathMenuList(NULL)
	, myCache("abcObjects", 2)
        {
        }

        ~ArchiveCacheEntry()
        {
            if (objectPathMenuList != NULL)
            {
                PY_Py_DECREF(objectPathMenuList);
            }
        }

	// Build a cache of constant transforms
	void	buildTransformCache(IObject root, const char *path)
	{
	    UT_WorkBuffer	fullpath;
	    for (size_t i = 0; i < root.getNumChildren(); ++i)
	    {
		const ObjectHeader	&ohead = root.getChildHeader(i);
		if (IXform::matches(ohead))
		{
		    IXform		xform(root, ohead.getName());
		    IXformSchema	&xs = xform.getSchema();
		    fullpath.sprintf("%s/%s", path, ohead.getName().c_str());
		    if (xs.isConstant())
		    {
			XformSample	xsample = xs.getValue(ISampleSelector(0.0));
			myTransforms[fullpath.buffer()] = xsample.getMatrix();
		    }
		    buildTransformCache(xform, fullpath.buffer());
		}
	    }
	}

	bool	findTransform(const char *fullpath, M44d &xform)
		{
		    if (myTransforms.size() == 0)
		    {
			IObject root = archive.getTop();
			buildTransformCache(root, "");
			//fprintf(stderr, "%d transforms\n", (int)myTransforms.size());
		    }
		    std::map<std::string, M44d>::const_iterator	it;
		    it = myTransforms.find(fullpath);
		    if (it != myTransforms.end())
		    {
			xform = it->second;
			return true;
		    }
		    return false;
		}

	IObject	findObject(IObject parent,
		    UT_WorkBuffer &fullpath, const char *component)
		{
		    fullpath.append("/");
		    fullpath.append(component);
		    ArchiveObjectKey	key(fullpath.buffer());
		    IObject		kid;
		    UT_CappedItemHandle	item = myCache.findItem(key);
		    if (item)
		    {
			kid = static_cast<ArchiveObjectItem *>(item.get())->getObject();
		    }
		    else
		    {
			kid = parent.getChild(component);
			if (kid.valid())
			    myCache.addItem(key, new ArchiveObjectItem(kid));
		    }
		    return kid;
		}

	IObject getObject(const std::string &objectPath)
	{
	    SOP_AlembicIn::PathList	pathList;
	    IObject			curr = archive.getTop();
	    UT_WorkBuffer		fullpath;

	    TokenizeObjectPath(objectPath, pathList);
	    for (SOP_AlembicIn::PathList::const_iterator I = pathList.begin();
		    I != pathList.end() && curr.valid(); ++I)
	    {
		curr = findObject(curr, fullpath, (*I).c_str());
	    }
	    return curr;
	}

        Abc::IArchive archive;
        std::string error;
        PY_PyObject * objectPathMenuList;
	std::map<std::string, M44d> myTransforms;
	UT_CappedCache	myCache;
    };

    typedef hboost::shared_ptr<ArchiveCacheEntry> ArchiveCacheEntryRcPtr;
    typedef std::map<std::vector<std::string>, ArchiveCacheEntryRcPtr> ArchiveCache;

    //-*************************************************************************

    size_t g_maxCache = 50;
    //for now, leak the pointer to the archive cache so we don't
    //crash at shutdown
    ArchiveCache * g_archiveCache(new ArchiveCache);

    //-*************************************************************************

    ArchiveCacheEntryRcPtr LoadArchive(const std::vector<std::string> & paths)
    {
        ArchiveCache::iterator I = g_archiveCache->find(paths);
        if (I != g_archiveCache->end())
        {
            return (*I).second;
        }
        ArchiveCacheEntryRcPtr entry = ArchiveCacheEntryRcPtr(
                new ArchiveCacheEntry);
        try
        {
            ::Alembic::AbcCoreFactory::IFactory factory;
            entry->archive = factory.getArchive( paths );
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
        (*g_archiveCache)[paths] = entry;
        return entry;
    }

    void	ClearArchiveFile(const std::vector<std::string> &paths)
    {
        ArchiveCache::iterator it = g_archiveCache->find(paths);
	if (it != g_archiveCache->end())
	    g_archiveCache->erase(it);
    }
    void	ClearArchiveCache()
    {
	delete g_archiveCache;
	g_archiveCache = new ArchiveCache;
    }

    //-**************************************************************************

    std::string filenamesToString(const std::vector<std::string> &filenames)
    {
	UT_WorkBuffer buffer;
	for (int i = 0; i < filenames.size(); ++i)
	{
	    if(i)
		buffer.append(", ");
	    buffer.append(filenames[i].c_str());
	}
	return buffer.toStdString();
    }
}

void
SOP_AlembicIn::clearCacheFile(const std::vector<std::string> *filenames)
{
    if (filenames)
	ClearArchiveFile(*filenames);
    else
	ClearArchiveCache();
}

void
SOP_AlembicIn::appendFileNames(std::vector<std::string> &filenames, fpreal t)
{
    int numlayers = evalInt("numlayers", 0, t);
    for (int i = 1; i <= numlayers; ++i)
    {
	if (evalIntInst("enablelayer#", &i, 0, t))
	{
	    UT_String fileName;
	    evalStringInst("layer#", &i, fileName, 0, t);
	    if (fileName.isstring())
		filenames.push_back(fileName.toStdString());
	}
    }

    UT_String name;
    evalString(name, "fileName", 0, t);
    if(name.isstring())
	filenames.push_back(name.toStdString());
}

// Class to help us walk the tree
//
// The sop_IAlembicWalker class needs to be in a public namespace for forward
// declarations.
class sop_IAlembicWalker
{
public:
    sop_IAlembicWalker(ArchiveCacheEntry &arch)
	: myArchive(arch)
	, myPathBuffer()
    {
    }
    IObject	nextObject(IObject parent, const std::string &component)
		{
		    return myArchive.findObject(parent, myPathBuffer,
				component.c_str());
		}
private:
    ArchiveCacheEntry	&myArchive;
    UT_WorkBuffer	myPathBuffer;
};

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
static PRM_Name prm_numlayersName("numlayers", "Number of Layers");
static PRM_Name prm_frameName("frame", "Frame");
static PRM_Name prm_fpsName("fps", "Frames Per Second");
static PRM_Name prm_objectPathName("objectPath", "Object Path");
static PRM_Name prm_includeXformName("includeXform", "Include Xform");
static PRM_Name prm_remapAttribName("remapAttributes", "Remap Attributes");

static PRM_Default prm_frameDefault(1, "$FF");
static PRM_Default prm_objectPathDefault(0, "");
static PRM_Default prm_fpsDefault(24, "$FPS");
static PRM_Default prm_includeXformDefault(true);

static PRM_ChoiceList	prm_objectPathMenu(PRM_CHOICELIST_TOGGLE,
        "def getFileName(node):\n"
	"    r = []\n"
	"    for i in range(node.evalParm('numlayers')):\n"
	"        if node.evalParm('enablelayer%d' % (i + 1,)):\n"
	"            p = node.evalParm('layer%d' % (i + 1,))\n"
	"            if p:\n"
	"                r.append(p)\n"
	"    p = node.evalParm('fileName')\n"
	"    if p:\n"
	"        r.append(p)\n"
	"    return r\n"
	"return __import__('_alembic_hom_extensions').alembicGetObjectPathListForMenu"
	"(getFileName(hou.pwd()))[:16380]",
	CH_PYTHON_SCRIPT);
// As of 16.5, Houdini has a limit of 16380 items in a UI menu, so we
// limit the list length to be safe

static PRM_Name prm_enablelayerName("enablelayer#", "");
static PRM_Name prm_layerName("layer#", "Layer #");

static PRM_Template prm_layersTemplate[] = {
    PRM_Template(PRM_TOGGLE, PRM_TYPE_TOGGLE_JOIN, 1, &prm_enablelayerName,
	    PRMoneDefaults),
    PRM_Template(PRM_FILE, 1, &prm_layerName),
    PRM_Template()
};

PRM_Template SOP_AlembicIn::myTemplateList[] =
{
    PRM_Template(PRM_FILE, 1, &prm_fileNameName),
    PRM_Template(PRM_MULTITYPE_LIST, prm_layersTemplate, 1,
            &prm_numlayersName, 0, 0, &PRM_SpareData::multiStartOffsetOne),
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
, myConstantUniqueId(-1)
, myConstantPointCount(0)
, myConstantPrimitiveCount(0)
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
    bool sop_flushed = false;

    const float now = context.getTime();

    args.includeXform = evalInt("includeXform", 0, now);
    if (gdp->getUniqueId() != myConstantUniqueId ||
	    gdp->getNumPoints() != myConstantPointCount ||
	    gdp->getNumPrimitives() != myConstantPrimitiveCount)
    {
	// When the SOP cache flushes my geometry, make sure to recreate
	// primitives etc.
	// This may also happen if the geometry is instanced by time shift or
	// by DOPs.
	sop_flushed = true;
    }

    std::vector<std::string> fileNames;
    appendFileNames(fileNames, now);

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
    fileobjecthash.sprintf("%s:%s:%d", filenamesToString(fileNames).c_str(), objectPath.c_str(),
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

    if (sop_flushed || strcmp(myFileObjectCache, fileobjecthash.buffer()) != 0)
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
	// std::cerr<<"clearing geo\n";
        gdp->clearAndDestroy();
    }
    else
    {
        gdp->destroyInternalNormalAttribute();
    }


    double fps = evalFloat("fps", 0, now);
    args.abcTime = evalFloat("frame", 0, now) / fps;
    args.isConstant = true;
    args.isTopologyConstant = true;
    args.reusePrimitives = myTopologyConstant && gdp->getNumPrimitives()>0;
    args.pointCount = 0;
    args.primCount = 0;
    args.nameMap = &nameMap;
    args.boss = UTgetInterrupt();
    args.activePatchRows = 0;
    args.activePatchCols = 0;


    if (!args.boss->opStart("Loading and walking Alembic data"))
    {
        args.boss->opEnd();
        return error();
    }

    if (fileNames.empty())
    {
        args.boss->opEnd();
        addWarning(SOP_MESSAGE, "No files specified.");
        return error();
    }

    try
    {
        ArchiveCacheEntryRcPtr cacheEntry = LoadArchive(fileNames);

        if (!cacheEntry->archive.valid() )
        {
            std::ostringstream buffer;
            buffer << "Alembic exception: ";
            buffer << cacheEntry->error;
            addWarning(SOP_MESSAGE, buffer.str().c_str());
            args.boss->opEnd();
            return error();
        }

        attachDetailStringData("abcFileName", filenamesToString(fileNames));

        IObject root = cacheEntry->archive.getTop();

        if ( pathList.empty() ) //walk the entire scene
        {
	    sop_IAlembicWalker	walker(*cacheEntry);
            for ( size_t i = 0; i < root.getNumChildren(); ++i )
            {
                walkObject( args, walker, root, root.getChildHeader(i),
                            pathList.end(), pathList.end(), xform, true);
            }
        }
        else //walk to a location + its children
        {
	    sop_IAlembicWalker	walker(*cacheEntry);
            PathList::const_iterator I = pathList.begin();

            const ObjectHeader *nextChildHeader = root.getChildHeader( *I );
            if ( nextChildHeader != NULL )
            {
                walkObject( args, walker, root, *nextChildHeader,
			I+1, pathList.end(), xform, true);
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
    catch ( const InterruptedException & /*e*/ )
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
    if (myTopologyConstant)
    {
	myConstantPointCount = gdp->getNumPoints();
	myConstantPrimitiveCount = gdp->getNumPrimitives();
	myConstantUniqueId = gdp->getUniqueId();
    }

    args.boss->opEnd();
    return error();
}

//-*****************************************************************************

void SOP_AlembicIn::nodeUnlocked()
{
}

//-*****************************************************************************

    static GA_RWAttributeRef
    findStringTuple(GU_Detail &gdp, GA_AttributeOwner owner,
            const char *name, int min_size=1)
    {
        return gdp.findStringTuple(owner, name, min_size);
    }
    static GA_RWAttributeRef
    addStringTuple(GU_Detail &gdp, GA_AttributeOwner owner,
            const char *name, int size)
    {
        return gdp.addStringTuple(owner, name, size);
    }
    static GA_RWAttributeRef
    findIntTuple(GU_Detail &gdp, GA_AttributeOwner owner,
            const char *name, int min_size)
    {
        return gdp.findIntTuple(owner, name, min_size);
    }
    static GA_RWAttributeRef
    addIntTuple(GU_Detail &gdp, GA_AttributeOwner owner,
            const char *name, int size)
    {
        return gdp.addIntTuple(owner, name, size);
    }
    static GA_RWAttributeRef
    addInt64Tuple(GU_Detail &gdp, GA_AttributeOwner owner,
            const char *name, int size)
    {
        return gdp.addIntTuple(owner, name, size,
		GA_Defaults(0),
		NULL,
		NULL,
		GA_STORE_INT64);
    }
    static GA_RWAttributeRef
    findFloatTuple(GU_Detail &gdp, GA_AttributeOwner owner,
            const char *name, int min_size)
    {
        return gdp.findFloatTuple(owner, name, min_size);
    }
    static GA_RWAttributeRef
    addFloatTuple(GU_Detail &gdp, GA_AttributeOwner owner,
            const char *name, int size, bool normal)
    {
        GA_RWAttributeRef h = gdp.addFloatTuple(owner, name, size);
        if (h.isValid() && normal)
        {
            h.getAttribute()->setTypeInfo(GA_TYPE_NORMAL);
        }
        return h;
    }
    static GA_RWAttributeRef
    addVelocity(GU_Detail &gdp, GA_AttributeOwner owner)
    {
	return gdp.addVelocityAttribute(owner);
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

GA_ROAttributeRef  SOP_AlembicIn::attachDetailStringData(
        const std::string &attrName, const std::string &value)
{
    GA_RWAttributeRef attr = findStringTuple(*gdp, GA_ATTRIB_DETAIL,
            attrName.c_str());
    if (attr.isInvalid())
    {
        attr = addStringTuple(*gdp, GA_ATTRIB_DETAIL, attrName.c_str(), 1);

        if (error() >= UT_ERROR_ABORT || attr.isInvalid())
        {
            std::ostringstream buffer;
            buffer << "Could not create detail string attribute: ";
            buffer << attrName;
            addWarning(SOP_MESSAGE, buffer.str().c_str());
        }
    }
    const GA_AIFStringTuple *aifstring = attr.getAIFStringTuple();
    if (aifstring)
    {
        aifstring->setString(attr.getAttribute(), GA_Offset(0),
                value.c_str(), 0);
    }
    else
    {
       return GA_ROAttributeRef();
    }

    return attr;
}

//-*****************************************************************************

void SOP_AlembicIn::walkObject( Args & args, sop_IAlembicWalker &walker,
	IObject parent, const ObjectHeader &ohead,
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
	    nextParentObject = walker.nextObject(parent, ohead.getName());
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
    else if ( ICurves::matches( ohead) )
    {
	ICurves curves(parent, ohead.getName());
	ICurvesSchema &cs = curves.getSchema();
	if (cs.getTopologyVariance() == kHeterogenousTopology)
	{
	    args.isTopologyConstant = false;
	}
	buildCurves( args, curves, parentXform, parentXformIsConstant );

	nextParentObject = curves;
    }
    else if ( IPoints::matches( ohead) )
    {
	IPoints points(parent, ohead.getName());
	IPointsSchema &ps = points.getSchema();
	if (!ps.isConstant())
	{
	    args.isTopologyConstant = false;
	}
	buildPoints( args, points, parentXform, parentXformIsConstant );

	nextParentObject = points;
    }
    else if ( INuPatch::matches( ohead ) )
    {
        INuPatch nupatch( parent, ohead.getName() );

        if ( nupatch.getSchema().getTopologyVariance()
                == kHeterogenousTopology )
        {
            args.isTopologyConstant = false;
        }

        buildNuPatch( args, nupatch, parentXform, parentXformIsConstant );

        nextParentObject = nupatch;
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
                walkObject( args, walker, nextParentObject,
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
                walkObject( args, walker, nextParentObject,
                        *nextChildHeader, I+1, E,
                                parentXform, parentXformIsConstant);
            }
        }
    }
}

//-*****************************************************************************

static std::string
fixAttributeName(const std::string &name)
{
    // Houdini 12 only allows "variable" style names for group/attribute names
    // Note that this may cause group collisions with trees like:
    //   - /ABC/foo/bar
    //   - /ABC/foo_bar
    //   - /ABC/foo.bar
    // since all three paths will be converted to _ABC_foo_bar
    //
    // The right thing todo is to maintain a map of groupnames to paths so that
    // we can export the correct node path.
    UT_String	var(name.c_str());
    if (var.forceValidVariableName())
	return std::string((const char *)var);
    return name;
}

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
        GA_Offset startPointOfs, GA_Offset endPointOfs,
        GA_Offset startPrimOfs, GA_Offset endPrimOfs)
{
    if (!param.valid()) { return; }

    GA_RWAttributeRef uvAttr;
    switch (param.getScope())
    {
    case kVaryingScope:
    case kVertexScope:
    {
        addOrFindTextureAttribute(GA_ATTRIB_POINT, uvAttr);
        break;
    }
    case kFacevaryingScope:
    {
        addOrFindTextureAttribute(GA_ATTRIB_VERTEX, uvAttr);
        break;
    }
    default:
        break;
    }

    if (uvAttr.isValid())
    {
        processArbitraryGeomParam<IV2fGeomParam, float>(
            args,
            param.getParent(),
            param.getHeader(),
            GA_STORE_REAL32,
            GA_TYPE_VOID,
            uvAttr,
            startPointOfs, endPointOfs,
            startPrimOfs, endPrimOfs,
            false);
    }
}

void SOP_AlembicIn::addWidths(Args & args, IFloatGeomParam param,
        GA_Offset startPointOfs, GA_Offset endPointOfs,
        GA_Offset startPrimOfs, GA_Offset endPrimOfs)
{
    if (!param.valid()) { return; }

    GA_RWAttributeRef widthAttr;
    switch (param.getScope())
    {
    case kVaryingScope:
    case kVertexScope:
    {
        addOrFindWidthAttribute(GA_ATTRIB_POINT, widthAttr);
        break;
    }
    case kFacevaryingScope:
    {
        addOrFindWidthAttribute(GA_ATTRIB_VERTEX, widthAttr);
        break;
    }
    default:
        break;
    }

    if (widthAttr.isValid())
    {
        processArbitraryGeomParam<IFloatGeomParam, float>(
            args,
            param.getParent(),
            param.getHeader(),
            GA_STORE_REAL32,
            GA_TYPE_VOID,
            widthAttr,
            startPointOfs, endPointOfs,
            startPrimOfs, endPrimOfs,
            false);
    }
}


//-*****************************************************************************

void SOP_AlembicIn::addNormals(Args & args, IN3fGeomParam param,
        GA_Offset startPointOfs, GA_Offset endPointOfs,
        GA_Offset startPrimOfs, GA_Offset endPrimOfs,
        bool parentXformIsConstant)
{
    if (!param.valid()) { return; }

    GA_RWAttributeRef nAttr;
    switch (param.getScope())
    {
    case kVaryingScope:
    case kVertexScope:
    {
        addOrFindNormalAttribute(GA_ATTRIB_POINT, nAttr);
        break;
    }
    case kFacevaryingScope:
    {
        addOrFindNormalAttribute(GA_ATTRIB_VERTEX, nAttr);
        break;
    }
    default:
        break;
    }

    if (nAttr.isValid())
    {
        processArbitraryGeomParam<IN3fGeomParam, float>(
                args,
                param.getParent(),
                param.getHeader(),
                GA_STORE_REAL32,
                GA_TYPE_NORMAL,
                nAttr,
                startPointOfs, endPointOfs,
                startPrimOfs, endPrimOfs,
                parentXformIsConstant);
    }

}

//-*****************************************************************************

bool SOP_AlembicIn::addOrFindTextureAttribute(GA_AttributeOwner owner,
        GA_RWAttributeRef & attr)
{
    attr = gdp->findTextureAttribute(owner);

    if (!attr.isValid())
    {
        attr = gdp->addTextureAttribute(owner);

        if (error() >= UT_ERROR_ABORT || !attr.isValid())
        {
            addError(SOP_MESSAGE, "could not create texture attribute.");
            return false;
        }
    }
    return true;
}

bool SOP_AlembicIn::addOrFindWidthAttribute(GA_AttributeOwner owner,
        GA_RWAttributeRef & attr)
{
    attr = gdp->addTuple(GA_STORE_REAL32, owner, GA_SCOPE_PUBLIC,
	    "width", 1, GA_Defaults(0.1));
    if (error() >= UT_ERROR_ABORT || !attr.isValid())
    {
	addError(SOP_MESSAGE, "could not create width attribute.");
	return false;
    }
    return true;
}


//-*****************************************************************************

bool SOP_AlembicIn::addOrFindNormalAttribute(GA_AttributeOwner owner,
        GA_RWAttributeRef & attr)
{
    attr = gdp->findNormalAttribute(owner);

    if (!attr.isValid())
    {
        attr = gdp->addNormalAttribute(owner);

        if (error() >= UT_ERROR_ABORT || !attr.isValid())
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
        GA_Offset startPointOfs, GA_Offset endPointOfs,
        GA_Offset startPrimOfs, GA_Offset endPrimOfs,
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
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (IHalfGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IHalfGeomParam, fpreal16>(
                    args, parent, propHeader,
                    GA_STORE_REAL16,
                    GA_TYPE_VOID,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (IDoubleGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IDoubleGeomParam, double>(
                    args, parent, propHeader,
                    GA_STORE_REAL64,
                    GA_TYPE_VOID,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (ICharGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<ICharGeomParam, int8>(
                    args, parent, propHeader,
                    GA_STORE_INT8,
                    GA_TYPE_VOID,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (IInt16GeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IInt16GeomParam, int16>(
                    args, parent, propHeader,
                    GA_STORE_INT16,
                    GA_TYPE_VOID,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (IInt32GeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IInt32GeomParam, int>(
                    args, parent, propHeader,
                    GA_STORE_INT32,
                    GA_TYPE_VOID,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (IInt64GeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IInt64GeomParam, int64>(
                    args, parent, propHeader,
                    GA_STORE_INT64,
                    GA_TYPE_VOID,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (IStringGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IStringGeomParam, std::string>(
                    args, parent, propHeader,
                    GA_STORE_STRING,
                    GA_TYPE_VOID,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (IV2fGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IV2fGeomParam, float>(
                    args, parent, propHeader,
                    GA_STORE_REAL32,
                    GA_TYPE_VOID,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (IV2dGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IV2dGeomParam, double>(
                    args, parent, propHeader,
                    GA_STORE_REAL64,
                    GA_TYPE_VOID,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (IV3fGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IV3fGeomParam, float>(
                    args, parent, propHeader,
                    GA_STORE_REAL32,
                    GA_TYPE_VECTOR,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    parentXformIsConstant);
        }
        else if (IV3dGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IV3dGeomParam, double>(
                    args, parent, propHeader,
                    GA_STORE_REAL64,
                    GA_TYPE_VECTOR,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    parentXformIsConstant);
        }
        else if (IN3fGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IN3fGeomParam, float>(
                    args, parent, propHeader,
                    GA_STORE_REAL32,
                    GA_TYPE_NORMAL,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    parentXformIsConstant);
        }
        else if (IN3dGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IN3dGeomParam, double>(
                    args, parent, propHeader,
                    GA_STORE_REAL64,
                    GA_TYPE_NORMAL,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    parentXformIsConstant);
        }
        else if (IC3hGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IC3hGeomParam, fpreal16>(
                    args, parent, propHeader,
                    GA_STORE_REAL16,
                    GA_TYPE_COLOR,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (IC3fGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IC3fGeomParam, float>(
                    args, parent, propHeader,
                    GA_STORE_REAL32,
                    GA_TYPE_COLOR,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (IC3cGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IC3cGeomParam, uint8>(
                    args, parent, propHeader,
                    GA_STORE_INT8,
                    GA_TYPE_COLOR,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (IP3fGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IP3fGeomParam, float>(
                    args, parent, propHeader,
                    GA_STORE_REAL32,
                    GA_TYPE_POINT,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (IP3dGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IP3dGeomParam, double>(
                    args, parent, propHeader,
                    GA_STORE_REAL64,
                    GA_TYPE_POINT,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
                    false);
        }
        else if (IBoolGeomParam::matches(propHeader))
        {
            processArbitraryGeomParam<IBoolGeomParam, uint8>(
                    args, parent, propHeader,
                    GA_STORE_INT8,
                    GA_TYPE_VOID,
                    GA_RWAttributeRef(),
                    startPointOfs, endPointOfs,
                    startPrimOfs, endPrimOfs,
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
        GA_Offset startPointOfs, GA_Offset endPointOfs,
        GA_Offset startPrimOfs, GA_Offset endPrimOfs,
        bool parentXformIsConstant
)
{
    GA_RWAttributeRef attr = existingAttr;

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

    // If it's a standard array property, the number of elements in
    // the sample's data can be queried from the Dimensions object
    size_t dimPoints = 1;
    if (propHeader.isArray())
	dimPoints = paramSample.getVals().get()->getDimensions().numPoints();
    size_t totalExtent = extent*arrayExtent;

    GA_AttributeOwner owner = GA_ATTRIB_PRIMITIVE;
    switch (paramSample.getScope())
    {
    case kVaryingScope:
    case kVertexScope:
        // TODO: For NURBS, kVertexScope should use GA_ATTRIB_VERTEX
        owner = GA_ATTRIB_POINT;
        break;
    case kFacevaryingScope:
        owner = GA_ATTRIB_VERTEX;
        break;
    case kConstantScope:
    case kUnknownScope:
	// constant (and unknown) properties don't iterate over the
	// array data, so the whole data needs to get stored in the
	// primitive
	totalExtent *= dimPoints;
    case kUniformScope:
        owner = GA_ATTRIB_PRIMITIVE;
    }

    if (!attr.isValid())
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
            attr = findIntTuple(*gdp, owner, aname.c_str(), totalExtent);
            if (!attr.isValid())
            {
                attr = addIntTuple(*gdp, owner,
                        aname.c_str(), totalExtent);
            }
        }
        else if (GAisFloatStorage(attrStorage))
        {
            attr = findFloatTuple(*gdp, owner, aname.c_str(),
                    totalExtent);
            if (!attr.isValid())
            {
                attr = addFloatTuple(*gdp, owner,
                        aname.c_str(), totalExtent,
                        attrTypeInfo == GA_TYPE_NORMAL);
            }
        }
        else
        {
            UT_ASSERT(attrStorage == GA_STORE_STRING);
            attr = findStringTuple(*gdp, owner, aname.c_str(),
                    totalExtent);
            if (!attr.isValid())
            {
                attr = addStringTuple(*gdp, owner,
                        aname.c_str(), totalExtent);
            }
        }
    }

    if (!attr.isValid())
    {
        //TODO, indicate as error, for now just skip it
        return;
    }

    setTypeInfo(attr, attrTypeInfo);
    applyArbitraryGeomParamSample<typename geomParamT::sample_type, podT>(
            args,
            paramSample,
            attr,
            totalExtent,
            startPointOfs, endPointOfs,
            startPrimOfs, endPrimOfs);
}

//-*****************************************************************************

template <typename geomParamSampleT, typename podT>
void SOP_AlembicIn::applyArbitraryGeomParamSample(
        Args & args,
        geomParamSampleT & paramSample,
        const GA_RWAttributeRef & attr,
        size_t totalExtent,
        GA_Offset startPointOfs, GA_Offset endPointOfs,
        GA_Offset startPrimOfs, GA_Offset endPrimOfs
)
{
    GA_RWHandleT<podT> h(attr.getAttribute());
    int	tsize = SYSmin((int)totalExtent, attr.getTupleSize());
    switch (paramSample.getScope())
    {
    case kUniformScope:
    {
        const podT * values =reinterpret_cast<const podT *>(
                paramSample.getVals()->get());

        size_t i = 0;
        for (GA_Offset primofs = startPrimOfs; primofs < endPrimOfs; ++primofs, ++i)
        {
            h.setV(primofs, values+i*totalExtent, tsize);
        }

        break;
    }
    case kVaryingScope:
    case kVertexScope:
    {
        // TODO, for kVaryingScope on a nupatch, need to interpolate
        // from the corner values and apply to the points as normal
        // We'll know if we're a nupatch if args.activePatchRows > 0
        // For now, skip it
        if (args.activePatchRows > 0 &&
                paramSample.getScope() == kVaryingScope )
        {
            return;
        }


        const podT * values = reinterpret_cast<const podT *>(
                paramSample.getVals()->get());

        size_t i = 0;
        for (GA_Offset pt = startPointOfs; pt < endPointOfs; ++pt, ++i)
        {
            h.setV(pt, values+i*totalExtent, tsize);
        }

        break;
    }
    case kFacevaryingScope:
    {
        const podT * values = reinterpret_cast<const podT *>(
                    paramSample.getVals()->get());

        size_t vertexIdx = 0;
        for (GA_Offset primOfs = startPrimOfs;
                primOfs < endPrimOfs; ++primOfs)
        {
	    GEO_Primitive *prim = gdp->getGEOPrimitive(primOfs);

            int vtxCount = prim->getVertexCount();
            for(int i=0; i < vtxCount; ++i, ++vertexIdx)
            {
		h.setV(prim->getVertexOffset(i), values+vertexIdx*totalExtent, tsize);
            }
        }

        break;
    }

    case kConstantScope:
    case kUnknownScope:
    default:
    {
        const podT * values = reinterpret_cast<const podT *>(
                paramSample.getVals()->get());
        for (size_t primofs = startPrimOfs;
                primofs < endPrimOfs; ++primofs)
        {
            h.setV(primofs, values, tsize);
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
        Args & args,
        IStringGeomParam::sample_type & paramSample,
        const GA_RWAttributeRef & attr,
        size_t totalExtent,
        GA_Offset startPointOfs, GA_Offset endPointOfs,
        GA_Offset startPrimOfs, GA_Offset endPrimOfs
)
{//TACO
    GA_RWHandleS h(attr.getAttribute());
    switch (paramSample.getScope())
    {
    case kUniformScope:
    {
        const std::string * values =reinterpret_cast<const std::string *>(
                paramSample.getVals()->get());

        size_t i = 0;
        for (GA_Offset primofs = startPrimOfs;
                primofs < endPrimOfs; ++primofs, ++i)
        {
            for (size_t j = 0; j < totalExtent; ++j)
            {
		h.set(primofs, j, values[i*totalExtent+j].c_str());
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
        for (GA_Offset pt = startPointOfs;
                pt < endPointOfs; ++pt, ++i)
        {
            for (size_t j = 0; j < totalExtent; ++j)
            {
		h.set(pt, j, values[i*totalExtent+j].c_str());
            }
        }

        break;
    }
    case kFacevaryingScope:
    {
        const std::string * values = reinterpret_cast<const std::string *>(
                    paramSample.getVals()->get());

        size_t vertexIdx = 0;
        for (GA_Offset primofs = startPrimOfs;
                primofs < endPrimOfs; ++primofs)
        {
	    GEO_Primitive *prim = gdp->getGEOPrimitive(primofs);

            int vtxCount = prim->getVertexCount();
            for(int i=0; i < vtxCount; ++i, ++vertexIdx)
            {
                for (size_t j = 0; j < totalExtent; ++j)
                {
		    h.set(prim->getVertexOffset(i), j, values[vertexIdx*totalExtent+j].c_str());
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

        for (GA_Offset primofs = startPrimOfs;
                primofs < endPrimOfs; ++primofs)
        {
            for (size_t j = 0; j < totalExtent; ++j)
            {
		h.set(primofs, j, values[j].c_str());
            }

        }

        break;
    }
    }
}

//-*****************************************************************************

bool SOP_AlembicIn::updateParmsFlags()
{
    bool	changed = false;

    int numlayers = evalInt("numlayers", 0, 0);
    for (int i = 1; i <= numlayers; ++i)
	changed |= enableParmInst("layer#", &i, evalIntInst("enablelayer#", &i, 0, 0));
    return changed;
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
	    // Update point/prim counts
	    Abc::P3fArraySamplePtr pSample =
		    ss.getPositionsProperty().getValue(sampleSelector);
	    std::string	groupName = fixAttributeName(getFullName(subd));
	    args.pointCount += pSample->size();     // Add # points
	    args.primCount += myPrimitiveCountCache[groupName];
            return;
        }
    }

    //store the primitive and point start indices
    GA_Offset startPointOfs = args.pointCount;
    GA_Offset startPrimOfs = args.primCount;

    GA_PrimitiveGroup * primGrp;
    if (args.reusePrimitives)
    {
        Abc::P3fArraySamplePtr pSample =
                ss.getPositionsProperty().getValue(sampleSelector);

	std::string	groupName = fixAttributeName(getFullName(subd));
        primGrp = reuseMesh(groupName, pSample, startPointOfs);

        args.pointCount += pSample->size(); // Add # points



        //always present in myPrimitiveCountCache if args.reusePrimitives
        args.primCount += myPrimitiveCountCache[groupName];

    }
    else
    {
        ISubDSchema::Sample sample = ss.getValue( sampleSelector );

        args.pointCount += sample.getPositions()->size(); // Add # points
        args.primCount += sample.getFaceCounts()->size(); // Add # faces


	std::string	groupName = fixAttributeName(getFullName(subd));
        primGrp = buildMesh(groupName,
                sample.getPositions(), sample.getFaceCounts(),
                sample.getFaceIndices());
    }

    GA_Offset endPointOfs = args.pointCount;
    GA_Offset endPrimOfs = args.primCount;


    addUVs(args, subd.getSchema().getUVsParam(),
            startPointOfs, endPointOfs, startPrimOfs, endPrimOfs);
    addArbitraryGeomParams(args, subd.getSchema().getArbGeomParams(),
            startPointOfs, endPointOfs, startPrimOfs, endPrimOfs,
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
	    // Update point/prim counts
	    Abc::P3fArraySamplePtr pSample =
		    schema.getPositionsProperty().getValue(sampleSelector);
	    std::string	groupName = fixAttributeName(getFullName(polymesh));
	    args.pointCount += pSample->size();     // Add # points
	    args.primCount += myPrimitiveCountCache[groupName];
            return;
        }
    }


    //store the primitive and point start indices
    GA_Offset startPointOfs = args.pointCount;
    GA_Offset startPrimOfs = args.primCount;

    GA_PrimitiveGroup * primGrp;
    if (args.reusePrimitives)
    {
        Abc::P3fArraySamplePtr pSample =
                schema.getPositionsProperty().getValue(sampleSelector);

	std::string	groupName = fixAttributeName(getFullName(polymesh));
        primGrp = reuseMesh(groupName, pSample, startPointOfs);

        args.pointCount += pSample->size();     // Add # points

        //always present in myPrimitiveCountCache if args.reusePrimitives
        args.primCount += myPrimitiveCountCache[groupName];
    }
    else
    {
        IPolyMeshSchema::Sample sample = schema.getValue( sampleSelector );

        args.pointCount += sample.getPositions()->size();   // Add # points
        args.primCount += sample.getFaceCounts()->size();   // Add # faces

	std::string	groupName = fixAttributeName(getFullName(polymesh));
        primGrp = buildMesh(groupName,
                sample.getPositions(), sample.getFaceCounts(),
                sample.getFaceIndices());
    }


    GA_Offset endPointOfs = args.pointCount;
    GA_Offset endPrimOfs = args.primCount;

    addUVs(args, polymesh.getSchema().getUVsParam(),
            startPointOfs, endPointOfs, startPrimOfs, endPrimOfs);
    addNormals(args, polymesh.getSchema().getNormalsParam(),
            startPointOfs, endPointOfs, startPrimOfs, endPrimOfs,
                    parentXformIsConstant);

    addArbitraryGeomParams(args, polymesh.getSchema().getArbGeomParams(),
            startPointOfs, endPointOfs, startPrimOfs, endPrimOfs,
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

void SOP_AlembicIn::buildCurves( Args & args, ICurves & curves,
        M44d parentXform, bool parentXformIsConstant)
{
    ISampleSelector sampleSelector( args.abcTime );
    ICurvesSchema &schema = curves.getSchema();
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
	    // Update point/prim counts
	    Abc::P3fArraySamplePtr pSample =
		    schema.getPositionsProperty().getValue(sampleSelector);
	    std::string	groupName = fixAttributeName(getFullName(curves));
	    args.pointCount += pSample->size();     // Add # points
	    args.primCount += myPrimitiveCountCache[groupName];
            return;
        }
    }


    //store the primitive and point start indices
    GA_Offset startPointOfs = args.pointCount;
    GA_Offset startPrimOfs = args.primCount;

    GA_PrimitiveGroup * primGrp;
    if (args.reusePrimitives)
    {
        Abc::P3fArraySamplePtr pSample =
                schema.getPositionsProperty().getValue(sampleSelector);

	std::string	groupName = fixAttributeName(getFullName(curves));
        primGrp = reuseMesh(groupName, pSample, startPointOfs);

        args.pointCount += pSample->size();     // Add # points

        //always present in myPrimitiveCountCache if args.reusePrimitives
        args.primCount += myPrimitiveCountCache[groupName];
    }
    else
    {
        ICurvesSchema::Sample sample = schema.getValue( sampleSelector );

        args.pointCount += sample.getPositions()->size();   // Add # points
        args.primCount += sample.getNumCurves();   // Add # curves

	std::string	groupName = fixAttributeName(getFullName(curves));
        primGrp = buildCurves(groupName,
                sample.getPositions(), sample.getCurvesNumVertices());
    }


    GA_Offset endPointOfs = args.pointCount;
    GA_Offset endPrimOfs = args.primCount;


    addUVs(args, curves.getSchema().getUVsParam(),
            startPointOfs, endPointOfs, startPrimOfs, endPrimOfs);
    addNormals(args, curves.getSchema().getNormalsParam(),
            startPointOfs, endPointOfs, startPrimOfs, endPrimOfs,
                    parentXformIsConstant);
    addWidths(args, curves.getSchema().getWidthsParam(),
            startPointOfs, endPointOfs, startPrimOfs, endPrimOfs);

    addArbitraryGeomParams(args, curves.getSchema().getArbGeomParams(),
            startPointOfs, endPointOfs, startPrimOfs, endPrimOfs,
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

void SOP_AlembicIn::buildPoints( Args & args, IPoints & points,
        M44d parentXform, bool parentXformIsConstant)
{
    ISampleSelector sampleSelector( args.abcTime );
    IPointsSchema &schema = points.getSchema();
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
	    // Update point/prim counts
	    Abc::P3fArraySamplePtr pSample =
		    schema.getPositionsProperty().getValue(sampleSelector);
	    args.pointCount += pSample->size();     // Add # points
	    args.primCount += 1;
            return;
        }
    }


    //store the primitive and point start indices
    GA_Offset startPointOfs = args.pointCount;
    GA_Offset startPrimOfs = args.primCount;

    GA_PrimitiveGroup * primGrp;
    if (args.reusePrimitives)
    {
        Abc::P3fArraySamplePtr pSample =
                schema.getPositionsProperty().getValue(sampleSelector);

	std::string	groupName = fixAttributeName(getFullName(points));
        primGrp = reuseMesh(groupName, pSample, startPointOfs);

        args.pointCount += pSample->size();     // Add # points

        //always present in myPrimitiveCountCache if args.reusePrimitives
        args.primCount += 1;
    }
    else
    {
        IPointsSchema::Sample sample = schema.getValue( sampleSelector );

        args.pointCount += sample.getPositions()->size();   // Add # points
        args.primCount += 1;	// Add single particle system

	std::string	groupName = fixAttributeName(getFullName(points));
        primGrp = buildPoints(groupName,
		sample.getPositions(),
		sample.getIds(),
		sample.getVelocities());
    }

    GA_Offset endPointOfs = args.pointCount;
    GA_Offset endPrimOfs = args.primCount;

    addWidths(args, points.getSchema().getWidthsParam(),
            startPointOfs, endPointOfs, startPrimOfs, endPrimOfs);

    addArbitraryGeomParams(args, points.getSchema().getArbGeomParams(),
            startPointOfs, endPointOfs, startPrimOfs, endPrimOfs,
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


void SOP_AlembicIn::buildNuPatch( Args & args, INuPatch & nupatch,
        M44d parentXform, bool parentXformIsConstant)
{
    ISampleSelector sampleSelector( args.abcTime );
    INuPatchSchema &schema = nupatch.getSchema();
    if (!schema.isConstant())
    {
        args.isConstant = false;
    }
    else if (args.reusePrimitives)
    {
        if (!args.includeXform || parentXformIsConstant)
        {
	    Abc::P3fArraySamplePtr pSample =
		    schema.getPositionsProperty().getValue(sampleSelector);
	    args.pointCount += pSample->size();     // Add # points
	    args.primCount += 1;
            return;
        }
    }


    //store the primitive and point start indices
    GA_Offset startPointOfs = args.pointCount;
    GA_Offset startPrimOfs = args.primCount;


    GEO_PrimNURBSurf * surfPrim = 0;


    GA_PrimitiveGroup * primGrp = 0;
    if (args.reusePrimitives)
    {
        surfPrim = dynamic_cast<GEO_PrimNURBSurf *>(
	    gdp->getGEOPrimitiveByIndex(args.primCount));

        Abc::P3fArraySamplePtr pSample =
                schema.getPositionsProperty().getValue(sampleSelector);

        //only update point positions
        for (size_t i = 0, e = pSample->size(); i < e; ++i)
        {
	    GA_Offset pt = gdp->pointOffset(GA_Index(startPointOfs+i));
	    gdp->setPos3(pt,
		    (*pSample)[i][0],
		    (*pSample)[i][1],
		    (*pSample)[i][2]);
        }

	std::string groupName = fixAttributeName(nupatch.getFullName().c_str());
        primGrp = gdp->findPrimitiveGroup( groupName.c_str() );



        args.pointCount += pSample->size();     // Add # points
        args.primCount += 1;
    }
    else
    {
        INuPatchSchema::Sample sample = schema.getValue( sampleSelector );
        args.pointCount += sample.getPositions()->size();   // Add # points
        args.primCount += 1;



        int cols = sample.getUKnot()->size() - sample.getUOrder();
        int rows = sample.getVKnot()->size() - sample.getVOrder();


        GU_PrimNURBSurf *surf = GU_PrimNURBSurf::build(
                gdp,
                rows,                   // rows
                cols,                   // columns
                sample.getUOrder(),     // U order
                sample.getVOrder(),     // V order
                0,                      // wrap U
                0,                      // wrap V
                0,//(uClosed) ? 0:1,        // interpEndsU
                0,//(vClosed) ? 0:1,        // interpEndsV
                GEO_PATCH_QUADS);

        //copy in the u and v knots
	GA_KnotVector	&uknots = surf->getUBasis()->getKnotVector();
	GA_KnotVector	&vknots = surf->getVBasis()->getKnotVector();
	for (int i = 0; i < sample.getUKnot()->size(); ++i)
	    uknots.setValue(i, sample.getUKnot()->get()[i]);
	for (int i = 0; i < sample.getVKnot()->size(); ++i)
	    vknots.setValue(i, sample.getVKnot()->get()[i]);

        for (int v = 0; v < rows; ++v)
        {
            for( int u = 0; u < cols; ++u )
            {
                const V3f & p = sample.getPositions()->get()[v * cols + u];
		GA_Offset ptofs = surf->getPointOffset(v, u);
		gdp->setPos3(ptofs, p[0], p[1], p[2]);

                //std::cerr << "ptnum: " << surf->getPointIndex(v, u) << std::endl;
            }
        }

        //TODO, check for getPositionWeights()
        //For now, don't throw any W on there.
        surf->weights(false);

	std::string groupName = fixAttributeName(nupatch.getFullName().c_str());
        primGrp = gdp->newPrimitiveGroup( groupName.c_str() );
        addToGroup(primGrp, surf);


        surfPrim = surf;
    }


    GA_Offset endPointOfs = args.pointCount;
    GA_Offset endPrimOfs = args.primCount;


    if (surfPrim)
    {
        // Make sure this is reset back to 0 when we're done even in the case
        // of an exception.
        ArgsRowColumnReset argsReset(args,
                surfPrim->getNumRows(), surfPrim->getNumCols());
        addArbitraryGeomParams(args, nupatch.getSchema().getArbGeomParams(),
            startPointOfs, endPointOfs, startPrimOfs, endPrimOfs,
                    parentXformIsConstant);
    }

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
        P3fArraySamplePtr positions, GA_Offset startPointOfs)
{
    GA_Offset pt = startPointOfs;
    for (size_t i = 0, e = positions->size(); i < e; ++i, ++pt)
    {
        gdp->setPos3(pt,
                (*positions)[i][0],
                (*positions)[i][1],
                (*positions)[i][2]);
    }

    return gdp->findPrimitiveGroup( groupName.c_str() );
}

GA_PrimitiveGroup * SOP_AlembicIn::buildMesh(
        const std::string & groupName, P3fArraySamplePtr positions,
        Int32ArraySamplePtr counts, Int32ArraySamplePtr indices)
{
    myPrimitiveCountCache[groupName] = counts->size();

    GA_PrimitiveGroup *primGrp = 0;
    if ( !groupName.empty() )
    {
        primGrp = gdp->newPrimitiveGroup( groupName.c_str() );
    }

    exint totalnpts = positions->size();

    GA_Offset startPtOfs = gdp->appendPointBlock(totalnpts);
    if (startPtOfs == GA_INVALID_OFFSET)
        return NULL;
    GEO_PolyCounts primsizes;
    exint totalnprims = counts->size();
    for (exint i=0; i<totalnprims; ++i)
        primsizes.append((*counts)[i]);
    GA_Offset startPrimOfs =
        GEO_PrimPoly::buildBlock(gdp, startPtOfs, totalnpts, primsizes, indices->get());
    if (startPrimOfs == GA_INVALID_OFFSET)
        return NULL;

    GA_Offset pt=startPtOfs; 
    for (size_t i = 0, e = positions->size(); i < e; ++i,++pt)
    {
        gdp->setPos3(pt,
                (*positions)[i][0],
                (*positions)[i][1],
                (*positions)[i][2]);
    }
    if (primGrp)
    {
        GA_Offset primofs = startPrimOfs;
        for (size_t i = 0, e = totalnprims; i < e; ++i,++primofs)
            primGrp->addOffset(primofs);
    }
    return primGrp;
}

GA_PrimitiveGroup * SOP_AlembicIn::buildCurves(
        const std::string & groupName, P3fArraySamplePtr positions,
        Int32ArraySamplePtr counts)
{
    myPrimitiveCountCache[groupName] = counts->size();

    GA_PrimitiveGroup *primGrp = 0;
    if ( !groupName.empty() )
    {
        primGrp = gdp->newPrimitiveGroup( groupName.c_str() );
    }

    exint totalnpts = positions->size();

    GA_Offset startPtOfs = gdp->appendPointBlock(totalnpts);
    if (startPtOfs == GA_INVALID_OFFSET)
        return NULL;
    GEO_PolyCounts primsizes;
    exint totalnprims = counts->size();

    // Since ICurvesSchema doesn't provide indices, we need to build
    // it ourselves in order to use buildBlock()
    int totalnindices = 0;
    for (exint i=0; i<totalnprims; ++i)
    {
        primsizes.append((*counts)[i]);
        totalnindices += (*counts)[i];
    }
    std::vector<int32> indices;
    indices.resize(totalnindices);
    for (exint i=0; i<totalnindices; ++i)
        indices[i] = i;
    GA_Offset startPrimOfs =
        GEO_PrimPoly::buildBlock(gdp, startPtOfs, totalnpts, primsizes, &indices[0], false);
    if (startPrimOfs == GA_INVALID_OFFSET)
        return NULL;

    GA_Offset pt=startPtOfs; 
    for (size_t i = 0, e = positions->size(); i < e; ++i,++pt)
    {
        gdp->setPos3(pt,
                (*positions)[i][0],
                (*positions)[i][1],
                (*positions)[i][2]);
    }
    if (primGrp)
    {
        GA_Offset primofs = startPrimOfs;
        for (size_t i = 0, e = totalnprims; i < e; ++i,++primofs)
            primGrp->addOffset(primofs);
    }
    return primGrp;
}

GA_PrimitiveGroup * SOP_AlembicIn::buildPoints(
        const std::string & groupName,
	P3fArraySamplePtr positions,
	UInt64ArraySamplePtr ids,
	V3fArraySamplePtr velocities)
{
    myPrimitiveCountCache[groupName] = 1;
    size_t npts = positions->size();
    GA_RWAttributeRef idRef;
    GA_RWAttributeRef vRef;

    if (ids && ids->size() == positions->size())
	idRef = addInt64Tuple(*gdp, GA_ATTRIB_POINT, "id", 1);
    if (velocities && velocities->size() == positions->size())
	vRef = addVelocity(*gdp, GA_ATTRIB_POINT);
    GA_RWHandleID id_h;
    if (idRef.isValid())
        id_h.bind(idRef.getAttribute());
    GA_RWHandleV3 v_h;
    if (vRef.isValid())
        v_h.bind(vRef.getAttribute());

    GA_Offset startPtOfs = gdp->appendPointBlock(npts);
    if (startPtOfs == GA_INVALID_OFFSET)
        return NULL;

    GA_Offset pt = startPtOfs;
    for ( size_t i = 0; i < npts; ++i, ++pt )
    {
        gdp->setPos3(pt,
                (*positions)[i][0],
                (*positions)[i][1],
                (*positions)[i][2]);
	if (id_h.isValid())
	{
	    id_h.set(pt, (int64)(*ids)[i]);
	}
	if (v_h.isValid())
	{
	    v_h.set(pt, UT_Vector3F((*velocities)[i].getValue()));
	}
    }

    GA_PrimitiveGroup *primGrp = 0;

    if ( !groupName.empty() )
    {
        primGrp = gdp->newPrimitiveGroup( groupName.c_str() );
    }

    GU_PrimParticle	*part = GU_PrimParticle::build(gdp, npts, 0);

    if (primGrp)
    {
	addToGroup(primGrp, part);
    }

    pt = startPtOfs;
    for ( exint ptN = 0; ptN < npts; ++ptN, ++pt)
    {
	part->appendParticle(gdp->pointOffset(pt));
    }

    return primGrp;
}


//-*****************************************************************************

void
newSopOperator(OP_OperatorTable *table)
{
    OP_Operator *alembic_op = new OP_Operator(
        "ref_alembic",                      // Internal name
        "Reference Alembic",                      // GUI name
        SOP_AlembicIn::myConstructor,   // Op Constructr
        SOP_AlembicIn::myTemplateList,  // GUI Definition
        0, 0,                           // Min,Max # of Inputs
        0, OP_FLAG_GENERATOR);          // Local Variables/Generator
    alembic_op->setIconName("SOP_alembic");

    table->addOperator(alembic_op);
}
namespace
{
    void DecomposeXForm(
            const Imath::M44d &mat,
            Imath::V3d &scale,
            Imath::V3d &shear,
            Imath::Quatd &rotation,
            Imath::V3d &translation
    )
    {
        Imath::M44d mat_remainder(mat);

        // Extract Scale, Shear
        Imath::extractAndRemoveScalingAndShear(mat_remainder, scale, shear);

        // Extract translation
        translation.x = mat_remainder[3][0];
        translation.y = mat_remainder[3][1];
        translation.z = mat_remainder[3][2];

        // Extract rotation
        rotation = extractQuat(mat_remainder);
    }

    M44d RecomposeXForm(
            const Imath::V3d &scale,
            const Imath::V3d &shear,
            const Imath::Quatd &rotation,
            const Imath::V3d &translation
    )
    {
        Imath::M44d scale_mtx, shear_mtx, rotation_mtx, translation_mtx;

        scale_mtx.setScale(scale);
        shear_mtx.setShear(shear);
        rotation_mtx = rotation.toMatrix44();
        translation_mtx.setTranslation(translation);

        return scale_mtx * shear_mtx * rotation_mtx * translation_mtx;
    }


    // when amt is 0, a is returned
    inline double lerp(double a, double b, double amt)
    {
        return (a + (b-a)*amt);
    }

    Imath::V3d lerp(const Imath::V3d &a, const Imath::V3d &b, double amt)
    {
        return Imath::V3d(lerp(a[0], b[0], amt),
                          lerp(a[1], b[1], amt),
                          lerp(a[2], b[2], amt));
    }

//-*****************************************************************************
// Utility functions for the python bindings

    static void
    appendFile(std::vector<std::string> &filenames, const char *name)
    {
	UT_String realname;

	// complete a path search in case it is in the geometry path
	UT_PathSearch::getInstance(UT_HOUDINI_GEOMETRY_PATH)->
		findFile(realname, name);
	filenames.push_back(realname.toStdString());
    }

    static void
    appendFileList(std::vector<std::string> &filenames, PY_PyObject *fileList)
    {
	if (!fileList)
	    return;

	if (PY_PyString_Check(fileList))
	{
	    const char *fileStr = PY_PyString_AsString(fileList);
	    if (fileStr && strlen(fileStr))
		appendFile(filenames, fileStr);
	}
	else if (PY_PySequence_Check(fileList))
	{
	    int numFiles = PY_PySequence_Size(fileList);
	    for (int i = 0; i < numFiles; ++i)
	    {
		PY_PyObject *fileObj = PY_PySequence_GetItem(fileList, i);
		if (PY_PyString_Check(fileObj))
		{
		    const char *fileStr = PY_PyString_AsString(fileObj);
		    if (fileStr && strlen(fileStr))
			appendFile(filenames, fileStr);
		}
	    }
	}
    }

//-*****************************************************************************
// Python bindings

    PY_PyObject *
    Py_AlembicGetLocalXform(PY_PyObject *self, PY_PyObject *args)
    {
 	PY_PyObject *fileList;
        const char * objectPath = NULL;
        double sampleTime = 0.0;
        bool isConstant = true;
        M44d localXform;
        if (!PY_PyArg_ParseTuple(args, "Osd", &fileList, &objectPath,
                &sampleTime)) return NULL;
        try
        {
	    std::vector<std::string> filenames;
	    appendFileList(filenames, fileList);
	    if (!filenames.size())
		PY_Py_RETURN_NONE;
            ArchiveCacheEntryRcPtr cacheEntry = LoadArchive(filenames);
            if (cacheEntry->archive.valid() )
            {
		bool	found = false;
#if 0
		static int		count = 0;
		static UT_StopWatch	timer;
		if (count == 0)
		    timer.start();
		count++;
		if (count % 500 == 0)
		    fprintf(stderr, "%d calls %g\n", count, timer.lap());
#endif
                IObject currentObject;
		if (cacheEntry->findTransform(objectPath, localXform))
		    found = true;
		else
		{
		    currentObject = cacheEntry->getObject(objectPath);
		}
                if (!found && currentObject.valid() &&
                        IXform::matches( currentObject.getHeader()))
                {
                    IXform xformObject(currentObject, kWrapExisting);
                    isConstant = xformObject.getSchema().isConstant();



                    TimeSamplingPtr timeSampling = xformObject.getSchema().getTimeSampling();
                    size_t numSamples = xformObject.getSchema().getNumSamples();


                    chrono_t inTime = sampleTime;
                    chrono_t outTime = sampleTime;

                    if (numSamples > 1)
                    {
                        const chrono_t epsilon = 1.0 / 10000.0;

                        std::pair<index_t, chrono_t> floorIndex =
                                timeSampling->getFloorIndex(sampleTime, numSamples);

                        //make sure we're not equal enough
                        if (fabs(floorIndex.second - sampleTime) > epsilon)
                        {
                            //make sure we're not before the first sample
                            if (floorIndex.second < sampleTime)
                            {
                                //make sure there's another sample available afterwards
                                if (floorIndex.first+1 < numSamples)
                                {
                                    inTime = floorIndex.second;
                                    outTime = timeSampling->getSampleTime(
                                            floorIndex.first+1);
                                }
                            }
                        }
                    }

                    //interpolate if necessary
                    if (inTime != outTime )
                    {
                        XformSample inSample = xformObject.getSchema().getValue(
                                ISampleSelector(inTime));

                        XformSample outSample = xformObject.getSchema().getValue(
                                ISampleSelector(outTime));

                        double t = (sampleTime - inTime) / (outTime - inTime);

                        Imath::V3d s_l,s_r,h_l,h_r,t_l,t_r;
                        Imath::Quatd quat_l,quat_r;

                        DecomposeXForm(inSample.getMatrix(), s_l, h_l, quat_l, t_l);
                        DecomposeXForm(outSample.getMatrix(), s_r, h_r, quat_r, t_r);

                        if ((quat_l ^ quat_r) < 0)
                        {
                            quat_r = -quat_r;
                        }

                        localXform = RecomposeXForm(lerp(s_l, s_r, t),
                                 lerp(h_l, h_r, t),
                                 Imath::slerp(quat_l, quat_r, t),
                                 lerp(t_l, t_r, t));

                    }
                    else
                    {
                        XformSample xformSample = xformObject.getSchema().getValue(
                                ISampleSelector(sampleTime));
                        localXform = xformSample.getMatrix();
                    }
                }
            }
        }
        catch (const std::exception & /*e*/)
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
	    IXform xformObject(obj, kWrapExisting);
	    if (xformObject.getSchema().isConstant())
	    {
		// constant transform
		PY_PyTuple_SET_ITEM(result, 1,
			PY_PyString_FromString("cxform"));
	    }
	    else
	    {
		PY_PyTuple_SET_ITEM(result, 1,
			PY_PyString_FromString("xform"));
	    }
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
        else if (IPoints::matches(header))
        {
            PY_PyTuple_SET_ITEM(result, 1,
                    PY_PyString_FromString("points"));
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
 	PY_PyObject *fileList;
        const char * objectPath = NULL;
        if (!PY_PyArg_ParseTuple(args, "Os", &fileList, &objectPath
                )) return NULL;
        try
        {
	    std::vector<std::string> filenames;
	    appendFileList(filenames, fileList);
	    if (!filenames.size())
		PY_Py_RETURN_NONE;
            ArchiveCacheEntryRcPtr cacheEntry = LoadArchive(filenames);

            if (cacheEntry->archive.valid())
            {
		IObject currentObject = cacheEntry->getObject(objectPath);
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
 	PY_PyObject *fileList;

        if (!PY_PyArg_ParseTuple(args, "O", &fileList)) return NULL;
        PY_PyObject * result = NULL;
        try
        {
	    std::vector<std::string> filenames;
	    appendFileList(filenames, fileList);
	    if (!filenames.size())
		PY_Py_RETURN_NONE;
            ArchiveCacheEntryRcPtr cacheEntry = LoadArchive(filenames);
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
        catch (const std::exception & /*e*/)
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
 	PY_PyObject *fileList;
        const char * objectPath = NULL;
        double sampleTime = 0.0;

        if (!PY_PyArg_ParseTuple(args, "Osd", &fileList, &objectPath,
                &sampleTime)) return NULL;

        bool isConstant = true;

        PY_PyObject * resultDict = PY_PyDict_New();

        try
        {
	    std::vector<std::string> filenames;
	    appendFileList(filenames, fileList);
	    if (!filenames.size())
		PY_Py_RETURN_NONE;
            ArchiveCacheEntryRcPtr cacheEntry = LoadArchive(filenames);
            if (cacheEntry->archive.valid() )
            {
		IObject currentObject = cacheEntry->getObject(objectPath);

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
