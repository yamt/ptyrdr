
/*-
 * Copyright (c)2005 YAMAMOTO Takashi,
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

/*
 * open a pty and dup it to stdin and stdout.
 */

/*
 * this can be useful when creating a vpn with ssh and pppd.
 * 
 * an example:
 * 	# pppd $(ptyrdr ssh peer_host pppd noauth) 10.0.0.1:10.0.0.2 noauth
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#if !defined(USE_POSIX_OPENPT)
#include <limits.h>
#include <util.h>
#endif

#define	USE_DAEMON

int
main(int argc, char *argv[])
{
	extern char **environ;
	int fd;
	const char *slavename;
#if !defined(USE_DAEMON)
	pid_t pid;
#endif /* !defined(USE_DAEMON) */
#if !defined(USE_POSIX_OPENPT)
	int slave;
	char slavename0[PATH_MAX+1];
#endif /* !defined(USE_POSIX_OPENPT) */

	if (argc < 2)
		exit(2);

	/*
	 * open a pseudo-tty.
	 */
#if defined(USE_POSIX_OPENPT)
	fd = posix_openpt(O_RDWR);
	if (fd == -1) {
		perror("openpt");
		exit(1);
	}
	if (grantpt(fd)) {
		perror("grantpt");
		exit(1);
	}
	slavename = ptsname(fd);
	if (slavename == NULL) {
		perror("ptsname");
		exit(1);
	}
#else /* !defined(USE_POSIX_OPENPT) */
	if (openpty(&fd, &slave, slavename0, NULL, NULL)) {
		perror("openpty");
		exit(1);
	}
	close(slave);
	slavename = slavename0;
#endif /* !defined(USE_POSIX_OPENPT) */

	/*
	 * print the name of the slave.
	 */
	if (printf("%s", slavename) < 0 || fflush(stdout) != 0) {
		perror("printf");
		exit(1);
	}

	/*
	 * dup the master fd to stdout and stdin.
	 *
	 * XXX what to do for stderr?
	 */
	if (dup2(fd, STDOUT_FILENO) == -1 || dup2(fd, STDIN_FILENO) == -1) {
		perror("dup2");
		exit(1);
	}
	if (fd != STDOUT_FILENO && fd != STDIN_FILENO) {
		close(fd);
	}

	/*
	 * daemonize and exec the specified program.
	 */
#if defined(USE_DAEMON)
	if (daemon(0, 1)) {
		perror("daemon");
		exit(1);
	}
	execve(argv[1], &argv[1], environ);
	perror("execve");
	exit(1);
#else /* defined(USE_DAEMON) */
	pid = fork();
	if (pid == (pid_t)-1) {
		perror("fork");
		exit(1);
	}
	if (pid == 0) {
		execve(argv[1], &argv[1], environ);
		perror("execve");
		_Exit(1);
	}
	exit(0);
	/* NOTREACHED */
#endif /* defined(USE_DAEMON) */
}
