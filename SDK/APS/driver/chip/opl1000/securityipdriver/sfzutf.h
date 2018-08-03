/* sfzutf.h
 *
 * Description: SFZUTF header.
 */

/*****************************************************************************
* Copyright (c) 2014-2017 INSIDE Secure B.V. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
* For more information or support, please go to our online support system at
* https://customersupport.insidesecure.com.
* In case you do not have an account for this system, please send an e-mail
* to ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

#ifndef INCLUDE_GUARD_SFZUTF_H
#define INCLUDE_GUARD_SFZUTF_H

#include "basic_defs.h"
#include "log.h"
#include "clib.h"
#include <stdlib.h>


#define SFZUTF_MALLOC(x)   malloc((x)>0?(x):1)
#define SFZUTF_CALLOC(x,y) malloc(x*y)
#define SFZUTF_FREE(x)     free(x)
#define SFZUTF_USLEEP(x)   usleep(x)


#define SFZUTF_MEMCMP(a,b,l) memcmp(a, b, l)
#define SFZUTF_MEMCPY(a,b,l) memcpy(a, b, l)

#define SFZUTF_STRLEN(s) sfzutf_strlen(s)

/* Compositions of above, for common string operations. */
#define SFZUTF_STREQ(s1,s2)                             \
  (SFZUTF_STRLEN(s1) == SFZUTF_STRLEN(s2) &&            \
   SFZUTF_MEMCMP((s1), (s2), SFZUTF_STRLEN(s1)) == 0)

#define SFZUTF_STREQ_PREFIX(s1,sp)                      \
  (SFZUTF_STRLEN(s1) >= SFZUTF_STRLEN(sp) &&            \
   SFZUTF_MEMCMP((s1), (sp), SFZUTF_STRLEN(sp)) == 0)

#define END_TEST_SUCCES 0
#define END_TEST_FAIL   (-1)
#define END_TEST_UNSUPPORTED    (-2)
#define END_TEST_ABORT  (-1000)

// Test framework function prototypes and defines
typedef int (*TFun)(int _i);
typedef void (*SFun)(void);


#define START_TEST(name)                       \
  static int name(int _i)                      \
  {                                            \
    LOG_INFO("%s:L%d> START\n", __func__, _i); \
    do

#define END_TEST while(0);                               \
    if (sfzutf_unsupported_quick_process()) {            \
      LOG_CRIT("%s:L%d> NOT SUPPORTED\n", __func__, _i); \
    } else {                                             \
      LOG_CRIT("%s:L%d> PASSED\n", __func__, _i);        \
    }                                                    \
    return END_TEST_SUCCES;                              \
  }


#ifdef GCOV_PROFILE
void
__gcov_flush(); /* Function to write profiles on disk. */

#define SFZUTF_GCOV_FLUSH  __gcov_flush()
#else /* !GCOV_PROFILE */
#define SFZUTF_GCOV_FLUSH
#endif /* GCOV_PROFILE */

#define SFZUTF_FAILURE(expr,info,status)                                       \
  do {                                                                         \
    SFZUTF_GCOV_FLUSH;                                                         \
    LOG_CRIT("%s:%d> FAILED: %s%s(%d)\n",__func__,__LINE__,expr,info,status); \
    return END_TEST_FAIL;                                                      \
  } while(0)

#define SFZUTF_UNSUPPORTED(expr,info)                                      \
  do {                                                                     \
    SFZUTF_GCOV_FLUSH;                                                     \
    LOG_CRIT("%s:%d> NOT SUPPORTED: %s%s\n",__func__,__LINE__,expr,info); \
    return END_TEST_UNSUPPORTED;                                                  \
  } while(0)


/* Mark test failures */
#define fail(info,status)             \
  do {                                \
      SFZUTF_FAILURE("",info,status); \
  } while(0)

