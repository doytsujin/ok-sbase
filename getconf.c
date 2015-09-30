/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

struct var {
	const char *k;
	long v;
};

static const struct var pathconf_l[] = {
	{ "FILESIZEBITS",                _PC_FILESIZEBITS         },
	{ "LINK_MAX",                    _PC_LINK_MAX             },
	{ "MAX_CANON",                   _PC_MAX_CANON            },
	{ "MAX_INPUT",                   _PC_MAX_INPUT            },
	{ "NAME_MAX",                    _PC_NAME_MAX             },
	{ "PATH_MAX",                    _PC_PATH_MAX             },
	{ "PIPE_BUF",                    _PC_PIPE_BUF             },
	{ "POSIX2_SYMLINKS",             _PC_2_SYMLINKS           },
	{ "POSIX_ALLOC_SIZE_MIN",        _PC_ALLOC_SIZE_MIN       },
	{ "POSIX_REC_INCR_XFER_SIZE",    _PC_REC_INCR_XFER_SIZE   },
	{ "POSIX_REC_MAX_XFER_SIZE",     _PC_REC_MAX_XFER_SIZE    },
	{ "POSIX_REC_MIN_XFER_SIZE",     _PC_REC_MIN_XFER_SIZE    },
	{ "POSIX_REC_XFER_ALIGN",        _PC_REC_XFER_ALIGN       },
	{ "SYMLINK_MAX",                 _PC_SYMLINK_MAX          },
	{ "_POSIX_CHOWN_RESTRICTED",     _PC_CHOWN_RESTRICTED     },
	{ "_POSIX_NO_TRUNC",             _PC_NO_TRUNC             },
	{ "_POSIX_VDISABLE",             _PC_VDISABLE             },
	{ "_POSIX_ASYNC_IO",             _PC_ASYNC_IO             },
	{ "_POSIX_PRIO_IO",              _PC_PRIO_IO              },
	{ "_POSIX_SYNC_IO",              _PC_SYNC_IO              },
	/*{ "_POSIX_TIMESTAMP_RESOLUTION", _PC_TIMESTAMP_RESOLUTION },*/
};

