
PACKAGE=	clibs
SHLIB_NAME=	libphony_gcc_s.so.1
SHLIBDIR?=	/lib

.include <bsd.opts.mk>

MK_SSP=		no
WARNS?=	2

LDFLAGS+=	-nodefaultlibs
LIBADD+=	c

SRCS+=		dummy.c

.include <bsd.lib.mk>
