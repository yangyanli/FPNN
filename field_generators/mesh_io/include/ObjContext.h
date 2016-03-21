#ifndef __OBJCONTEXT_H
#define __OBJCONTEXT_H

#include <stdio.h>
#include "usercallbacks.h"

typedef struct _tagObjContext {
  void *scanner;
  ObjParseCallbacks *user_cb;
  FILE *is;
} ObjContext;

/* Defined in wavefrontobj.l */
void InitObjScanner(ObjContext *ctx);
void DestroyObjScanner(ObjContext *ctx);

#endif /* __OBJCONTEXT_H */
