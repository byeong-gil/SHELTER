/*
 * Copyright (c) 2016-2021, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <platform_def.h>

#include <common/bl_common.h>
#include <common/desc_image_load.h>

/*******************************************************************************
 * Following descriptor provides BL image/ep information that gets used
 * by BL2 to load the images and also subset of this information is
 * passed to next BL image. The image loading sequence is managed by
 * populating the images in required loading order. The image execution
 * sequence is managed by populating the `next_handoff_image_id` with
 * the next executable image id.
 ******************************************************************************/
static bl_mem_params_node_t bl2_mem_params_descs[] = {
#ifdef SCP_BL2_BASE
	/* Fill SCP_BL2 related information if it exists */
    {
	    .image_id = SCP_BL2_IMAGE_ID,

	    SET_STATIC_PARAM_HEAD(ep_info, PARAM_IMAGE_BINARY,
		    VERSION_2, entry_point_info_t, SECURE | NON_EXECUTABLE),

	    SET_STATIC_PARAM_HEAD(image_info, PARAM_IMAGE_BINARY,
		    VERSION_2, image_info_t, 0),
	    .image_info.image_base = SCP_BL2_BASE,
	    .image_info.image_max_size = PLAT_CSS_MAX_SCP_BL2_SIZE,

	    .next_handoff_image_id = INVALID_IMAGE_ID,
    },
#endif /* SCP_BL2_BASE */

#ifdef EL3_PAYLOAD_BASE
	/* Fill EL3 payload related information (BL31 is EL3 payload)*/
    {
	    .image_id = BL31_IMAGE_ID,

	    SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,
		    VERSION_2, entry_point_info_t,
		    SECURE | EXECUTABLE | EP_FIRST_EXE),
	    .ep_info.pc = EL3_PAYLOAD_BASE,
	    .ep_info.spsr = SPSR_64(MODE_EL3, MODE_SP_ELX,
		    DISABLE_ALL_EXCEPTIONS),

	    SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,
		    VERSION_2, image_info_t,
		    IMAGE_ATTRIB_PLAT_SETUP | IMAGE_ATTRIB_SKIP_LOADING),

	    .next_handoff_image_id = INVALID_IMAGE_ID,
    },

#else /* EL3_PAYLOAD_BASE */

	/* Fill BL31 related information */
    {
	    .image_id = BL31_IMAGE_ID,

	    SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,
		    VERSION_2, entry_point_info_t,
		    SECURE | EXECUTABLE | EP_FIRST_EXE),
	    .ep_info.pc = BL31_BASE,
	    .ep_info.spsr = SPSR_64(MODE_EL3, MODE_SP_ELX,
		    DISABLE_ALL_EXCEPTIONS),
#if DEBUG
	    .ep_info.args.arg3 = ARM_BL31_PLAT_PARAM_VAL,
#endif

	    SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,
		    VERSION_2, image_info_t, IMAGE_ATTRIB_PLAT_SETUP),
	    .image_info.image_base = BL31_BASE,
	    .image_info.image_max_size = BL31_LIMIT - BL31_BASE,

# ifdef BL32_BASE
	    .next_handoff_image_id = BL32_IMAGE_ID,
# else
	    .next_handoff_image_id = BL33_IMAGE_ID,
# endif
    },
	/* Fill HW_CONFIG related information */
    {
	    .image_id = HW_CONFIG_ID,
	    SET_STATIC_PARAM_HEAD(ep_info, PARAM_IMAGE_BINARY,
		    VERSION_2, entry_point_info_t, NON_SECURE | NON_EXECUTABLE),
	    SET_STATIC_PARAM_HEAD(image_info, PARAM_IMAGE_BINARY,
		    VERSION_2, image_info_t, IMAGE_ATTRIB_SKIP_LOADING),
	    .next_handoff_image_id = INVALID_IMAGE_ID,
    },
	/* Fill SOC_FW_CONFIG related information */
    {
	    .image_id = SOC_FW_CONFIG_ID,
	    SET_STATIC_PARAM_HEAD(ep_info, PARAM_IMAGE_BINARY,
		    VERSION_2, entry_point_info_t, SECURE | NON_EXECUTABLE),
	    SET_STATIC_PARAM_HEAD(image_info, PARAM_IMAGE_BINARY,
		    VERSION_2, image_info_t, IMAGE_ATTRIB_SKIP_LOADING),
	    .next_handoff_image_id = INVALID_IMAGE_ID,
    },
