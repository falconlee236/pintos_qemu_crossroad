#ifndef __PROJECTS_PROJECT1_MAPDATA_H__
#define __PROJECTS_PROJECT1_MAPDATA_H__

#include <stdio.h>
#include "projects/crossroads/position.h"


extern int crossroads_step;

void map_draw(void);
void map_draw_vehicle(char id, int row, int col);
void map_draw_reset(void);

#endif /* __PROJECTS_PROJECT1_MAPDATA_H__ */