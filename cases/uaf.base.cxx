#include "sys_config.h"
#include "stdlib.h"
#include <string.h>

#define BUFF_SZ      100

// this struct stays globally visible for the entire file
// if a local struct is desired, define one inside the function appropriately
typedef struct COBJ {
PUBLIC
  char buf[BUFF_SZ];
  int l;
} cObj;


//
// Synopsis: Vul type: use after free (UAF)
//           Pattern:  single proc, basic
//           Scope:    local
//           Flow:     no
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Expect:   must
//

void uaf_basic(int random_val)
{
	cObj *p = MEM_NEW (cObj);
	int i;
	if (p == 0)
		return;

	for (i=0; i<100; i++) {
		// the loop ensures that this case is non trivial
		if (random_val != 10)
			random_val++;
	}
	MEM_DELETE(p);

	for (i=0; i<100; i++) {
		// the loop ensures that this case is non trivial
		if (random_val > 10)
			random_val--;
	}
	p->l = 0;  // UAF
	return;
}


//
// Synopsis: Vul type: null pointer dereferenceing
//           Pattern:  single proc, copy prop 
//           Scope:    local
//           Flow:     no
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Expect:   must
//

void uaf_cprop(int random_val)
{
	int i;
	cObj *p = MEM_NEW (cObj);
	if (p == 0)
		return;

	cObj *q = p;
	for (i=0; i<100; i++) {
		// the loop ensures that this case is non trivial
		if (random_val != 10)
			random_val++;
	}
	MEM_DELETE(p);

	for (i=0; i<100; i++) {
		// the loop ensures that this case is non trivial
		if (random_val > 10)
			random_val--;
	}

	q->l = 0;  // UAF
	return;
}


//
// Synopsis: Vul type: null pointer dereferenceing
//           Pattern:  single proc, copy prop (multi levels) 
//           Scope:    local
//           Flow:     no
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Expect:   must
//

void uaf_cprop2(int random_val, cObj *q)
{
  int i;
  cObj *p = MEM_NEW (cObj);
    if (p == 0)
        return;
    q = p+1;
    for (i=0; i<100; i++) {
        // the loop ensures that this case is non trivial
        if (random_val != 10) 
            random_val++;
    }
    MEM_DELETE(p);

    for (i=0; i<100; i++) {
        // the loop ensures that this case is non trivial
        if (random_val > 10) 
            random_val--;
    }
    cObj *r = q-1;
    r->l = 0;  // UAF
    return;
}


//
// Synopsis: Vul type: use after free (UAF)
//           Pattern:  single proc, simple flow sensitive 
//           Scope:    file static 
//           Flow:     yes
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Expect:   must, specific path
//

int uaf_sfs(int random_val)
{
    cObj *p = MEM_NEW (cObj);
    int i;
    if (p == 0)
        return 0;
  
    for (i=0; i<100; i++) {
        // the loop ensures that this case is non trivial
        if (random_val != 10) {
            random_val++;
            MEM_DELETE(p);
        }
    }
    return p->l;
    
}


//
// Synopsis: Vul type: use after free (UAF)
//           Pattern:  single proc, nested flow sensitive 
//           Scope:    file static 
//           Flow:     nested
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Expect:   must, specific path
//

int uaf_nfs(int random_val)
{
    cObj *p = MEM_NEW (cObj);
    int i;
    if (p == 0)
        return 0;
  
    for (i=0; i<100; i++) {
        // the loop ensures that this case is non trivial
        if (random_val != 10) {
            random_val++;
            if (random_val == 20)
	        MEM_DELETE(p);
        }
    }
    return p->l;
    
}


//
// Synopsis: Vul type: use after free (UAF)
//           Pattern:  single proc, nest flow sensitive 
//           Scope:    file static 
//           Flow:     nested
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Semantic: system API used
//           Expect:   must, specific path
//

int uaf_nfs_api(int random_val)
{
    cObj *p = MEM_NEW (cObj);
    int i;
    if (p == 0)
        return 0;
  
    for (i=0; i<100; i++) {
        // the loop ensures that this case is non trivial
        if (random_val != 10) {
            random_val++;
            if (random_val == 20)
	        MEM_DELETE(p);
        }
    }
    MEMSET(p, 0, sizeof(cObj));
    return 1;
     
}



//
// Synopsis: Vul type: use after free (UAF)
//           Pattern:  single proc, nest flow sensitive, copy prop 
//           Scope:    file static 
//           Flow:     nested
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Semantic: system API used
//           Expect:   must, specific path
//

