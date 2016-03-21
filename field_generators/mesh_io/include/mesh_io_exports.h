#pragma once
#ifndef MESH_IO_EXPORTS_H_
#define MESH_IO_EXPORTS_H_

#if defined WIN32 || defined _WIN32 || defined WINCE || defined __MINGW32__
#ifdef MESH_IO_API_EXPORTS
#define MESH_IO_EXPORTS __declspec(dllexport)
#else
#define MESH_IO_EXPORTS __declspec(dllimport)
#endif
#else
#define MESH_IO_EXPORTS
#endif

#endif  //#ifndef MESH_IO_EXPORTS_H_
