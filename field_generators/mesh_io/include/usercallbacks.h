#ifndef __USERCALLBACKS_H
#define __USERCALLBACKS_H

#include <stddef.h>

typedef int (*AddVertexFunc)(double x, double y, double z, double w, void *userData);
typedef int (*AddTexelFunc)(double x, double y, double w, void *userData);
typedef int (*AddNormalFunc)(double x, double y, double z, void *userData);
typedef int (*StartLineFunc)(void *userData);
typedef int (*AddToLineFunc)(int v, size_t vt, void *userData);
typedef int (*StartFaceFunc)(void *userData);
typedef int (*AddToFaceFunc)(size_t v, size_t vt, size_t vn, void *userData);
typedef int (*StartObjectFunc)(char *name, void *userData);
typedef int (*StartGroupFunc)(void *userData);
typedef int (*AddGroupNameFunc)(char *name, void *userData);
typedef int (*RefMaterialLibFunc)(char *filename, void *userData);
typedef int (*UseMaterialFunc)(char *material, void *userData);
typedef int (*SmoothingGroupFunc)(size_t groupid, void *userData);

typedef struct tagObjParseCallbacks {
  AddVertexFunc onVertex;
  AddTexelFunc onTexel;
  AddNormalFunc onNormal;
  StartLineFunc onStartLine;
  AddToLineFunc onAddToLine;
  StartFaceFunc onStartFace;
  AddToFaceFunc onAddToFace;
  StartObjectFunc onStartObject;
  StartGroupFunc onStartGroup;
  AddGroupNameFunc onGroupName;
  RefMaterialLibFunc onRefMaterialLib;
  UseMaterialFunc onUseMaterial;
  SmoothingGroupFunc onSmoothingGroup;
  void *userData;
} ObjParseCallbacks;

typedef int (*OffAddVertexFunc)(double x, double y, double z, double r, double g, double b, double a, void *userData);
typedef int (*OffStartFaceFunc)(void *userData);
typedef int (*OffAddToFaceFunc)(size_t v, void *userData);
typedef int (*OffEndFaceFunc)(double r, double g, double b, double a, void *userData);
typedef struct tagOffParseCallbacks {
  OffAddVertexFunc onVertex;
  OffStartFaceFunc onStartFace;
  OffAddToFaceFunc onAddToFace;
  OffEndFaceFunc onEndFace;
  void *userData;
} OffParseCallbacks;

typedef int (*AddMaterialFunc)(char *name, void *userData);
typedef int (*SetAmbientColorFunc)(double r, double g, double b, void *userData);
typedef int (*SetDiffuseColorFunc)(double r, double g, double b, void *userData);
typedef int (*SetSpecularColorFunc)(double r, double g, double b, void *userData);
typedef int (*SetSpecularExponentFunc)(double se, void *userData);
typedef int (*SetOpticalDensityFunc)(double d, void *userData);
typedef int (*SetAlphaDFunc)(double a, void *userData);
typedef int (*SetAlphaTrFunc)(double a, void *userData);
typedef int (*SetTransmissionFunc)(double r, double g, double b, void *userData);
typedef int (*SetIllumModelFunc)(int model, void *userData);
typedef int (*SetAmbientTextureMapFunc)(char *path, void *userData);
typedef int (*SetDiffuseTextureMapFunc)(char *path, void *userData);

typedef struct tagMtlParseCallbacks {
  AddMaterialFunc onAddMaterial;
  SetAmbientColorFunc onSetAmbientColor;
  SetDiffuseColorFunc onSetDiffuseColor;
  SetSpecularColorFunc onSetSpecularColor;
  SetSpecularExponentFunc onSetSpecularExponent;
  SetOpticalDensityFunc onSetOpticalDensity;
  SetAlphaDFunc onSetAlphaD;
  SetAlphaTrFunc onSetAlphaTr;
  SetTransmissionFunc onSetTransmission;
  SetIllumModelFunc onSetIllumModel;
  SetAmbientTextureMapFunc onSetAmbientTextureMap;
  SetDiffuseTextureMapFunc onSetDiffuseTextureMap;
  void *userData;
} MtlParseCallbacks;

#endif /* __USERCALLBACKS_H */
