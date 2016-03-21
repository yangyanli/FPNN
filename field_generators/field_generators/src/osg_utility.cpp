#define _USE_MATH_DEFINES
#include <cmath>

#include <osg/Geode>
#include <osg/Point>
#include <osg/Depth>
#include <osg/Version>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osg/BlendColor>
#include <osg/LineSegment>
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>

#include "osg_utility.h"

namespace OSGUtility {
const char* vertex_shader_fancy = {
//"#version 330 core\n"
    "varying vec3 normal, eyeVec, lightDir;\n"
        "void main()\n"
        "{\n"
        "vec4 vertexInEye = gl_ModelViewMatrix * gl_Vertex;\n"
        "eyeVec = -vertexInEye.xyz;\n"
        "lightDir = vec3(gl_LightSource[0].position - vertexInEye.xyz);\n"
        "normal = gl_NormalMatrix * gl_Normal;\n"
        "gl_Position = ftransform();\n"
        "}\n" };

const char* fragment_shader_fancy = {
//"#version 330 core\n"
    "uniform vec4 lightDiffuse;\n"
        "uniform vec4 lightSpecular;\n"
        "uniform float shininess;\n"
        "varying vec3 normal, eyeVec, lightDir;\n"
        "void main (void)\n"
        "{\n"
        "vec4 finalColor = gl_FrontLightModelProduct.sceneColor;\n"
        "vec3 N = normalize(normal);\n"
        "vec3 L = normalize(lightDir);\n"
        "float lambert = dot(N,L);\n"
        "if (lambert > 0.0)\n"
        "{\n"
        "finalColor += lightDiffuse * lambert;\n"
        "vec3 E = normalize(eyeVec);\n"
        "vec3 R = reflect(-L, N);\n"
        "float specular = pow(max(dot(R, E), 0.0), shininess);\n"
        "finalColor += lightSpecular * specular;\n"
        "}\n"
        "gl_FragColor = finalColor;\n"
        "}\n" };

void applyShaderFancy(osg::Node* node) {
  osg::ref_ptr < osg::Program > program = new osg::Program;
  program->addShader(new osg::Shader(osg::Shader::VERTEX, vertex_shader_fancy));
  program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragment_shader_fancy));
  osg::StateSet* state_set = node->getOrCreateStateSet();
  state_set->setAttributeAndModes(program.get());
  state_set->addUniform(new osg::Uniform("lightDiffuse", osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f)));
  state_set->addUniform(new osg::Uniform("lightSpecular", osg::Vec4(1.0f, 1.0f, 0.4f, 1.0f)));
  state_set->addUniform(new osg::Uniform("shininess", 64.0f));

  return;
}

const char* vertex_shader_normal = { "varying vec3 normal;\n"
    "void main(void)\n"
    "{\n"
    "  normal = (normalize(gl_Normal)+1)/2;\n"
    "  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    "}\n" };

const char* fragment_shader_normal = { "varying vec3 normal;\n"
    "void main(void)\n"
    "{\n"
    "  gl_FragColor = vec4(normal, 1.0);\n"
    "}\n" };

void applyShaderNormal(osg::Node* node) {
  osg::ref_ptr < osg::Program > program = new osg::Program;
  program->addShader(new osg::Shader(osg::Shader::VERTEX, vertex_shader_normal));
  program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragment_shader_normal));
  osg::StateSet* state_set = node->getOrCreateStateSet();
  state_set->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  state_set->setAttributeAndModes(program.get());
}

osg::Geode* drawCylinder(const osg::LineSegment& axis, double thickness, const osg::Vec4& color) {
  return drawCylinder(axis.start(), axis.end(), thickness, color);
}

