
#include <stdio.h>
#include "local.h"

/* Now in libexif
int exif_entry_gps_initialize() {
	return 0;
}
*/

int
__srget(FILE *fp)
{
	_SET_ORIENTATION(fp, -1);
	if (__srefill(fp) == 0) {
		fp->_r--;
		return (*fp->_p++);
	}
	return (EOF);
}
