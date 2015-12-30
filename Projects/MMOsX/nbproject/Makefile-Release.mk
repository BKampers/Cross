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
	${OBJECTDIR}/_ext/1a8992ba/Crank.o \
	${OBJECTDIR}/_ext/1a8992ba/Engine.o \
	${OBJECTDIR}/_ext/1a8992ba/Ignition.o \
	${OBJECTDIR}/_ext/1a8992ba/Injection.o \
	${OBJECTDIR}/_ext/1a8992ba/InjectionTimer.o \
	${OBJECTDIR}/_ext/1a8992ba/MeasurementTable.o \
	${OBJECTDIR}/_ext/1a8992ba/Measurements.o \
	${OBJECTDIR}/_ext/1a8992ba/Messaging.o \
	${OBJECTDIR}/_ext/1a8992ba/Table.o \
	${OBJECTDIR}/_ext/1a8992ba/main.o \
	${OBJECTDIR}/_ext/8dbad817/ApiStatus.o \
	${OBJECTDIR}/_ext/8dbad817/Communication.o \
	${OBJECTDIR}/_ext/8dbad817/Json.o \
	${OBJECTDIR}/_ext/8dbad817/JsonParser.o \
	${OBJECTDIR}/_ext/8dbad817/JsonWriter.o \
	${OBJECTDIR}/_ext/8dbad817/PersistentElementManager.o \
	${OBJECTDIR}/_ext/b1ff5924/AnalogInput.o \
	${OBJECTDIR}/_ext/b1ff5924/Control.o \
	${OBJECTDIR}/_ext/b1ff5924/ExternalInterrupt.o \
	${OBJECTDIR}/_ext/b1ff5924/FiledMemory.o \
	${OBJECTDIR}/_ext/b1ff5924/LcdStub.o \
	${OBJECTDIR}/_ext/b1ff5924/PinStub.o \
	${OBJECTDIR}/_ext/b1ff5924/SocketCommunication.o \
	${OBJECTDIR}/_ext/b1ff5924/SocketHandler.o \
	${OBJECTDIR}/_ext/b1ff5924/Timers.o


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

${OBJECTDIR}/_ext/1a8992ba/Crank.o: ../../Applications/Motor\ Management/Crank.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1a8992ba
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1a8992ba/Crank.o ../../Applications/Motor\ Management/Crank.c

${OBJECTDIR}/_ext/1a8992ba/Engine.o: ../../Applications/Motor\ Management/Engine.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1a8992ba
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1a8992ba/Engine.o ../../Applications/Motor\ Management/Engine.c

${OBJECTDIR}/_ext/1a8992ba/Ignition.o: ../../Applications/Motor\ Management/Ignition.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1a8992ba
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1a8992ba/Ignition.o ../../Applications/Motor\ Management/Ignition.c

${OBJECTDIR}/_ext/1a8992ba/Injection.o: ../../Applications/Motor\ Management/Injection.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1a8992ba
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1a8992ba/Injection.o ../../Applications/Motor\ Management/Injection.c

${OBJECTDIR}/_ext/1a8992ba/InjectionTimer.o: ../../Applications/Motor\ Management/InjectionTimer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1a8992ba
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1a8992ba/InjectionTimer.o ../../Applications/Motor\ Management/InjectionTimer.c

${OBJECTDIR}/_ext/1a8992ba/MeasurementTable.o: ../../Applications/Motor\ Management/MeasurementTable.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1a8992ba
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1a8992ba/MeasurementTable.o ../../Applications/Motor\ Management/MeasurementTable.c

${OBJECTDIR}/_ext/1a8992ba/Measurements.o: ../../Applications/Motor\ Management/Measurements.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1a8992ba
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1a8992ba/Measurements.o ../../Applications/Motor\ Management/Measurements.c

