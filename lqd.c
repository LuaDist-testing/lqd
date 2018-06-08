/*
* lqd.c
* high-precision mathematical library for Lua 5.1 based on qd
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 30 Apr 2012 14:08:01
* This code is hereby placed in the public domain.
*/

#include "qd/c_qd.h"
#ifdef USE_DD
#define C(n)	c_dd_##n
#define N	2
#define SN	"(2)"
#else
#define C(n)	c_qd_##n
#define N	4
#define SN	"(4)"
#endif

#include "lua.h"
#include "lauxlib.h"

#define MYNAME		"qd"
#define MYVERSION	MYNAME SN " library for " LUA_VERSION " / Apr 2012"
#define MYTYPE		MYNAME " number"

#define MAXDIGITS	(16*N)
static int DIGITS=MAXDIGITS;

static double *Pnew(lua_State *L)
{
 double *x=lua_newuserdata(L,N*sizeof(double));
 luaL_getmetatable(L,MYTYPE);
 lua_setmetatable(L,-2);
 return x;
}

static double *Pget(lua_State *L, int i, double *x)
{
 switch (lua_type(L,i))
 {
  case LUA_TNUMBER:
   C(copy_d)(lua_tonumber(L,i),x);
   return x;
  case LUA_TSTRING:
   C(read)(lua_tostring(L,i),x);
   return x;
  default:
   return luaL_checkudata(L,i,MYTYPE);
 }
 return NULL;
}

static int Ltype(lua_State *L)			/** type(x) */
{
 luaL_checkudata(L,1,MYTYPE);
 lua_pushliteral(L,MYTYPE);
 return 1;
}

static int Ldigits(lua_State *L)		/** digits([n]) */
{
 lua_pushinteger(L,DIGITS);
 DIGITS=luaL_optint(L,1,DIGITS);
 if (DIGITS>MAXDIGITS) DIGITS=MAXDIGITS;
 return 1;
}

static int Lnumber(lua_State *L)		/** number(x) */
{
 double *x=NULL;
 lua_settop(L,1);
 switch (lua_type(L,1))
 {
  case LUA_TNUMBER:
  case LUA_TSTRING:
   x=Pnew(L);
 }
 Pget(L,1,x);
 return 1;
}

static int Pdo1(lua_State *L, void (*f)(const double *a, double *b))
{
 double x[N];
 double *a=Pget(L,1,x);
 double *b=Pnew(L);
 f(a,b);
 return 1;
}

static int Pdo2(lua_State *L, void (*f)(const double *a, const double *b, double *c))
{
 double x[N],y[N];
 double *a=Pget(L,1,x);
 double *b=Pget(L,2,y);
 double *c=Pnew(L);
 f(a,b,c);
 return 1;
}

static int Pdo3(lua_State *L, void (*f)(const double *a, double *b, double *c))
{
 double x[N];
 double *a=Pget(L,1,x);
 double *b=Pnew(L);
 double *c=Pnew(L);
 f(a,b,c);
 return 2;
}

static int Ltonumber(lua_State *L)		/** tonumber(x) */
{
 double x[N];
 double *a=Pget(L,1,x);
 lua_pushnumber(L,a[0]);
 return 1;
}

static int Ltostring(lua_State *L)		/** tostring(x,[n]) */
{
 char s[MAXDIGITS+20];
 double x[N];
 double *a=Pget(L,1,x);
 int n=luaL_optint(L,2,DIGITS);
 if (n>MAXDIGITS) n=MAXDIGITS;
 C(swrite)(a,n,s,sizeof(s));
 lua_pushstring(L,s);
 return 1;
}


static int Lrand(lua_State *L)			/** rand() */
{
 C(rand)(Pnew(L));
 return 1;
}

static int Pcompare(lua_State *L)
{
 int result;
 double x[N],y[N];
 double *a=Pget(L,1,x);
 double *b=Pget(L,2,y);
 C(comp)(a,b,&result);
 return result;
}

static int Lcompare(lua_State *L)		/** compare(x,y) */
{
 lua_pushinteger(L,Pcompare(L));
 return 1;
}

static int Leq(lua_State *L)
{
 lua_pushboolean(L,Pcompare(L)==0);
 return 1;
}

static int Llt(lua_State *L)
{
 lua_pushboolean(L,Pcompare(L)<0);
 return 1;
}

static int Lipow(lua_State *L)			/** ipow(x,n) */
{
 int n=luaL_checkint(L,2);
 double x[N];
 double *a=Pget(L,1,x);
 double *b=Pnew(L);
 C(npwr)(a,n,b);
 return 1;
}

static int Liroot(lua_State *L)			/** iroot(x,n) */
{
 int n=luaL_checkint(L,2);
 double x[N];
 double *a=Pget(L,1,x);
 double *b=Pnew(L);
 C(nroot)(a,n,b);
 return 1;
}

