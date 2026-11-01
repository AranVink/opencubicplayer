/* OpenCP Module Player
 * Copyright (c) 2026 Stian Skjelstad
 *
 * DJGPP static executable launcher.
 */

#include "config.h"
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "types.h"
#include "boot/pmain.h"

extern struct bootupstruct bootup;

int main(int argc, char *argv[])
{
	char cwd[PATH_MAX];
	char program_dir[PATH_MAX];
	const char *backsep;
	const char *sep;
	int absolute;
	size_t len;

	if (!getcwd(cwd, sizeof(cwd)))
	{
		perror("getcwd()");
		return EXIT_FAILURE;
	}

	sep = strrchr(argv[0], '/');
	backsep = strrchr(argv[0], '\\');
	if (!sep || (backsep && backsep > sep))
	{
		sep = backsep;
	}
	absolute = (argv[0][0] == '/' || argv[0][0] == '\\' ||
		(isalpha((unsigned char)argv[0][0]) && argv[0][1] == ':' &&
		 (argv[0][2] == '/' || argv[0][2] == '\\')));

	if (sep && absolute)
	{
		len = sep - argv[0] + 1;
		if (len >= sizeof(program_dir))
		{
			fprintf(stderr, "Executable path is too long\n");
			return EXIT_FAILURE;
		}
		memcpy(program_dir, argv[0], len);
		program_dir[len] = 0;
	} else if (sep) {
		size_t cwdlen = strlen(cwd);
		size_t pathlen = sep - argv[0] + 1;
		int needs_sep = cwdlen && cwd[cwdlen - 1] != '/' && cwd[cwdlen - 1] != '\\';
		if (cwdlen + needs_sep + pathlen + 1 > sizeof(program_dir))
		{
			fprintf(stderr, "Executable path is too long\n");
			return EXIT_FAILURE;
		}
		memcpy(program_dir, cwd, cwdlen);
		if (needs_sep)
		{
			program_dir[cwdlen++] = '\\';
		}
		memcpy(program_dir + cwdlen, argv[0], pathlen);
		program_dir[cwdlen + pathlen] = 0;
	} else {
		len = strlen(cwd);
		if (len + 2 > sizeof(program_dir))
		{
			fprintf(stderr, "Working directory path is too long\n");
			return EXIT_FAILURE;
		}
		memcpy(program_dir, cwd, len);
		if (len && program_dir[len - 1] != '/' && program_dir[len - 1] != '\\')
		{
			program_dir[len++] = '\\';
		}
		program_dir[len] = 0;
	}

	return bootup.main(argc, argv, program_dir, program_dir, program_dir, program_dir, program_dir);
}
