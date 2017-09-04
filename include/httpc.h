#include <3ds.h>

Result httpDownloadData(const char* url);
u8* httpRetrieveData();
size_t httpBufSize();
void httpFree();