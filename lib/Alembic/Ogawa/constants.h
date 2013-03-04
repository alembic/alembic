#ifndef _Ogawa_Forward_Declarations_h_
#define _Ogawa_Forward_Declarations_h_

#include <Ogawa/ns.h>

#include <stdint.h>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

// a few useful constant values
const uint64_t INVALID_GROUP = 0x7fffffffffffffffULL;
const uint64_t EMPTY_GROUP   = 0x0000000000000000ULL;
const uint64_t INVALID_DATA  = 0xffffffffffffffffULL;
const uint64_t EMPTY_DATA    = 0x8000000000000000ULL;

} // End namespace OGAWA_LIB_VERSION_NS

using namespace OGAWA_LIB_VERSION_NS;

} // End namespace Ogawa

#endif