/*
 * Copyright (C) 2008 Martin Willi
 * HSR Hochschule fuer Technik Rapperswil
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

/**
 * @defgroup jitike_dispatcher jitike_dispatcher
 * @{ @ingroup jitike
 */

#ifndef JITIKE_DISPATCHER_H_
#define JITIKE_DISPATCHER_H_

#include "jitike_socket.h"
#include "jitike_segments.h"
#include "jitike_cache.h"
#include "jitike_kernel.h"
#include "jitike_attribute.h"

typedef struct jitike_dispatcher_t jitike_dispatcher_t;

/**
 * The dispatcher pulls messages in a thread an processes them.
 */
struct jitike_dispatcher_t {

	/**
	 * Destroy a jitike_dispatcher_t.
	 */
	void (*destroy)(jitike_dispatcher_t *this);
};

/**
 * Create a jitike_dispatcher instance pulling from socket.
 *
 * @param socket		socket to pull messages from
 * @param segments		segments to control based on received messages
 * @param cache			message cache to use for resynchronization
 * @param kernel		kernel helper
 * @param attr			HA enabled pool
 * @return				dispatcher object
 */
jitike_dispatcher_t *jitike_dispatcher_create(jitike_socket_t *socket,
									jitike_segments_t *segments, jitike_cache_t *cache,
									jitike_kernel_t *kernel, jitike_attribute_t *attr);

#endif /** JITIKE_DISPATCHER_ @}*/
