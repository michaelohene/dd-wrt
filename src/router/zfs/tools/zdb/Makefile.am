include $(top_srcdir)/config/Rules.am

AM_CPPFLAGS += -DDEBUG

DEFAULT_INCLUDES += \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/lib/libspl/include

sbin_PROGRAMS = zdb

zdb_SOURCES = \
	zdb.c \
	zdb_il.c

zdb_LDADD = \
	$(top_builddir)/lib/libnvpair/libnvpair.la \
	$(top_builddir)/lib/libuutil/libuutil.la \
	$(top_builddir)/lib/libzpool/libzpool.la \
	$(top_builddir)/lib/libzfs/libzfs.la \
	$(top_builddir)/lib/libzfs_core/libzfs_core.la -ltirpc

zdb_LDFLAGS = -rpath $(TOP)/util-linux/.libs
