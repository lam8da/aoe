#ifndef AOE_SHIP_H
#define AOE_SHIP_H

#include <stddef.h>

struct ship_20 {
	void *file_slp_shp_data;
	size_t num4;
	int num8;
	unsigned numC;
	void *filedata2, *filedata2_off8;
	void *slp_data, *slp_data_off20;
};

struct ship_20 *shp_init(struct ship_20 *this, const char *fname, int res_id);

#endif