osg::Geode* drawCylinder(const osg::Vec3& base, const osg::Vec3& top, double thickness, const osg::Vec4& color) {
  osg::Geode* geode = new osg::Geode();

  osg::Vec3 center = (top + base) / 2;
  osg::Vec3 offset = top - base;
  osg::Vec3 zAxis(0.0, 0.0, 1.0);
  osg::Quat rotation;
  rotation.makeRotate(zAxis, offset);

  osg::Cylinder* cylinder = new osg::Cylinder(center, thickness, offset.length());
  cylinder->setRotation(rotation);

  osg::ShapeDrawable* drawable = new osg::ShapeDrawable(cylinder);
  drawable->setColor(color);
  geode->addDrawable(drawable);

  return geode;
}

osg::Geode* drawCylinders(const osg::Vec3Array* bases, const osg::Vec3Array* tops, const std::vector<double>& radii, const osg::Vec4Array* colors,
    int subdivide_level) {
  osg::Geode* geode = new osg::Geode();

  size_t vertices_num = 32 * subdivide_level;
  std::vector<int> quads(2 * vertices_num);
  for (size_t i = 0, i_end = vertices_num / 2; i < i_end; ++i) {
    quads[4 * i + 0] = 2 * i + 0;
    quads[4 * i + 1] = 2 * i + 1;
    quads[4 * i + 2] = 2 * i + 3;
    quads[4 * i + 3] = 2 * i + 2;
  }
  quads[2 * vertices_num - 2] = 1;
  quads[2 * vertices_num - 1] = 0;

  size_t partition_size = (subdivide_level == 1) ? (1024) : (512);
  size_t item_num = bases->size();
  size_t partition_num = (item_num + partition_size - 1) / partition_size;
  for (size_t i = 0, i_end = partition_num; i < i_end; ++i) {
    size_t cylinder_offset = i * partition_size;
    size_t cylinder_num = (cylinder_offset + partition_size > item_num) ? (item_num - cylinder_offset) : (partition_size);
    osg::ref_ptr < osg::Vec3Array > cylinders_normals(new osg::Vec3Array(cylinder_num * vertices_num));
    osg::ref_ptr < osg::Vec3Array > cylinders_vertices(new osg::Vec3Array(cylinder_num * vertices_num));
    osg::ref_ptr < osg::Vec4Array > cylinders_colors(new osg::Vec4Array(cylinder_num * vertices_num));
    osg::ref_ptr < osg::DrawElementsUShort > cylinder_quads(new osg::DrawElementsUShort(GL_QUADS, cylinder_num * quads.size()));

    for (size_t j = 0; j < cylinder_num; ++j) {
      size_t cylinder_idx = cylinder_offset + j;
      const osg::Vec3& head = tops->at(cylinder_idx);
      const osg::Vec3& tail = bases->at(cylinder_idx);
      const osg::Vec4& color = colors->at(cylinder_idx);

      osg::Vec3 offset(head - tail);
      osg::Vec3 cross(1.0f, 0.0f, 0.0f);
      if (std::abs(offset * cross) > 0.9f)
        cross = osg::Vec3(0.0f, 1.0f, 0.0f);
      osg::Vec3 base = offset ^ cross;
      base = base * (radii[cylinder_idx] / base.length());
      osg::Quat rotation(4 * M_PI / vertices_num, offset);

      size_t vertex_offset = j * vertices_num;
      for (size_t k = 0, k_end = vertices_num / 2; k < k_end; ++k) {
        osg::Vec3 normal = base;
        normal.normalize();

        size_t head_offset = vertex_offset + 2 * k + 0;
        cylinders_normals->at(head_offset) = normal;
        cylinders_vertices->at(head_offset) = head + base;
        cylinders_colors->at(head_offset) = color;

        size_t tail_offset = vertex_offset + 2 * k + 1;
        cylinders_normals->at(tail_offset) = normal;
        cylinders_vertices->at(tail_offset) = tail + base;
        cylinders_colors->at(tail_offset) = color;

        base = rotation * base;
      }

      size_t triangle_offset = j * quads.size();
      for (size_t k = 0, k_end = quads.size(); k < k_end; ++k) {
        cylinder_quads->at(triangle_offset + k) = vertex_offset + quads[k];
      }
    }

    osg::ref_ptr < osg::Geometry > geometry = new osg::Geometry;
    geometry->setUseDisplayList(true);
    geometry->setVertexArray(cylinders_vertices);
#if OSG_VERSION_LESS_THAN(3,1,0)
    geometry->setNormalArray(cylinders_normals);
    geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
    geometry->setColorArray(cylinders_colors);
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
#else
    geometry->setNormalArray(cylinders_normals, osg::Array::BIND_PER_VERTEX);
    geometry->setColorArray(cylinders_colors, osg::Array::BIND_PER_VERTEX);
#endif
    geometry->addPrimitiveSet(cylinder_quads.get());
    geode->addDrawable(geometry);
  }

  return geode;
}

