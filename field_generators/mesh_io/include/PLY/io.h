/*
 A C++ reader/writer of .ply files.

 Addapted to C++ streams by Thijs van Lankveld.

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

#ifndef __PLY_IO_H__
#define __PLY_IO_H__

#include "PLY/object.h"

#include <stdlib.h>

namespace PLY {
// The Reader for extracting Items from a PLY stream.
struct Reader {
  Header& header;
  std::istream* stream;

  Reader(Header& h) :
      header(h), stream(0) {
  }
  Reader(Header& h, std::istream& is) :
      header(h), stream(&is) {
  }
  Reader(Header& h, const char* file_name) :
      header(h), stream(0) {
    if (!open_file(file_name))
      close_file();
  }

  bool open_file(const char* file_name);
  void close_file();

  bool read_header();
  bool read_data(Storage* store);

private:
  bool read_stream_type(const std::string& word, Stream_type& type);
  bool read_scalar(const std::string& word, Scalar_type& type);

  bool read_ascii_value(const Scalar_type& type, double& value);
  bool read_binary_value(const Scalar_type& type, double& value);
  void read(char* ptr, size_t n);

  inline bool read_value(const Scalar_type& type, double& value) {
    if (header.stream_type == ASCII)
      return read_ascii_value(type, value);
    else
      return read_binary_value(type, value);
  }

  inline bool read_size(const Scalar_type& type, size_t& size) {
    double value;
    if (!read_value(type, value))
      return false;
    size = (size_t) value;
    return true;
  }

  void read_ascii_string(char* str);
};
// struct Reader

// The Writer for storing Items into a PLY stream.
struct Writer {
  Header& header;
  std::ostream* stream;

  Writer(Header& h) :
      header(h), stream(0) {
  }
  Writer(Header& h, std::ostream& os) :
      header(h), stream(&os) {
  }
  Writer(Header& h, const char* file_name, const Stream_type& type = ASCII) :
      header(h), stream(0) {
    if (!open_file(file_name, type))
      close_file();
  }

  bool open_file(const char* file_name, const Stream_type& type = ASCII);
  void close_file();

  bool write_header();
  bool write_data(Storage* store);
  bool write_element(const Element& elem, Array* collect);
  inline bool write_object(const Element& elem, Item* obj) {
    if (header.stream_type == ASCII)
      return write_ascii_object(elem, obj);
    else
      return write_binary_object(elem, obj);
  }

private:
  bool write_ascii_value(const Scalar_type& type, const double& value);
  bool write_binary_value(const Scalar_type& type, const double& value);
  void write(char* ptr, size_t n);

  bool write_ascii_object(const Element& elem, Item* obj);
  bool write_binary_object(const Element& elem, Item* obj);
};
// class Writer

#define HANDLE_FAULT(msg) {\
	switch (ON_FAULT) {\
		case OVERLOOK:\
			break;\
		case WARN:\
		case SCOLD:\
		case PUNISH:\
			std::cerr << msg << std::endl;\
			switch (ON_FAULT) {\
			case WARN:\
				break;\
			case PUNISH:\
				exit(1);\
			case SCOLD:\
				return false;\
			}\
		}\
	}
} // namespace PLY

#endif // __PLY_IO_H__
