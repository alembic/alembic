//
//  Copyright (c) 2009, Tweak Software
//  All rights reserved.
// 
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//
//     * Neither the name of the Tweak Software nor the names of its
//       contributors may be used to endorse or promote products
//       derived from this software without specific prior written
//       permission.
// 
//  THIS SOFTWARE IS PROVIDED BY Tweak Software ''AS IS'' AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL Tweak Software BE LIABLE FOR
//  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
//  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//
#ifndef __Gto__Protocols__h__
#define __Gto__Protocols__h__

#define GTO_PROTOCOL_OBJECT              "object"
#define GTO_PROTOCOL_POLYGON             "polygon"
#define GTO_PROTOCOL_NURBS               "NURBS"
#define GTO_PROTOCOL_CATMULL_CLARK       "catmull-clark"     // sub-d
#define GTO_PROTOCOL_LOOP                "loop"              // sub-d
#define GTO_PROTOCOL_TRANSFORM           "transform"
#define GTO_PROTOCOL_TEXCHANNEL          "texChannel"        // deprecated
#define GTO_PROTOCOL_PARTICLE            "particle"
#define GTO_PROTOCOL_DIFFERENCE          "difference"
#define GTO_PROTOCOL_SORTED              "sorted"
#define GTO_PROTOCOL_IMAGE               "image"
#define GTO_PROTOCOL_MATERIAL            "material"
#define GTO_PROTOCOL_GROUP               "group"
#define GTO_PROTOCOL_STRAND              "strand"
#define GTO_PROTOCOL_CAMERA              "camera"
#define GTO_PROTOCOL_LIGHT               "light"
#define GTO_PROTOCOL_GRID3D              "grid3D"

#define GTO_COMPONENT_POINTS             "points"
#define GTO_COMPONENT_SURFACE            "surface"
#define GTO_COMPONENT_INDICES            "indices"
#define GTO_COMPONENT_ELEMENTS           "elements"
#define GTO_COMPONENT_OBJECT             "object"
#define GTO_COMPONENT_MAPPINGS           "mappings"
#define GTO_COMPONENT_SMOOTHING          "smoothing"
#define GTO_COMPONENT_NORMALS            "normals"
#define GTO_COMPONENT_SHELLS             "shells"
#define GTO_COMPONENT_IMAGE              "image"
#define GTO_COMPONENT_STRAND             "strand"
#define GTO_COMPONENT_CHANNELS           "channels"

#define GTO_PROPERTY_POSITION            "position"          // float[3][]
#define GTO_PROPERTY_VELOCITY            "velocity"          // float[3][]
#define GTO_PROPERTY_MASS                "mass"              // float[1][]
#define GTO_PROPERTY_WEIGHT              "weight"            // float[1][]
#define GTO_PROPERTY_DEGREE              "degree"            // float[1][]
#define GTO_PROPERTY_UKNOTS              "uKnots"            // float[1][]
#define GTO_PROPERTY_VKNOTS              "vKnots"            // float[1][]
#define GTO_PROPERTY_URANGE              "uRange"            // float[1][]
#define GTO_PROPERTY_VRANGE              "vRange"            // float[1][]
#define GTO_PROPERTY_UFORM               "uForm"             // float[1][]
#define GTO_PROPERTY_VFORM               "vForm"             // float[1][]
#define GTO_PROPERTY_NAME                "name"              // string[1][1]
#define GTO_PROPERTY_PROTOCOL            "protocol"          // string[1][]
#define GTO_PROPERTY_PROTOCOL_VERSION    "protocolVersion"   // string[1][]
#define GTO_PROPERTY_GLOBAL_MATRIX       "globalMatrix"      // float[16][1]
#define GTO_PROPERTY_BOUNDINGBOX         "boundingBox"       // float[6][1]
#define GTO_PROPERTY_PARENT              "parent"            // string[1][1]
#define GTO_PROPERTY_TYPE                "type"              // byte[1][]
#define GTO_PROPERTY_SIZE                "size"              // multiple
#define GTO_PROPERTY_VERTEX              "vertex"            // int[1][]
#define GTO_PROPERTY_NORMAL              "normal"            // multiple
#define GTO_PROPERTY_ST                  "st"                // multiple
#define GTO_PROPERTY_STROOT              "stRoot"            // multiple
#define GTO_PROPERTY_METHOD              "method"            // int[1][1]
#define GTO_PROPERTY_VERTICES            "vertices"          // int[1][]
#define GTO_PROPERTY_ELEMENTS            "elements"          // int[1][]
#define GTO_PROPERTY_PIXELS              "pixels"
#define GTO_PROPERTY_WIDTH               "width"
#define GTO_PROPERTY_INTERP_BOUNDARY     "interpolateBoundary" // byte[1][1]

#define GTO_POLYGON_POLYGON              0
#define GTO_POLYGON_TRIANGLE             1
#define GTO_POLYGON_QUAD                 2
#define GTO_POLYGON_TSTRIP               3
#define GTO_POLYGON_QSTRIP               4
#define GTO_POLYGON_FAN                  5

#define GTO_SMOOTHING_METHOD_NONE           0
#define GTO_SMOOTHING_METHOD_SMOOTH         1
#define GTO_SMOOTHING_METHOD_FACETED        2
#define GTO_SMOOTHING_METHOD_PARTITIONED    3
#define GTO_SMOOTHING_METHOD_DISCONTINUOUS  4

#define GTO_INTERPRET_COORDINATE   "coordinate"
#define GTO_INTERPRET_NORMAL       "normal"
#define GTO_INTERPRET_SIZE         "size"
#define GTO_INTERPRET_4X4          "4x4"
#define GTO_INTERPRET_3X3          "3x3"
#define GTO_INTERPRET_ROW_MAJOR    "row-major"
#define GTO_INTERPRET_COLUMN_MAJOR "column-major"
#define GTO_INTERPRET_QUATERNION   "quaternion"
#define GTO_INTERPRET_COMPLEX      "complex"
#define GTO_INTERPRET_INDICES      "indices"
#define GTO_INTERPRET_BBOX         "bbox"
#define GTO_INTERPRET_FRUSTUM      "frustum"
#define GTO_INTERPRET_HOMOGENEOUS  "homogeneous"
#define GTO_INTERPRET_RGB          "RGB"
#define GTO_INTERPRET_RGBA         "RGBA"
#define GTO_INTERPRET_BGR          "BGR"
#define GTO_INTERPRET_ABGR         "ABGR"

#endif // __Gto__Protocols__h__