static const struct var sysconf_l[] = {
	{ "AIO_LISTIO_MAX",                    _SC_AIO_LISTIO_MAX               },
	{ "AIO_MAX",                           _SC_AIO_MAX                      },
	{ "AIO_PRIO_DELTA_MAX",                _SC_AIO_PRIO_DELTA_MAX           },
	{ "ARG_MAX",                           _SC_ARG_MAX                      },
	{ "ATEXIT_MAX",                        _SC_ATEXIT_MAX                   },
	{ "BC_BASE_MAX",                       _SC_BC_BASE_MAX                  },
	{ "BC_DIM_MAX",                        _SC_BC_DIM_MAX                   },
	{ "BC_SCALE_MAX",                      _SC_BC_SCALE_MAX                 },
	{ "BC_STRING_MAX",                     _SC_BC_STRING_MAX                },
	{ "CHILD_MAX",                         _SC_CHILD_MAX                    },
	{ "COLL_WEIGHTS_MAX",                  _SC_COLL_WEIGHTS_MAX             },
	{ "DELAYTIMER_MAX",                    _SC_DELAYTIMER_MAX               },
	{ "EXPR_NEST_MAX",                     _SC_EXPR_NEST_MAX                },
	{ "HOST_NAME_MAX",                     _SC_HOST_NAME_MAX                },
	{ "IOV_MAX",                           _SC_IOV_MAX                      },
	{ "LINE_MAX",                          _SC_LINE_MAX                     },
	{ "LOGIN_NAME_MAX",                    _SC_LOGIN_NAME_MAX               },
	{ "NGROUPS_MAX",                       _SC_NGROUPS_MAX                  },
	{ "MQ_OPEN_MAX",                       _SC_MQ_OPEN_MAX                  },
	{ "MQ_PRIO_MAX",                       _SC_MQ_PRIO_MAX                  },
	{ "OPEN_MAX",                          _SC_OPEN_MAX                     },
	{ "_POSIX_ADVISORY_INFO",              _SC_ADVISORY_INFO                },
	{ "_POSIX_BARRIERS",                   _SC_BARRIERS                     },
	{ "_POSIX_ASYNCHRONOUS_IO",            _SC_ASYNCHRONOUS_IO              },
	{ "_POSIX_CLOCK_SELECTION",            _SC_CLOCK_SELECTION              },
	{ "_POSIX_CPUTIME",                    _SC_CPUTIME                      },
	{ "_POSIX_FSYNC",                      _SC_FSYNC                        },
	{ "_POSIX_IPV6",                       _SC_IPV6                         },
	{ "_POSIX_JOB_CONTROL",                _SC_JOB_CONTROL                  },
	{ "_POSIX_MAPPED_FILES",               _SC_MAPPED_FILES                 },
	{ "_POSIX_MEMLOCK",                    _SC_MEMLOCK                      },
	{ "_POSIX_MEMLOCK_RANGE",              _SC_MEMLOCK_RANGE                },
	{ "_POSIX_MEMORY_PROTECTION",          _SC_MEMORY_PROTECTION            },
	{ "_POSIX_MESSAGE_PASSING",            _SC_MESSAGE_PASSING              },
	{ "_POSIX_MONOTONIC_CLOCK",            _SC_MONOTONIC_CLOCK              },
	{ "_POSIX_PRIORITIZED_IO",             _SC_PRIORITIZED_IO               },
	{ "_POSIX_PRIORITY_SCHEDULING",        _SC_PRIORITY_SCHEDULING          },
	{ "_POSIX_RAW_SOCKETS",                _SC_RAW_SOCKETS                  },
	{ "_POSIX_READER_WRITER_LOCKS",        _SC_READER_WRITER_LOCKS          },
	{ "_POSIX_REALTIME_SIGNALS",           _SC_REALTIME_SIGNALS             },
	{ "_POSIX_REGEXP",                     _SC_REGEXP                       },
	{ "_POSIX_SAVED_IDS",                  _SC_SAVED_IDS                    },
	{ "_POSIX_SEMAPHORES",                 _SC_SEMAPHORES                   },
	{ "_POSIX_SHARED_MEMORY_OBJECTS",      _SC_SHARED_MEMORY_OBJECTS        },
	{ "_POSIX_SHELL",                      _SC_SHELL                        },
	{ "_POSIX_SPAWN",                      _SC_SPAWN                        },
	{ "_POSIX_SPIN_LOCKS",                 _SC_SPIN_LOCKS                   },
	{ "_POSIX_SPORADIC_SERVER",            _SC_SPORADIC_SERVER              },
	{ "_POSIX_SS_REPL_MAX",                _SC_SS_REPL_MAX                  },
	{ "_POSIX_SYNCHRONIZED_IO",            _SC_SYNCHRONIZED_IO              },
	{ "_POSIX_THREAD_ATTR_STACKADDR",      _SC_THREAD_ATTR_STACKADDR        },
	{ "_POSIX_THREAD_ATTR_STACKSIZE",      _SC_THREAD_ATTR_STACKSIZE        },
	{ "_POSIX_THREAD_CPUTIME",             _SC_THREAD_CPUTIME               },
	{ "_POSIX_THREAD_PRIO_INHERIT",        _SC_THREAD_PRIO_INHERIT          },
	{ "_POSIX_THREAD_PRIO_PROTECT",        _SC_THREAD_PRIO_PROTECT          },
	{ "_POSIX_THREAD_PRIORITY_SCHEDULING", _SC_THREAD_PRIORITY_SCHEDULING   },
	{ "_POSIX_THREAD_PROCESS_SHARED",      _SC_THREAD_PROCESS_SHARED        },
	{ "_POSIX_THREAD_ROBUST_PRIO_INHERIT", _SC_THREAD_ROBUST_PRIO_INHERIT   },
	{ "_POSIX_THREAD_ROBUST_PRIO_PROTECT", _SC_THREAD_ROBUST_PRIO_PROTECT   },
	{ "_POSIX_THREAD_SAFE_FUNCTIONS",      _SC_THREAD_SAFE_FUNCTIONS        },
	{ "_POSIX_THREAD_SPORADIC_SERVER",     _SC_THREAD_SPORADIC_SERVER       },
	{ "_POSIX_THREADS",                    _SC_THREADS                      },
	{ "_POSIX_TIMEOUTS",                   _SC_TIMEOUTS                     },
	{ "_POSIX_TIMERS",                     _SC_TIMERS                       },
	{ "_POSIX_TRACE",                      _SC_TRACE                        },
	{ "_POSIX_TRACE_EVENT_FILTER",         _SC_TRACE_EVENT_FILTER           },
	{ "_POSIX_TRACE_EVENT_NAME_MAX",       _SC_TRACE_EVENT_NAME_MAX         },
	{ "_POSIX_TRACE_INHERIT",              _SC_TRACE_INHERIT                },
	{ "_POSIX_TRACE_LOG",                  _SC_TRACE_LOG                    },
	{ "_POSIX_TRACE_NAME_MAX",             _SC_TRACE_NAME_MAX               },
	{ "_POSIX_TRACE_SYS_MAX",              _SC_TRACE_SYS_MAX                },
	{ "_POSIX_TRACE_USER_EVENT_MAX",       _SC_TRACE_USER_EVENT_MAX         },
	{ "_POSIX_TYPED_MEMORY_OBJECTS",       _SC_TYPED_MEMORY_OBJECTS         },
	{ "_POSIX_VERSION",                    _SC_VERSION                      },
	{ "_POSIX_V7_ILP32_OFF32",             _SC_V7_ILP32_OFF32               },
	{ "_POSIX_V7_ILP32_OFFBIG",            _SC_V7_ILP32_OFFBIG              },
	{ "_POSIX_V7_LP64_OFF64",              _SC_V7_LP64_OFF64                },
	{ "_POSIX_V7_LPBIG_OFFBIG",            _SC_V7_LPBIG_OFFBIG              },
	{ "_POSIX2_C_BIND",                    _SC_2_C_BIND                     },
	{ "_POSIX2_C_DEV",                     _SC_2_C_DEV                      },
	{ "_POSIX2_CHAR_TERM",                 _SC_2_CHAR_TERM                  },
	{ "_POSIX2_FORT_DEV",                  _SC_2_FORT_DEV                   },
	{ "_POSIX2_FORT_RUN",                  _SC_2_FORT_RUN                   },
	{ "_POSIX2_LOCALEDEF",                 _SC_2_LOCALEDEF                  },
	{ "_POSIX2_PBS",                       _SC_2_PBS                        },
	{ "_POSIX2_PBS_ACCOUNTING",            _SC_2_PBS_ACCOUNTING             },
	{ "_POSIX2_PBS_CHECKPOINT",            _SC_2_PBS_CHECKPOINT             },
	{ "_POSIX2_PBS_LOCATE",                _SC_2_PBS_LOCATE                 },
	{ "_POSIX2_PBS_MESSAGE",               _SC_2_PBS_MESSAGE                },
	{ "_POSIX2_PBS_TRACK",                 _SC_2_PBS_TRACK                  },
	{ "_POSIX2_SW_DEV",                    _SC_2_SW_DEV                     },
	{ "_POSIX2_UPE",                       _SC_2_UPE                        },
	{ "_POSIX2_VERSION",                   _SC_2_VERSION                    },
	{ "PAGE_SIZE",                         _SC_PAGE_SIZE                    },
	{ "PAGESIZE",                          _SC_PAGESIZE                     },
	{ "PTHREAD_DESTRUCTOR_ITERATIONS",     _SC_THREAD_DESTRUCTOR_ITERATIONS },
	{ "PTHREAD_KEYS_MAX",                  _SC_THREAD_KEYS_MAX              },
	{ "PTHREAD_STACK_MIN",                 _SC_THREAD_STACK_MIN             },
	{ "PTHREAD_THREADS_MAX",               _SC_THREAD_THREADS_MAX           },
	{ "RE_DUP_MAX",                        _SC_RE_DUP_MAX                   },
	{ "RTSIG_MAX",                         _SC_RTSIG_MAX                    },
	{ "SEM_NSEMS_MAX",                     _SC_SEM_NSEMS_MAX                },
	{ "SEM_VALUE_MAX",                     _SC_SEM_VALUE_MAX                },
	{ "SIGQUEUE_MAX",                      _SC_SIGQUEUE_MAX                 },
	{ "STREAM_MAX",                        _SC_STREAM_MAX                   },
	{ "SYMLOOP_MAX",                       _SC_SYMLOOP_MAX                  },
	{ "TIMER_MAX",                         _SC_TIMER_MAX                    },
	{ "TTY_NAME_MAX",                      _SC_TTY_NAME_MAX                 },
	{ "TZNAME_MAX",                        _SC_TZNAME_MAX                   },
	{ "_XOPEN_CRYPT",                      _SC_XOPEN_CRYPT                  },
	{ "_XOPEN_ENH_I18N",                   _SC_XOPEN_ENH_I18N               },
	{ "_XOPEN_REALTIME",                   _SC_XOPEN_REALTIME               },
	{ "_XOPEN_REALTIME_THREADS",           _SC_XOPEN_REALTIME_THREADS       },
	{ "_XOPEN_SHM",                        _SC_XOPEN_SHM                    },
	{ "_XOPEN_STREAMS",                    _SC_XOPEN_STREAMS                },
	{ "_XOPEN_UNIX",                       _SC_XOPEN_UNIX                   },
	/*{ "_XOPEN_UUCP",                       _SC_XOPEN_UUCP                   },*/
	{ "_XOPEN_VERSION",                    _SC_XOPEN_VERSION                },
};

