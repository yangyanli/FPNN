#pragma once
#ifndef FILE_FORMAT_PLY_H
#define FILE_FORMAT_PLY_H

#include "PLY/unknown.h"

namespace PLY {
// A Value whose data is a Int8.
struct CharValue: public Value {
  char val;

  CharValue() :
      val(0) {
  }
  CharValue(const char& v) :
      val(v) {
  }
  virtual ~CharValue() {
  }
  bool get_scalar(const Property& prop, double& value) const {
    if (prop.type != SCALAR)
      return false;
    value = val;
    return true;
  }
  bool set_scalar(const Property& prop, const double& value) {
    if (prop.type != SCALAR)
      return false;
    val = (char) value;
    return true;
  }
};
// struct CharValue

// A Value whose data is a Int16.
struct ShortValue: public Value {
  short val;

  ShortValue() :
      val(0) {
  }
  ShortValue(const short& v) :
      val(v) {
  }
  virtual ~ShortValue() {
  }
  bool get_scalar(const Property& prop, double& value) const {
    if (prop.type != SCALAR)
      return false;
    value = val;
    return true;
  }
  bool set_scalar(const Property& prop, const double& value) {
    if (prop.type != SCALAR)
      return false;
    val = (short) value;
    return true;
  }
};
// struct ShortValue

// A Value whose data is a Int32.
struct IntValue: public Value {
  int val;

  IntValue() :
      val(0) {
  }
  IntValue(const int& v) :
      val(v) {
  }
  virtual ~IntValue() {
  }
  bool get_scalar(const Property& prop, double& value) const {
    if (prop.type != SCALAR)
      return false;
    value = val;
    return true;
  }
  bool set_scalar(const Property& prop, const double& value) {
    if (prop.type != SCALAR)
      return false;
    val = (int) value;
    return true;
  }
};
// struct IntValue

// A Value whose data is a UInt8.
struct UCharValue: public Value {
  unsigned char val;

  UCharValue() :
      val(0) {
  }
  UCharValue(const unsigned char& v) :
      val(v) {
  }
  virtual ~UCharValue() {
  }
  bool get_scalar(const Property& prop, double& value) const {
    if (prop.type != SCALAR)
      return false;
    value = val;
    return true;
  }
  bool set_scalar(const Property& prop, const double& value) {
    if (prop.type != SCALAR)
      return false;
    val = (unsigned char) value;
    return true;
  }
};
// struct UCharValue

// A Value whose data is a UInt16.
struct UShortValue: public Value {
  unsigned short val;

  UShortValue() :
      val(0) {
  }
  UShortValue(const unsigned short& v) :
      val(v) {
  }
  virtual ~UShortValue() {
  }
  bool get_scalar(const Property& prop, double& value) const {
    if (prop.type != SCALAR)
      return false;
    value = val;
    return true;
  }
  bool set_scalar(const Property& prop, const double& value) {
    if (prop.type != SCALAR)
      return false;
    val = (unsigned short) value;
    return true;
  }
};
// struct UShortValue

// A Value whose data is a UInt32.
struct UIntValue: public Value {
  unsigned int val;

  UIntValue() :
      val(0) {
  }
  UIntValue(const unsigned int& v) :
      val(v) {
  }
  virtual ~UIntValue() {
  }
  bool get_scalar(const Property& prop, double& value) const {
    if (prop.type != SCALAR)
      return false;
    value = val;
    return true;
  }
  bool set_scalar(const Property& prop, const double& value) {
    if (prop.type != SCALAR)
      return false;
    val = (unsigned int) value;
    return true;
  }
};
// struct UIntValue

// A Value whose data is a Float32.
struct FloatValue: public Value {
  float val;

  FloatValue() :
      val(0) {
  }
  FloatValue(const float& v) :
      val(v) {
  }
  virtual ~FloatValue() {
  }
  bool get_scalar(const Property& prop, double& value) const {
    if (prop.type != SCALAR)
      return false;
    value = val;
    return true;
  }
  bool set_scalar(const Property& prop, const double& value) {
    if (prop.type != SCALAR)
      return false;
    val = (float) value;
    return true;
  }
};
// struct FloatValue

// A Value whose data is a Float64.
struct DoubleValue: public Value {
  double val;

