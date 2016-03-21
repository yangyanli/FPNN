#include <osg/Version>

#include "cgal_types.h"
#include "osg_utility.h"

#include "mesh_model.h"

MeshModel::MeshModel(void) :
    vertices_(new osg::Vec3Array), colors_(new osg::Vec4Array), face_normals_(new osg::Vec3Array) {
}

MeshModel::~MeshModel(void) {
}

void MeshModel::updateImpl(void) {
  osg::ref_ptr<osg::Geode> geode(new osg::Geode);
  osg::ref_ptr < osg::Geometry > geometry = new osg::Geometry;
  geometry->setUseDisplayList(true);
  geometry->setVertexArray(vertices_);
#if OSG_VERSION_LESS_THAN(3,1,0)
  geometry->setColorArray(colors_);
  geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
#else
  geometry->setColorArray(colors_, osg::Array::BIND_PER_VERTEX);
#endif
  if (faces_.empty()) {
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices_->size()));
  } else {
#if OSG_VERSION_LESS_THAN(3,1,0)
    geometry->setNormalArray(face_normals_);
    geometry->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
#else
    geometry->setNormalArray(face_normals_, osg::Array::BIND_PER_PRIMITIVE_SET);
#endif
    for (size_t i = 0, i_end = faces_.size(); i < i_end; ++i) {
      size_t vertex_num = faces_[i].size();
      osg::ref_ptr < osg::DrawElementsUInt > face = new osg::DrawElementsUInt(GL_TRIANGLE_FAN, vertex_num);
      for (size_t j = 0; j < vertex_num; ++j)
        face->at(j) = faces_[i][j];

      geometry->addPrimitiveSet(face.get());
    }
  }

  geode->addDrawable(geometry);
  content_root_->addChild(geode);

  return;
}

double MeshModel::sampleScan(PclPointCloud::Ptr point_cloud, int resolution, double noise) {
  osg::ref_ptr < osg::Vec3Array > eye_directions = new osg::Vec3Array;
  osg::ref_ptr < osg::Vec3Array > eye_positions = new osg::Vec3Array;
  OSGUtility::sampleOnSphere(eye_positions, eye_directions, 2);

  return Renderable::virtualScan(eye_directions, resolution, noise, point_cloud);
}

double MeshModel::virtualScan(PclPointCloud::Ptr point_cloud, int resolution, double noise) {
  osg::Vec3 axis(0.0f, 1.0f, 0.0f);
  osg::Vec3 initial_direction_look_down(-1.0f, -1.0f, 0.0f);
  initial_direction_look_down.normalize();
  osg::Vec3 initial_direction_look_up(-1.0f, 0.2f, 0.0f);
  initial_direction_look_up.normalize();
  int view_num = 12;
  osg::ref_ptr < osg::Vec3Array > eye_directions = new osg::Vec3Array;
  for (int i = 0; i < view_num; ++i) {
    osg::Matrix rotation = osg::Matrix::rotate(2 * M_PI / view_num * i, axis);
    eye_directions->push_back(initial_direction_look_down * rotation);
    eye_directions->push_back(initial_direction_look_up * rotation);
  }

  return Renderable::virtualScan(eye_directions, resolution, noise, point_cloud);
}

void MeshModel::savePOVRay(const std::string& filename, osg::Matrix transformation) {
  std::ofstream fout(filename);

#ifdef USE_SMOOTH_TRIANGLES
  osg::Quat rotation = transformation.getRotate();
  osg::ref_ptr<osg::Vec3Array> vertex_normals(new osg::Vec3Array(vertices_->size()));

  for (size_t i = 0, i_end = faces_.size(); i < i_end; ++ i)
  {
    const std::vector<int>& face = faces_[i];
    osg::Vec3 face_normal = face_normals_->at(i);
    face_normal = rotation*face_normal;

    for (size_t j = 0, j_end = face.size(); j < j_end; ++ j)
    vertex_normals->at(face[j]) += face_normal;
  }

  for (size_t i = 0, i_end = vertex_normals->size(); i < i_end; ++ i)
  vertex_normals->at(i).normalize();

  for (size_t i = 0, i_end = faces_.size(); i < i_end; ++ i)
  {
    const std::vector<int>& face = faces_[i];
    osg::Vec3 v_0 = transformation.preMult(vertices_->at(face[0]));
    osg::Vec3 n_0 = vertex_normals->at(face[0]);
    for (size_t j = 0, j_end = face.size()-2; j < j_end; ++ j)
    {
      osg::Vec3 v_1 = transformation.preMult(vertices_->at(face[j+1]));
      osg::Vec3 n_1 = vertex_normals->at(face[j+1]);
      osg::Vec3 v_2 = transformation.preMult(vertices_->at(face[j+2]));
      osg::Vec3 n_2 = vertex_normals->at(face[j+2]);

      fout << "smooth_triangle{"
      << "<" << v_0.x() << ", " << v_0.y() << ", " << v_0.z() << ">, <" << n_0.x() << "," << n_0.y() << "," << n_0.z() << ">,"
      << "<" << v_1.x() << ", " << v_1.y() << ", " << v_1.z() << ">, <" << n_1.x() << "," << n_1.y() << "," << n_1.z() << ">,"
      << "<" << v_2.x() << ", " << v_2.y() << ", " << v_2.z() << ">, <" << n_2.x() << "," << n_2.y() << "," << n_2.z() << ">"
      << "texture { pigment { rgb <0.8 0.8 0.8> filter 0 } finish { ambient ambient_value diffuse diffuse_value phong phong_value}}}\n";
    }
  }
#else
  for (size_t i = 0, i_end = faces_.size(); i < i_end; ++i) {
    const std::vector<int>& face = faces_[i];
    osg::Vec3 v_0 = transformation.preMult(vertices_->at(face[0]));
    osg::Vec4 c_0 = colors_->at(face[0]);
    for (size_t j = 0, j_end = face.size() - 2; j < j_end; ++j) {
      osg::Vec3 v_1 = transformation.preMult(vertices_->at(face[j + 1]));
      osg::Vec3 v_2 = transformation.preMult(vertices_->at(face[j + 2]));

      osg::Vec4 c_1 = colors_->at(face[j + 1]);
      osg::Vec4 c_2 = colors_->at(face[j + 2]);

      osg::Vec4 color = (c_0 + c_1 + c_2) / 3;

      fout << "triangle{" << "<" << v_0.x() << ", " << v_0.y() << ", " << v_0.z() << ">," << "<" << v_1.x() << ", " << v_1.y() << ", " << v_1.z() << ">," << "<"
          << v_2.x() << ", " << v_2.y() << ", " << v_2.z() << ">" << "texture { pigment { rgb <" << color.r() << " " << color.g() << " " << color.b()
          << "> filter 0 } finish { ambient ambient_value diffuse diffuse_value phong phong_value}}}\n";
    }
  }
#endif

  return;
}

