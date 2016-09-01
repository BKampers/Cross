#ifndef _SIMPLE_H_
#define	_SIMPLE_H_

#include "Types.h"
#include "ApiStatus.h"


typedef struct
{
    const char* name;
    void (*Call) ();
} TestFunction;


#define EXPECT_TRUE(ACTUAL) expectTrue(ACTUAL, __LINE__)
#define EXPECT_FALSE(ACTUAL) expectFalse(ACTUAL, __LINE__)
#define EXPECT_NULL(ACTUAL) expectNull(ACTUAL, __LINE__)
#define EXPECT_NOT_NULL(ACTUAL) expectNotNull(ACTUAL, __LINE__)
#define EXPECT_EQUAL_INT(EXPECTED, ACTUAL) expectEqualInt(EXPECTED, ACTUAL, __LINE__)
#define EXPECT_UNEQUAL_INT(UNEXPECTED, ACTUAL) expectUnequalInt(UNEXPECTED, ACTUAL, __LINE__)
#define EXPECT_EQUAL_DOUBLE(EXPECTED, ACTUAL, PRECISION) expectEqualDouble(EXPECTED, ACTUAL, PRECISION, __LINE__)
#define EXPECT_EQUAL_STRING(EXPECTED, ACTUAL) expectEqualString(EXPECTED, ACTUAL, __LINE__)
#define EXPECT_STATUS(EXPECTED, ACTUAL) expectEqualStatus(EXPECTED, ACTUAL, __LINE__)
#define EXPECT_OK(ACTUAL) expectEqualStatus(OK, ACTUAL, __LINE__)

#define ASSERT_FALSE(ACTUAL) if (! EXPECT_FALSE(ACTUAL)) {return;}
#define ASSERT_TRUE(ACTUAL) if (! EXPECT_TRUE(ACTUAL)) {return;}
#define ASSERT_NULL(ACTUAL) if (! EXPECT_NULL(ACTUAL)) {return;}
#define ASSERT_NOT_NULL(ACTUAL) if (! EXPECT_NOT_NULL(ACTUAL)) {return;}
#define ASSERT_EQUAL_INT(EXPECTED, ACTUAL) if (! EXPECT_EQUAL_INT(EXPECTED, ACTUAL)) {return;}
#define ASSERT_UNEQUAL_INT(UNEXPECTED, ACTUAL) if (! EXPECT_UNEQUAL_INT(UNEXPECTED, ACTUAL)) {return;}
#define ASSERT_EQUAL_DOUBLE(EXPECTED, ACTUAL, PRECISION) if (! EXPECT_EQUAL_DOUBLE(EXPECTED, ACTUAL, PRECISION)) {return;}
#define ASSERT_EQUAL_STRING(EXPECTED, ACTUAL) if (! EXPECT_EQUAL_STRING(EXPECTED, ACTUAL)) {return;}
#define ASSERT_STATUS(EXPECTED, ACTUAL) if (! EXPECT_STATUS(EXPECTED, ACTUAL)) {return;}
#define ASSERT_OK(ACTUAL) if (! EXPECT_OK(ACTUAL)) {return;}

void setBeforeTest(void (*before));
void testAll(TestFunction* testFunctions, int count);

void startSuite(const char* suiteName);
void finishSuite();
void start(const char* testName);
void finish();

bool expectTrue(bool actual, int line);
bool expectFalse(bool actual, int line);
bool expectNull(void* actual, int line);
bool expectNotNull(void* actual, int line);
bool expectEqualInt(int expected, int actual, int line);
bool expectUnequalInt(int unexpected, int actual, int line);
bool expectEqualDouble(double expected, double actual, double precision, int line);
bool expectEqualString(const char* expected, const char* actual, int line);
bool expectEqualStatus(Status expected, Status actual, int line);


#endif	/* _SIMPLE_H_ */