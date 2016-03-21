%code requires {
#include "ObjContext.h"
#include "mesh_io.h"
}

%pure-parser
%name-prefix="Obj_"
%locations
%defines
%error-verbose
%parse-param { ObjContext* context }
%lex-param { void* scanner  }

%union
{
  int intValue;    /* integer */
  double fValue;    /* double value */
  char *string;    /* character string */
};

%code {

#define YYERROR_VERBOSE 1

int Obj_lex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner);
void Obj_error(YYLTYPE* locp, ObjContext* context, const char* err);

#define scanner context->scanner

}

%token ERR
%token EOL
%token <fValue> DECIMAL
%token <intValue> INTEGER
%token <string> WORD
%token MATERIALLIB_MARKER
%token <string> MTLFILEPATH
%token USEMATERIAL_MARKER
%token NULL_MARKER
%token VERTEX_MARKER
%token TEXEL_MARKER
%token NORMAL_MARKER
%token LINE_MARKER
%token FACE_MARKER
%token GROUP_MARKER
%token OBJECT_MARKER
%token SMOOTHINGGROUP_MARKER
%token CAMERA_MARKER
%token OFF_WORD

%type <fValue> coord

/* start of grammar */
%start objfile;

%%

objfile: 
objfile string EOL
| objfile string
| objfile EOL
|
;

string:
vertex
| object
| group
| texel
| normal
| line
| face
| materiallib
| usematerial
| smoothinggroup
| camera
;

camera:
CAMERA_MARKER coord coord coord
;

vertex:
VERTEX_MARKER coord coord coord {
  int res = 0;
  if(context->user_cb->onVertex)
    res = context->user_cb->onVertex($2, $3, $4, 1.0f, 
				     context->user_cb->userData); 
  if(res) return res;
}
| VERTEX_MARKER coord coord coord coord { 
  int res = 0;
  if(context->user_cb->onVertex)
    res = context->user_cb->onVertex($2, $3, $4, $5, 
				     context->user_cb->userData); 
  if(res) return res;
}

coord:
DECIMAL { $$ = $1; };
| INTEGER { $$ = (double)$1; };

object:
OBJECT_MARKER objectname
;

objectname:
WORD {
  int res = 0;
  if(context->user_cb->onStartObject)
    res = context->user_cb->onStartObject($1, 
                                          context->user_cb->userData);
  free($1);
  if(res) return res;
}
| INTEGER { 
  int res = 0;
  char buff[128];
  if(context->user_cb->onStartObject)
    {
      sprintf(buff, "%i", $1);
      res = context->user_cb->onStartObject(buff, context->user_cb->userData);
    }
  if(res) return res;
}

group:
GROUP_MARKER groupnamelist
;

groupnamelist:
groupnamelist groupname
| { 
  int res = 0;
  if(context->user_cb->onStartGroup)
    res = context->user_cb->onStartGroup(context->user_cb->userData); 
  if(res) return res;
}

groupname:
WORD {
  int res = 0;
  if(context->user_cb->onGroupName)
    res = context->user_cb->onGroupName($1, 
                                        context->user_cb->userData);
  free($1);
  if(res) return res;
}
| INTEGER { 
  int res = 0;
  char buff[128];
  if(context->user_cb->onGroupName)
    {
      sprintf(buff, "%i", $1);
      res = context->user_cb->onGroupName(buff, context->user_cb->userData);
    }
  if(res) return res;
}

texel:
TEXEL_MARKER coord coord { 
  int res = 0;
  if(context->user_cb->onTexel)
    res = context->user_cb->onTexel($2, $3, 0.0f, context->user_cb->userData); 
  if(res) return res;
}
|
TEXEL_MARKER coord coord coord { 
  int res = 0;
  if(context->user_cb->onTexel)
    res = context->user_cb->onTexel($2, $3, $4, context->user_cb->userData); 
  if(res) return res;
}

normal:
NORMAL_MARKER coord coord coord { 
  int res = 0;
  if(context->user_cb->onNormal)
    res = context->user_cb->onNormal($2, $3, $4, context->user_cb->userData); 
  if(res) return res;
}

