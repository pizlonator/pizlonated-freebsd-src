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

#ifndef _FORKEXECHELPER_H_
#define _FORKEXECHELPER_H_

struct forkexechelper;
typedef struct forkexechelper forkexechelper;

/* FilBSD doesn't support vfork(), which makes it a bit tricky to reliably get the errno from
   the exec side of a fork/exec.

   You can use this helper as follows:

       forkexechelper* helper = forkexechelper_create()
       switch (fork()) {
       case -1:
           forkexechelper_finish(heler);
           ... handle error ...
           break;
       case 0:
           forkexechelper_start_child(helper);
           execve(... whatever ...);
           forkexechelper_set_errno_in_child(helper);
           exit(whatever);
       default:
           if (forkexechelper_finish(helper) < 0)
               fprintf(strerr, "Error in exec: %s\n", strerror(errno));
           ... call waitpid or whatever ...
           break;
       }

   This works because forkexechelper_set_errno_in_child stashes the errno somewhere that
   forkexechelper_finish can retrieve it.

   If you forget to call forkexechelper_start_child in the child, then forkexechelper_finish
   will block forver. */

/* Create a forkexechelper. Do this before forking. Never fails (crashes if it would
   have failed). */
forkexechelper* forkexechelper_create(void);

/* Tell forkexechelper that we're in the child. Do this right after forking, in the child. */
void forkexechelper_start_child(forkexechelper* helper);

/* Tell forkexechelper that an errno happened in the child. You should probably exit after this. */
void forkexechelper_set_errno_in_child(forkexechelper* helper);

/* Ask forkexechelper to wait for the exec to happen or for an error to be sent. This returns as
   soon as the child execs, or whenever the child calls forkexechelper_set_errno_in_child, or
   immediately if the fork never happened.

   No matter what, this preserves the value of the errno variable.

   If the exec happened successfully, this returns zero.

   If forkexechelper_set_errno_in_child was called, this returns what the errno was when
   forkexechelper_set_errno_in_child was called in the child.

   If the fork never happened, then this returns zero. */
int forkexechelper_finish(forkexechelper* helper);

#endif /* _FORKEXECHELPER_H_ */

