%option reentrant
%option bison-locations
%option bison-bridge
%option prefix="Obj_"
%option noyywrap
%option yylineno

word [\_a-zA-Z]+[a-zA-Z0-9\_\.\-]*
mtlfile [\_a-zA-Z0-9\.\\/][:\\/\.a-z\_A-Z0-9\-]*\.mtl
null (\(null\)|null)

%{
#include "objparser.y.h"
#include "ObjContext.h"
#include <stdlib.h>
#include <string.h>

#define YY_EXTRA_TYPE ObjContext*
#define YY_USER_ACTION yylloc->first_line = yylineno; 


#define YY_INPUT(buf,result,max_size) \
{ \
  char c = fgetc(yyextra->is); \
  if(feof(yyextra->is)) \
    result = YY_NULL; \
  else \
  { \
    buf[0] = c; \
    result = 1; \
  } \
}

%}



%%


[\n\r] { return EOL; }

#.*  {; /* skip comments */}

[ \t]+ ; /* skip whitespace */

v  { return VERTEX_MARKER; }

o  { return OBJECT_MARKER; }

g  { return GROUP_MARKER; }

vt { return TEXEL_MARKER; }

vn { return NORMAL_MARKER; }

l  { return LINE_MARKER; }

f  { return FACE_MARKER; }

c  { return CAMERA_MARKER; }

mtllib  { return MATERIALLIB_MARKER; }

usemtl  { return USEMATERIAL_MARKER; }

s  { return SMOOTHINGGROUP_MARKER; }

[\-\+]?(([0-9]*\.[0-9]*([eE][\-\+]?[0-9]+)?)|([0-9]+[eE][\-\+]?[0-9]+)) {
  yylval->fValue = (double)atof(yytext);
  return DECIMAL;
}

[\-\+]?[0-9]+ {
  yylval->intValue = atoi(yytext);
  return INTEGER;
}

[/\n] { return *yytext; }

off {
  return OFF_WORD;
}

{mtlfile} {
  yylval->string = strdup(yytext);
  return MTLFILEPATH;
}

{null} {
  return NULL_MARKER;
}

{word} {
  yylval->string = strdup(yytext);
  return WORD;
}

. return ERR;

%%

void InitObjScanner(ObjContext *ctx)
{
  yylex_init(&(ctx->scanner));
  yyset_extra(ctx, ctx->scanner);
}

void DestroyObjScanner(ObjContext *ctx)
{
  yylex_destroy(ctx->scanner);
}
