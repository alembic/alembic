#ifndef __AbcImportStrings_h__
#define __AbcImportStrings_h__

#include <maya/MStringResourceId.h>

class MString;
class MStringArray;

namespace AbcImportStrings
{
	extern const MStringResourceId kErrorInvalidAlembic;
	extern const MStringResourceId kErrorConnectionNotFound;
	extern const MStringResourceId kErrorConnectionNotMade;
	extern const MStringResourceId kWarningNoAnimatedParticleSupport;
	extern const MStringResourceId kWarningUnsupportedAttr;
	extern const MStringResourceId kWarningSkipIndexNonArray;
	extern const MStringResourceId kWarningSkipOddlyNamed;
	extern const MStringResourceId kWarningSkipNoSamples;
    extern const MStringResourceId kAEAlembicAttributes;

		// Register all strings
	MStatus registerMStringResources(void);

	MString getString(const MStringResourceId &stringId);

    MString replaceString(const MString& text, const MString& str, const MStringResourceId& stringId);
}

#endif