void MeshModel::savePOVRay(const std::string& filename, const std::string& colorname, osg::Matrix transformation /*= osg::Matrix::identity()*/) {
  QReadLocker locker(&read_write_lock_);

  std::ofstream fout(filename);

  for (size_t i = 0, i_end = faces_.size(); i < i_end; ++i) {
    const std::vector<int>& face = faces_[i];
    osg::Vec3 v_0 = transformation.preMult(vertices_->at(face[0]));
    osg::Vec4 c_0 = colors_->at(face[0]);
    for (size_t j = 0, j_end = face.size() - 2; j < j_end; ++j) {
      osg::Vec3 v_1 = transformation.preMult(vertices_->at(face[j + 1]));
      osg::Vec3 v_2 = transformation.preMult(vertices_->at(face[j + 2]));

      fout << "triangle{" << "<" << v_0.x() << ", " << v_0.y() << ", " << v_0.z() << ">," << "<" << v_1.x() << ", " << v_1.y() << ", " << v_1.z() << ">," << "<"
          << v_2.x() << ", " << v_2.y() << ", " << v_2.z() << ">" << "texture { pigment { " << colorname
          << " filter 0 } finish { ambient ambient_value diffuse diffuse_value phong phong_value}}}\n";
    }
  }

  fout.close();

  return;
}

void MeshModel::scale(double expected_height) {
  osg::BoundingBox bbox;
  for (size_t i = 0, i_end = vertices_->size(); i < i_end; ++i)
    bbox.expandBy(vertices_->at(i));

  const osg::Vec3& center = bbox.center();
  double scale = expected_height / (bbox.yMax() - bbox.yMin());
  osg::Matrix scaling(osg::Matrix::scale(scale, scale, scale));
  osg::Matrix translation = osg::Matrix::translate(center.x(), -bbox.yMin(), center.z());
  osg::Matrix transformation = scaling * translation;

  QWriteLocker locker(&read_write_lock_);
  expired_ = true;

  for (size_t i = 0, i_end = vertices_->size(); i < i_end; ++i)
    vertices_->at(i) = transformation.preMult(vertices_->at(i));

  return;
}

void MeshModel::merge(const MeshModel& mesh_model, osg::Matrix transformation) {
  QWriteLocker locker(&read_write_lock_);
  expired_ = true;

  colors_->insert(colors_->end(), mesh_model.colors_->begin(), mesh_model.colors_->end());

  bool apply_transformation = (!transformation.isIdentity());

  size_t vertex_offset = vertices_->size();
  vertices_->insert(vertices_->end(), mesh_model.vertices_->begin(), mesh_model.vertices_->end());
  if (apply_transformation) {
    for (size_t i = vertex_offset, i_end = vertices_->size(); i < i_end; ++i)
      vertices_->at(i) = transformation.preMult(vertices_->at(i));
  }

  size_t face_offset = faces_.size();
  faces_.insert(faces_.end(), mesh_model.faces_.begin(), mesh_model.faces_.end());
  for (size_t i = face_offset, i_end = faces_.size(); i < i_end; ++i) {
    std::vector<int>& face = faces_[i];
    for (size_t j = 0, j_end = face.size(); j < j_end; ++j) {
      face[j] += vertex_offset;
    }
  }

  face_normals_->insert(face_normals_->end(), mesh_model.face_normals_->begin(), mesh_model.face_normals_->end());
  if (apply_transformation) {
    osg::Quat rotation = transformation.getRotate();
    for (size_t i = face_offset, i_end = faces_.size(); i < i_end; ++i) {
      face_normals_->at(i) = rotation * face_normals_->at(i);
    }
  }

  return;
}
