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
 * @defgroup jitike_message jitike_message
 * @{ @ingroup jitike
 */

#ifndef JITIKE_MESSAGE_H_
#define JITIKE_MESSAGE_H_

#include <library.h>
#include <networking/host.h>
#include <utils/identification.h>
#include <sa/ike_sa_id.h>
#include <selectors/traffic_selector.h>

/**
 * Protocol version of this implementation
 */
#define JITIKE_MESSAGE_VERSION 3

typedef struct jitike_message_t jitike_message_t;
typedef enum jitike_message_type_t jitike_message_type_t;
typedef enum jitike_message_attribute_t jitike_message_attribute_t;
typedef union jitike_message_value_t jitike_message_value_t;

/**
 * Type of a HA message
 */
enum jitike_message_type_t {
	/** add a completely new IKE_SA */
	JITIKE_IKE_ADD = 1,
	/** update an existing IKE_SA (identities, address update, ...) */
	JITIKE_IKE_UPDATE,
	/** update initiator message id */
	JITIKE_IKE_MID_INITIATOR,
	/** update responder message id */
	JITIKE_IKE_MID_RESPONDER,
	/** delete an existing IKE_SA */
	JITIKE_IKE_DELETE,
	/** add a new CHILD_SA */
	JITIKE_CHILD_ADD,
	/** delete an existing CHILD_SA */
	JITIKE_CHILD_DELETE,
	/** segments the sending node is giving up */
	JITIKE_SEGMENT_DROP,
	/** segments the sending node is taking over */
	JITIKE_SEGMENT_TAKE,
	/** status with the segments the sending node is currently serving */
	JITIKE_STATUS,
	/** segments the receiving node is requested to resync */
	JITIKE_RESYNC,
	/** IV synchronization for IKEv1 Main/Aggressive mode */
	JITIKE_IKE_IV,
};

/**
 * Enum names for message types
 */
extern enum_name_t *jitike_message_type_names;

/**
 * Type of attributes contained in a message
 */
enum jitike_message_attribute_t {
	/** ike_sa_id_t*, to identify IKE_SA */
	JITIKE_IKE_ID = 1,
	/** ike_sa_id_t*, identifies IKE_SA which gets rekeyed */
	JITIKE_IKE_REKEY_ID,
	/** identification_t*, local identity */
	JITIKE_LOCAL_ID,
	/** identification_t*, remote identity */
	JITIKE_REMOTE_ID,
	/** identification_t*, remote EAP identity */
	JITIKE_REMOTE_EAP_ID,
	/** host_t*, local address */
	JITIKE_LOCAL_ADDR,
	/** host_t*, remote address */
	JITIKE_REMOTE_ADDR,
	/** char*, name of configuration */
	JITIKE_CONFIG_NAME,
	/** uint32_t, bitset of ike_condition_t */
	JITIKE_CONDITIONS,
	/** uint32_t, bitset of ike_extension_t */
	JITIKE_EXTENSIONS,
	/** host_t*, local virtual IP */
	JITIKE_LOCAL_VIP,
	/** host_t*, remote virtual IP */
	JITIKE_REMOTE_VIP,
	/** host_t*, known peer addresses (used for MOBIKE) */
	JITIKE_PEER_ADDR,
	/** uint8_t, initiator of an exchange, TRUE for local */
	JITIKE_INITIATOR,
	/** chunk_t, initiators nonce */
	JITIKE_NONCE_I,
	/** chunk_t, responders nonce */
	JITIKE_NONCE_R,
	/** chunk_t, diffie hellman shared secret */
	JITIKE_SECRET,
	/** chunk_t, SKd of old SA if rekeying */
	JITIKE_OLD_SKD,
	/** uint16_t, pseudo random function */
	JITIKE_ALG_PRF,
	/** uint16_t, old pseudo random function if rekeying */
	JITIKE_ALG_OLD_PRF,
	/** uint16_t, encryption algorithm */
	JITIKE_ALG_ENCR,
	/** uint16_t, encryption key size in bytes */
	JITIKE_ALG_ENCR_LEN,
	/** uint16_t, integrity protection algorithm */
	JITIKE_ALG_INTEG,
	/** uint16_t, DH group */
	JITIKE_ALG_DH,
	/** uint8_t, IPsec mode, TUNNEL|TRANSPORT|... */
	JITIKE_IPSEC_MODE,
	/** uint8_t, IPComp protocol */
	JITIKE_IPCOMP,
	/** uint32_t, inbound security parameter index */
	JITIKE_INBOUND_SPI,
	/** uint32_t, outbound security parameter index */
	JITIKE_OUTBOUND_SPI,
	/** uint16_t, inbound security parameter index */
	JITIKE_INBOUND_CPI,
	/** uint16_t, outbound security parameter index */
	JITIKE_OUTBOUND_CPI,
	/** traffic_selector_t*, local traffic selector */
	JITIKE_LOCAL_TS,
	/** traffic_selector_t*, remote traffic selector */
	JITIKE_REMOTE_TS,
	/** uint32_t, message ID */
	JITIKE_MID,
	/** uint16_t, HA segment */
	JITIKE_SEGMENT,
	/** uint16_t, Extended Sequence numbers */
	JITIKE_ESN,
	/** uint8_t, IKE version */
	JITIKE_IKE_VERSION,
	/** chunk_t, own DH public value */
	JITIKE_LOCAL_DH,
	/** chunk_t, remote DH public value */
	JITIKE_REMOTE_DH,
	/** chunk_t, shared secret for IKEv1 key derivation */
	JITIKE_PSK,
	/** chunk_t, IV for next IKEv1 message */
	JITIKE_IV,
	/** uint16_t, auth_method_t for IKEv1 key derivation */
	JITIKE_AUTH_METHOD,
};

/**
 * Union to enumerate typed attributes in a message
 */
union jitike_message_value_t {
	uint8_t u8;
	uint16_t u16;
	uint32_t u32;
	char *str;
	chunk_t chunk;
	ike_sa_id_t *ike_sa_id;
	identification_t *id;
	host_t *host;
	traffic_selector_t *ts;
};

/**
 * Abstracted message passed between nodes in a HA cluster.
 */
struct jitike_message_t {

	/**
	 * Get the type of the message.
	 *
	 * @return		message type
	 */
	jitike_message_type_t (*get_type)(jitike_message_t *this);

	/**
	 * Add an attribute to a message.
	 *
	 * @param attribute		attribute type to add
	 * @param ...			attribute specific data
	 */
	void (*add_attribute)(jitike_message_t *this,
						  jitike_message_attribute_t attribute, ...);

	/**
	 * Create an enumerator over all attributes in a message.
	 *
	 * @return				enumerator over attribute, jitike_message_value_t
	 */
	enumerator_t* (*create_attribute_enumerator)(jitike_message_t *this);

	/**
	 * Get the message in a encoded form.
	 *
	 * @return				chunk pointing to internal data
	 */
	chunk_t (*get_encoding)(jitike_message_t *this);

	/**
	 * Destroy a jitike_message_t.
	 */
	void (*destroy)(jitike_message_t *this);
};

/**
 * Create a new jitike_message instance, ready for adding attributes
 *
 * @param type				type of the message
 */
jitike_message_t *jitike_message_create(jitike_message_type_t type);

/**
 * Create a jitike_message from encoded data.
 *
 * @param data				encoded message data
 */
jitike_message_t *jitike_message_parse(chunk_t data);

#endif /** JITIKE_MESSAGE_ @}*/