osg::Geode* drawCone(const osg::LineSegment& axis, double thickness, const osg::Vec4& color) {
  return drawCone(axis.start(), axis.end(), thickness, color);
}

osg::Geode* drawCone(const osg::Vec3& base, const osg::Vec3& top, double radius, const osg::Vec4& color) {
  osg::Vec3 offset = top - base;
  osg::Vec3 zAxis(0.0, 0.0, 1.0);
  osg::Quat rotation;
  rotation.makeRotate(zAxis, offset);

  osg::Cone* cone = new osg::Cone(base, radius, offset.length());
  cone->setRotation(rotation);

  osg::Geode* geode = new osg::Geode();
  osg::ShapeDrawable* drawable = new osg::ShapeDrawable(cone);
  drawable->setColor(color);
  geode->addDrawable(drawable);

  return geode;
}

osg::Group* drawArrow(const osg::Vec3& base, const osg::Vec3& top, double radius, double head_length, const osg::Vec4& color) {
  osg::Group* group = new osg::Group;
  osg::Vec3 offset = top - base;
  offset.normalize();
  group->addChild(OSGUtility::drawCylinder(base, top, radius, color));
  group->addChild(OSGUtility::drawCone(top, top + offset * head_length, radius * 1.5, color));

  return group;
}

osg::Geode* drawSphere(const osg::Vec3& center, double radius, const osg::Vec4& color) {
  osg::Geode* geode = new osg::Geode();

  osg::Sphere* sphere = new osg::Sphere(center, radius);
  osg::ShapeDrawable* drawable = new osg::ShapeDrawable(sphere);
  drawable->setColor(color);
  geode->addDrawable(drawable);

  return geode;
}

