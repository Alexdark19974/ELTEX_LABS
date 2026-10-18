#include "nano.h"

int main(int argc, char **argv)
{
	struct sigaction act = {0};

	act.sa_sigaction = &s_handler;
	act.sa_flags = SA_SIGINFO;

	sigaction(SIGWINCH, &act, NULL);

	return nano_editor(argc, argv);
}