static int Lpow(lua_State *L)			/** pow(x,y) */
{
 double x[N],y[N];
 double *a=Pget(L,1,x);
 double *b=Pget(L,2,y);
 double *c=Pnew(L);
 C(log)(a,c);
 C(mul)(b,c,c);
 C(exp)(c,c);
 return 1;
}

static int Lmod(lua_State *L)			/** mod(x,y) */
{
 double x[N],y[N];
 double *a=Pget(L,1,x);
 double *b=Pget(L,2,y);
 double *c=Pnew(L);
 C(div)(a,b,c);
 C(floor)(c,c);
 C(mul)(b,c,c);
 C(sub)(a,c,c);
 return 1;
}

#define DO(d,f)	static int L##f(lua_State *L) { return d(L,C(f)); }
#define DO1(f)	DO(Pdo1,f)
#define DO2(f)	DO(Pdo2,f)
#define DO3(f)	DO(Pdo3,f)

DO1(abs)					/** abs(x) */
DO1(acos)					/** acos(x) */
DO1(acosh)					/** acosh(x) */
DO1(asin)					/** asin(x) */
DO1(asinh)					/** asinh(x) */
DO1(atan)					/** atan(x) */
DO1(atanh)					/** atanh(x) */
DO1(ceil)					/** ceil(x) */
DO1(cos)					/** cos(x) */
DO1(cosh)					/** cosh(x) */
DO1(exp)					/** exp(x) */
DO1(floor)					/** floor(x) */
DO1(log)					/** log(x) */
DO1(log10)					/** log10(x) */
DO1(neg)					/** neg(x) */
DO1(nint)					/** round(x) */
DO1(aint)					/** trunc(x) */
DO1(sin)					/** sin(x) */
DO1(sinh)					/** sinh(x) */
DO1(sqr)					/** sqr(x) */
DO1(sqrt)					/** sqrt(x) */
DO1(tan)					/** tan(x) */
DO1(tanh)					/** tanh(x) */

DO2(add)					/** add(x,y) */
DO2(div)					/** div(x,y) */
DO2(mul)					/** mul(x,y) */
DO2(sub)					/** sub(x,y) */
DO2(atan2)					/** atan2(y,x) */

DO3(sincos)					/** sincos(x) */
DO3(sincosh)					/** sincosh(x) */

static const luaL_Reg R[] =
{
	{ "__add",	Ladd	},		/** __add(x,y) */
	{ "__div",	Ldiv	},		/** __div(x,y) */
	{ "__eq",	Leq	},		/** __eq(x,y) */
	{ "__lt",	Llt	},		/** __lt(x,y) */
	{ "__mod",	Lmod	},		/** __mod(x,y) */
	{ "__mul",	Lmul	},		/** __mul(x,y) */
	{ "__pow",	Lpow	},		/** __pow(x,y) */
	{ "__sub",	Lsub	},		/** __sub(x,y) */
	{ "__tostring",	Ltostring},		/** __tostring(x) */
	{ "__unm",	Lneg	},		/** __unm(x) */
	{ "round",	Lnint	},
	{ "trunc",	Laint	},
#define DECLARE(f)	{ #f, L##f },
DECLARE(abs)
DECLARE(acos)
DECLARE(acosh)
DECLARE(add)
DECLARE(asin)
DECLARE(asinh)
DECLARE(atan)
DECLARE(atan2)
DECLARE(atanh)
DECLARE(ceil)
DECLARE(compare)
DECLARE(cos)
DECLARE(cosh)
DECLARE(digits)
DECLARE(div)
DECLARE(exp)
DECLARE(floor)
DECLARE(ipow)
DECLARE(iroot)
DECLARE(log)
DECLARE(log10)
DECLARE(mod)
DECLARE(mul)
DECLARE(neg)
DECLARE(number)
DECLARE(pow)
DECLARE(rand)
DECLARE(sin)
DECLARE(sincos)
DECLARE(sincosh)
DECLARE(sinh)
DECLARE(sqr)
DECLARE(sqrt)
DECLARE(sub)
DECLARE(tan)
DECLARE(tanh)
DECLARE(tonumber)
DECLARE(tostring)
DECLARE(type)
	{ NULL,		NULL	}
};

LUALIB_API int luaopen_qd(lua_State *L)
{
 unsigned int old_cw;
 fpu_fix_start(&old_cw);			/* may affect rest of app! */
 luaL_newmetatable(L,MYTYPE);
 lua_setglobal(L,MYNAME);
 luaL_register(L,MYNAME,R);
 lua_pushliteral(L,"version");			/** version */
 lua_pushliteral(L,MYVERSION);
 lua_settable(L,-3);
 lua_pushliteral(L,"__index");
 lua_pushvalue(L,-2);
 lua_settable(L,-3);
 lua_pushliteral(L,"pi");			/** pi */
 C(pi)(Pnew(L));
 lua_settable(L,-3);
 return 1;
}