static void triangulateSphere(osg::Vec3Array* vertices, osg::Vec3Array* normals, std::vector<int>& triangles, int subdivide_level = 2) {
  //http://sarvanz.blogspot.com/2013/07/sphere-triangulation-using-icosahedron.html
  vertices->resize(12);

  double theta = 26.56505117707799 * M_PI / 180.0; // refer paper for theta value
  double sin_theta = std::sin(theta);
  double cos_theta = std::cos(theta);
  vertices->at(0) = osg::Vec3(0.0f, 0.0f, -1.0f); // the lower vertex
  // the lower pentagon
  double phi = M_PI / 5.0;
  for (int i = 1; i < 6; ++i) {
    vertices->at(i) = osg::Vec3(cos_theta * std::cos(phi), cos_theta * std::sin(phi), -sin_theta);
    phi += 2.0 * M_PI / 5.0;
  }
  // the upper pentagon
  phi = 0.0;
  for (int i = 6; i < 11; ++i) {
    vertices->at(i) = osg::Vec3(cos_theta * std::cos(phi), cos_theta * std::sin(phi), sin_theta);
    phi += 2.0 * M_PI / 5.0;
  }
  vertices->at(11) = osg::Vec3(0.0f, 0.0f, 1.0f); // the upper vertex

  triangles.clear();
  triangles.reserve(20 * 3);
  triangles.push_back(0);
  triangles.push_back(2);
  triangles.push_back(1);
  triangles.push_back(0);
  triangles.push_back(3);
  triangles.push_back(2);
  triangles.push_back(0);
  triangles.push_back(4);
  triangles.push_back(3);
  triangles.push_back(0);
  triangles.push_back(5);
  triangles.push_back(4);
  triangles.push_back(0);
  triangles.push_back(1);
  triangles.push_back(5);

  triangles.push_back(1);
  triangles.push_back(2);
  triangles.push_back(7);
  triangles.push_back(2);
  triangles.push_back(3);
  triangles.push_back(8);
  triangles.push_back(3);
  triangles.push_back(4);
  triangles.push_back(9);
  triangles.push_back(4);
  triangles.push_back(5);
  triangles.push_back(10);
  triangles.push_back(5);
  triangles.push_back(1);
  triangles.push_back(6);

  triangles.push_back(1);
  triangles.push_back(7);
  triangles.push_back(6);
  triangles.push_back(2);
  triangles.push_back(8);
  triangles.push_back(7);
  triangles.push_back(3);
  triangles.push_back(9);
  triangles.push_back(8);
  triangles.push_back(4);
  triangles.push_back(10);
  triangles.push_back(9);
  triangles.push_back(5);
  triangles.push_back(6);
  triangles.push_back(10);

  triangles.push_back(6);
  triangles.push_back(7);
  triangles.push_back(11);
  triangles.push_back(7);
  triangles.push_back(8);
  triangles.push_back(11);
  triangles.push_back(8);
  triangles.push_back(9);
  triangles.push_back(11);
  triangles.push_back(9);
  triangles.push_back(10);
  triangles.push_back(11);
  triangles.push_back(10);
  triangles.push_back(6);
  triangles.push_back(11);

  while (subdivide_level--) {
    std::vector<int> subdivided;
    for (size_t i = 0, i_end = triangles.size() / 3; i < i_end; ++i) {
      size_t v = vertices->size();
      vertices->push_back((vertices->at(triangles[3 * i + 0]) + vertices->at(triangles[3 * i + 1])) / 2);
      vertices->back().normalize();
      vertices->push_back((vertices->at(triangles[3 * i + 1]) + vertices->at(triangles[3 * i + 2])) / 2);
      vertices->back().normalize();
      vertices->push_back((vertices->at(triangles[3 * i + 2]) + vertices->at(triangles[3 * i + 0])) / 2);
      vertices->back().normalize();

      subdivided.push_back(triangles[3 * i + 0]);
      subdivided.push_back(v + 0);
      subdivided.push_back(v + 2);
      subdivided.push_back(v + 0);
      subdivided.push_back(triangles[3 * i + 1]);
      subdivided.push_back(v + 1);
      subdivided.push_back(v + 2);
      subdivided.push_back(v + 1);
      subdivided.push_back(triangles[3 * i + 2]);
      subdivided.push_back(v + 0);
      subdivided.push_back(v + 1);
      subdivided.push_back(v + 2);
    }
    triangles = subdivided;
  }

  normals->resize(vertices->size());
  for (size_t i = 0, i_end = vertices->size(); i < i_end; i++)
    normals->at(i) = vertices->at(i);

  return;
}

void sampleOnSphere(osg::Vec3Array* vertices, osg::Vec3Array* normals, int subdivide_level) {
  std::vector<int> triangles;
  triangulateSphere(vertices, normals, triangles, subdivide_level);

  return;
}

