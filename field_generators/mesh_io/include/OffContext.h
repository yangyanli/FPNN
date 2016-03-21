#ifndef __OFFCONTEXT_H
#define __OFFCONTEXT_H

#include <stdio.h>
#include "usercallbacks.h"

typedef struct _tagOffContext {
  void *scanner;
  OffParseCallbacks *user_cb;
  FILE *is;
} OffContext;

/* Defined in off.l */
void InitOffScanner(OffContext *ctx);
void DestroyOffScanner(OffContext *ctx);

#endif /* __OFFCONTEXT_H */
