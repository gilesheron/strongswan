/*
 * Copyright (C) 2010 Martin Willi
 * Copyright (C) 2010 revosec AG
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
 * @defgroup jitike_cache jitike_cache
 * @{ @ingroup jitike
 */

#ifndef JITIKE_CACHE_H_
#define JITIKE_CACHE_H_

typedef struct jitike_cache_t jitike_cache_t;

#include "jitike_message.h"
#include "jitike_kernel.h"
#include "jitike_socket.h"

#include <collections/enumerator.h>

#include <sa/ike_sa.h>

/**
 * HA message caching facility, allows reintegration of new nodes.
 */
struct jitike_cache_t {

	/**
	 * Cache an IKE specific message.
	 *
	 * @param ike_sa		associated IKE_SA
	 * @param message		message to cache
	 */
	void (*cache)(jitike_cache_t *this, ike_sa_t *ike_sa, jitike_message_t *message);

	/**
	 * Delete a cache entry for an IKE_SA.
	 *
	 * @param ike_sa		cache entry to delete
	 */
	void (*delete)(jitike_cache_t *this, ike_sa_t *ike_sa);

	/**
	 * Resync a segment to the node using the cached messages.
	 *
	 * @param segment		segment to resync
	 */
	void (*resync)(jitike_cache_t *this, u_int segment);

	/**
	 * Destroy a jitike_cache_t.
	 */
	void (*destroy)(jitike_cache_t *this);
};

/**
 * Create a jitike_cache instance.
 *
 * @param kernel		kernel helper
 * @param socket		socket to send resync messages
 * @param tunnel		HA tunnel
 * @param resync 		request a resync during startup?
 * @param count			total number of segments
 */
jitike_cache_t *jitike_cache_create(jitike_kernel_t *kernel, jitike_socket_t *socket,
							jitike_tunnel_t *tunnel, bool resync, u_int count);

#endif /** JITIKE_CACHE_H_ @}*/