osg::Geode* drawSpheres(const osg::Vec3Array* centers, double radius, const osg::Vec4Array* colors, int subdivide_level) {
  osg::Geode* geode = new osg::Geode();

  osg::ref_ptr<osg::Vec3Array> vertices(new osg::Vec3Array()), normals(new osg::Vec3Array());
  std::vector<int> triangles;
  triangulateSphere(vertices, normals, triangles, subdivide_level);
  for (size_t i = 0, i_end = vertices->size(); i < i_end; i++)
    vertices->at(i) *= radius;

  size_t partition_size = (subdivide_level == 1) ? (512) : (128);
  size_t item_num = centers->size();
  size_t partition_num = (item_num + partition_size - 1) / partition_size;
  for (size_t i = 0, i_end = partition_num; i < i_end; ++i) {
    size_t sphere_offset = i * partition_size;
    size_t sphere_num = (sphere_offset + partition_size > item_num) ? (item_num - sphere_offset) : (partition_size);
    osg::ref_ptr < osg::Vec3Array > spheres_normals(new osg::Vec3Array(sphere_num * vertices->size()));
    osg::ref_ptr < osg::Vec3Array > spheres_vertices(new osg::Vec3Array(sphere_num * vertices->size()));
    osg::ref_ptr < osg::Vec4Array > spheres_colors(new osg::Vec4Array(sphere_num * vertices->size()));
    osg::ref_ptr < osg::DrawElementsUShort > sphere_triangles(new osg::DrawElementsUShort(GL_TRIANGLES, sphere_num * triangles.size()));

    for (size_t j = 0; j < sphere_num; ++j) {
      size_t vertex_offset = j * vertices->size();
      for (size_t k = 0, k_end = vertices->size(); k < k_end; ++k) {
        spheres_normals->at(vertex_offset + k) = normals->at(k);
        spheres_vertices->at(vertex_offset + k) = vertices->at(k) + centers->at(sphere_offset + j);
        spheres_colors->at(vertex_offset + k) = colors->at(sphere_offset + j);
      }

      size_t triangle_offset = j * triangles.size();
      for (size_t k = 0, k_end = triangles.size(); k < k_end; ++k) {
        sphere_triangles->at(triangle_offset + k) = vertex_offset + triangles[k];
      }
    }

    osg::ref_ptr < osg::Geometry > geometry = new osg::Geometry;
    geometry->setUseDisplayList(true);
    geometry->setVertexArray(spheres_vertices);
#if OSG_VERSION_LESS_THAN(3,1,0)
    geometry->setNormalArray(spheres_normals);
    geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
    geometry->setColorArray(spheres_colors);
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
#else
    geometry->setNormalArray(spheres_normals, osg::Array::BIND_PER_VERTEX);
    geometry->setColorArray(spheres_colors, osg::Array::BIND_PER_VERTEX);
#endif
    geometry->addPrimitiveSet(sphere_triangles.get());
    geode->addDrawable(geometry);
  }

  return geode;
}

osg::Geode* drawSpheres(const osg::Vec3Array* centers, const std::vector<double>& radii, const osg::Vec4Array* colors, int subdivide_level) {
  osg::Geode* geode = new osg::Geode();

  osg::ref_ptr<osg::Vec3Array> vertices(new osg::Vec3Array()), normals(new osg::Vec3Array());
  std::vector<int> triangles;
  triangulateSphere(vertices, normals, triangles, subdivide_level);

  size_t partition_size = (subdivide_level == 1) ? (512) : (128);
  size_t item_num = centers->size();
  size_t partition_num = (item_num + partition_size - 1) / partition_size;
  for (size_t i = 0, i_end = partition_num; i < i_end; ++i) {
    size_t sphere_offset = i * partition_size;
    size_t sphere_num = (sphere_offset + partition_size > item_num) ? (item_num - sphere_offset) : (partition_size);
    osg::ref_ptr < osg::Vec3Array > spheres_normals(new osg::Vec3Array(sphere_num * vertices->size()));
    osg::ref_ptr < osg::Vec3Array > spheres_vertices(new osg::Vec3Array(sphere_num * vertices->size()));
    osg::ref_ptr < osg::Vec4Array > spheres_colors(new osg::Vec4Array(sphere_num * vertices->size()));
    osg::ref_ptr < osg::DrawElementsUShort > sphere_triangles(new osg::DrawElementsUShort(GL_TRIANGLES, sphere_num * triangles.size()));

    for (size_t j = 0; j < sphere_num; ++j) {
      size_t vertex_offset = j * vertices->size();
      for (size_t k = 0, k_end = vertices->size(); k < k_end; ++k) {
        spheres_normals->at(vertex_offset + k) = normals->at(k);
        spheres_vertices->at(vertex_offset + k) = vertices->at(k) * radii[sphere_offset + j] + centers->at(sphere_offset + j);
        spheres_colors->at(vertex_offset + k) = colors->at(sphere_offset + j);
      }

      size_t triangle_offset = j * triangles.size();
      for (size_t k = 0, k_end = triangles.size(); k < k_end; ++k) {
        sphere_triangles->at(triangle_offset + k) = vertex_offset + triangles[k];
      }
    }

    osg::ref_ptr < osg::Geometry > geometry = new osg::Geometry;
    geometry->setUseDisplayList(true);
    geometry->setVertexArray(spheres_vertices);
#if OSG_VERSION_LESS_THAN(3,1,0)
    geometry->setNormalArray(spheres_normals);
    geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
    geometry->setColorArray(spheres_colors);
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
#else
    geometry->setNormalArray(spheres_normals, osg::Array::BIND_PER_VERTEX);
    geometry->setColorArray(spheres_colors, osg::Array::BIND_PER_VERTEX);
#endif
    geometry->addPrimitiveSet(sphere_triangles.get());
    geode->addDrawable(geometry);
  }

  return geode;
}

