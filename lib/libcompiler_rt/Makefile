
.include <src.opts.mk>

PACKAGE=	lib${LIB}
LIB=		phony_compiler_rt
MK_SSP=		no
NO_PIC=
WARNS?=		2

CFLAGS+=	${PICFLAG}
CFLAGS+=	-fvisibility=hidden

.include "Makefile.inc"

.include <bsd.lib.mk>