int uaf_nfs_cprop_api(int random_val, cObj *q)
{
    int i;
    cObj *p = MEM_NEW (cObj);
    if (p == 0)
        return 0;

    q = p;
    for (i=0; i<100; i++) {
        // the loop ensures that this case is non trivial
        if (random_val != 10) {
            random_val++;
            if (random_val == 20)
	        MEM_DELETE(p);
        }
    }
    MEMSET(q, 0, sizeof(cObj));
    return 1;
     
}


//
// Synopsis: Vul type: use after free (UAF)
//           Pattern:  single proc, nest flow sensitive 
//           Scope:    file static 
//           Flow:     nested
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Semantic: system API used
//           Expect:   must, specific path
//

int uaf_sfs_api(int random_val)
{
  int i;
  cObj *p = MEM_NEW (cObj);
  cObj *q = MEM_NEW(cObj);
  if (p == 0)
    return 0;
  if (q == 0)
    return 0;
  
  for (i=0; i<100; i++) {
    // the loop ensures that this case is non trivial
    if (random_val != 10) {
      random_val++;
      if (random_val == 20)
	q = p;
    }
    MEM_DELETE(p);
    MEMSET(q, 0, sizeof(cObj));
    return 1;
  }     
}


//
// Synopsis: Vul type: use after free (UAF)
//           Pattern:  single proc, nest flow sensitive 
//           Scope:    file static 
//           Flow:     nested
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Semantic: system API used
//           Expect:   must, specific path
//

int uaf_sfs2_api(int random_val)
{
  int i;
  cObj *p = MEM_NEW (cObj);
  cObj *q = MEM_NEW(cObj);
  if (p == 0)
    return 0;
  if (q == 0)
    return 0;

  q = p;
  for (i=0; i<100; i++) {
    // the loop ensures that this case is non trivial
    if (random_val != 10) {
      random_val++;
      MEM_DELETE(p);
      if (random_val == 20)
	MEMSET(q, 0, sizeof(cObj));
    }

    return 1;
  }
     
}


//
// Synopsis: Vul type: use after free (UAF)
//           Pattern:  single proc, nest flow sensitive 
//           Scope:    file static 
//           Flow:     nested
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Semantic: system API used
//           Expect:   must, specific path
//

int uaf_sfs3_api(int random_val, int m)
{
  int i;
  cObj *p = MEM_NEW (cObj);
  cObj *q;

  if (m > 5)
    q = p;
  if (q == 0)
    return 0;

  for (i=0; i<100; i++) {
    // the loop ensures that this case is non trivial
    if (random_val != 10) {
      random_val++;
    }
  }
  if (m > 10)
    MEM_DELETE(p);
  if (m == 6)  // value range analysis needed here to know must
    MEMSET(q, 0, sizeof(cObj));

  return 1;     
}


//
// Synopsis: Vul type: use after free
//           Pattern:  single proc, field 
//           Scope:    local
//           Flow:     no
//           Field:    yes, one level
//           Context:  no
//           Lang:     C/C++
//           Expect:   must
//

typedef struct Base {
  char a[20];
} base;

typedef struct One_Level {
PUBLIC
  int i;
  int m1:2;
  int m2:4;
  base* p;
  int j;
} one_level;

void drf_rba_flds(int random_val, int i)
{
  one_level lcl_class;
  lcl_class.p =  (base *)MEM_NEW(one_level);

  if (lcl_class.p == 0)
    return;
  base *q = lcl_class.p;
  
  for (i=0; i<100; ++i) {
    // the loop ensures that this case is non-trivial
    if (random_val != 10) {
      random_val++;
      lcl_class.i = i;
    }
  }
  MEM_DELETE(q);
  MEMSET(lcl_class.p, 0, sizeof(base));
  return;
}


//
// Synopsis: Vul type: use after free
//           Pattern:  single proc, field 
//           Scope:    local
//           Flow:     no
//           Field:    yes, one level
//           Context:  no
//           Lang:     C/C++
//           Expect:   must
//

typedef struct Base2 {
  char a[20];
} base2;

typedef struct One_Level2 {
PUBLIC
  int i;
  int m1:2;
  int m2:4;
  base2* p;
  int j;
} one_level2;