static const struct var confstr_l[] = {
	{ "PATH",                           _CS_PATH},
	{ "POSIX_V7_ILP32_OFF32_CFLAGS",    _CS_POSIX_V7_ILP32_OFF32_CFLAGS    },
	{ "POSIX_V7_ILP32_OFF32_LDFLAGS",   _CS_POSIX_V7_ILP32_OFF32_LDFLAGS   },
	{ "POSIX_V7_ILP32_OFF32_LIBS",      _CS_POSIX_V7_ILP32_OFF32_LIBS      },
	{ "POSIX_V7_ILP32_OFFBIG_CFLAGS",   _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS   },
	{ "POSIX_V7_ILP32_OFFBIG_LDFLAGS",  _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS  },
	{ "POSIX_V7_ILP32_OFFBIG_LIBS",     _CS_POSIX_V7_ILP32_OFFBIG_LIBS     },
	{ "POSIX_V7_LP64_OFF64_CFLAGS",     _CS_POSIX_V7_LP64_OFF64_CFLAGS     },
	{ "POSIX_V7_LP64_OFF64_LDFLAGS",    _CS_POSIX_V7_LP64_OFF64_LDFLAGS    },
	{ "POSIX_V7_LP64_OFF64_LIBS",       _CS_POSIX_V7_LP64_OFF64_LIBS       },
	{ "POSIX_V7_LPBIG_OFFBIG_CFLAGS",   _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS   },
	{ "POSIX_V7_LPBIG_OFFBIG_LDFLAGS",  _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS  },
	{ "POSIX_V7_LPBIG_OFFBIG_LIBS",     _CS_POSIX_V7_LPBIG_OFFBIG_LIBS     },
	/*{ "POSIX_V7_THREADS_CFLAGS",        _CS_POSIX_V7_THREADS_CFLAGS        },*/
	/*{ "POSIX_V7_THREADS_LDFLAGS",       _CS_POSIX_V7_THREADS_LDFLAGS       },*/
	{ "POSIX_V7_WIDTH_RESTRICTED_ENVS", _CS_POSIX_V7_WIDTH_RESTRICTED_ENVS },
	/*{ "V7_ENV",                         _CS_V7_ENV                         },*/
};

