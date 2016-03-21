//	A C++ reader/writer of .ply files.
//	Base types.
//
//	Adapted to C++ streams by Thijs van Lankveld.

/*
 Header for PLY polygon files.

 - Greg Turk

 A PLY file contains a single polygonal _object_.

 An object is composed of lists of _elements_.  Typical elements are
 vertices, faces, edges and materials.

 Each type of element for a given object has one or more _properties_
 associated with the element type.  For instance, a vertex element may
 have as properties three floating-point values x,y,z and three unsigned
 chars for red, green and blue.

 -----------------------------------------------------------------------

 Copyright (c) 1998 Georgia Institute of Technology.  All rights reserved.

 Permission to use, copy, modify and distribute this software and its
 documentation for any purpose is hereby granted without fee, provided
 that the above copyright notice and this permission notice appear in
 all copies of this software and that you do not sell the software.

 THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef __PLY_BASE_H__
#define __PLY_BASE_H__

#include <iostream>

namespace PLY {
const char _VERSION[] = "1.2.0";

enum Stream_type {
  ASCII = 1, BINARY_BE, BINARY_LE
};
enum Scalar_type {
  StartType = 0, Int8, Int16, Int32, UInt8, UInt16, UInt32, Float32, Float64, EndType
};
enum Variable_type {
  SCALAR, LIST, STRING
};
enum Verbose {
  OVERLOOK, WARN, SCOLD, PUNISH
};

const char* const Type_names[] = { "invalid", "int8", "int16", "int32", "uint8", "uint16", "uint32", "float32", "float64" };
const char* const Old_type_names[] = { "invalid", "char", "short", "int", "uchar", "ushort", "uint", "float", "double" };
const size_t Type_bytes[] = { 0, 1, 2, 4, 1, 2, 4, 4, 8, 0 };
const int BIG_STRING = 4096;
const Verbose ON_FAULT = SCOLD;
} // namespace PLY

#endif // __PLY_BASE_H__
