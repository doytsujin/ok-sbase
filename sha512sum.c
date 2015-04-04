/* See LICENSE file for copyright and license details. */
#include <stdint.h>
#include <stdio.h>

#include "crypt.h"
#include "sha512.h"
#include "util.h"

static struct sha512 s;
struct crypt_ops sha512_ops = {
	sha512_init,
	sha512_update,
	sha512_sum,
	&s,
};

static void
usage(void)
{
	eprintf("usage: %s [-c] [file ...]\n", argv0);
}

int
main(int argc, char *argv[])
{
	int (*cryptfunc)(int, char **, struct crypt_ops *, uint8_t *, size_t) = cryptmain;
	uint8_t md[SHA512_DIGEST_LENGTH];

	ARGBEGIN {
	case 'c':
		cryptfunc = cryptcheck;
		break;
	default:
		usage();
	} ARGEND;

	return cryptfunc(argc, argv, &sha512_ops, md, sizeof(md)) ||
	       !!(fshut(stdin, "<stdin>") + fshut(stdout, "<stdout>"));
}
