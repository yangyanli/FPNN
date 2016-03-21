#include <set>
#include <vector>
#include <cstdio>
#include <string>

#include "mesh_io.h"

int ReadOffFile(FILE *stream, OffParseCallbacks *ucb) {
  unsigned int line_len = 2048;
  char* line = new char[line_len];

  std::fgets(line, line_len, stream);

  int vertex_count, face_count, edge_count;
 
  std::string line_str(line);
  if (line_str.find("OFF") != 0) {
    delete line;
    return -1;
  }

  if (line_str == "OFF\n") {
    if (fscanf(stream, "%d %d %d", &vertex_count, &face_count, &edge_count) != 3) {
      delete line;
      return -1;
    }
  } else {
    if (sscanf(line+3, "%d %d %d", &vertex_count, &face_count, &edge_count) != 3) {
      delete line;
      return -1;
    }
  }

  double x, y, z;
  double r_default, g_default, b_default, a_default;
  r_default = g_default = b_default = 0.8;
  a_default = 1.0;
  while (vertex_count--) {
    do {
      int c = fgetc(stream);
      if (c == '#')
        std::fgets(line, line_len, stream);
      else {
        ungetc(c, stream);
        break;
      }
    } while (true);

    fscanf(stream, "%lf %lf %lf", &x, &y, &z);
    ucb->onVertex(x, y, z, r_default, g_default, b_default, a_default, ucb->userData);
  }

  std::set < std::pair<int, int> > edge_set;
  while (face_count--) {
    do {
      int c = fgetc(stream);
      if (c == '#')
        std::fgets(line, line_len, stream);
      else {
        ungetc(c, stream);
        break;
      }
    } while (true);

    fscanf(stream, "%d", &vertex_count);

    int vertex_id;
    ucb->onStartFace(ucb->userData);
    while (vertex_count--) {
      fscanf(stream, "%d", &vertex_id);
      ucb->onAddToFace(vertex_id, ucb->userData);
    }
  }

  delete line;

  return 0;
}
