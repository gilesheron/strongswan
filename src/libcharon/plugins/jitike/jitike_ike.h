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
 * @defgroup jitike_ike jitike_ike
 * @{ @ingroup jitike
 */

#ifndef JITIKE_IKE_H_
#define JITIKE_IKE_H_

typedef struct jitike_ike_t jitike_ike_t;

#include "jitike_socket.h"
#include "jitike_tunnel.h"
#include "jitike_segments.h"
#include "jitike_cache.h"

#include <daemon.h>

/**
 * Listener to synchronize IKE_SAs.
 */
struct jitike_ike_t {

	/**
	 * Implements bus listener interface.
	 */
	listener_t listener;

	/**
	 * Destroy a jitike_ike_t.
	 */
	void (*destroy)(jitike_ike_t *this);
};

/**
 * Create a jitike_ike instance.
 *
 * @param socket		socket to use for sending synchronization messages
 * @param tunnel		tunnel securing sync messages, if any
 * @param cache			message cache
 * @return				IKE listener
 */
jitike_ike_t *jitike_ike_create(jitike_socket_t *socket, jitike_tunnel_t *tunnel,
						jitike_cache_t *cache);

#endif /** JITIKE_IKE_ @}*/
