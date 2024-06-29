MDSRCS+= \
	amd64_archlevel.c

.if ${MK_ASAN} != "no"
# Disable ASAN for amd64_archlevel.c since its code is executed before the
# sanitizer runtime can initialize itself.
CFLAGS.amd64_archlevel.c+=	-fno-sanitize=address
.endif
