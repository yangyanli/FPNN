%code requires {
#include "MtlContext.h"
#include "mesh_io.h"
}

%pure-parser
%name-prefix="Mtl_"
%locations
%defines
%error-verbose
%parse-param { MtlContext* context }
%lex-param { void* scanner  }

%union
{
  int intValue;    /* integer */
  double fValue;    /* double value */
  char *string;    /* character string */
};

%code {

#define YYERROR_VERBOSE 1

int Mtl_lex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner);
void Mtl_error(YYLTYPE* locp, MtlContext* context, const char* err);

#define scanner context->scanner

}

%token ERR
%token EOL
%token <fValue> DECIMAL
%token <intValue> INTEGER
%token <string> WORD
%token <string> FILEPATH
%token NEWMTL_MARKER
%token KA_MARKER
%token KD_MARKER
%token KS_MARKER
%token D_MARKER
%token TR_MARKER
%token TF_MARKER
%token NS_MARKER
%token NI_MARKER
%token ILLUM_MARKER
%token MAPKA_MARKER
%token MAPKD_MARKER
%token MAPKS_MARKER
%token MAPD_MARKER
%token MAPBUMP_MARKER

%type <fValue> fparam

/* start of grammar */
%start mtlfile;

%%

mtlfile: 
| mtlfile material EOL
| mtlfile material
| mtlfile EOL
|
;

material: 
NEWMTL_MARKER WORD EOL materialdef {
  int res = 0;
  if(context->user_cb->onAddMaterial)
    res = context->user_cb->onAddMaterial($2, 
                                          context->user_cb->userData);
  free($2);
  if(res) return res;
} /* sometimes tools, like for example Blender are generating 
material names which looks like file path (for textures based on the file).
These names does not match the word, but the file paths. So to prevent
the error we also "allows" such material names.*/
| NEWMTL_MARKER FILEPATH EOL materialdef {
  int res = 0;
  if(context->user_cb->onAddMaterial)
    res = context->user_cb->onAddMaterial($2, 
                                          context->user_cb->userData);
  free($2);
  if(res) return res;
}
;

materialdef: parameters
;

parameters:
| parameters parameter EOL
;

parameter:
ambient
| diffuse
| specular
| alpha
| transmission
| ns
| ni
| illum
| mapka
| mapkd
| mapks
| mapd
| mapbump
;

ambient:
KA_MARKER fparam fparam fparam {
  int res = 0;
  if(context->user_cb->onSetAmbientColor)
    res = context->user_cb->onSetAmbientColor($2, $3, $4, 
                                              context->user_cb->userData); 
  if(res) return res;
}

diffuse:
KD_MARKER fparam fparam fparam {
  int res = 0;
  if(context->user_cb->onSetDiffuseColor)
    res = context->user_cb->onSetDiffuseColor($2, $3, $4, 
                                              context->user_cb->userData); 
  if(res) return res;
}

specular:
KS_MARKER fparam fparam fparam {
  int res = 0;
  if(context->user_cb->onSetSpecularColor)
    res = context->user_cb->onSetSpecularColor($2, $3, $4, 
                                               context->user_cb->userData);
  if(res) return res;
}

alpha:
D_MARKER fparam {
  int res = 0;
  if(context->user_cb->onSetAlphaD)
    res = context->user_cb->onSetAlphaD($2,
                                       context->user_cb->userData);
  if(res) return res;
}
|
TR_MARKER fparam {
  int res = 0;
  if(context->user_cb->onSetAlphaTr)
    res = context->user_cb->onSetAlphaTr($2,
                                       context->user_cb->userData);
  if(res) return res;
}

transmission:
TF_MARKER fparam fparam fparam {
  int res = 0;
  if(context->user_cb->onSetTransmission)
    res = context->user_cb->onSetTransmission($2, $3, $4,
                                       context->user_cb->userData);
  if(res) return res;
}

ns:
NS_MARKER fparam {
  int res = 0;
  if(context->user_cb->onSetSpecularExponent)
    res = context->user_cb->onSetSpecularExponent($2,
                                                context->user_cb->userData);
  if(res) return res;
}

ni:
NI_MARKER fparam {
  int res = 0;
  if(context->user_cb->onSetOpticalDensity)
    res = context->user_cb->onSetOpticalDensity($2,
                                                context->user_cb->userData);
  if(res) return res;
}

illum:
ILLUM_MARKER INTEGER {
  int res = 0;
  if(context->user_cb->onSetIllumModel)
    res = context->user_cb->onSetIllumModel($2,
                                            context->user_cb->userData);
  if(res) return res;
}

mapka:
MAPKA_MARKER FILEPATH {
  int res = 0;
  if(context->user_cb->onSetAmbientTextureMap)
    res = context->user_cb->onSetAmbientTextureMap($2,
						   context->user_cb->userData);
  free($2);
  if(res) return res;
}

mapkd:
MAPKD_MARKER FILEPATH {
  int res = 0;
  if(context->user_cb->onSetDiffuseTextureMap)
    res = context->user_cb->onSetDiffuseTextureMap($2,
						   context->user_cb->userData);
  free($2);
  if(res) return res;
}

mapks:
MAPKS_MARKER FILEPATH {
}

mapd:
MAPD_MARKER FILEPATH {
}

mapbump:
MAPBUMP_MARKER FILEPATH {
}

fparam:
DECIMAL { $$ = $1; }
| INTEGER { $$ = (double)$1; }

%%

#ifdef  __cplusplus
extern "C" {
#endif

void 
Mtl_error(YYLTYPE* locp, MtlContext* context, const char* err)
{
  fprintf(stderr, "Line %i: %s\n", locp->first_line, err);
}

int 
ReadMtlFile(FILE *stream, MtlParseCallbacks *mcb)
{
  MtlContext ctx;
  int res;
  InitMtlScanner(&ctx);
  ctx.user_cb = mcb;
  ctx.is = stream;
  res = Mtl_parse(&ctx);
  DestroyMtlScanner(&ctx);
  return res;
}

#ifdef  __cplusplus
}
#endif
