#include <stdio.h>
#include <string.h>
#include <time.h>

#include "Simple.h"


/*
** Private
*/

#define MESSAGE_LENGTH 128

char message[MESSAGE_LENGTH];

const char* currentSuiteName = NULL; 
const char* currentTestName = NULL;

time_t suitStartTime;
time_t testStartTime;


double timeSince(time_t time)
{
    return (clock() - time) / 4000.0;
}


double testTime()
{
    return timeSince(testStartTime);
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


/*
** Interface
*/

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
    bool pass = (expected == actual);
    if (pass)
    {
        snprintf(message, MESSAGE_LENGTH, "%d", actual);
    }
    else
    {
        snprintf(message, MESSAGE_LENGTH, "expected: %d, actual: %d", expected, actual);
    }
    printTestLine(pass, line, message);
    return pass;
}


bool expectUnequalInt(int unexpected, int actual, int line)
{
    bool pass = (unexpected != actual);
    if (pass)
    {
        snprintf(message, MESSAGE_LENGTH, "%d", actual);
    }
    else
    {
        snprintf(message, MESSAGE_LENGTH, "unexpected: %d, actual: %d", unexpected, actual);
    }
    printTestLine(pass, line, message);
    return pass;
}


bool expectEqualDouble(double expected, double actual, double precision, int line)
{
    bool pass = ((expected - precision <= actual) && (actual <= expected + precision));
    if (pass)
    {
        snprintf(message, MESSAGE_LENGTH, "%f", actual);
    }
    else
    {
        snprintf(message, MESSAGE_LENGTH, "expected: %f, actual: %f", expected, actual);
    }
    printTestLine(pass, line, message);
    return pass;
}


bool expectEqualString(const char* expected, const char* actual, int line)
{
    bool pass = (expected != NULL) && (actual != NULL) && (strcmp(expected, actual) == 0);
    if (pass)
    {
        snprintf(message, MESSAGE_LENGTH, "%s", actual);
    }
    else
    {
        snprintf(message, MESSAGE_LENGTH, "expected: %s, actual: %s", expected, actual);
    }
    printTestLine(pass, line, message);
    return pass;
}


bool expectEqualStatus(Status expected, Status actual, int line)
{
    return expectEqualString(expected, actual, line);
}
