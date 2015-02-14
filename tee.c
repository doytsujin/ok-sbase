/* See LICENSE file for copyright and license details. */
#include <signal.h>
#include <stdio.h>

#include "util.h"

static void
usage(void)
{
	eprintf("usage: %s [-ai] [file...]\n", argv0);
}

int
main(int argc, char *argv[])
{
	int aflag = 0, iflag = 0;
	char buf[BUFSIZ];
	int i, nfps;
	size_t n;
	FILE **fps = NULL;

	ARGBEGIN {
	case 'a':
		aflag = 1;
		break;
	case 'i':
		iflag = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (iflag && signal(SIGINT, SIG_IGN) == SIG_ERR)
		eprintf("signal:");
	nfps = argc + 1;
	fps = ecalloc(nfps, sizeof *fps);

	for (i = 0; argc > 0; argc--, argv++, i++)
		if (!(fps[i] = fopen(*argv, aflag ? "a" : "w")))
			eprintf("fopen %s:", *argv);
	fps[i] = stdout;

	while ((n = fread(buf, 1, sizeof buf, stdin)) > 0) {
		for (i = 0; i < nfps; i++) {
			if (fwrite(buf, 1, n, fps[i]) != n)
				eprintf("%s: write error:", buf);
		}
	}
	if (ferror(stdin))
		eprintf("<stdin>: read error:");

	return 0;
}
