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

#include <string.h>
#include <inttypes.h>

#include <sa/ikev2/keymat_v2.h>
#include <sa/ikev1/keymat_v1.h>

#include "jitike_redis.h"

#define DEFAULT_REDIS_HOSTNAME		"127.0.0.1"
#define DEFAULT_REDIS_PORT		6379
#define DEFAULT_REDIS_TIMEOUT_SEC	1
#define DEFAULT_REDIS_TIMEOUT_USEC	500000

typedef struct private_ike_redis_t private_ike_redis_t;
typedef struct private_jitike_redis_t private_jitike_redis_t;

/**
 * Data stored in redis.
 */
struct private_ike_redis_t {
	uint64_t spi_i;
	uint64_t spi_r;
};

/**
 * Redis variables
 */
struct private_jitike_redis_t {

	/**
	 * Public jitike_redis_t interface.
	 */
	jitike_redis_t public;

	/**
	 * Redis connection context.
	 */
	redisContext *ctx;

	/**
	 * Hostname to connect to.
	 */
	char *hostname;

	/**
	 * Port to connect to.
	 */
	int port;

	/**
	 * Connection timeout.
	 */
	struct timeval timeout;
};

METHOD(listener_t, alert, bool,
	private_jitike_redis_t *this, ike_sa_t *ike_sa, alert_t alert, va_list args)
{
	ike_sa_id_t *ike_sa_id;

	if ((ike_sa_id = ike_sa->get_id(ike_sa)) == NULL) {
		DBG1(DBG_CFG, "jitike_redis alert Error getting ike_sa");
		goto jitike;
	}

	DBG1(DBG_CFG, "jitike_redis received alert for SPI %.16"PRIx64"_i %.16"PRIx64"_r",
		be64toh(ike_sa_id->get_initiator_spi(ike_sa_id)),
		be64toh(ike_sa_id->get_responder_spi(ike_sa_id)));

jitike:
	return TRUE;
}

METHOD(listener_t, ike_keys, bool,
	private_jitike_redis_t *this, ike_sa_t *ike_sa, diffie_hellman_t *dh,
	chunk_t dh_other, chunk_t nonce_i, chunk_t nonce_r, ike_sa_t *rekey,
	shared_key_t *shared, auth_method_t method)
{
	ike_sa_id_t *ike_sa_id;
	redisReply *reply = NULL;
	private_ike_redis_t save;

	if ((ike_sa_id = ike_sa->get_id(ike_sa)) == NULL) {
		DBG1(DBG_CFG, "jitike_redis ike_keys Error getting ike_sa");
		goto jitike;
	}

	/* Store the keys in redis */
	//sprintf(ibuf, "%.16"PRIx64, be64toh(ike_sa_id->get_initiator_spi(ike_sa_id)));
	//sprintf(rbuf, "%.16"PRIx64, be64toh(ike_sa_id->get_responder_spi(ike_sa_id)));
	save.spi_i = ike_sa_id->get_initiator_spi(ike_sa_id);
	save.spi_r = ike_sa_id->get_responder_spi(ike_sa_id);

	reply = redisCommand(this->ctx, "HMSET %.16"PRIx64" ikesa %b",be64toh(ike_sa_id->get_initiator_spi(ike_sa_id)),
		&save, sizeof(private_ike_redis_t));

	DBG1(DBG_CFG, "jitike_redis received ike_keys for SPI %.16"PRIx64"_i %.16"PRIx64"_r",
		be64toh(ike_sa_id->get_initiator_spi(ike_sa_id)),
		be64toh(ike_sa_id->get_responder_spi(ike_sa_id)));

	/* Pull the keys from redis */
	reply = redisCommand(this->ctx, "HGET %.16"PRIx64" ikesa",
		be64toh(ike_sa_id->get_initiator_spi(ike_sa_id)));

	DBG1(DBG_CFG, "jitike_redis pulled data from redis: %.16"PRIx64"_i %.16"PRIx64"_r",
		be64toh(save.spi_i), be64toh(save.spi_r));

jitike:
	if (reply != NULL) {
		freeReplyObject(reply);
	}

	return TRUE;
}

METHOD(listener_t, ike_updown, bool,
	private_jitike_redis_t *this, ike_sa_t *ike_sa, bool up)
{
	ike_sa_id_t *ike_sa_id;

	if ((ike_sa_id = ike_sa->get_id(ike_sa)) == NULL) {
		DBG1(DBG_CFG, "jitike_redis ike_updown Error getting ike_sa");
		goto jitike;
	}

	DBG1(DBG_CFG, "jitike_redis received ike_updown for SPI %.16"PRIx64"_i %.16"PRIx64"_r",
		be64toh(ike_sa_id->get_initiator_spi(ike_sa_id)),
		be64toh(ike_sa_id->get_responder_spi(ike_sa_id)));

jitike:
	return TRUE;
}

