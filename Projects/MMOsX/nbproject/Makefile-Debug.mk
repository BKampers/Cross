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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/445223610/Crank.o \
	${OBJECTDIR}/_ext/445223610/Engine.o \
	${OBJECTDIR}/_ext/445223610/Ignition.o \
	${OBJECTDIR}/_ext/445223610/Injection.o \
	${OBJECTDIR}/_ext/445223610/InjectionTimer.o \
	${OBJECTDIR}/_ext/445223610/MeasurementTable.o \
	${OBJECTDIR}/_ext/445223610/Measurements.o \
	${OBJECTDIR}/_ext/445223610/Messaging.o \
	${OBJECTDIR}/_ext/445223610/Table.o \
	${OBJECTDIR}/_ext/445223610/main.o \
	${OBJECTDIR}/_ext/1917134825/PersistentElementManager.o \
	${OBJECTDIR}/_ext/1788660382/JsonMessage.o \
	${OBJECTDIR}/_ext/1788660382/jsonparse.o \
	${OBJECTDIR}/_ext/1308665564/AnalogInput.o \
	${OBJECTDIR}/_ext/1308665564/ApiStatus.o \
	${OBJECTDIR}/_ext/1308665564/ExternalInterrupt.o \
	${OBJECTDIR}/_ext/1308665564/FiledMemory.o \
	${OBJECTDIR}/_ext/1308665564/LcdStub.o \
	${OBJECTDIR}/_ext/1308665564/PinStub.o \
	${OBJECTDIR}/_ext/1308665564/SocketCommunication.o \
	${OBJECTDIR}/_ext/1308665564/Timers.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mmosx

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mmosx: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mmosx ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/445223610/Crank.o: ../../Applications/Motor\ Management/Crank.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/445223610
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/445223610/Crank.o ../../Applications/Motor\ Management/Crank.c

${OBJECTDIR}/_ext/445223610/Engine.o: ../../Applications/Motor\ Management/Engine.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/445223610
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/445223610/Engine.o ../../Applications/Motor\ Management/Engine.c

${OBJECTDIR}/_ext/445223610/Ignition.o: ../../Applications/Motor\ Management/Ignition.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/445223610
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/445223610/Ignition.o ../../Applications/Motor\ Management/Ignition.c

${OBJECTDIR}/_ext/445223610/Injection.o: ../../Applications/Motor\ Management/Injection.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/445223610
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/445223610/Injection.o ../../Applications/Motor\ Management/Injection.c

${OBJECTDIR}/_ext/445223610/InjectionTimer.o: ../../Applications/Motor\ Management/InjectionTimer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/445223610
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/445223610/InjectionTimer.o ../../Applications/Motor\ Management/InjectionTimer.c

${OBJECTDIR}/_ext/445223610/MeasurementTable.o: ../../Applications/Motor\ Management/MeasurementTable.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/445223610
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/445223610/MeasurementTable.o ../../Applications/Motor\ Management/MeasurementTable.c

${OBJECTDIR}/_ext/445223610/Measurements.o: ../../Applications/Motor\ Management/Measurements.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/445223610
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/445223610/Measurements.o ../../Applications/Motor\ Management/Measurements.c

${OBJECTDIR}/_ext/445223610/Messaging.o: ../../Applications/Motor\ Management/Messaging.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/445223610
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/445223610/Messaging.o ../../Applications/Motor\ Management/Messaging.c

${OBJECTDIR}/_ext/445223610/Table.o: ../../Applications/Motor\ Management/Table.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/445223610
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/445223610/Table.o ../../Applications/Motor\ Management/Table.c

${OBJECTDIR}/_ext/445223610/main.o: ../../Applications/Motor\ Management/main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/445223610
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/445223610/main.o ../../Applications/Motor\ Management/main.c

${OBJECTDIR}/_ext/1917134825/PersistentElementManager.o: ../../Generic/PersistentElementManager.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1917134825
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1917134825/PersistentElementManager.o ../../Generic/PersistentElementManager.c

${OBJECTDIR}/_ext/1788660382/JsonMessage.o: ../../Json/source/JsonMessage.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1788660382
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1788660382/JsonMessage.o ../../Json/source/JsonMessage.c

${OBJECTDIR}/_ext/1788660382/jsonparse.o: ../../Json/source/jsonparse.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1788660382
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1788660382/jsonparse.o ../../Json/source/jsonparse.c

${OBJECTDIR}/_ext/1308665564/AnalogInput.o: ../../Platforms/MacOS/AnalogInput.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1308665564
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1308665564/AnalogInput.o ../../Platforms/MacOS/AnalogInput.c

${OBJECTDIR}/_ext/1308665564/ApiStatus.o: ../../Platforms/MacOS/ApiStatus.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1308665564
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1308665564/ApiStatus.o ../../Platforms/MacOS/ApiStatus.c

${OBJECTDIR}/_ext/1308665564/ExternalInterrupt.o: ../../Platforms/MacOS/ExternalInterrupt.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1308665564
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1308665564/ExternalInterrupt.o ../../Platforms/MacOS/ExternalInterrupt.c

${OBJECTDIR}/_ext/1308665564/FiledMemory.o: ../../Platforms/MacOS/FiledMemory.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1308665564
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1308665564/FiledMemory.o ../../Platforms/MacOS/FiledMemory.c

${OBJECTDIR}/_ext/1308665564/LcdStub.o: ../../Platforms/MacOS/LcdStub.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1308665564
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1308665564/LcdStub.o ../../Platforms/MacOS/LcdStub.c

${OBJECTDIR}/_ext/1308665564/PinStub.o: ../../Platforms/MacOS/PinStub.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1308665564
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1308665564/PinStub.o ../../Platforms/MacOS/PinStub.c

${OBJECTDIR}/_ext/1308665564/SocketCommunication.o: ../../Platforms/MacOS/SocketCommunication.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1308665564
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1308665564/SocketCommunication.o ../../Platforms/MacOS/SocketCommunication.c

${OBJECTDIR}/_ext/1308665564/Timers.o: ../../Platforms/MacOS/Timers.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1308665564
	${RM} "$@.d"
	$(COMPILE.c) -g -I../../API -I../../Json/include -I../../Applications/Motor\ Management -I../../Platforms/MacOS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1308665564/Timers.o ../../Platforms/MacOS/Timers.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mmosx

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
