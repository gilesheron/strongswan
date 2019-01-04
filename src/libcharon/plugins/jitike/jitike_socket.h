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
 * @defgroup jitike_socket jitike_socket
 * @{ @ingroup jitike
 */

#ifndef JITIKE_SOCKET_H_
#define JITIKE_SOCKET_H_

#include "jitike_message.h"

#include <sa/ike_sa.h>

typedef struct jitike_socket_t jitike_socket_t;

/**
 * Socket to send/received SA synchronization data
 */
struct jitike_socket_t {

	/**
	 * Push synchronization information to the responsible node.
	 *
	 * @param message	message to send
	 */
	void (*push)(jitike_socket_t *this, jitike_message_t *message);

	/**
	 * Pull synchronization information from a peer we are responsible.
	 *
	 * @return			received message
	 */
	jitike_message_t *(*pull)(jitike_socket_t *this);

	/**
	 * Destroy a jitike_socket_t.
	 */
	void (*destroy)(jitike_socket_t *this);
};

/**
 * Create a jitike_socket instance.
 */
jitike_socket_t *jitike_socket_create(char *local, char *remote);

#endif /** JITIKE_SOCKET_ @}*/
