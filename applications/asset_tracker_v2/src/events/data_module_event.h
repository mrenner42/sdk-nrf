/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef _DATA_MODULE_EVENT_H_
#define _DATA_MODULE_EVENT_H_

/**
 * @brief Data module event
 * @defgroup data_module_event Data module event
 * @{
 */

#include <app_event_manager.h>
#include <app_event_manager_profiler_tracer.h>
#include "cloud/cloud_codec/cloud_codec.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Data event types submitted by Data module. */
enum data_module_event_type {
	/** All data has been received for a given sample request. */
	DATA_EVT_DATA_READY,

	/** Send newly sampled data.
	 *  The event has an associated payload of type @ref data_module_data_buffers in
	 *  the `data.buffer` member.
	 */
	DATA_EVT_DATA_SEND,

	/** Send older batched data.
	 *  The event has an associated payload of type @ref data_module_data_buffers in
	 *  the `data.buffer` member.
	 */
	DATA_EVT_DATA_SEND_BATCH,

	/** Send UI button data.
	 *  The event has an associated payload of type @ref data_module_data_buffers in
	 *  the `data.buffer` member.
	 */
	DATA_EVT_UI_DATA_SEND,

	/** UI button data is ready to be sent. */
	DATA_EVT_UI_DATA_READY,

	/** Send neighbor cell measurements.
	 *  The event has an associated payload of type @ref data_module_data_buffers in
	 *  the `data.buffer` member.
	 */
	DATA_EVT_NEIGHBOR_CELLS_DATA_SEND,

	/** Send A-GPS request.
	 *  The event has an associated payload of type @ref data_module_data_buffers in
	 *  the `data.buffer` member.
	 */
	DATA_EVT_AGPS_REQUEST_DATA_SEND,

	/** Send the initial device configuration.
	 *  The event has an associated payload of type @ref cloud_data_cfg in
	 *  the `data.cfg` member.
	 */
	DATA_EVT_CONFIG_INIT,

	/** Send the updated device configuration.
	 *  The event has an associated payload of type @ref cloud_data_cfg in
	 *  the `data.cfg` member.
	 */
	DATA_EVT_CONFIG_READY,

	/** Acknowledge the applied device configuration to cloud.
	 *  The event has an associated payload of type @ref data_module_data_buffers in
	 *  the `data.buffer` member.
	 */
	DATA_EVT_CONFIG_SEND,

	/** Get the recent device configuration from cloud. */
	DATA_EVT_CONFIG_GET,

	/** Date time has been obtained. */
	DATA_EVT_DATE_TIME_OBTAINED,

	/** The data module has performed all procedures to prepare for
	 *  a shutdown of the system. The event carries the ID (id) of the module.
	 */
	DATA_EVT_SHUTDOWN_READY,

	/** An irrecoverable error has occurred in the data module. Error details are
	 *  attached in the event structure.
	 */
	DATA_EVT_ERROR
};

/** @brief Structure that contains a pointer to encoded data. */
struct data_module_data_buffers {
	char *buf;
	size_t len;
	/** Object paths used in lwM2M. NULL terminated. */
	char paths[CONFIG_CLOUD_CODEC_LWM2M_PATH_LIST_ENTRIES_MAX]
		  [CONFIG_CLOUD_CODEC_LWM2M_PATH_ENTRY_SIZE_MAX];
	uint8_t valid_object_paths;
};

/** @brief Data module event. */
struct data_module_event {
	/** Data module application event header. */
	struct app_event_header header;
	/** Data module event type. */
	enum data_module_event_type type;

	union {
		/** Variable that carries a pointer to data encoded by the module. */
		struct data_module_data_buffers buffer;
		/** Variable that carries the current device configuration. */
		struct cloud_data_cfg cfg;
		/** Module ID, used when acknowledging shutdown requests. */
		uint32_t id;
		/** Code signifying the cause of error. */
		int err;
	} data;
};

APP_EVENT_TYPE_DECLARE(data_module_event);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* _DATA_MODULE_EVENT_H_ */
