#******************************************************************************
#
# Makefile - Rules for building the Software UART Example.
#
# Copyright (c) 2009 Luminary Micro, Inc.  All rights reserved.
# 
# Software License Agreement
# 
# Luminary Micro, Inc. (LMI) is supplying this software for use solely and
# exclusively on LMI's microcontroller products.
# 
# The software is owned by LMI and/or its suppliers, and is protected under
# applicable copyright laws.  All rights are reserved.  You may not combine
# this software with "viral" open-source software in order to form a larger
# program.  Any use in violation of the foregoing restrictions may subject
# the user to criminal sanctions under applicable laws, as well as to civil
# liability for the breach of the terms and conditions of this license.
# 
# THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
# OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
# LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
# CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 4049 of the Software UART Example.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=LM3S3748

#
# The base directory for DriverLib.
#
ROOT=../..

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find header files that do not live in the source directory.
#
IPATH=../..

#
# The default rule, which causes the Software UART Example to be built.
#
all: ${COMPILER}
all: ${COMPILER}/sw01270.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir ${COMPILER}

#
# Rules for building the Software UART Example.
#
${COMPILER}/sw01270.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/sw01270.axf: ${COMPILER}/sw01270.o
${COMPILER}/sw01270.axf: ${COMPILER}/sw_uart_basic.o
${COMPILER}/sw01270.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
ifeq (${COMPILER}, gcc)
${COMPILER}/sw01270.axf: sw01270.ld
endif
SCATTERgcc_sw01270=sw01270.ld
ifeq (${COMPILER}, sourcerygxx)
${COMPILER}/sw01270.axf: sw01270_sourcerygxx.ld
endif
SCATTERsourcerygxx_sw01270=lm3s3748-rom.ld -T sw01270_sourcerygxx.ld
ENTRY_sw01270=ResetISR
CFLAGSgcc=-DTARGET_IS_DUSTDEVIL_RA0
CFLAGSsourcerygxx=-DTARGET_IS_DUSTDEVIL_RA0

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
