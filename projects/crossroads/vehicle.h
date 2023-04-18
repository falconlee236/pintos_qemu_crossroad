#ifndef __PROJECTS_PROJECT1_VEHICLE_H__
#define __PROJECTS_PROJECT1_VEHICLE_H__

#include "projects/crossroads/position.h"
#include "threads/synch.h"

#define VEHICLE_STATUS_READY 	0
#define VEHICLE_STATUS_RUNNING	1
#define VEHICLE_STATUS_FINISHED	2

struct vehicle_info {
	char id;
	char state;
	char start;
	char dest;
	struct position position;
	struct lock **map_locks;
};

struct semaphore *cnt, *traffic;
int total_cnt;
int wait_cnt;

void vehicle_loop(void *vi);

#endif /* __PROJECTS_PROJECT1_VEHICLE_H__ */