  DoubleValue() :
      val(0) {
  }
  DoubleValue(const double& v) :
      val(v) {
  }
  virtual ~DoubleValue() {
  }
  bool get_scalar(const Property& prop, double& value) const {
    if (prop.type != SCALAR)
      return false;
    value = val;
    return true;
  }
  bool set_scalar(const Property& prop, const double& value) {
    if (prop.type != SCALAR)
      return false;
    val = (double) value;
    return true;
  }
};
// struct DoubleValue

// A list Value whose items are Int32.
struct IntList: public Value {
  std::vector<int> val;

  IntList() :
      val() {
  }
  virtual ~IntList() {
  }
  bool get_size(const Property& prop, size_t& size) const {
    if (prop.type != LIST)
      return false;
    size = val.size();
    return true;
  }
  bool get_item(const Property& prop, const size_t& num, double& value) const {
    if (prop.type != LIST)
      return false;
    value = val[num];
    return true;
  }
  bool set_size(const Property& prop, const size_t& size) {
    if (prop.type != LIST)
      return false;
    val.resize(size);
    return true;
  }
  bool set_item(const Property& prop, const size_t& num, const double& value) {
    if (prop.type != LIST)
      return false;
    val[num] = (int) value;
    return true;
  }
};
// struct IntList

/// A list Value whose items are UInt32.
struct UIntList: public Value {
  std::vector<unsigned int> val;

  UIntList() :
      val() {
  }
  virtual ~UIntList() {
  }
  bool get_size(const Property& prop, size_t& size) const {
    if (prop.type != LIST)
      return false;
    size = val.size();
    return true;
  }
  bool get_item(const Property& prop, const size_t& num, double& value) const {
    if (prop.type != LIST)
      return false;
    value = val[num];
    return true;
  }
  bool set_size(const Property& prop, const size_t& size) {
    if (prop.type != LIST)
      return false;
    val.resize(size);
    return true;
  }
  bool set_item(const Property& prop, const size_t& num, const double& value) {
    if (prop.type != LIST)
      return false;
    val[num] = (unsigned int) value;
    return true;
  }
};
// struct UIntList

// Example object classes.

// An example Item: a vertex with x,y,z coordinates.
struct VertexItem: public Item {
  FloatValue value_x, value_y, value_z;
  UCharValue value_red, value_green, value_blue, value_alpha;

  VertexItem() :
      value_x(0), value_y(0), value_z(0), value_red(200), value_green(200), value_blue(200), value_alpha(255) {
  }
  VertexItem(float x, float y, float z) :
      value_x(x), value_y(y), value_z(z), value_red(200), value_green(200), value_blue(200), value_alpha(255) {
  }
  VertexItem(float x, float y, float z, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) :
      value_x(x), value_y(y), value_z(z), value_red(200), value_green(green), value_blue(blue), value_alpha(alpha) {
  }

  Value* get_value(const Element& elem, const Property& prop) {
    if (prop.name.compare(prop_x.name.c_str()) == 0)
      return &value_x;
    else if (prop.name.compare(prop_y.name.c_str()) == 0)
      return &value_y;
    else if (prop.name.compare(prop_z.name.c_str()) == 0)
      return &value_z;
    else if (prop.name.compare(prop_red.name.c_str()) == 0)
      return &value_red;
    else if (prop.name.compare(prop_green.name.c_str()) == 0)
      return &value_green;
    else if (prop.name.compare(prop_blue.name.c_str()) == 0)
      return &value_blue;
    else if (prop.name.compare(prop_alpha.name.c_str()) == 0)
      return &value_alpha;
    return 0;
  }

  bool make_element(Element& elem) const {
    elem.name = name;
    elem.props.push_back(prop_x);
    elem.props.push_back(prop_y);
    elem.props.push_back(prop_z);
    elem.props.push_back(prop_red);
    elem.props.push_back(prop_green);
    elem.props.push_back(prop_blue);
    elem.props.push_back(prop_alpha);
    return true;
  }

  static const char* name;

  float x() const {
    double dval;
    value_x.get_scalar(prop_x, dval);
    return (float) dval;
  }
  float y() const {
    double dval;
    value_y.get_scalar(prop_y, dval);
    return (float) dval;
  }
  float z() const {
    double dval;
    value_z.get_scalar(prop_z, dval);
    return (float) dval;
  }

  void x(float coord) {
    value_x.set_scalar(prop_x, coord);
  }
  void y(float coord) {
    value_y.set_scalar(prop_y, coord);
  }
  void z(float coord) {
    value_z.set_scalar(prop_z, coord);
  }

  static const Property prop_x;
  static const Property prop_y;
  static const Property prop_z;

