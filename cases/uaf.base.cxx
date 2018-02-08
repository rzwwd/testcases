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


#if 1


//
// Synopsis: Vul type: null pointer dereferenceing
//           Pattern:  single proc, field 
//           Scope:    local
//           Flow:     no
//           Field:    yes, one level
//           Context:  no
//           Lang:     C/C++
//           Expect:   must
//

typedef struct One_Level {
PUBLIC
  int i;
  int m1:2;
  int m2:4;
  int *p;
  int j;
} one_level;

int drf_rba_flds(int random_val, int i)
{
    one_level lcl_class;

    lcl_class.p = 0;
    for (i=0; i<100; ++i) {
        // the loop ensures that this case is non-trivial
        if (random_val != 10) {
            random_val++;
            lcl_class.i = i;
        }
    }
    return *(lcl_class.p);
}


//
// Synopsis: Vul type: null pointer dereferenceing
//           Pattern:  single proc, field
//           Scope:    local
//           Flow:     no
//           Field:    union, one level
//           Context:  no
//           Lang:     C/C++
//           Expect:   must
//

typedef struct  One_Level3 {
PUBLIC
  int i;
  int m1:2;
  int m2:4;
  int *p;
  int j;
} one_level3;

typedef struct One_Level2 {
PUBLIC
    int i;
    int m1:2;
    int m2:4;
    int *p;
    int j;
} one_level2;
  
typedef union U_lvl1 {
PUBLIC
  one_level2 u_separate;
  int u_ary[4];
} u_lvl1;

static int glb_uflds;


int drf_rba_uflds(int random_val)
{
    u_lvl1 u_class;
    int i;
    u_class.u_separate.p = &glb_uflds;
    for (i=0; i<4; ++i) {
        if (i == 2) {
            random_val++;
            u_class.u_ary[i] = 0; // p is at index 2
        }
    }
    return *(u_class.u_separate.p);
}


//
// Synopsis: Vul type: null pointer dereferenceing
//           Pattern:  single proc, field
//           Scope:    local
//           Flow:     no
//           Field:    two levels
//           Context:  no
//           Lang:     C/C++
//           Expect:   must
//

typedef struct One_level_x {
PUBLIC
  int i;
  int m1:2;
  int m2:4;
  int *p;
  int j;
} one_level_x;

typedef struct Two_level_x {
PUBLIC
  int a;
  one_level_x b;
  int c;
} two_level_x;

int drf_rba_mflds(int random_val, int i)
{
    two_level_x lcl_class;
    one_level_x *pb = &(lcl_class.b);

    for (i=0; i<100; ++i) {
        // the loop ensures that this case is non-trivial
        if (random_val != 10) {
            random_val++;
        }
    }
    return *(pb->p);
}


//
// Synopsis: Vul type: null pointer dereferenceing
//           Pattern:  single proc, flow sensitive
//           Scope:    file global 
//           Flow:     yes, nested
//           Field:    no
//           Context:  no
//           Lang:     C/C++
//           Expect:   must, specific path
//           Remark:   statis var with no addr taken 
//                     and assigned is really local
//

static int glb_fgb;
static int *p_fgb;

int drf_rba_sfs_fgb(int random_val)
{
  int i;
    for (i=0; i<100; i++) {
        if (random_val != 10) {
            random_val++;
            p_fgb = &glb_fgb;
        }
    }
    return *p_fgb;
}


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
