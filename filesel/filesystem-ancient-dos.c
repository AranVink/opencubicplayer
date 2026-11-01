/* Ancient compressed-file support requires a C++ library unavailable in the
 * minimal DJGPP build.
 */

#include "config.h"
#include "types.h"
#include "filesystem.h"
#include "filesystem-ancient.h"

struct ocpfilehandle_t *ancient_filehandle(char *compressionmethod, int compressionmethod_len, struct ocpfilehandle_t *s)
{
	(void)compressionmethod;
	(void)compressionmethod_len;
	(void)s;
	return 0;
}
