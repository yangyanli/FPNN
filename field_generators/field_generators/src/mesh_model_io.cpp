#include <boost/filesystem.hpp>

#include "PLY/io.h"
#include "common.h"
#include "mesh_io.h"
#include "file_format_obj.h"
#include "file_format_off.h"
#include "file_format_ply.h"
#include "osg_viewer_widget.h"

#include "mesh_model.h"

bool MeshModel::load(const std::string& filename, OSGViewerWidget* osg_viewer_widget) {
  QWriteLocker locker(&read_write_lock_);
  expired_ = true;

  if (!boost::filesystem::exists(filename))
    return false;

  bool flag = false;
  std::string extension = boost::filesystem::path(filename).extension().string();
  if (extension == ".obj") {
    flag = readObjFile(filename);
  } else if (extension == ".off") {
    flag = readOffFile(filename);
  } else if (extension == ".ply") {
    flag = readPlyFile(filename);
  }

  locker.unlock();
  if (flag && osg_viewer_widget != nullptr) {
    osg_viewer_widget->centerScene();
  }

  return flag;
}

bool MeshModel::save(const std::string& filename) {
  QReadLocker locker(&read_write_lock_);
  expired_ = true;

  bool flag = false;
  std::string extension = boost::filesystem::path(filename).extension().string();
  if (extension == ".ply") {
    flag = savePlyFile(filename);
  }

  return flag;
}

bool MeshModel::readObjFile(const std::string& filename) {
  FILE* obj_file = fopen(filename.c_str(), "r");
  if (!obj_file)
    return false;

  ObjParseCallbacks ocb;
  memset(&ocb, 0, sizeof(ocb));
  ocb.onVertex = &FileFormatObj::addVertex;
  ocb.onTexel = &FileFormatObj::addTexel;
  ocb.onNormal = &FileFormatObj::addNormal;
  ocb.onStartLine = &FileFormatObj::startLine;
  ocb.onAddToLine = &FileFormatObj::addToLine;
  ocb.onStartFace = &FileFormatObj::startFace;
  ocb.onAddToFace = &FileFormatObj::addToFace;
  ocb.onStartObject = &FileFormatObj::startObject;
  ocb.onStartGroup = &FileFormatObj::startGroup;
  ocb.onGroupName = &FileFormatObj::addGroupName;
  ocb.onRefMaterialLib = &FileFormatObj::refMaterialLib;
  ocb.onUseMaterial = &FileFormatObj::useMaterial;
  ocb.onSmoothingGroup = &FileFormatObj::smoothingGroup;

  ObjFileContent* pofc = new ObjFileContent();
  pofc->filename = filename;

  ocb.userData = pofc;
  if (ReadObjFile(obj_file, &ocb) != 0) {
    fclose(obj_file);
    delete pofc;
    return false;
  }
  fclose(obj_file);

  ObjFileContent& ofc = *pofc;
  vertices_->clear();
  vertices_->assign(ofc.vertices->begin(), ofc.vertices->end());

  colors_->assign(vertices_->size(), osg::Vec4(0.8, 0.8, 0.8, 1.0));

  faces_.clear();
  for (size_t i = 0, i_end = ofc.faces.size(); i < i_end; ++i) {
    faces_.push_back(std::vector<int>());
    for (size_t j = 0, j_end = ofc.faces[i].facevertices.size(); j < j_end; ++j)
      faces_.back().push_back(ofc.faces[i].facevertices[j].vertex_idx - 1);
  }

  face_normals_->assign(ofc.faces.size(), osg::Vec3(0.0f, 0.0f, 0.0f));
  for (size_t i = 0, i_end = faces_.size(); i < i_end; ++i) {
    osg::Vec3 vector_0_1(vertices_->at(faces_[i][1]) - vertices_->at(faces_[i][0]));
    osg::Vec3 vector_0_2(vertices_->at(faces_[i][2]) - vertices_->at(faces_[i][0]));
    face_normals_->at(i) = vector_0_1 ^ vector_0_2;
    face_normals_->at(i).normalize();
  }

  delete pofc;
  return true;
}

