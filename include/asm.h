#include <3ds.h>
#include "util.h"

u32 svc_30(void *entry_fn, ...);
Result svcGlobalBackdoor(s32 (*callback)(void));
bool checkSvcGlobalBackdoor();
Result svcMiniBackDoor(void *target);
void invalidate_icache();
Result checkRunningCFW();
Result checkLumaVersion(lumainfo *version);