METHOD(listener_t, ike_rekey, bool,
	private_jitike_redis_t *this, ike_sa_t *old, ike_sa_t *new)
{
	ike_sa_id_t *ike_sa_id_old;
	ike_sa_id_t *ike_sa_id_new;

	if ((ike_sa_id_old = old->get_id(old)) == NULL) {
		DBG1(DBG_CFG, "jitike_redis ike_rekey Error getting old ike_sa");
		goto jitike;
	}

	if ((ike_sa_id_new = new->get_id(new)) == NULL) {
		DBG1(DBG_CFG, "jitike_redis ike_rekey Error getting new ike_sa");
		goto jitike;
	}

	DBG1(DBG_CFG, "jitike_redis received ike_rekey for old SPI %.16"PRIx64"_i %.16"PRIx64"_r, new SPI %.16"PRIx64"_i %.16"PRIx64"_r",
		be64toh(ike_sa_id_old->get_initiator_spi(ike_sa_id_old)),
		be64toh(ike_sa_id_old->get_responder_spi(ike_sa_id_old)),
		be64toh(ike_sa_id_new->get_initiator_spi(ike_sa_id_new)),
		be64toh(ike_sa_id_new->get_responder_spi(ike_sa_id_new)));

jitike:
	return TRUE;
}

METHOD(listener_t, ike_state_change, bool,
	private_jitike_redis_t *this, ike_sa_t *ike_sa, ike_sa_state_t new)
{
	ike_sa_id_t *ike_sa_id;

	if ((ike_sa_id = ike_sa->get_id(ike_sa)) == NULL) {
		DBG1(DBG_CFG, "jitike_redis ike_state_change Error getting ike_sa");
		goto jitike;
	}

	DBG1(DBG_CFG, "jitike_redis received ike_state_change for SPI %.16"PRIx64"_i %.16"PRIx64"_r",
		be64toh(ike_sa_id->get_initiator_spi(ike_sa_id)),
		be64toh(ike_sa_id->get_responder_spi(ike_sa_id)));

jitike:
	return TRUE;
}

METHOD(listener_t, message_hook, bool,
	private_jitike_redis_t *this, ike_sa_t *ike_sa, message_t *message,
	bool incoming, bool plain)
{
	ike_sa_id_t *ike_sa_id;

	if ((ike_sa_id = ike_sa->get_id(ike_sa)) == NULL) {
		DBG1(DBG_CFG, "jitike_redis message_hook Error getting ike_sa");
		goto jitike;
	}

	DBG1(DBG_CFG, "jitike_redis received message_hook for SPI %.16"PRIx64"_i %.16"PRIx64"_r",
		be64toh(ike_sa_id->get_initiator_spi(ike_sa_id)),
		be64toh(ike_sa_id->get_responder_spi(ike_sa_id)));

jitike:
	return TRUE;
}

/**
 * Tear-down redis parameters.
 */
METHOD(jitike_redis_t, destroy, void,
        private_jitike_redis_t *this)
{
	redisFree(this->ctx);

	free(this);
}

/**
 * Setup redis parameters.
 */
jitike_redis_t *jitike_redis_create(void)
{
	private_jitike_redis_t *this;

	INIT(this,
		.public = {
			.listener = {
				.alert = _alert,
				.ike_keys = _ike_keys,
				.ike_updown = _ike_updown,
				.ike_rekey = _ike_rekey,
				.ike_state_change = _ike_state_change,
				.message = _message_hook,
			},
			.destroy = _destroy,
                },
		.hostname = lib->settings->get_str(lib->settings, "%s.plugins.jitike.redis.hostname",
						DEFAULT_REDIS_HOSTNAME, lib->ns),
		.port = lib->settings->get_int(lib->settings, "%s.plugins.jitike.redis.port",
						DEFAULT_REDIS_PORT, lib->ns),
		.timeout = {
			.tv_sec = lib->settings->get_int(lib->settings, "%s.plugins.jitike.redis.connect_sec",
						DEFAULT_REDIS_TIMEOUT_SEC, lib->ns),
			.tv_usec = lib->settings->get_int(lib->settings, "%s.plugins.jitike.redis.connect_usec",
						DEFAULT_REDIS_TIMEOUT_USEC, lib->ns),
		},
        );

	this->ctx = redisConnectWithTimeout(this->hostname, this->port, this->timeout);
	if (this->ctx == NULL || this->ctx->err) {
		if (this->ctx) {
			DBG1(DBG_CFG, "Connection error to %s/%d: %s",
				this->hostname, this->port, this->ctx->errstr);
			redisFree(this->ctx);
		} else {
			DBG1(DBG_CFG, "Connection error: cannot allocate redis context");
		}

		return NULL;
	}

	return &this->public;
}
