#include "file_format_ply.h"

namespace PLY {
const char* VertexItem::name = "vertex";
const Property VertexItem::prop_x = Property("x", SCALAR, Float32);
const Property VertexItem::prop_y = Property("y", SCALAR, Float32);
const Property VertexItem::prop_z = Property("z", SCALAR, Float32);
const Property VertexItem::prop_red = Property("red", SCALAR, UInt8);
const Property VertexItem::prop_green = Property("green", SCALAR, UInt8);
const Property VertexItem::prop_blue = Property("blue", SCALAR, UInt8);
const Property VertexItem::prop_alpha = Property("alpha", SCALAR, UInt8);
const char* FaceItem::name = "face";
const Property FaceItem::prop_ind = Property("vertex_indices", LIST, UInt32, UInt8);
} // namespace PLY
