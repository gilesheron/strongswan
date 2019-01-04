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
 * @defgroup jitike_segments jitike_segments
 * @{ @ingroup jitike
 */

#ifndef JITIKE_SEGMENTS_H_
#define JITIKE_SEGMENTS_H_

#include <daemon.h>

typedef struct jitike_segments_t jitike_segments_t;

typedef uint16_t segment_mask_t;

/**
 * maximum number of segments
 */
#define SEGMENTS_MAX (sizeof(segment_mask_t)*8)

/**
 * Get the bit in the mask of a segment
 */
#define SEGMENTS_BIT(segment) (0x01 << (segment - 1))

#include "jitike_socket.h"
#include "jitike_tunnel.h"
#include "jitike_kernel.h"

/**
 * Segmentation of peers into active and passive.
 */
struct jitike_segments_t {

	/**
	 * Implements listener interface to catch daemon shutdown.
	 */
	listener_t listener;

	/**
	 * Activate a set of IKE_SAs identified by a segment.
	 *
	 * @param segment	numerical segment to takeover, 0 for all
	 * @param notify	whether to notify other nodes about activation
	 */
	void (*activate)(jitike_segments_t *this, u_int segment, bool notify);

	/**
	 * Deactivate a set of IKE_SAs identified by a segment.
	 *
	 * @param segment	numerical segment to takeover, 0 for all
	 * @param notify	whether to notify other nodes about deactivation
	 */
	void (*deactivate)(jitike_segments_t *this, u_int segment, bool notify);

	/**
	 * Handle a status message from the remote node.
	 *
	 * @param mask		segments the remote node is serving actively
	 */
	void (*handle_status)(jitike_segments_t *this, segment_mask_t mask);

	/**
	 * Check if a given segment is currently active.
	 *
	 * @param segment	segment to check
	 * @return			TRUE if segment active
	 */
	bool (*is_active)(jitike_segments_t *this, u_int segment);

	/**
	 * Return the number of segments
	 *
	 * @return			number of segments
	 */
	u_int (*count)(jitike_segments_t *this);

	/**
	 * Destroy a jitike_segments_t.
	 */
	void (*destroy)(jitike_segments_t *this);
};

/**
 * Create a jitike_segments instance.
 *
 * @param socket		socket to communicate segment (de-)activation
 * @param kernel		interface to control segments at kernel level
 * @param tunnel		HA tunnel
 * @param count			number of segments the cluster uses
 * @param node			node, currently 1 or 0
 * @param monitor		should we use monitoring functionality
 * @return				segment object
 */
jitike_segments_t *jitike_segments_create(jitike_socket_t *socket, jitike_kernel_t *kernel,
								  jitike_tunnel_t *tunnel, u_int count, u_int node,
								  bool monitor);

#endif /** JITIKE_SEGMENTS_ @}*/