${OBJECTDIR}/_ext/1a8992ba/Messaging.o: ../../Applications/Motor\ Management/Messaging.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1a8992ba
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1a8992ba/Messaging.o ../../Applications/Motor\ Management/Messaging.c

${OBJECTDIR}/_ext/1a8992ba/Table.o: ../../Applications/Motor\ Management/Table.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1a8992ba
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1a8992ba/Table.o ../../Applications/Motor\ Management/Table.c

${OBJECTDIR}/_ext/1a8992ba/main.o: ../../Applications/Motor\ Management/main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1a8992ba
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1a8992ba/main.o ../../Applications/Motor\ Management/main.c

${OBJECTDIR}/_ext/8dbad817/ApiStatus.o: ../../Generic/ApiStatus.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/8dbad817
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8dbad817/ApiStatus.o ../../Generic/ApiStatus.c

${OBJECTDIR}/_ext/8dbad817/Communication.o: ../../Generic/Communication.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/8dbad817
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8dbad817/Communication.o ../../Generic/Communication.c

${OBJECTDIR}/_ext/8dbad817/Json.o: ../../Generic/Json.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/8dbad817
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8dbad817/Json.o ../../Generic/Json.c

${OBJECTDIR}/_ext/8dbad817/JsonParser.o: ../../Generic/JsonParser.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/8dbad817
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8dbad817/JsonParser.o ../../Generic/JsonParser.c

${OBJECTDIR}/_ext/8dbad817/JsonWriter.o: ../../Generic/JsonWriter.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/8dbad817
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8dbad817/JsonWriter.o ../../Generic/JsonWriter.c

${OBJECTDIR}/_ext/8dbad817/PersistentElementManager.o: ../../Generic/PersistentElementManager.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/8dbad817
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8dbad817/PersistentElementManager.o ../../Generic/PersistentElementManager.c

${OBJECTDIR}/_ext/b1ff5924/AnalogInput.o: ../../Platforms/MacOS/AnalogInput.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b1ff5924
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b1ff5924/AnalogInput.o ../../Platforms/MacOS/AnalogInput.c

${OBJECTDIR}/_ext/b1ff5924/Control.o: ../../Platforms/MacOS/Control.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b1ff5924
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b1ff5924/Control.o ../../Platforms/MacOS/Control.c

${OBJECTDIR}/_ext/b1ff5924/ExternalInterrupt.o: ../../Platforms/MacOS/ExternalInterrupt.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b1ff5924
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b1ff5924/ExternalInterrupt.o ../../Platforms/MacOS/ExternalInterrupt.c

${OBJECTDIR}/_ext/b1ff5924/FiledMemory.o: ../../Platforms/MacOS/FiledMemory.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b1ff5924
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b1ff5924/FiledMemory.o ../../Platforms/MacOS/FiledMemory.c

${OBJECTDIR}/_ext/b1ff5924/LcdStub.o: ../../Platforms/MacOS/LcdStub.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b1ff5924
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b1ff5924/LcdStub.o ../../Platforms/MacOS/LcdStub.c

${OBJECTDIR}/_ext/b1ff5924/PinStub.o: ../../Platforms/MacOS/PinStub.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b1ff5924
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b1ff5924/PinStub.o ../../Platforms/MacOS/PinStub.c

${OBJECTDIR}/_ext/b1ff5924/SocketCommunication.o: ../../Platforms/MacOS/SocketCommunication.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b1ff5924
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b1ff5924/SocketCommunication.o ../../Platforms/MacOS/SocketCommunication.c

${OBJECTDIR}/_ext/b1ff5924/SocketHandler.o: ../../Platforms/MacOS/SocketHandler.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b1ff5924
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b1ff5924/SocketHandler.o ../../Platforms/MacOS/SocketHandler.c

${OBJECTDIR}/_ext/b1ff5924/Timers.o: ../../Platforms/MacOS/Timers.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b1ff5924
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b1ff5924/Timers.o ../../Platforms/MacOS/Timers.c

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
