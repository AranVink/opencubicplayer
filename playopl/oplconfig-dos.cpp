#include "config.h"
#include "types.h"

extern "C"
{
#include "boot/plinkman.h"
#include "boot/psetting.h"
#include "stuff/err.h"
#include "stuff/piperun.h"
#include "playopl/oplconfig.h"

OCP_INTERNAL char *opl_config_retrowave_device(const struct PipeProcessAPI_t *, const struct configAPI_t *)
{
	return 0;
}

OCP_INTERNAL int opl_config_init(struct PluginInitAPI_t *)
{
	return errOk;
}

OCP_INTERNAL void opl_config_done(struct PluginCloseAPI_t *)
{
}
}
