/* OpenCP Module Player
 * DOS builds do not provide the POSIX pipe/fork/exec process runner.
 */

#include "config.h"
#include <errno.h>
#include <stdio.h>
#include "types.h"
#include "piperun.h"

void *ocpPipeProcess_create(const char * const commandLine[])
{
	(void)commandLine;
	fprintf(stderr, "External process support is unavailable in this DOS build\n");
	errno = ENOSYS;
	return 0;
}

int ocpPipeProcess_destroy(void *process)
{
	(void)process;
	errno = ENOSYS;
	return -1;
}

int ocpPipeProcess_terminate(void *process)
{
	(void)process;
	errno = ENOSYS;
	return -1;
}

int ocpPipeProcess_read_stdout(void *process, char *const buffer, unsigned size)
{
	(void)process;
	(void)buffer;
	(void)size;
	errno = ENOSYS;
	return -1;
}

int ocpPipeProcess_read_stderr(void *process, char *const buffer, unsigned size)
{
	(void)process;
	(void)buffer;
	(void)size;
	errno = ENOSYS;
	return -1;
}

const struct PipeProcessAPI_t PipeProcess =
{
	ocpPipeProcess_create,
	ocpPipeProcess_destroy,
	ocpPipeProcess_terminate,
	ocpPipeProcess_read_stdout,
	ocpPipeProcess_read_stderr
};
