#ifndef __MTLCONTEXT_H
#define __MTLCONTEXT_H

#include <stdio.h>
#include "usercallbacks.h"

typedef struct _tagMtlContext {
  void *scanner;
  MtlParseCallbacks *user_cb;
  FILE *is;
} MtlContext;

/* Defined in wavefrontobj.l */
void InitMtlScanner(MtlContext *ctx);
void DestroyMtlScanner(MtlContext *ctx);

#endif /* __MTLCONTEXT_H */