#define fail_if(expr,info,status)             \
  do {                                        \
    if (expr) {                               \
      SFZUTF_FAILURE(""#expr" ",info,status); \
    }                                         \
  } while(0)

#define fail_unless(expr,info,status)          \
  do {                                         \
    if (!(expr)) {                             \
      SFZUTF_FAILURE("!"#expr" ",info,status); \
    }                                          \
  } while(0)


/* Mark test as unsupported if condition is true. */
#define unsupported(info)        \
  do {                           \
    SFZUTF_UNSUPPORTED("",info); \
  } while(0)

#define unsupported_if(expr,info)          \
  do {                                     \
    if (expr) {                            \
      SFZUTF_UNSUPPORTED(""#expr" ",info); \
    }                                      \
  } while(0)

#define unsupported_unless(expr,info)       \
  do {                                      \
    if (!(expr)) {                          \
      SFZUTF_UNSUPPORTED("!"#expr" ",info); \
    }                                       \
  } while(0)

#define unsupported_quick(info)                                       \
  do {                                                                \
    LOG_CRIT("%s:%d> NOT SUPPORTED: %s\n", __FUNC__, __LINE_, info); \
    sfzutf_unsupported_quick();                                       \
  } while(0)


struct TestSuite
{
    struct TestSuite * NextSuite_p;
    const char * Name_p;
    struct TestCase * TestCaseList_p;
    bool Enabled;
};

struct TestCase
{
    struct TestCase * NextCase_p;
    const char * Name_p;
    SFun FixtureStartFunc;
    SFun FixtureEndFunc;
    struct Test * TestList_p;
    int TestsSuccess;
    int TestsFailed;
    bool Enabled;
};

struct Test
{
    struct Test * NextTest_p;
    const char * Name_p;
    TFun TestFunc;
    int Start;
    int NumberOfLoops;
    bool Enabled;
};


typedef enum
{
    SFZUTF_ENABLE_UNDETERMINED,
    SFZUTF_ENABLE_SINGLE,
    SFZUTF_ENABLE_AFTER,
    SFZUTF_ENABLE_ALL
}
SfzUtfEnable_t;


/* Note:
 *       Each suite needs to provide a interface that builds the test suite.
 *
 * Example:
 * int build_suite(void)
 * {
 *     struct TestSuite * TestSuite_p = sfzutf_tsuite_create("TestSuite name");
 *     if (TestSuite_p != NULL)
 *     {
 *         struct TestCase *TestCase_p = sfzutf_tcase_create(TestSuite_p, "TestCase name");
 *         if (TestCase_p != NULL)
 *         {
 *             if (sfzutf_test_add(TestCase_p, <test function>) != 0) goto FuncErrorReturn;
 *             ...
 *         }
 *         else
 *         {
 *             goto FuncErrorReturn;
 *         }
 *         TestCase_p = sfzutf_tcase_create(TestSuite_p, "Next TestCase name");
 *         if (TestCase_p != NULL)
 *         {
 *             if (sfzutf_test_add(TestCase_p, <test function>) != 0) goto FuncErrorReturn;
 *             ...
 *         }
 *         ...
 *     }
 *     return 0;
 *
 * FuncErrorReturn:
 *     return -1;
 * }
 **/

/* TestSuite helpers */
struct TestSuite *
sfzutf_tsuite_create(
        const char * Name_p);

void
sfzutf_tsuite_release(
        const char * Name_p);

struct TestSuite *
sfzutf_tsuite_get(
        const char * const SuiteName_p);

bool
sfzutf_tsuite_enable(
        struct TestSuite * TestSuite_p,
        const char * const SuiteName_p,
        const char * const TCaseName_p,
        const char * const TestName_p,
        const int * const IterValue_p,
        const SfzUtfEnable_t OrigEnableMode);

void
sfzutf_tsuite_disable(
        struct TestSuite * TestSuite_p);

int
sfzutf_tsuite_run(
        struct TestSuite * TestSuite_p);


/* TestCase helpers */
struct TestCase *
sfzutf_tcase_create(
        struct TestSuite * TestSuite_p,
        const char *Name_p);

void
sfzutf_tcase_release(
        struct TestCase *TestCaseList_p,
        const char *Name_p);

int
sfzutf_tcase_add_fixture(
        struct TestCase * TestCase_p,
        SFun StartFunc,
        SFun EndFunc);

/* Test helpers */
#define sfzutf_test_add(case,func) \
    sfzutf_ttest_create(case,func,""#func"",0)

#define sfzutf_test_add_loop(case,func,loops) \
    sfzutf_ttest_create(case,func,""#func"",loops)

int
sfzutf_ttest_create(
        struct TestCase * TestCase_p,
        TFun TestFunction,
        const char * Name_p,
        int NumberOfLoops);

void
sfzutf_ttest_release(
        struct Test * TestList_p,
        const char * Name_p);

void
sfzutf_unsupported_quick(void);

bool
sfzutf_unsupported_quick_process(void);



void * sfzutf_discard_const(const void * Ptr_p);



#endif /* Include Guard */

/* end of file sfzutf.h */