line:
LINE_MARKER linedescrlist
;

linedescrlist:
linedescrlist pair
| { 
  int res = 0;
  if(context->user_cb->onStartLine)
    res = context->user_cb->onStartLine(context->user_cb->userData); 
  if(res) return res;
}

pair: 
INTEGER '/' INTEGER { 
  int res = 0;
  if(context->user_cb->onAddToLine)
    res = context->user_cb->onAddToLine($1, $3, context->user_cb->userData); 
  if(res) return res;
}
| INTEGER { 
  int res = 0;
  if(context->user_cb->onAddToLine)
    res = context->user_cb->onAddToLine($1, 0, context->user_cb->userData); 
  if(res) return res;
}

face:
FACE_MARKER vertexdescrlist
;

vertexdescrlist: 
vertexdescrlist tripple
| { 
  int res = 0;
  if(context->user_cb->onStartFace)
    res = context->user_cb->onStartFace(context->user_cb->userData); 
  if(res) return res;
}

tripple: 
INTEGER '/' INTEGER '/' INTEGER { 
  int res = 0;
  if($1 < 1 || $3 < 1 || $5 < 1) /* index must be >= 1 */
    return -100;
  if(context->user_cb->onAddToFace)
    res = context->user_cb->onAddToFace($1, $3, $5, 
					context->user_cb->userData); 
  if(res) return res;
} 
| INTEGER '/' '/' INTEGER { 
  int res = 0;
  if($1 < 1 || $4 < 1) /* index must be >= 1 */
    return -101;
  if(context->user_cb->onAddToFace)
    res = context->user_cb->onAddToFace($1, 0, $4, context->user_cb->userData); 
  if(res) return res;
}
| INTEGER '/' INTEGER { 
  int res = 0;
  if($1 < 1 || $3 < 1) /* index must be >= 1 */
    return -102;
  if(context->user_cb->onAddToFace)
    res = context->user_cb->onAddToFace($1, $3, 0, context->user_cb->userData); 
  if(res) return res;
}
| INTEGER { 
  int res = 0;
  if($1 < 1) /* index must be >= 1 */
    return -103;
  if(context->user_cb->onAddToFace)
    res = context->user_cb->onAddToFace($1, 0, 0, context->user_cb->userData); 
  if(res) return res;
}

materiallib:
MATERIALLIB_MARKER MTLFILEPATH { 
  int res = 0;
  if(context->user_cb->onRefMaterialLib)
    res = context->user_cb->onRefMaterialLib($2, 
                                             context->user_cb->userData); 
  free($2);
  if(res) return res;
}

usematerial:
USEMATERIAL_MARKER WORD { 
  int res = 0;
  if(context->user_cb->onUseMaterial)
    res = context->user_cb->onUseMaterial($2, 
                                          context->user_cb->userData); 
  free($2);
  if(res) return res;
}
| USEMATERIAL_MARKER NULL_MARKER { 
  int res = 0;
  if(context->user_cb->onUseMaterial)
    res = context->user_cb->onUseMaterial("", 
                                          context->user_cb->userData); 
  if(res) return res;
}


smoothinggroup:
SMOOTHINGGROUP_MARKER groupid
;

groupid: 
OFF_WORD {
  int res = 0;
  if(context->user_cb->onSmoothingGroup)
    res = context->user_cb->onSmoothingGroup(0, context->user_cb->userData); 
  if(res) return res;
}
| INTEGER {
  int res = 0;
  if(context->user_cb->onSmoothingGroup)
    res = context->user_cb->onSmoothingGroup($1, context->user_cb->userData); 
  if(res) return res;
}

%%

void 
Obj_error(YYLTYPE* locp, ObjContext* context, const char* err)
{
  fprintf(stderr, "Line %i: %s\n", locp->first_line, err);
}

int 
ReadObjFile(FILE *stream, ObjParseCallbacks *ucb)
{
  ObjContext ctx;
  int res;
  InitObjScanner(&ctx);
  ctx.user_cb = ucb;
  ctx.is = stream;
  res = Obj_parse(&ctx);
  DestroyObjScanner(&ctx);
  return res;
}
