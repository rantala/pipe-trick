#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#ifndef ROUNDS
#define ROUNDS 1000000
#endif

static int efd;

static void
sig_handler(int sig)
{
	if (eventfd_write(efd, 1) < 0)
		abort();
}

int main(void)
{
	int i, epoll_fd;
	eventfd_t tmp;
	struct epoll_event epoll_ev;
	struct sigaction sa;
	efd = eventfd(0, 0);
	if (efd < 0)
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
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, efd, &epoll_ev) < 0)
		abort();
	write(STDOUT_FILENO, "START\n", 6);
	for (i=0; i < ROUNDS; ++i) {
		if (raise(SIGUSR1))
			abort();
		if (epoll_wait(epoll_fd, &epoll_ev, 1, -1) < 0)
			abort();
		if (read(efd, &tmp, sizeof(eventfd_t)) != sizeof(eventfd_t))
			abort();
	}
	write(STDOUT_FILENO, "OK\n", 3);
	return 0;
}
