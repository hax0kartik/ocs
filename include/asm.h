#include <3ds.h>

typedef (*backdoor_fn)(u32 arg0, u32 arg1);
u32 svc_30(void *entry_fn, ...);
Result svcGlobalBackdoor(s32 (*callback)(void));
bool checkSvcGlobalBackdoor();
Result svcMiniBackDoor(void *target);
void invalidate_icache();