void drf_rba_flds2(int random_val, int i)
{
  one_level2 lcl_class;
  lcl_class.p =  (base2 *)MEM_NEW(one_level2);

  if (lcl_class.p == 0)
    return;
  base2 *q = lcl_class.p;
  
  for (i=0; i<100; ++i) {
    // the loop ensures that this case is non-trivial
    if (random_val != 10) {
      random_val++;
      lcl_class.i = i;
    }
  }
  MEM_DELETE(lcl_class.p);
  MEMSET(q, 0, sizeof(base2));
  return;
}


//
// Synopsis: Vul type: use after free
//           Pattern:  single proc, field
//           Scope:    local
//           Flow:     no
//           Field:    union, two levels
//           Context:  no
//           Lang:     C/C++
//           Expect:   must
//

typedef struct Base3 {
  char a[20];
} base3;

typedef struct  Two_Level3 {
PUBLIC
  int i;
  int m1:2;
  int m2:4;
  base3 *q;
  int j;
} two_level3;

typedef struct One_Level3 {
PUBLIC
    int i;
    int m1:2;
    int m2:4;
    base3 *p;
    int j;
} one_level3;
  
#if 0
typedef union U_lvl1 {
PUBLIC
  one_level3 u_separate;
  int u_ary[4];
} u_lvl1;

static int glb_uflds;
#endif

int drf_rba_mfld(int random_val)
{
    two_level3 lcl_2lvl;
    one_level3 lcl_1lvl;
    int i;
    lcl_2lvl.q = MEM_NEW(base3);
    if (lcl_2lvl.q == 0)
      return 0;
    lcl_1lvl.p = lcl_2lvl.q;
    
    for (i=0; i<4; ++i) {
        if (i == 2) {
            random_val++;
        }
    }
    MEM_DELETE(lcl_1lvl.p);
    MEMSET(lcl_2lvl.q, 0, sizeof(base3));
    return 1;
}


//
// Synopsis: Vul type: use after free
//           Pattern:  single proc, field
//           Scope:    local
//           Flow:     no
//           Field:    union, three levels
//           Context:  no
//           Lang:     C/C++
//           Expect:   must
//

typedef struct Base4 {
  char a[20];
} base4;

typedef struct One_Level4 {
PUBLIC
    int i;
    int m1:2;
    int m2:4;
    base4 *pbase;
    int j;
} one_level4;

typedef struct  Two_Level4 {
PUBLIC
  int i;
  int m1:2;
  int m2:4;
  one_level4 *p1lvl;
  int j;
} two_level4;

int drf_rba_mfld2(int random_val)
{
    two_level4 lcl_2lvl;
    one_level4 lcl_1lvl;
    int i;
    lcl_1lvl.pbase = MEM_NEW(base4);
    if (lcl_1lvl.pbase == 0)
      return 0;
    lcl_2lvl.p1lvl = &lcl_1lvl;
    
    for (i=0; i<4; ++i) {
        if (i == 2) {
            random_val++;
        }
    }
    MEM_DELETE(lcl_1lvl.pbase);
    MEMSET(lcl_2lvl.p1lvl->pbase, 0, sizeof(base4));
    return 1;
}


//
// Synopsis: Vul type: use after free
//           Pattern:  single proc, flow free
//           Scope:    file global 
//           Flow:     no
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Expect:   must
//

static int glb_fgb;
static int *p_fgb;

int uaf_sff_fglb(int random_val)
{
  int i;
  int *p_fgb = MEM_NEW(int);
  if (p_fgb == 0)
    return 0;
  int *p = MEM_NEW(int);
  int *q = p_fgb;
  for (i=0; i<100; i++) {
    if (random_val != 10) {
      random_val++;    
    }
  }
  MEM_DELETE(q);
  MEMCPY(p, p_fgb, sizeof(int));
  return 1;
}


//
// Synopsis: Vul type: use after free
//           Pattern:  single proc, flow sensitive
//           Scope:    file global 
//           Flow:     yes, nested
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Expect:   must, certain
//

static int glb_fgb1;
static int *p_fgb1;

int uaf_nfs_fglb(int random_val)
{
  int i;
  int *p_fgb1 = MEM_NEW(int);
  if (p_fgb1 == 0)
    return 0;
  int *p = MEM_NEW(int);
  int *q = p_fgb1;
  for (i=0; i<100; i++) {
    if (random_val != 10) {
      random_val++;    
      MEM_DELETE(q);
    }
  }
  MEMCPY(p, p_fgb1, sizeof(int));
  return 1;
}


