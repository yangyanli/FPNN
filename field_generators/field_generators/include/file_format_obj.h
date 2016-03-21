#pragma once
#ifndef FILE_FORMAT_OBJ_H
#define FILE_FORMAT_OBJ_H

#include <string>
#include <osg/Vec3d>
#include <osg/Vec4>
#include <osg/Array>

typedef enum tagIlluminationModel {
  NoLighting = 0, // No lighting. Color on and Ambient off.
  DiffuseOnly = 1, // Diffuse lighting only. Color on and Ambient on.
  DiffuseAndSpecular = 2, // Highlight on.
  // Both diffuse lighting and specular highlights.
  ReflectionAndRaytrace = 3, // Reflection on and Ray trace on.
  GlassOnRaytraceOn = 4, // Transparency: Glass on
  // Reflection: Ray trace on
  FresnelOnRaytraceOn = 5, // Reflection: Fresnel on and Ray trace on
  RefractionOnFresnelOffRaytraceOn = 6, // Transparency: Refraction on
  // Reflection: Fresnel off and Ray trace on
  RefractionOnFresnelOnRaytraceOn = 7, // Transparency: Refraction on
  // Reflection: Fresnel on and Ray trace on
  ReflectionOnRaytraceOff = 8, // Reflection on and Ray trace off
  GlassOnRaytraceOff = 9, // Transparency: Glass on
  // Reflection: Ray trace off
  CastsShadowOnInvisSurfaces = 10 //Casts shadows onto invisible surfaces
} IlluminationModel;

struct ObjMaterial {
  ObjMaterial() :
      ambient(-1000.0, -1000.0, -1000.0), diffuse(-1000.0, -1000.0, -1000.0), specular(-1000.0, -1000.0, -1000.0), alpha_d(1.0), alpha_d_set(false), alpha_tr(
          1.0), alpha_tr_set(false), ns(0.0), illum(DiffuseAndSpecular), ni(0.0), map_KaID(-1), map_KdID(-1) {
  }

  std::string name;
  osg::Vec3d ambient; // Ambient color of the material. This is the  "base" color
  osg::Vec3d diffuse; // Diffuse color of the material
  osg::Vec3d specular; // Specular color weighted by the specular coefficient
  double alpha_d; // Alpha transparency
  bool alpha_d_set;
  double alpha_tr;
  bool alpha_tr_set;
  double ns; // Specular component of the Phong shading model ranges between 0 and 1000
  IlluminationModel illum; // Illumination model to be used by the material
  double ni; // Optical density. Specifies the optical density for the surface.
  // This is also known as index of refraction.

  // Texture file names
  std::string map_Ka; // Ambient texture map
  unsigned int map_KaID; // Ambient texture ID
  std::string map_Kd; // Diffuse texture map
  unsigned int map_KdID; // Diffuse texture ID
  std::string map_Ks; // Specular texture map
  std::string map_Kal; // Alpha texture map
  std::string map_bump; // Bump map

  bool operator ==(const ObjMaterial &m) const {
    return this->name == m.name;
  }
  bool operator ==(const std::string &name) const {
    return this->name == name;
  }
};

typedef std::vector<ObjMaterial> ObjMaterials;

struct ObjLineVertex {
  size_t vertex_idx;
  size_t texture_coord_idx;
};

typedef std::vector<ObjLineVertex> ObjLineVertexVector;

struct ObjLine {
  ObjLineVertexVector linevertices; // line description is vertex/texture index pairs
  size_t material;
};

typedef std::vector<ObjLine> ObjLineVector;

struct ObjFaceVertex {
  size_t vertex_idx;
  size_t texture_coord_idx;
  size_t normal_idx;
};

typedef std::vector<ObjFaceVertex> ObjFaceVertexVector;

struct ObjFace {
  ObjFace() :
      material(0), id_in_class(0), class_id(0), element_id(0) {
  }
  ObjFaceVertexVector facevertices; // face description is three vertex/texture/normal index triples
  size_t material;     // Matreial index started from 1. If == 0 then no material was assigned
  size_t class_id;   // Index starts from 1. Don't need to be considered while reading files
  size_t id_in_class;     // Index starts from 1. Don't need to be considered while reading files
  size_t element_id;
};

typedef std::vector<ObjFace> ObjFaceVector;

struct ObjGroup {
  ObjGroup(size_t first_idx, size_t last_idx) :
      first_face_idx(first_idx), last_face_idx(last_idx) {
  }
  size_t first_face_idx;        // face index started from 1
  size_t last_face_idx;         // this face is not belong to the group
};

typedef std::map<std::string, ObjGroup> ObjectMap;
typedef std::map<std::string, ObjGroup> GroupMap;
typedef std::map<size_t, ObjGroup> SmoothingGroupMap;
typedef std::list<std::string> CurrentGroupList;

struct ObjFileContent {
  ObjFileContent(void) :
      current_mtl_idx(0), vertices(new osg::Vec3dArray), texcoords(new osg::Vec2dArray), normals(new osg::Vec3dArray) {
  }
  virtual ~ObjFileContent(void) {
  }

  std::string filename;

  osg::ref_ptr<osg::Vec3dArray> vertices;
  osg::ref_ptr<osg::Vec2dArray> texcoords;
  osg::ref_ptr<osg::Vec3dArray> normals;

  ObjLineVector lines;
  ObjFaceVector faces;

  ObjectMap objects;
  std::string current_object;

  GroupMap groups;
  CurrentGroupList current_groups;

  SmoothingGroupMap smoothing_groups;
  size_t current_smoothing_group;

  ObjMaterials materials;
  ObjMaterial current_mtl;     // Temporary variable where (partially) parsed material is stored
  size_t current_mtl_idx; // Matreial index started from 1. If == 0 then no material was assigned
};

class FileFormatObj {
public:
  FileFormatObj(void);
  virtual ~FileFormatObj(void);

  // mtl
  static int addMaterial(char *name, void *userData);
  static int setAmbientColor(double r, double g, double b, void *userData);
  static int setDiffuseColor(double r, double g, double b, void *userData);
  static int setSpecularColor(double r, double g, double b, void *userData);
  static int setSpecularExponent(double se, void *userData);
  static int setOpticalDensity(double d, void *userData);
  static int setAlphaD(double a, void *userData);
  static int setAlphaTr(double a, void *userData);
  static int setIllumModel(int model, void *userData);
  static int setAmbientTextureMap(char *path, void *userData);
  static int setDiffuseTextureMap(char *path, void *userData);

  // obj
  static int addVertex(double x, double y, double z, double w, void *userData);
  static int addTexel(double u, double v, double w, void *userData);
  static int addNormal(double x, double y, double z, void *userData);
  static int startLine(void *userData);
  static int addToLine(int v, size_t vt, void *userData);
  static int startFace(void *userData);
  static int addToFace(size_t v, size_t vt, size_t vn, void *userData);
  static int startObject(char *name, void *userData);
  static int startGroup(void *userData);
  static int addGroupName(char *name, void *userData);
  static int refMaterialLib(char *filename, void *userData);
  static int useMaterial(char *material, void *userData);
  static int smoothingGroup(size_t groupid, void *userData);
};

#endif // FILE_FORMAT_OBJ_H
