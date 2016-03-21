%option reentrant
%option bison-locations
%option bison-bridge
%option prefix="Mtl_"
%option noyywrap
%option yylineno

word [a-zA-Z]+[a-zA-Z0-9\_\.\-]*
filepath [a-zA-Z\.\\/][:\\/\.a-z\_A-Z0-9]*\.(jpg|JPG|bmp|BMP|png|PNG)

%{
#include "mtlparser.y.h"
#include "MtlContext.h"
#include <stdlib.h>
#include <string.h>

#define YY_EXTRA_TYPE MtlContext*
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

#.*  ; /* skip comments */

[ \t]+ ; /* skip whitespace */
newmtl  { return NEWMTL_MARKER; }
Ka      { return KA_MARKER; }
Kd      { return KD_MARKER; }
Ks      { return KS_MARKER; }
d       { return D_MARKER; }
Tr      { return TR_MARKER; }
Tf      { return TF_MARKER; }
Ns      { return NS_MARKER; }
Ni      { return NI_MARKER; }
illum   { return ILLUM_MARKER; }
map_Ka  { return MAPKA_MARKER; }
map_Kd  { return MAPKD_MARKER; }
map_Ks  { return MAPKS_MARKER; }
map_d   { return MAPD_MARKER; }
[(map bump)(bump)] { return MAPBUMP_MARKER; }

[\-\+]?[0-9]*\.[0-9]+([eE][\-\+]?[0-9]+)? {
  yylval->fValue = (double)atof(yytext);
  return DECIMAL;
}

[\-\+]?[0-9][0-9]* {
  yylval->intValue = atoi(yytext);
  return INTEGER;
}

[/\n] { return *yytext; }

{filepath} {
  yylval->string = strdup(yytext);
  return FILEPATH;
}

{word} {
  yylval->string = strdup(yytext);
  return WORD;
}

. return ERR;

%%

void InitMtlScanner(MtlContext *ctx)
{
  yylex_init(&(ctx->scanner));
  yyset_extra(ctx, ctx->scanner);
}

void DestroyMtlScanner(MtlContext *ctx)
{
  yylex_destroy(ctx->scanner);
}
