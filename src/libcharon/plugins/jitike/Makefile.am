AM_CPPFLAGS = \
	-I$(top_srcdir)/src/libstrongswan \
	-I$(top_srcdir)/src/libcharon \
	-I$(top_srcdir)/src/libcharon/plugins/ha \
	-DIPSEC_PIDDIR=\"${piddir}\"

AM_CFLAGS = \
	$(PLUGIN_CFLAGS)

if MONOLITHIC
noinst_LTLIBRARIES = libstrongswan-jitike.la
else
plugin_LTLIBRARIES = libstrongswan-jitike.la
endif

libstrongswan_jitike_la_SOURCES = \
  jitike_plugin.h jitike_plugin.c \
  jitike_message.h jitike_message.c \
  jitike_socket.h jitike_socket.c \
  jitike_tunnel.h jitike_tunnel.c \
  jitike_dispatcher.h jitike_dispatcher.c \
  jitike_segments.h jitike_segments.c \
  jitike_cache.h jitike_cache.c \
  jitike_kernel.h jitike_kernel.c \
  jitike_ctl.h jitike_ctl.c \
  jitike_ike.h jitike_ike.c \
  jitike_redis.h jitike_redis.c \
  jitike_child.h jitike_child.c \
  jitike_attribute.h jitike_attribute.c
libstrongswan_jitike_la_LIBADD = -lhiredis
libstrongswan_jitike_la_LDFLAGS = -module -avoid-version
