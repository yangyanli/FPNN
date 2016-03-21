// A C++ reader/writer of .ply files.
// Undescribed data objects.
// These contain the actual data.
// Generally, the user will implement subclasses
// for their own object types.
//
//Copyright (C) 2013  INRIA - Sophia Antipolis
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Author(s):      Thijs van Lankveld

#ifndef __PLY_UNKNOWN_H__
#define __PLY_UNKNOWN_H__

#include "PLY/object.h"

namespace PLY {
// A polymorphic Value representing a generic Property.
struct PolyValue: public Value {
  char* data;

  PolyValue() :
      data(nullptr) {
  }
  ~PolyValue() {
    deinit();
  }

  bool get_scalar(const Property& prop, double& value) const;
  bool set_scalar(const Property& prop, const double& value);
  bool get_size(const Property& prop, size_t& size) const;
  bool get_item(const Property& prop, const size_t& num, double& value) const;
  bool set_size(const Property& prop, const size_t& size);
  bool set_item(const Property& prop, const size_t& num, const double& value);
  bool get_string(const Property& prop, char* str) const;
  bool set_string(const Property& prop, const char* str);

private:
  PolyValue(const PolyValue&) :
      data(nullptr) {
  }

  void deinit() {
    if (data) {
      delete[] data;
      data = 0;
    }
  }

  // Convert a pointer to a value.
  bool to_value(const char* ptr, const Scalar_type& type, double& value) const;

  // Convert a value to a pointer.
  bool to_pointer(const double& value, const Scalar_type& type, char* ptr) const;

  // Copy bytes from one array to another.
  void copy(const char* from, char* to, const size_t& num) const;
};
// struct PolyValue

// An Item containing only polymorphic Values.
struct PolyObject: public Item {
  Value** values;

  PolyObject() :
      values(nullptr), num(0) {
  }
  virtual ~PolyObject() {
    deinit();
  }

  void prepare(const Element& elem);
  Value* get_value(const Element& elem, const Property& prop);

private:
  PolyObject(const PolyObject&) :
      values(nullptr), num(0) {
  }

  void init(size_t size);
  void deinit();

  size_t num;
};
// struct PolyObject

// An Array containing only Items with polymorphic Values.
struct PolyArray: public Array {
  Item** objects;
  size_t incr;

  PolyArray() :
      objects(nullptr), incr(0), num(0) {
  }
  virtual ~PolyArray() {
    deinit();
  }

  virtual size_t size() {
    return num;
  }
  virtual void prepare(const size_t& size);
  virtual void clear();
  virtual void restart() {
    incr = 0;
  }
  virtual Item& next_object();
  virtual bool store_object() {
    return true;
  }

private:
  PolyArray(const PolyArray&) :
      objects(nullptr), incr(0), num(0) {
  }

  void init(size_t size);
  void deinit();

  size_t num;
};
// struct PolyArray
}// namespace PLY

#endif // __PLY_UNKNOWN_H__
