#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/620440e/stm32f10x_flash_stub.o \
	${OBJECTDIR}/_ext/8dbad817/ApiStatus.o \
	${OBJECTDIR}/_ext/b24da73c/FlashDriver.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f1

# Test Object Files
TESTOBJECTFILES= \
	${TESTDIR}/_ext/513a94e0/FlashDriverTest.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libstmstub.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libstmstub.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libstmstub.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libstmstub.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libstmstub.a

${OBJECTDIR}/_ext/620440e/stm32f10x_flash_stub.o: ../../Applications/STM\ Stub/stm32f10x_flash_stub.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/620440e
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/620440e/stm32f10x_flash_stub.o ../../Applications/STM\ Stub/stm32f10x_flash_stub.c

${OBJECTDIR}/_ext/8dbad817/ApiStatus.o: ../../Generic/ApiStatus.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/8dbad817
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8dbad817/ApiStatus.o ../../Generic/ApiStatus.c

${OBJECTDIR}/_ext/b24da73c/FlashDriver.o: ../../Platforms/STM32/FlashDriver.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b24da73c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b24da73c/FlashDriver.o ../../Platforms/STM32/FlashDriver.c

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-tests-subprojects .build-conf ${TESTFILES}
.build-tests-subprojects:

${TESTDIR}/TestFiles/f1: ${TESTDIR}/_ext/513a94e0/FlashDriverTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.c}   -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS} 


${TESTDIR}/_ext/513a94e0/FlashDriverTest.o: ../../Applications/STM\ Stub/tests/FlashDriverTest.c 
	${MKDIR} -p ${TESTDIR}/_ext/513a94e0
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/_ext/513a94e0/FlashDriverTest.o ../../Applications/STM\ Stub/tests/FlashDriverTest.c


${OBJECTDIR}/_ext/620440e/stm32f10x_flash_stub_nomain.o: ${OBJECTDIR}/_ext/620440e/stm32f10x_flash_stub.o ../../Applications/STM\ Stub/stm32f10x_flash_stub.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/620440e
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/620440e/stm32f10x_flash_stub.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/620440e/stm32f10x_flash_stub_nomain.o ../../Applications/STM\ Stub/stm32f10x_flash_stub.c;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/620440e/stm32f10x_flash_stub.o ${OBJECTDIR}/_ext/620440e/stm32f10x_flash_stub_nomain.o;\
	fi

${OBJECTDIR}/_ext/8dbad817/ApiStatus_nomain.o: ${OBJECTDIR}/_ext/8dbad817/ApiStatus.o ../../Generic/ApiStatus.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/8dbad817
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/8dbad817/ApiStatus.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8dbad817/ApiStatus_nomain.o ../../Generic/ApiStatus.c;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/8dbad817/ApiStatus.o ${OBJECTDIR}/_ext/8dbad817/ApiStatus_nomain.o;\
	fi

${OBJECTDIR}/_ext/b24da73c/FlashDriver_nomain.o: ${OBJECTDIR}/_ext/b24da73c/FlashDriver.o ../../Platforms/STM32/FlashDriver.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b24da73c
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/b24da73c/FlashDriver.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b24da73c/FlashDriver_nomain.o ../../Platforms/STM32/FlashDriver.c;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/b24da73c/FlashDriver.o ${OBJECTDIR}/_ext/b24da73c/FlashDriver_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f1 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libstmstub.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
