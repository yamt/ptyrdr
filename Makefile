# $Id$

PROG=	ptyrdr
NOMAN=

WARNS?=	3

LDADD+=	-lutil
DPADD+= ${LIBUTIL}

CFLAGS+=	-DUSE_POSIX_OPENPT

.include <bsd.prog.mk>
