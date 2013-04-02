#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#ifndef ROUNDS
#define ROUNDS 1000000
#endif

static int pipefd[2];

static void
sig_handler(int sig)
{
	if (write(pipefd[1], (void *)"a", 1) != 1)
		abort();
}

int main(void)
{
	int i, tmp, epoll_fd;
	struct epoll_event epoll_ev;
	struct sigaction sa;
	if (pipe(pipefd) < 0)
		abort();
	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = sig_handler;
	if (sigaction(SIGUSR1, &sa, NULL) < 0)
		abort();
	epoll_fd = epoll_create1(0);
	if (epoll_fd < 0)
		abort();
	memset(&epoll_ev, 0, sizeof(struct epoll_event));
	epoll_ev.events = EPOLLIN;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipefd[0], &epoll_ev) < 0)
		abort();
	write(STDOUT_FILENO, "START\n", 6);
	for (i=0; i < ROUNDS; ++i) {
		if (raise(SIGUSR1))
			abort();
		if (epoll_wait(epoll_fd, &epoll_ev, 1, -1) < 0)
			abort();
		if (read(pipefd[0], &tmp, 1) != 1)
			abort();
	}
	write(STDOUT_FILENO, "OK\n", 3);
	return 0;
}