bool MeshModel::readOffFile(const std::string& filename) {
  FILE* off_file = fopen(filename.c_str(), "r");
  if (!off_file)
    return false;

  OffParseCallbacks ocb;
  memset(&ocb, 0, sizeof(ocb));
  ocb.onVertex = &FileFormatOff::addVertex;
  ocb.onStartFace = &FileFormatOff::startFace;
  ocb.onAddToFace = &FileFormatOff::addToFace;
  ocb.onEndFace = &FileFormatOff::endFace;

  OffFileContent* pofc = new OffFileContent();
  pofc->filename = filename;

  ocb.userData = pofc;
  if (ReadOffFile(off_file, &ocb) != 0) {
    fclose(off_file);
    delete pofc;
    return false;
  }

  fclose(off_file);

  OffFileContent& ofc = *pofc;

  vertices_->clear();
  vertices_->assign(ofc.vertices->begin(), ofc.vertices->end());
  colors_->assign(vertices_->size(), osg::Vec4(0.8, 0.8, 0.8, 1.0));

  faces_.clear();
  face_normals_->assign(ofc.faces.size(), osg::Vec3(0.0f, 0.0f, 0.0f));
  for (size_t i = 0, i_end = ofc.faces.size(); i < i_end; ++i) {
    faces_.push_back(std::vector<int>());
    std::vector<int>& face = faces_.back();
    for (size_t j = 0, j_end = ofc.faces[i].facevertices.size(); j < j_end; ++j)
      face.push_back(ofc.faces[i].facevertices[j].vertex_idx);
  }

  for (size_t i = 0, i_end = faces_.size(); i < i_end; ++i) {
    osg::Vec3 vector_0_1(vertices_->at(faces_[i][1]) - vertices_->at(faces_[i][0]));
    osg::Vec3 vector_0_2(vertices_->at(faces_[i][2]) - vertices_->at(faces_[i][0]));
    face_normals_->at(i) = vector_0_1 ^ vector_0_2;
    face_normals_->at(i).normalize();
  }

  delete pofc;
  return true;
}

bool MeshModel::readPlyFile(const std::string& filename) {
  PLY::Header header;
  PLY::Reader reader(header);
  if (!reader.open_file(filename.c_str()))
    return false;

  PLY::Storage storage(header);
  PLY::Element& vertex = *header.find_element(PLY::VertexItem::name);
  PLY::Element& face = *header.find_element(PLY::FaceItem::name);

  PLY::VertexArray vertices;
  storage.set_collection(header, vertex, vertices);
  std::vector<PLY::FaceItem> collection;
  PLY::FaceExternal faces(collection);
  storage.set_collection(header, face, faces);

  // Read the data in the file into the storage.
  if (!reader.read_data(&storage)) {
    reader.close_file();
    return false;
  }
  reader.close_file();

  vertices_->clear();
  vertices_->reserve(vertices.size());
  colors_->clear();
  colors_->reserve(vertices.size());
  if (vertices.size() > 0) {
    vertices.restart();
    for (size_t i = 0, i_end = vertices.size(); i < i_end; ++i) {
      PLY::VertexItem& v = vertices.next<PLY::VertexItem>();
      vertices_->push_back(osg::Vec3(v.x(), v.y(), v.z()));
      colors_->push_back(osg::Vec4(v.red() / 255.0f, v.green() / 255.0f, v.blue() / 255.0f, v.alpha() / 255.0f));
    }
  }

  faces_.clear();
  if (collection.size() > 0) {
    for (size_t i = 0, i_end = collection.size(); i < i_end; ++i) {
      PLY::FaceItem& face = collection[i];
      faces_.push_back(std::vector<int>());
      for (size_t j = 0, j_end = face.size(); j < j_end; ++j)
        faces_.back().push_back(face.vertex(j));
    }

    face_normals_->assign(collection.size(), osg::Vec3(0.0f, 0.0f, 0.0f));
    for (size_t i = 0, i_end = faces_.size(); i < i_end; ++i) {
      osg::Vec3 vector_0_1(vertices_->at(faces_[i][1]) - vertices_->at(faces_[i][0]));
      osg::Vec3 vector_0_2(vertices_->at(faces_[i][2]) - vertices_->at(faces_[i][0]));
      face_normals_->at(i) = vector_0_1 ^ vector_0_2;
      face_normals_->at(i).normalize();
    }
  }

  return true;
}

bool MeshModel::savePlyFile(const std::string& filename) {
  PLY::Header header;
  PLY::Writer writer(header);
  if (!writer.open_file(filename.c_str(), PLY::BINARY_LE))
    return false;

  PLY::VertexItem().describe_element(header);
  PLY::FaceItem().describe_element(header);
  PLY::Element& vertex = *header.find_element(PLY::VertexItem::name);
  PLY::Element& face = *header.find_element(PLY::FaceItem::name);

  // Finally, there must be actual data to store.
  std::vector<PLY::VertexItem> vert_data;
  for (size_t i = 0, i_end = vertices_->size(); i < i_end; ++i) {
    osg::Vec3& v = vertices_->at(i);
    vert_data.push_back(PLY::VertexItem(v.x(), v.y(), v.z()));
  }

  std::vector<PLY::FaceItem> face_data;
  for (size_t i = 0, i_end = faces_.size(); i < i_end; ++i) {
    std::vector<int>& face = faces_[i];
    face_data.push_back(PLY::FaceItem(face.size()));
    for (size_t j = 0, j_end = face.size(); j < j_end; ++j)
      face_data.back().vertex(j, face[j]);
  }

  PLY::Storage storage(header);
  PLY::VertexArray vertices;
  storage.set_collection(header, vertex, vertices);
  vertices.prepare(vert_data.size());
  for (size_t n = 0; n < vert_data.size(); ++n)
    vertices.next<PLY::VertexItem>() = vert_data[n];

  PLY::FaceExternal faces(face_data);
  storage.set_collection(header, face, faces);

  if (!writer.write_data(&storage)) {
    writer.close_file();
    return false;
  }

  writer.close_file();
  return true;
}