//
// Synopsis: Vul type: use after free
//           Pattern:  single proc, flow sensitive
//           Scope:    file global 
//           Flow:     yes, nested
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Expect:   must, certain
//

static int glb_fgb2;
static int *p_fgb2;

int uaf_nfs_fglb2(int random_val)
{
  int i;
  int *p_fgb2 = MEM_NEW(int);
  if (p_fgb2 == 0)
    return 0;
  int *p = MEM_NEW(int);
  int *q = p_fgb2;
  for (i=0; i<100; i++) {
    if (random_val != 10) {
      random_val++;    
    }
  }
  MEM_DELETE(p_fgb2);
  if (p != 0)
    MEMCPY(p, q, sizeof(int));
  return 1;
}


#if 0

//
// Synopsis: Vul type: null pointer dereferenceing
//           Pattern:  single proc, flow sensitive
//           Scope:    global
//           Flow:     nested 
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Expect:   may, specific path
//           Remark:   global scope causes may
//

static int glb_fgb2;
extern int *p_fglb;

int drf_rba_glb(int random_val, int j)
{
  int i;
    for (i=0; i<100; i++) {
        if (random_val != 10) {
            random_val++;
            if ((j & 0x1) != 1)
                p_fglb = &glb_fgb2;
        }
    }
    return *p_fglb;
}


//
// Synopsis: Vul type: null pointer dereferenceing
//           Pattern:  single proc, flow sensitive
//           Scope:    formal
//           Flow:     yes
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Expect:   may, function entry
//

int drf_rba_fml(int random_val, int *p)
{
    int i;
    for (i=0; i<100; i++) {
        if (random_val != 10) {
            random_val++;
            if ((i & 0x1) != 1)
                *p = i; 
        }
    }
    return i;
}


//
// Synopsis: Vul type: null pointer dereferenceing
//           Pattern:  single proc, flow sensitive
//           Scope:    formal - addr saved
//           Flow:     yes
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Expect:   may, function entry
//

int drf_rba_fmt_ads(int random_val, int *p)
{
    int rand_ary[100] = {1};
    int *q = rand_ary;
    int i;
    for (i=0; i<100; i++) {
        if ((i & 0x1) != 1) {
            p = q;
        }
    }
    return *q;
}


//
// Synopsis: Vul type: null pointer dereferenceing
//           Pattern:  single proc, flow sensitive
//           Scope:    formal - addr saved
//           Flow:     yes
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Expect:   may, function entry, specific path
//

static int glb_j;

int drf_rba_fmt_ads2(int random_val, int *p)
{
    int *q;
    if (random_val != 10)
        q = &glb_j;
    else {
        if ((random_val & 0x1) == 1) {
            q = p;
        }
    }
    return *q;
}


//
// Synopsis: Vul type: null pointer dereferenceing
//           Pattern:  single proc, flow sensitive 
//           Scope:    formal - addr saved
//           Flow:     yes, nested
//           Field:    yes, one level
//           Context:  no
//           Lang:     C/C++
//           Expect:   may, function entry, specific path
//

typedef struct One_level_z {
PUBLIC
  int i;
  int m1:2;
  int m2:4;
  int *p;
  int j;
} one_level_z;

static int glb_z;

int drf_rba_nflds(int random_val, int *p)
{
    one_level_z *q;

    if (random_val != 10) {
        q->p = &glb_z;
    }
    else {
        if ((random_val & 0x1) == 1);
            q->p = p;
    }
    return *(q->p);
}


//
// Synopsis: Vul type: null pointer dereferenceing
//           Pattern:  single proc, flow sensitive
//           Scope:    formal - addr saved
//           Flow:     yes, nested
//           Field:    yes, two levels
//           Context:  no
//           Lang:     C/C++
//           Expect:   may, function entry, specific path
//

typedef struct One_level_y {
PUBLIC
  int i;
  int m1:2;
  int m2:4;
  int *p;
  int j;
} one_level_y;

typedef struct Two_level_y {
PUBLIC
  int a;
  one_level_y b;
  int c;
} two_level_y;

static one_level_y glb_nf;

int drf_rba_n2flds(int random_val, int *p)
{
    two_level_y lcl_class;
    one_level_y *q;

    q = &lcl_class.b;
    if (random_val != 10) {
        q->p = glb_nf.p;
    }
    else {
        if ((random_val & 0x1) == 1);
            q->p = p;
    }
    return *(q->p);
}

#endif
