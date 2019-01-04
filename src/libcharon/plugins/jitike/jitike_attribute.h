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
 * @defgroup jitike_attribute jitike_attribute
 * @{ @ingroup jitike
 */

#ifndef JITIKE_ATTRIBUTE_H_
#define JITIKE_ATTRIBUTE_H_

#include "jitike_kernel.h"
#include "jitike_segments.h"

#include <attributes/attribute_provider.h>

typedef struct jitike_attribute_t jitike_attribute_t;

/**
 * A HA enabled in memory address pool attribute provider.
 */
struct jitike_attribute_t {

	/**
	 * Implements attribute provider interface.
	 */
	attribute_provider_t provider;

	/**
	 * Reserve an address for a passive IKE_SA.
	 *
	 * @param name			pool name to reserve address in
	 * @param address		address to reserve
	 */
	void (*reserve)(jitike_attribute_t *this, char *name, host_t *address);

	/**
	 * Destroy a jitike_attribute_t.
	 */
	void (*destroy)(jitike_attribute_t *this);
};

/**
 * Create a jitike_attribute instance.
 */
jitike_attribute_t *jitike_attribute_create(jitike_kernel_t *kernel, jitike_segments_t *segments);

#endif /** JITIKE_ATTRIBUTE_H_ @}*/
