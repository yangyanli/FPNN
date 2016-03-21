#ifndef __MESH_READER_H
#define __MESH_READER_H

#include <stdio.h>
#include "usercallbacks.h"

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * Read OBJ file into memory and initialize the 
 * content structure
 * @param stream - obj file handle opened for reading
 * @param ucb - structure with user callback function 
 *              pointers which will be called during
 *              parsing
 * @return 0 if successful, otherwise the error code
 */
int ReadObjFile(FILE *stream, ObjParseCallbacks *ucb);

/**
 * Read MTL file into memory and initialize the 
 * content structure
 * @param stream - obj file handle opened for reading
 * @param ucb - structure with user callback function 
 *              pointers which will be called during
 *              parsing
 * @return 0 if successful, otherwise the error code
 */
int ReadMtlFile(FILE *stream, MtlParseCallbacks *ucb);

/**
 * Read OFF file into memory and initialize the 
 * content structure
 * @param stream - off file handle opened for reading
 * @param ucb - structure with user callback function 
 *              pointers which will be called during
 *              parsing
 * @return 0 if successful, otherwise the error code
 */
int ReadOffFile(FILE *stream, OffParseCallbacks *ucb);

#ifdef  __cplusplus
}
#endif

#endif /* __MESH_READER_H */
