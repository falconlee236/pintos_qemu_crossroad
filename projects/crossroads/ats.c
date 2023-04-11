#include "projects/crossroads/ats.h"
#include "projects/crossroads/crossroads.h"
#include "projects/crossroads/vehicle.h"
#include "projects/crossroads/map.h"
#include "devices/timer.h"

extern int crossroads_step;

void* unitstep_changed(){
	/**  DO NOT THOUCH THIS FILE  **/
	timer_msleep(1000);
	return NULL;
}