static const struct var limits_l[] = {
	{ "_POSIX_CLOCKRES_MIN",                 _POSIX_CLOCKRES_MIN                 },
	{ "_POSIX_AIO_LISTIO_MAX",               _POSIX_AIO_LISTIO_MAX               },
	{ "_POSIX_AIO_MAX",                      _POSIX_AIO_MAX                      },
	{ "_POSIX_ARG_MAX",                      _POSIX_ARG_MAX                      },
	{ "_POSIX_CHILD_MAX",                    _POSIX_CHILD_MAX                    },
	{ "_POSIX_DELAYTIMER_MAX",               _POSIX_DELAYTIMER_MAX               },
	{ "_POSIX_HOST_NAME_MAX",                _POSIX_HOST_NAME_MAX                },
	{ "_POSIX_LINK_MAX",                     _POSIX_LINK_MAX                     },
	{ "_POSIX_LOGIN_NAME_MAX",               _POSIX_LOGIN_NAME_MAX               },
	{ "_POSIX_MAX_CANON",                    _POSIX_MAX_CANON                    },
	{ "_POSIX_MAX_INPUT",                    _POSIX_MAX_INPUT                    },
	{ "_POSIX_MQ_OPEN_MAX",                  _POSIX_MQ_OPEN_MAX                  },
	{ "_POSIX_MQ_PRIO_MAX",                  _POSIX_MQ_PRIO_MAX                  },
	{ "_POSIX_NAME_MAX",                     _POSIX_NAME_MAX                     },
	{ "_POSIX_NGROUPS_MAX",                  _POSIX_NGROUPS_MAX                  },
	{ "_POSIX_OPEN_MAX",                     _POSIX_OPEN_MAX                     },
	{ "_POSIX_PATH_MAX",                     _POSIX_PATH_MAX                     },
	{ "_POSIX_PIPE_BUF",                     _POSIX_PIPE_BUF                     },
	{ "_POSIX_RE_DUP_MAX",                   _POSIX_RE_DUP_MAX                   },
	{ "_POSIX_RTSIG_MAX",                    _POSIX_RTSIG_MAX                    },
	{ "_POSIX_SEM_NSEMS_MAX",                _POSIX_SEM_NSEMS_MAX                },
	{ "_POSIX_SEM_VALUE_MAX",                _POSIX_SEM_VALUE_MAX                },
	{ "_POSIX_SIGQUEUE_MAX",                 _POSIX_SIGQUEUE_MAX                 },
	{ "_POSIX_SSIZE_MAX",                    _POSIX_SSIZE_MAX                    },
	/*{ "_POSIX_SS_REPL_MAX",                  _POSIX_SS_REPL_MAX                  },*/
	{ "_POSIX_STREAM_MAX",                   _POSIX_STREAM_MAX                   },
	{ "_POSIX_SYMLINK_MAX",                  _POSIX_SYMLINK_MAX                  },
	{ "_POSIX_SYMLOOP_MAX",                  _POSIX_SYMLOOP_MAX                  },
	{ "_POSIX_THREAD_DESTRUCTOR_ITERATIONS", _POSIX_THREAD_DESTRUCTOR_ITERATIONS },
	{ "_POSIX_THREAD_KEYS_MAX",              _POSIX_THREAD_KEYS_MAX              },
	{ "_POSIX_THREAD_THREADS_MAX",           _POSIX_THREAD_THREADS_MAX           },
	{ "_POSIX_TIMER_MAX",                    _POSIX_TIMER_MAX                    },
	{ "_POSIX_TTY_NAME_MAX",                 _POSIX_TTY_NAME_MAX                 },
	{ "_POSIX_TZNAME_MAX",                   _POSIX_TZNAME_MAX                   },
	{ "_POSIX2_BC_BASE_MAX",                 _POSIX2_BC_BASE_MAX                 },
	{ "_POSIX2_BC_DIM_MAX",                  _POSIX2_BC_DIM_MAX                  },
	{ "_POSIX2_BC_SCALE_MAX",                _POSIX2_BC_SCALE_MAX                },
	{ "_POSIX2_BC_STRING_MAX",               _POSIX2_BC_STRING_MAX               },
	{ "_POSIX2_CHARCLASS_NAME_MAX",          _POSIX2_CHARCLASS_NAME_MAX          },
	{ "_POSIX2_COLL_WEIGHTS_MAX",            _POSIX2_COLL_WEIGHTS_MAX            },
	{ "_POSIX2_EXPR_NEST_MAX",               _POSIX2_EXPR_NEST_MAX               },
	{ "_POSIX2_LINE_MAX",                    _POSIX2_LINE_MAX                    },
	{ "_POSIX2_RE_DUP_MAX",                  _POSIX2_RE_DUP_MAX                  },
};

