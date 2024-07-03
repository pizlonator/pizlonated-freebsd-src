/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Epic Games, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "namespace.h"
#include <errno.h>
#include <fcntl.h>
#include <forkexechelper.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "un-namespace.h"

#include <stdfil.h>

struct forkexechelper {
    int pipefds[2];
};

forkexechelper* forkexechelper_create(void)
{
    forkexechelper* result = malloc(sizeof(forkexechelper));
    ZASSERT(result);
    ZASSERT(!pipe(result->pipefds));
    ZASSERT(!_fcntl(result->pipefds[1], F_SETFD, FD_CLOEXEC));
    return result;
}

void forkexechelper_start_child(forkexechelper* helper)
{
    ZASSERT(!_close(helper->pipefds[0]));
}

void forkexechelper_set_errno_in_child(forkexechelper* helper)
{
    int my_errno = errno;
    for (;;) {
        int result = _write(helper->pipefds[1], &my_errno, sizeof(my_errno));
        if (result == sizeof(my_errno))
            return;
        ZASSERT(result == -1);
        ZASSERT(errno == EINTR);
    }
}

static void destroy(forkexechelper* helper, int saved_errno)
{
    ZASSERT(!_close(helper->pipefds[0]));
    free(helper);
    errno = saved_errno;
}

int forkexechelper_finish(forkexechelper* helper)
{
    int saved_errno = errno;
    ZASSERT(!_close(helper->pipefds[1]));

    for (;;) {
        int my_errno;
        int result = _read(helper->pipefds[0], &my_errno, sizeof(my_errno));
        if (result == sizeof(my_errno)) {
            destroy(helper, saved_errno);
            return my_errno;
        }
        if (!result) {
            destroy(helper, saved_errno);
            return 0;
        }
        ZASSERT(result == -1);
        ZASSERT(errno == EINTR);
    }
}
