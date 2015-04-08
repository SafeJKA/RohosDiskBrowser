/* * * * * * * * * * * * * * * * * * * 
* (C)   j3d1 aka Егор                 *
* ICQ:  8370005						  *
* mail: aux.in.out@gmail.com		  *
 * * * * * * * * * * * * * * * * * * */


#define AF_TYPE_STR			0
#define AF_TYPE_ANY			1
#define AF_TYPE_REP			2
#define AF_TYPE_ONETIME		3

#define PARSER_IGNORE_SPACE	0

typedef struct RET_STRING
{
	char	*str;
	DWORD	x;
	DWORD	dx;
}RETSTR;


typedef struct AF_
{
	char	*str;
	DWORD	dx;
	char	cmd;
	DWORD	rep;

}AF;

void free_af(AF *af,DWORD afc);
void free_crs(RETSTR *crs,DWORD afc);
AF *make_af(char *rgl_equ,DWORD rsz,DWORD *af_cnt);
RETSTR *parse_string(char *str,DWORD sz,AF *af,DWORD afc,char flag);