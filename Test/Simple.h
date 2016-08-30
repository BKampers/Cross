#ifndef _SIMPLE_H_
#define	_SIMPLE_H_

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "Types.h"
#include "ApiStatus.h"


time_t suitStartTime;
time_t testStartTime;

const char* currentSuiteName = NULL; 
const char* currentTestName = NULL;


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


double timeSince(time_t time)
{
    return (clock() - time) / 4000.0;
}


double testTime()
{
    return timeSince(testStartTime);
}


void startSuite(const char* suiteName)
{
    currentSuiteName = suiteName;
    printf("%%SUITE_STARTING%% %s\n", suiteName);
    printf("%%SUITE_STARTED%%\n");
    suitStartTime = clock();
}


void finishSuite()
{
    printf("%%SUITE_FINISHED%% time=%f\n", timeSince(suitStartTime));
    currentSuiteName = NULL;
}


void start(const char* testName)
{
    currentTestName = testName;
    printf("%%TEST_STARTED%% %s (%s)\n", currentTestName, currentSuiteName);
    testStartTime = clock();
}


void finish()
{
    printf("%%TEST_FINISHED%% time=%f %s (%s)\n", testTime(), currentTestName, currentSuiteName);
    currentTestName = NULL;
}



void printTestLine(bool pass, int line, const char* message)
{
    printf("%s time=%f testname=%s (%s) message=(%d) %s\n",
        (pass) ? "%TEST_PASSED%" : "%TEST_FAILED%",
        testTime(),
        currentTestName,
        currentSuiteName,
        line,
        message
    );
    
}


bool expectTrue(bool actual, int line)
{
    bool pass = actual != FALSE;
    printTestLine(pass, line, (pass) ? "TRUE" : "FALSE");
    return actual;
}


bool expectFalse(bool actual, int line)
{
    bool pass = actual == FALSE;
    printTestLine(pass, line, (pass) ? "FALSE" : "TRUE");
    return pass;
}


bool expectNull(void* actual, int line)
{
    bool pass = (actual == NULL);
    printTestLine(pass, line, (pass) ? "NULL" : "NOT NULL");
    return pass;
}


bool expectNotNull(void* actual, int line)
{
    bool pass = (actual != NULL);
    printTestLine(pass, line,  (pass) ? "NOT NULL" : "NULL");
    return pass;
}


bool expectEqualInt(int expected, int actual, int line)
{
    char message[64];
    bool pass = (expected == actual);
    if (pass)
    {
        sprintf(message, "%d", actual);
    }
    else
    {
        sprintf(message, "expected: %d, actual: %d", expected, actual);
    }
    printTestLine(pass, line, message);
    return pass;
}


bool expectUnequalInt(int unexpected, int actual, int line)
{
    char message[64];
    bool pass = (unexpected != actual);
    if (pass)
    {
        sprintf(message, "%d", actual);
    }
    else
    {
        sprintf(message, "unexpected: %d, actual: %d", unexpected, actual);
    }
    printTestLine(pass, line, message);
    return pass;
}


bool expectEqualDouble(double expected, double actual, double precision, int line)
{
    char message[64];
    bool pass = ((expected - precision <= actual) && (actual <= expected + precision));
    if (pass)
    {
        sprintf(message, "%f", actual);
    }
    else
    {
        sprintf(message, "expected: %f, actual: %f", expected, actual);
    }
    printTestLine(pass, line, message);
    return pass;
}


bool expectEqualString(const char* expected, const char* actual, int line)
{
    char message[1024];
    bool pass = (expected != NULL) && (actual != NULL) && (strcmp(expected, actual) == 0);
    if (pass)
    {
        sprintf(message, "%s", actual);
    }
    else
    {
        sprintf(message, "expected: %s, actual: %s", expected, actual);
    }
    printTestLine(pass, line, message);
    return pass;
}


bool expectEqualStatus(Status expected, Status actual, int line)
{
    return expectEqualString(expected, actual, line);
}


#endif	/* _SIMPLE_H_ */