osg::Geode* drawDisk(const osg::Vec3& center, double radius, const osg::Vec3& normal, const osg::Vec4& color) {
  osg::Vec3 cross(1.0f, 0.0f, 0.0f);
  if (std::abs(normal * cross) > 0.9f)
    cross = osg::Vec3(0.0f, 1.0f, 0.0f);
  osg::Vec3 base = normal ^ cross;
  base = base * (radius / base.length());

  size_t vertices_num = 32;
  osg::Quat rotation(2 * M_PI / vertices_num, normal);
  osg::ref_ptr < osg::Vec3Array > vertices(new osg::Vec3Array(vertices_num));
  for (size_t i = 0; i < vertices_num; i++) {
    vertices->at(i) = center + base;
    base = rotation * base;
  }
  vertices->push_back(center);

  osg::ref_ptr < osg::Vec4Array > colors = new osg::Vec4Array;
  colors->push_back(color);
  osg::ref_ptr < osg::Vec3Array > normals = new osg::Vec3Array;
  normals->push_back(normal);

  osg::Geode* geode = new osg::Geode;
  osg::Geometry* geometry = new osg::Geometry;
  geometry->setUseDisplayList(true);
  geometry->setVertexArray(vertices);
#if OSG_VERSION_LESS_THAN(3,1,0)
  geometry->setNormalArray(normals);
  geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
  geometry->setColorArray(colors);
  geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
#else
  geometry->setNormalArray(normals, osg::Array::BIND_OVERALL);
  geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
#endif

  osg::ref_ptr < osg::DrawElementsUShort > faces = new osg::DrawElementsUShort(GL_TRIANGLES, 3 * vertices_num);
  for (size_t i = 0; i < vertices_num; ++i) {
    faces->at(3 * i + 0) = i;
    faces->at(3 * i + 1) = (i + 1) % vertices_num;
    faces->at(3 * i + 2) = vertices_num;
  }
  geometry->addPrimitiveSet(faces.get());
  geode->addDrawable(geometry);

  return geode;
}

osg::Geode* drawBox(const osg::Vec3& center, double width, const osg::Vec4& color) {
  osg::Geode* geode = new osg::Geode();

  osg::Box* box = new osg::Box(center, width);
  osg::ShapeDrawable* drawable = new osg::ShapeDrawable(box);
  drawable->setColor(color);
  geode->addDrawable(drawable);

  return geode;
}

osg::Geode* drawBox(const osg::Vec3& center, double length_x, double length_y, double length_z, const osg::Vec4& color) {
  osg::Geode* geode = new osg::Geode();

  osg::Box* box = new osg::Box(center, length_x, length_y, length_z);
  osg::ShapeDrawable* drawable = new osg::ShapeDrawable(box);
  drawable->setColor(color);
  geode->addDrawable(drawable);

  return geode;
}

