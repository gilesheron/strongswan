/*
 * Copyright (C) 2015 Tobias Brunner
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

#include "jitike_ctl.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <threading/thread.h>
#include <processing/jobs/callback_job.h>

#define JITIKE_FIFO IPSEC_PIDDIR "/charon.jitike"

typedef struct private_jitike_ctl_t private_jitike_ctl_t;

/**
 * Private data of an jitike_ctl_t object.
 */
struct private_jitike_ctl_t {

	/**
	 * Public jitike_ctl_t interface.
	 */
	jitike_ctl_t public;

	/**
	 * Segments to control
	 */
	jitike_segments_t *segments;

	/**
	 * Resynchronization message cache
	 */
	jitike_cache_t *cache;
};

/**
 * Change the permissions of the control FIFO, returns TRUE on success
 */
static bool change_fifo_permissions()
{
	if (chown(JITIKE_FIFO, lib->caps->get_uid(lib->caps),
			  lib->caps->get_gid(lib->caps)) != 0)
	{
		DBG1(DBG_CFG, "changing HA FIFO permissions failed: %s",
			 strerror(errno));
		return FALSE;
	}
	return TRUE;
}

/**
 * Deletes and creates the control FIFO, returns TRUE on success
 */
static bool recreate_fifo()
{
	mode_t old;
	bool success = TRUE;

	unlink(JITIKE_FIFO);
	old = umask(S_IRWXO);
	if (mkfifo(JITIKE_FIFO, S_IRUSR | S_IWUSR) != 0)
	{
		DBG1(DBG_CFG, "creating HA FIFO %s failed: %s", JITIKE_FIFO,
			 strerror(errno));
		success = FALSE;
	}
	umask(old);
	return success && change_fifo_permissions();
}

/**
 * FIFO dispatching function
 */
static job_requeue_t dispatch_fifo(private_jitike_ctl_t *this)
{
	int fifo;
	bool oldstate;
	char buf[8];
	u_int segment;
	struct stat sb;

	oldstate = thread_cancelability(TRUE);
	fifo = open(JITIKE_FIFO, O_RDONLY);
	thread_cancelability(oldstate);
	if (fifo == -1 || fstat(fifo, &sb) != 0 || !S_ISFIFO(sb.st_mode))
	{
		if (fifo == -1 && errno != ENOENT)
		{
			DBG1(DBG_CFG, "opening HA FIFO failed: %s", strerror(errno));
		}
		else
		{
			DBG1(DBG_CFG, "%s is not a FIFO, recreate it", JITIKE_FIFO);
			recreate_fifo();
		}
		if (fifo != -1)
		{
			close(fifo);
		}
		sleep(1);
		return JOB_REQUEUE_FAIR;
	}

	memset(buf, 0, sizeof(buf));
	if (read(fifo, buf, sizeof(buf)-1) > 1)
	{
		segment = atoi(&buf[1]);
		if (segment)
		{
			switch (buf[0])
			{
				case '+':
					this->segments->activate(this->segments, segment, TRUE);
					break;
				case '-':
					this->segments->deactivate(this->segments, segment, TRUE);
					break;
				case '*':
					this->cache->resync(this->cache, segment);
					break;
				default:
					break;
			}
		}
	}
	close(fifo);

	return JOB_REQUEUE_DIRECT;
}

METHOD(jitike_ctl_t, destroy, void,
	private_jitike_ctl_t *this)
{
	unlink(JITIKE_FIFO);
	free(this);
}

/**
 * See header
 */
jitike_ctl_t *jitike_ctl_create(jitike_segments_t *segments, jitike_cache_t *cache)
{
	private_jitike_ctl_t *this;
	struct stat sb;

	INIT(this,
		.public = {
			.destroy = _destroy,
		},
		.segments = segments,
		.cache = cache,
	);

	if (stat(JITIKE_FIFO, &sb) == 0)
	{
		if (!S_ISFIFO(sb.st_mode))
		{
			DBG1(DBG_CFG, "%s is not a FIFO, recreate it", JITIKE_FIFO);
			recreate_fifo();
		}
		else if (access(JITIKE_FIFO, R_OK|W_OK) != 0)
		{
			DBG1(DBG_CFG, "accessing HA FIFO %s denied, recreate it", JITIKE_FIFO);
			recreate_fifo();
		}
		else
		{
			change_fifo_permissions();
		}
	}
	else if (errno == ENOENT)
	{
		recreate_fifo();
	}
	else
	{
		DBG1(DBG_CFG, "accessing HA FIFO %s failed: %s", JITIKE_FIFO,
			 strerror(errno));
	}

	lib->processor->queue_job(lib->processor,
		(job_t*)callback_job_create_with_prio((callback_job_cb_t)dispatch_fifo,
			this, NULL, (callback_job_cancel_t)return_false, JOB_PRIO_CRITICAL));
	return &this->public;
}