# ifdef BL32_BASE
	/* Fill BL32 related information */
    {
	    .image_id = BL32_IMAGE_ID,

	    SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,
#if ENABLE_RME
		    VERSION_2, entry_point_info_t, REALM | EXECUTABLE),
#else
		    VERSION_2, entry_point_info_t, SECURE | EXECUTABLE),
#endif
	    .ep_info.pc = BL32_BASE,

	    SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,
		    VERSION_2, image_info_t, 0),
	    .image_info.image_base = BL32_BASE,
	    .image_info.image_max_size = BL32_LIMIT - BL32_BASE,

	    .next_handoff_image_id = BL33_IMAGE_ID,
    },

	/*
	 * Fill BL32 external 1 related information.
	 * A typical use for extra1 image is with OP-TEE where it is the pager image.
	 */
    {
	    .image_id = BL32_EXTRA1_IMAGE_ID,

	    SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,
		    VERSION_2, entry_point_info_t, SECURE | NON_EXECUTABLE),

	    SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,
		    VERSION_2, image_info_t, IMAGE_ATTRIB_SKIP_LOADING),
	    .image_info.image_base = BL32_BASE,
	    .image_info.image_max_size = BL32_LIMIT - BL32_BASE,

	    .next_handoff_image_id = INVALID_IMAGE_ID,
    },

	/*
	 * Fill BL32 external 2 related information.
	 * A typical use for extra2 image is with OP-TEE where it is the paged image.
	 */
    {
	    .image_id = BL32_EXTRA2_IMAGE_ID,

	    SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,
		    VERSION_2, entry_point_info_t, SECURE | NON_EXECUTABLE),

	    SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,
		    VERSION_2, image_info_t, IMAGE_ATTRIB_SKIP_LOADING),
#ifdef SPD_opteed
	    .image_info.image_base = ARM_OPTEE_PAGEABLE_LOAD_BASE,
	    .image_info.image_max_size = ARM_OPTEE_PAGEABLE_LOAD_SIZE,
#endif
	    .next_handoff_image_id = INVALID_IMAGE_ID,
    },

	/* Fill TOS_FW_CONFIG related information */
    {
	    .image_id = TOS_FW_CONFIG_ID,
	    SET_STATIC_PARAM_HEAD(ep_info, PARAM_IMAGE_BINARY,
		    VERSION_2, entry_point_info_t, SECURE | NON_EXECUTABLE),
	    SET_STATIC_PARAM_HEAD(image_info, PARAM_IMAGE_BINARY,
		    VERSION_2, image_info_t, IMAGE_ATTRIB_SKIP_LOADING),
	    .next_handoff_image_id = INVALID_IMAGE_ID,
    },
# endif /* BL32_BASE */

	/* Fill BL33 related information */
    {
	    .image_id = BL33_IMAGE_ID,
	    SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,
		    VERSION_2, entry_point_info_t, NON_SECURE | EXECUTABLE),
# ifdef PRELOADED_BL33_BASE
	    .ep_info.pc = PRELOADED_BL33_BASE,

	    SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,
		    VERSION_2, image_info_t, IMAGE_ATTRIB_SKIP_LOADING),
# else
	    .ep_info.pc = PLAT_ARM_NS_IMAGE_BASE,

	    SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,
		    VERSION_2, image_info_t, 0),
	    .image_info.image_base = PLAT_ARM_NS_IMAGE_BASE,
	    .image_info.image_max_size = ARM_DRAM1_BASE + ARM_DRAM1_SIZE
		    - PLAT_ARM_NS_IMAGE_BASE,
# endif /* PRELOADED_BL33_BASE */

	    .next_handoff_image_id = INVALID_IMAGE_ID,
    },
	/* Fill NT_FW_CONFIG related information */
    {
	    .image_id = NT_FW_CONFIG_ID,
	    SET_STATIC_PARAM_HEAD(ep_info, PARAM_IMAGE_BINARY,
		    VERSION_2, entry_point_info_t, NON_SECURE | NON_EXECUTABLE),
	    SET_STATIC_PARAM_HEAD(image_info, PARAM_IMAGE_BINARY,
		    VERSION_2, image_info_t, IMAGE_ATTRIB_SKIP_LOADING),
	    .next_handoff_image_id = INVALID_IMAGE_ID,
    }
#endif /* EL3_PAYLOAD_BASE */
};

REGISTER_BL_IMAGE_DESCS(bl2_mem_params_descs)
