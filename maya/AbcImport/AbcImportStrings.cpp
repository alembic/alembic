#include "AbcImportStrings.h"

#include <maya/MStringResource.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>

namespace AbcImportStrings
{
	#define kPluginId  "AbcImport"

	const MStringResourceId kErrorInvalidAlembic					( kPluginId, "kErrorInvalidAlembic", 				MString( "is not a valid Alembic file" ) );
	const MStringResourceId kErrorConnectionNotFound				( kPluginId, "kErrorConnectionNotFound", 			MString( "not found for connection" ) );
	const MStringResourceId kErrorConnectionNotMade					( kPluginId, "kErrorConnectionNotMade", 			MString( "connection not made" ) );
	const MStringResourceId kWarningNoAnimatedParticleSupport		( kPluginId, "kWarningNoAnimatedParticleSupport", 	MString( "Currently no support for animated particle system" ) );
	const MStringResourceId kWarningUnsupportedAttr					( kPluginId, "kWarningUnsupportedAttr", 	MString( "Unsupported attr, skipping: " ) );
	const MStringResourceId kWarningSkipIndexNonArray				( kPluginId, "kWarningSkipIndexNonArray", 	MString( "Skipping indexed or non-array property: " ) );
	const MStringResourceId kWarningSkipOddlyNamed					( kPluginId, "kWarningSkipOddlyNamed", 	MString( "Skipping oddly named property: " ) );
	const MStringResourceId kWarningSkipNoSamples					( kPluginId, "kWarningSkipNoSamples", 	MString( "Skipping property with no samples: " ) );
	const MStringResourceId kAEAlembicAttributes					( kPluginId, "kAEAlembicAttributes", 	MString( "Alembic Attributes" ) );
}

//String registration
MStatus AbcImportStrings::registerMStringResources(void)
{
	MStringResource::registerString( kErrorInvalidAlembic );
	MStringResource::registerString( kErrorConnectionNotFound );
	MStringResource::registerString( kErrorConnectionNotMade );
	MStringResource::registerString( kWarningNoAnimatedParticleSupport );	
	MStringResource::registerString( kWarningUnsupportedAttr );
	MStringResource::registerString( kWarningSkipIndexNonArray );
	MStringResource::registerString( kWarningSkipOddlyNamed );
	MStringResource::registerString( kWarningSkipNoSamples );
    MStringResource::registerString( kAEAlembicAttributes  );

	return MS::kSuccess;
}

//string retrieval
MString AbcImportStrings::getString(const MStringResourceId &stringId)
{
	MStatus status;
	return MStringResource::getString(stringId, status);
}

MString AbcImportStrings::replaceString(const MString& text, const MString& str, const MStringResourceId& stringId)
{
    MString replacedText;
    MString src = MString("\"") + str + "\"";
    MString dst = MString("\"") + getString(stringId) + "\"";

    // Find and replace the string
    MString prefix, suffix = text;
    int strIdx = -1;
    while ((strIdx = suffix.indexW(src)) >= 0) {
        prefix = (strIdx > 0) ? suffix.substringW(0, strIdx - 1) : "";
        suffix = (strIdx + src.numChars() < suffix.numChars()) ? 
            suffix.substringW(strIdx + src.numChars(), suffix.numChars() - 1) : "";
        replacedText += prefix;
        replacedText += dst;
        replacedText += suffix;
    }

    return replacedText;
}
