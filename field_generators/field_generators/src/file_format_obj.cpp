#include <algorithm>

#include <boost/filesystem.hpp>

#include "mesh_io.h"
#include "file_format_obj.h"

FileFormatObj::FileFormatObj(void) {

}

FileFormatObj::~FileFormatObj(void) {

}

int FileFormatObj::addVertex(double x, double y, double z, double w, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ofc->vertices->push_back(osg::Vec3d(x, y, z));

  return 0;
}

int FileFormatObj::addTexel(double u, double v, double w, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ofc->texcoords->push_back(osg::Vec2d(u, v));

  return 0;
}

int FileFormatObj::addNormal(double x, double y, double z, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ofc->normals->push_back(osg::Vec3d(x, y, z));

  return 0;
}

int FileFormatObj::startLine(void *userData) {
  return 0;
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ObjLine f;
  ofc->lines.push_back(f);
  ofc->lines.rend()->material = ofc->current_mtl_idx;
  return 0;
}

int FileFormatObj::addToLine(int v, size_t vt, void *userData) {
  return 0;
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ObjLineVertex fv;
  fv.vertex_idx = v;
  fv.texture_coord_idx = vt;
  ofc->lines.rbegin()->linevertices.push_back(fv);
  return 0;
}

int FileFormatObj::startFace(void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ObjFace f;
  ofc->faces.push_back(f);
  ofc->faces.rbegin()->material = ofc->current_mtl_idx;
  return 0;
}

int FileFormatObj::addToFace(size_t v, size_t vt, size_t vn, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ObjFaceVertex fv = { v, vt, vn };
  ofc->faces.rbegin()->facevertices.push_back(fv);
  return 0;
}

int FileFormatObj::startObject(char *name, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;

  if (!ofc->current_object.empty())
    ofc->objects.find(ofc->current_object)->second.last_face_idx = ofc->faces.size();

  ObjGroup g((ofc->faces.empty() ? 1 : ofc->faces.size()), 0);
  ofc->objects.insert(ObjectMap::value_type(name, g));
  ofc->current_object = name;

  return 0;
}

int FileFormatObj::startGroup(void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;

  if (ofc->current_groups.empty())
    return 0;

  for (CurrentGroupList::const_iterator i = ofc->current_groups.begin(); i != ofc->current_groups.end(); ++i) {
    ofc->groups.find(*i)->second.last_face_idx = ofc->faces.size() + 1;
  }

  ofc->current_groups.clear();

  return 0;
}

int FileFormatObj::addGroupName(char *name, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ObjGroup g((ofc->faces.empty() ? 1 : ofc->faces.size() + 1), 0);
  ofc->groups.insert(GroupMap::value_type(name, g));
  ofc->current_groups.push_back(name);
  return 0;
}

int FileFormatObj::smoothingGroup(size_t groupid, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  if (groupid == 0) {
    // smoothing group is turned off
    // do nothing and "s off" appears but not previous "s N" met.
    if (!ofc->smoothing_groups.empty()) {
      SmoothingGroupMap::iterator gi = ofc->smoothing_groups.find(ofc->current_smoothing_group);
      gi->second.last_face_idx = ofc->faces.empty() ? 1 : ofc->faces.size();
    }
  } else {
    ObjGroup g((ofc->faces.empty() ? 1 : ofc->faces.size()), 0);
    ofc->smoothing_groups.insert(SmoothingGroupMap::value_type(groupid, g));
    ofc->current_smoothing_group = groupid;
  }
  return 0;
}