osg::Geode* drawTetrahedron(const osg::Vec3& center, double radius, const osg::Vec4& color) {
  double offset = radius / std::sqrt(3);
  osg::ref_ptr < osg::Vec3Array > vertices = new osg::Vec3Array(4);
  vertices->at(0) = center + osg::Vec3(-offset, -offset, -offset);
  vertices->at(1) = center + osg::Vec3(+offset, +offset, -offset);
  vertices->at(2) = center + osg::Vec3(+offset, -offset, +offset);
  vertices->at(3) = center + osg::Vec3(-offset, +offset, +offset);

  osg::ref_ptr < osg::Vec4Array > colors = new osg::Vec4Array;
  colors->push_back(color);

  osg::ref_ptr < osg::Vec3Array > normals = new osg::Vec3Array(4);
  normals->at(0) = osg::Vec3(+offset, -offset, -offset);
  normals->at(0).normalize();
  normals->at(1) = osg::Vec3(-offset, -offset, +offset);
  normals->at(1).normalize();
  normals->at(2) = osg::Vec3(-offset, +offset, -offset);
  normals->at(2).normalize();
  normals->at(3) = osg::Vec3(+offset, +offset, +offset);
  normals->at(3).normalize();

  osg::Geode* geode = new osg::Geode;
  osg::Geometry* geometry = new osg::Geometry;
  geometry->setUseDisplayList(true);
  geometry->setVertexArray(vertices);
#if OSG_VERSION_LESS_THAN(3,1,0)
  geometry->setNormalArray(normals);
  geometry->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
  geometry->setColorArray(colors);
  geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
#else
  geometry->setNormalArray(normals, osg::Array::BIND_PER_PRIMITIVE_SET);
  geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
#endif

  osg::ref_ptr < osg::DrawElementsUShort > face_0 = new osg::DrawElementsUShort(GL_TRIANGLES, 3);
  face_0->at(0) = 0;
  face_0->at(1) = 1;
  face_0->at(2) = 2;
  geometry->addPrimitiveSet(face_0.get());

  osg::ref_ptr < osg::DrawElementsUShort > face_1 = new osg::DrawElementsUShort(GL_TRIANGLES, 3);
  face_1->at(0) = 0;
  face_1->at(1) = 2;
  face_1->at(2) = 3;
  geometry->addPrimitiveSet(face_1.get());

  osg::ref_ptr < osg::DrawElementsUShort > face_2 = new osg::DrawElementsUShort(GL_TRIANGLES, 3);
  face_2->at(0) = 0;
  face_2->at(1) = 3;
  face_2->at(2) = 1;
  geometry->addPrimitiveSet(face_2.get());

  osg::ref_ptr < osg::DrawElementsUShort > face_3 = new osg::DrawElementsUShort(GL_TRIANGLES, 3);
  face_3->at(0) = 1;
  face_3->at(1) = 3;
  face_3->at(2) = 2;
  geometry->addPrimitiveSet(face_3.get());

  geode->addDrawable(geometry);

  return geode;
}

osg::Geode* drawPolygon(osg::Vec3Array* polygon, const osg::Vec4& color) {
  osg::Vec4Array* colors = new osg::Vec4Array;
  osg::Vec4 transparentColor = color;
  transparentColor.a() = 0.3f;
  colors->push_back(transparentColor);

  osg::Geometry* geometry = new osg::Geometry;
  geometry->setUseDisplayList(true);
  geometry->setVertexArray(polygon);
#if OSG_VERSION_LESS_THAN(3,1,0)
  geometry->setColorArray(colors);
  geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
#else
  geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
#endif
  geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, polygon->size()));

  osg::Geode* geode = new osg::Geode();
  geode->addDrawable(geometry);

  return geode;
}