  unsigned char red() const {
    double dval;
    value_red.get_scalar(prop_red, dval);
    return (unsigned char) dval;
  }
  unsigned char green() const {
    double dval;
    value_green.get_scalar(prop_green, dval);
    return (unsigned char) dval;
  }
  unsigned char blue() const {
    double dval;
    value_blue.get_scalar(prop_blue, dval);
    return (unsigned char) dval;
  }
  unsigned char alpha() const {
    double dval;
    value_alpha.get_scalar(prop_alpha, dval);
    return (unsigned char) dval;
  }

  void red(unsigned char color) {
    value_red.set_scalar(prop_red, color);
  }
  void green(unsigned char color) {
    value_green.set_scalar(prop_green, color);
  }
  void blue(unsigned char color) {
    value_blue.set_scalar(prop_blue, color);
  }
  void alpha(unsigned char color) {
    value_alpha.set_scalar(prop_alpha, color);
  }

  static const Property prop_red;
  static const Property prop_green;
  static const Property prop_blue;
  static const Property prop_alpha;
};
// struct VertexItem

// An example Item: a face with a list of indices.
struct FaceItem: public Item {
  PolyValue indices;

  FaceItem() {
  }
  FaceItem(const size_t& size) {
    indices.set_size(prop_ind, size);
  }
  FaceItem(const FaceItem& f) {
    if (f.indices.data != 0) {
      size_t size;
      double val;
      f.indices.get_size(prop_ind, size);
      indices.set_size(prop_ind, size);
      for (size_t n = 0; n < size; ++n) {
        f.indices.get_item(prop_ind, n, val);
        indices.set_item(prop_ind, n, val);
      }
    }
  }

  Value* get_value(const Element& elem, const Property& prop) {
    if (prop.name.compare(prop_ind.name.c_str()) == 0)
      return &indices;
    return 0;
  }

  bool make_element(Element& elem) const {
    elem.name = name;
    elem.props.push_back(prop_ind);
    return true;
  }

  size_t size() const {
    size_t s;
    indices.get_size(prop_ind, s);
    return s;
  }
  size_t vertex(const size_t& num) const {
    double index;
    indices.get_item(prop_ind, num, index);
    return (size_t) index;
  }
  void size(const size_t& size) {
    indices.set_size(prop_ind, size);
  }
  void vertex(const size_t& num, const size_t& index) {
    indices.set_item(prop_ind, num, (double) index);
  }

  static const char* name;
  static const Property prop_ind;
};
// struct FaceItem

// An example Array: a collection of VertexItems.
struct VertexArray: public PolyArray {
  VertexArray() :
      PolyArray() {
  }

  Item& next_object() {
    if (objects[incr] == 0)
      objects[incr] = new VertexItem;
    return *objects[incr++];
  }
};
// struct VertexArray

/// An example Array: a collection of [Faces](\ref FaceItem).
struct FaceArray: public PolyArray {
  /// Default constructor.
  FaceArray() :
      PolyArray() {
  }

  // Get the next Item.
  Item& next_object() {
    if (objects[incr] == 0)
      objects[incr] = new FaceItem;
    return *objects[incr++];
  }
};
// struct FaceArray

// An example Array: a collection of VertexItems stored extrenally.
struct VertexExternal: public Array {
  std::vector<VertexItem>& vertices;
  size_t incr;

  VertexExternal(std::vector<VertexItem>& v) :
      Array(), vertices(v), incr(0) {
  }

  size_t size() {
    return vertices.size();
  }
  void prepare(const size_t& size) {
    vertices.reserve(size);
    restart();
  }
  void clear() {
    vertices.clear();
  }
  void restart() {
    incr = 0;
  }
  bool store_object() {
    return true;
  }

  Item& next_object() {
    if (vertices.size() <= incr)
      vertices.resize(incr + 1);
    return vertices[incr++];
  }
};
// struct VertexExternal

// An example Array: a collection of FaceItems stored extrenally.
struct FaceExternal: public Array {
  std::vector<FaceItem>& faces;
  size_t incr;

  FaceExternal(std::vector<FaceItem>& f) :
      Array(), faces(f), incr(0) {
  }

  size_t size() {
    return faces.size();
  }
  void prepare(const size_t& size) {
    faces.reserve(size);
    restart();
  }
  void clear() {
    faces.clear();
  }
  void restart() {
    incr = 0;
  }
  bool store_object() {
    return true;
  }

  Item& next_object() {
    if (faces.size() <= incr)
      faces.resize(incr + 1);
    return faces[incr++];
  }
};
// struct FaceExternal
}// namespace PLY

#endif // FILE_FORMAT_PLY_H
