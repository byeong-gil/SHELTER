#
# Copyright (c) 2013-2021, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

BL2_SOURCES		+=	bl2/bl2_image_load_v2.c			\
				bl2/bl2_main.c				\
				bl2/${ARCH}/bl2_arch_setup.c		\
				lib/locks/exclusive/${ARCH}/spinlock.S	\
				plat/common/${ARCH}/platform_up_stack.S	\
				${MBEDTLS_SOURCES}

ifeq (${ARCH},aarch64)
BL2_SOURCES		+=	common/aarch64/early_exceptions.S
endif

ifeq (${ENABLE_RME},1)
include lib/gpt/gpt.mk
# Using RME, run BL2 at EL3
BL2_SOURCES		+=      bl2/${ARCH}/bl2_el3_entrypoint.S	\
				bl2/${ARCH}/bl2_el3_exceptions.S	\
				${GPT_LIB_SRCS}
BL2_LINKERFILE		:=	bl2/bl2.ld.S

else ifeq (${BL2_AT_EL3},0)
# Normal operation, no RME, no BL2 at EL3
BL2_SOURCES		+=	bl2/${ARCH}/bl2_entrypoint.S
BL2_LINKERFILE		:=	bl2/bl2.ld.S

else
# BL2 at EL3, no RME
BL2_SOURCES		+=	bl2/${ARCH}/bl2_el3_entrypoint.S	\
				bl2/${ARCH}/bl2_el3_exceptions.S	\
				lib/cpus/${ARCH}/cpu_helpers.S		\
				lib/cpus/errata_report.c

ifeq (${DISABLE_MTPMU},1)
BL2_SOURCES		+=	lib/extensions/mtpmu/${ARCH}/mtpmu.S
endif

ifeq (${ARCH},aarch64)
BL2_SOURCES		+=	lib/cpus/aarch64/dsu_helpers.S
endif

BL2_LINKERFILE		:=	bl2/bl2_el3.ld.S
endif