void
usage(void)
{
	eprintf("%s -v spec arc arco\n", argv0);
}

int
main(int argc, char *argv[])
{
	size_t len;
	long res;
	int i;
	char *str;

	ARGBEGIN {
	case 'v':
		/* ignore */
		(void)(EARGF(usage()));
		break;
	} ARGEND;

	if (argc == 1) {
		/* sysconf */
		for (i = 0; i < LEN(sysconf_l); i++) {
			if (strcmp(argv[0], sysconf_l[i].k))
				continue;
			errno = 0;
			if ((res = sysconf(sysconf_l[i].v)) < 0) {
				if (errno)
					eprintf("sysconf %ld:", sysconf_l[i].v);
				puts("undefined");
			} else {
				printf("%ld\n", res);
			}
			return 0;
		}
		/* confstr */
		for (i = 0; i < LEN(confstr_l); i++) {
			if (strcmp(argv[0], confstr_l[i].k))
				continue;
			errno = 0;
			if (!(len = confstr(confstr_l[i].v, NULL, 0))) {
				if (errno)
					eprintf("confstr %ld:", confstr_l[i].v);
				puts("undefined");
			} else {
				str = emalloc(len);
				errno = 0;
				if (!confstr(confstr_l[i].v, str, len)) {
					if (errno)
						eprintf("confstr %ld:", confstr_l[i].v);
					puts("undefined");
				} else {
					puts(str);
				}
				free(str);
			}
			return 0;
		}
		/* limits */
		for (i = 0; i < LEN(limits_l); i++) {
			if (strcmp(argv[0], limits_l[i].k))
				continue;
			printf("%ld\n", limits_l[i].v);
			return 0;
		}
	} else if (argc == 2) {
		/* pathconf */
		for (i = 0; i < LEN(pathconf_l); i++) {
			if (strcmp(argv[0], pathconf_l[i].k))
				continue;
			errno = 0;
			if ((res = pathconf(argv[1], pathconf_l[i].v)) < 0) {
				if (errno)
					eprintf("pathconf %ld:", pathconf_l[i].v);
				puts("undefined");
			} else {
				printf("%ld\n", res);
			}
			return 0;
		}
	} else {
		usage();
	}

	puts("undefined");

	return 1;
}
