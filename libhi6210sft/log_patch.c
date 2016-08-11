#include <utils/Log.h>
int __android_janklog_print(int a1, int a2, const char *a3, ...)
{
	ALOGI(a3);
  return 0;
}
void __android_logPower_print(void) {
}
int isLogEnabled(void) {
	return 0;
}
int __android_log_exception_msg_write(int result, int a2, char a3, int a4) {
	ALOGE(a3);

	return result;
}
