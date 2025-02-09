/*
 * Copyright (c) 2021, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RMMD_PRIVATE_H
#define RMMD_PRIVATE_H

#include <context.h>

/*******************************************************************************
 * Constants that allow assembler code to preserve callee-saved registers of the
 * C runtime context while performing a security state switch.
 ******************************************************************************/
#define RMMD_C_RT_CTX_X19		0x0
#define RMMD_C_RT_CTX_X20		0x8
#define RMMD_C_RT_CTX_X21		0x10
#define RMMD_C_RT_CTX_X22		0x18
#define RMMD_C_RT_CTX_X23		0x20
#define RMMD_C_RT_CTX_X24		0x28
#define RMMD_C_RT_CTX_X25		0x30
#define RMMD_C_RT_CTX_X26		0x38
#define RMMD_C_RT_CTX_X27		0x40
#define RMMD_C_RT_CTX_X28		0x48
#define RMMD_C_RT_CTX_X29		0x50
#define RMMD_C_RT_CTX_X30		0x58

#define RMMD_C_RT_CTX_SIZE		0x60
#define RMMD_C_RT_CTX_ENTRIES		(RMMD_C_RT_CTX_SIZE >> DWORD_SHIFT)

#ifndef __ASSEMBLER__
#include <services/rmi_svc.h>
#include <stdint.h>

typedef enum rmm_state {
	RMM_STATE_RESET = 0,
	RMM_STATE_IDLE
} rmm_state_t;

/*
 * Data structure used by the RMM dispatcher (RMMD) in EL3 to track context of
 * the RMM at R-EL2.
 */
typedef struct rmmd_rmm_context {
	uint64_t c_rt_ctx;
	cpu_context_t cpu_ctx;
	rmm_state_t state;
} rmmd_rmm_context_t;

/* Functions used to enter/exit the RMM synchronously */
uint64_t rmmd_rmm_sync_entry(rmmd_rmm_context_t *ctx);
__dead2 void rmmd_rmm_sync_exit(uint64_t rc);

/* Assembly helpers */
uint64_t rmmd_rmm_enter(uint64_t *c_rt_ctx);
void __dead2 rmmd_rmm_exit(uint64_t c_rt_ctx, uint64_t ret);

/* Reference to PM ops for the RMMD */
extern const spd_pm_ops_t rmmd_pm;

#endif /* __ASSEMBLER__ */

#endif /* RMMD_PRIVATE_H */