int FileFormatObj::refMaterialLib(char *filename, void *userData) {
  FILE *mtlfile = nullptr;
  boost::filesystem::path path(filename);
  if (path.is_relative()) {
    ObjFileContent *ofc = (ObjFileContent*) userData;
    const std::string& obj_filename = ofc->filename;
    boost::filesystem::path obj_path(obj_filename);
    std::string mtl_filename = obj_path.parent_path().string() + "/" + std::string(filename);
    mtlfile = fopen(mtl_filename.c_str(), "r");
  } else
    mtlfile = fopen(filename, "r");

  if (!mtlfile) {
    fprintf(stderr, "Warning: can not open material file '%s'.\n"
        "Materials described in this file will not be available.\n", filename);
    return 0;
  }
  MtlParseCallbacks mcb;
  memset(&mcb, 0, sizeof(mcb));
  mcb.onAddMaterial = &FileFormatObj::addMaterial;
  mcb.onSetAmbientColor = &FileFormatObj::setAmbientColor;
  mcb.onSetDiffuseColor = &FileFormatObj::setDiffuseColor;
  mcb.onSetSpecularColor = &FileFormatObj::setSpecularColor;
  mcb.onSetSpecularExponent = &FileFormatObj::setSpecularExponent;
  mcb.onSetOpticalDensity = &FileFormatObj::setOpticalDensity;
  mcb.onSetAlphaD = &FileFormatObj::setAlphaD;
  mcb.onSetAlphaTr = &FileFormatObj::setAlphaTr;
  mcb.onSetIllumModel = &FileFormatObj::setIllumModel;
  mcb.onSetAmbientTextureMap = &FileFormatObj::setAmbientTextureMap;
  mcb.onSetDiffuseTextureMap = &FileFormatObj::setDiffuseTextureMap;
  mcb.userData = userData;
  int res = ReadMtlFile(mtlfile, &mcb);
  fclose(mtlfile);
  if (res != 0)
    fprintf(stderr, "Warning: error %i while reading '%s' MTL file.\n"
        "Materials described in this file will not be available.\n", res, filename);
  return 0;
}

int FileFormatObj::useMaterial(char *material, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  if (material[0] == '\0')
    ofc->current_mtl_idx = 0;
  else {
    ObjMaterials::const_iterator mat = std::find(ofc->materials.begin(), ofc->materials.end(), material);

    if (mat != ofc->materials.end())
      ofc->current_mtl_idx = mat - ofc->materials.begin() + 1;
    else
      ofc->current_mtl_idx = 0;
  }

  return 0;
}

int FileFormatObj::addMaterial(char *name, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ofc->materials.push_back(ofc->current_mtl);
  ofc->current_mtl.map_Ka = "";
  ofc->current_mtl.map_Kd = "";
  ofc->current_mtl.map_KaID = -1;
  ofc->current_mtl.map_KdID = -1;
  ofc->materials.rbegin()->name = name;
  return 0;
}

int FileFormatObj::setAmbientColor(double r, double g, double b, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ofc->current_mtl.ambient.x() = r;
  ofc->current_mtl.ambient.y() = g;
  ofc->current_mtl.ambient.z() = b;
  return 0;
}

int FileFormatObj::setDiffuseColor(double r, double g, double b, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ofc->current_mtl.diffuse.x() = r;
  ofc->current_mtl.diffuse.y() = g;
  ofc->current_mtl.diffuse.z() = b;
  return 0;
}

int FileFormatObj::setSpecularColor(double r, double g, double b, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ofc->current_mtl.specular.x() = r;
  ofc->current_mtl.specular.y() = g;
  ofc->current_mtl.specular.z() = b;
  return 0;
}

int FileFormatObj::setSpecularExponent(double se, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ofc->current_mtl.ns = 0.128f * se;
  return 0;
}

int FileFormatObj::setOpticalDensity(double d, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ofc->current_mtl.ni = d;
  return 0;
}

int FileFormatObj::setAlphaD(double a, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ofc->current_mtl.alpha_d = a;
  ofc->current_mtl.alpha_d_set = true;
  return 0;
}

int FileFormatObj::setAlphaTr(double a, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ofc->current_mtl.alpha_tr = a;
  ofc->current_mtl.alpha_tr_set = true;
  return 0;
}

int FileFormatObj::setIllumModel(int model, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ofc->current_mtl.illum = IlluminationModel(model);
  return 0;
}

int FileFormatObj::setAmbientTextureMap(char *path, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ofc->current_mtl.map_Ka = path;
  ofc->current_mtl.map_KaID = -1;
  return 0;
}

int FileFormatObj::setDiffuseTextureMap(char *path, void *userData) {
  ObjFileContent *ofc = (ObjFileContent*) userData;
  ofc->current_mtl.map_Kd = path;
  ofc->current_mtl.map_KdID = -1;
  return 0;
}
