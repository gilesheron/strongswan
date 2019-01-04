/*
 * Copyright (c) 2019 Cisco and/or its affiliates.
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
 * @defgroup jitike_redis jitike_redis
 * @{ @ingroup jitike
 */

#ifndef JITIKE_REDIS_H_
#define JITIKE_REDIS_H_

#include <daemon.h>
#include <hiredis/hiredis.h>

typedef struct jitike_redis_t jitike_redis_t;

/**
 * Access to redis
 */
struct jitike_redis_t {

	/**
	 * Implements bus listener interface.
	 */
	listener_t listener;

        /**
         * Destroy a jitike_redis_t.
         */
        void (*destroy)(jitike_redis_t *this);
};

/**
 * Create a jitike_redis_t instance.
 *
 * @return            Listens for IKE messages and plumbs them into redis.
 */
jitike_redis_t *jitike_redis_create(void);

#endif /** JITIKE_REDIS_H_ @}*/
