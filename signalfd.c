#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#ifndef ROUNDS
#define ROUNDS 1000000
#endif

int main(void)
{
	int i, epoll_fd, sfd;
	struct epoll_event epoll_ev;
	struct signalfd_siginfo tmp;
	sigset_t ss;
	if (sigemptyset(&ss) < 0)
		abort();
	if (sigaddset(&ss, SIGUSR1) < 0)
		abort();
	sfd = signalfd(-1, &ss, 0);
	if (sfd < 0)
		abort();
	if (sigprocmask(SIG_BLOCK, &ss, 0) < 0)
		abort();
	epoll_fd = epoll_create1(0);
	if (epoll_fd < 0)
		abort();
	memset(&epoll_ev, 0, sizeof(struct epoll_event));
	epoll_ev.events = EPOLLIN;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sfd, &epoll_ev) < 0)
		abort();
	write(STDOUT_FILENO, "START\n", 6);
	for (i=0; i < ROUNDS; ++i) {
		if (raise(SIGUSR1))
			abort();
		if (epoll_wait(epoll_fd, &epoll_ev, 1, -1) < 0)
			abort();
		if (read(sfd, &tmp, sizeof(tmp)) != sizeof(tmp))
			abort();
	}
	write(STDOUT_FILENO, "OK\n", 3);
	return 0;
}
