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
 * @defgroup jitike_ctl jitike_ctl
 * @{ @ingroup jitike
 */

#ifndef JITIKE_CTL_H_
#define JITIKE_CTL_H_

#include "jitike_segments.h"
#include "jitike_cache.h"

typedef struct jitike_ctl_t jitike_ctl_t;

/**
 * HA Sync control interface using a FIFO.
 */
struct jitike_ctl_t {

	/**
	 * Destroy a jitike_ctl_t.
	 */
	void (*destroy)(jitike_ctl_t *this);
};

/**
 * Create a jitike_ctl instance.
 *
 * @param segments	segments to control
 * @param cache		message cache for resynchronization
 * @return			HA control interface
 */
jitike_ctl_t *jitike_ctl_create(jitike_segments_t *segments, jitike_cache_t *cache);

#endif /** JITIKE_CTL_ @}*/