osg::Geode* drawBBox(osg::BoundingBox* bbox, double thickness, const osg::Vec4& color) {
  osg::CompositeShape* composite_shape = new osg::CompositeShape();
  std::vector < osg::ref_ptr<osg::LineSegment> > edges;
  edges.push_back(new osg::LineSegment(osg::Vec3(bbox->xMin(), bbox->yMin(), bbox->zMin()), osg::Vec3(bbox->xMax(), bbox->yMin(), bbox->zMin())));
  edges.push_back(new osg::LineSegment(osg::Vec3(bbox->xMin(), bbox->yMin(), bbox->zMin()), osg::Vec3(bbox->xMin(), bbox->yMax(), bbox->zMin())));
  edges.push_back(new osg::LineSegment(osg::Vec3(bbox->xMin(), bbox->yMin(), bbox->zMin()), osg::Vec3(bbox->xMin(), bbox->yMin(), bbox->zMax())));

  edges.push_back(new osg::LineSegment(osg::Vec3(bbox->xMin(), bbox->yMax(), bbox->zMax()), osg::Vec3(bbox->xMax(), bbox->yMax(), bbox->zMax())));
  edges.push_back(new osg::LineSegment(osg::Vec3(bbox->xMin(), bbox->yMax(), bbox->zMax()), osg::Vec3(bbox->xMin(), bbox->yMin(), bbox->zMax())));
  edges.push_back(new osg::LineSegment(osg::Vec3(bbox->xMin(), bbox->yMax(), bbox->zMax()), osg::Vec3(bbox->xMin(), bbox->yMax(), bbox->zMin())));

  edges.push_back(new osg::LineSegment(osg::Vec3(bbox->xMax(), bbox->yMax(), bbox->zMin()), osg::Vec3(bbox->xMin(), bbox->yMax(), bbox->zMin())));
  edges.push_back(new osg::LineSegment(osg::Vec3(bbox->xMax(), bbox->yMax(), bbox->zMin()), osg::Vec3(bbox->xMax(), bbox->yMin(), bbox->zMin())));
  edges.push_back(new osg::LineSegment(osg::Vec3(bbox->xMax(), bbox->yMax(), bbox->zMin()), osg::Vec3(bbox->xMax(), bbox->yMax(), bbox->zMax())));

  edges.push_back(new osg::LineSegment(osg::Vec3(bbox->xMax(), bbox->yMin(), bbox->zMax()), osg::Vec3(bbox->xMin(), bbox->yMin(), bbox->zMax())));
  edges.push_back(new osg::LineSegment(osg::Vec3(bbox->xMax(), bbox->yMin(), bbox->zMax()), osg::Vec3(bbox->xMax(), bbox->yMax(), bbox->zMax())));
  edges.push_back(new osg::LineSegment(osg::Vec3(bbox->xMax(), bbox->yMin(), bbox->zMax()), osg::Vec3(bbox->xMax(), bbox->yMin(), bbox->zMin())));

  for (size_t i = 0, i_end = edges.size(); i < i_end; ++i) {
    osg::Vec3 center = (edges[i]->start() + edges[i]->end()) / 2;
    osg::Vec3 offset = edges[i]->end() - edges[i]->start();
    osg::Vec3 zAxis(0.0, 0.0, 1.0);
    osg::Vec3 rotation = zAxis ^ offset;
    float angle = acos((zAxis * offset) / offset.length());
    osg::Cylinder* cylinder = new osg::Cylinder(center, thickness, offset.length());
    cylinder->setRotation(osg::Quat(angle, rotation));
    composite_shape->addChild(cylinder);
  }

  osg::Geode* geode = new osg::Geode();
  osg::ShapeDrawable* drawable = new osg::ShapeDrawable(composite_shape);
  drawable->setColor(color);
  geode->addDrawable(drawable);

  return geode;
}

void computeNodePathToRoot(osg::Node& node, osg::NodePath& np) {
  np.clear();
  osg::NodePathList nodePaths = node.getParentalNodePaths();
  if (!nodePaths.empty()) {
    np = nodePaths.front();
  }

  return;
}
}
;
