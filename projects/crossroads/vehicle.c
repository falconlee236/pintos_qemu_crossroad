
#include <stdio.h>
#include <stdlib.h>

#include "threads/thread.h"
#include "threads/synch.h"
#include "projects/crossroads/vehicle.h"
#include "projects/crossroads/map.h"
#include "projects/crossroads/ats.h"

/* path. A:0 B:1 C:2 D:3 */
const struct position vehicle_path[4][4][10] = {
	/* from A */ {
		/* to A */
		{{-1,-1},},
		/* to B */
		{{4,0},{4,1},{4,2},{5,2},{6,2},{-1,-1},},
		/* to C */
		{{4,0},{4,1},{4,2},{4,3},{4,4},{4,5},{4,6},{-1,-1},},
		/* to D */
		{{4,0},{4,1},{4,2},{4,3},{4,4},{3,4},{2,4},{1,4},{0,4},{-1,-1}}
	},
	/* from B */ {
		/* to A */
		{{6,4},{5,4},{4,4},{3,4},{2,4},{2,3},{2,2},{2,1},{2,0},{-1,-1}},
		/* to B */
		{{-1,-1},},
		/* to C */
		{{6,4},{5,4},{4,4},{4,5},{4,6},{-1,-1},},
		/* to D */
		{{6,4},{5,4},{4,4},{3,4},{2,4},{1,4},{0,4},{-1,-1},}
	},
	/* from C */ {
		/* to A */
		{{2,6},{2,5},{2,4},{2,3},{2,2},{2,1},{2,0},{-1,-1},},
		/* to B */
		{{2,6},{2,5},{2,4},{2,3},{2,2},{3,2},{4,2},{5,2},{6,2},{-1,-1}},
		/* to C */
		{{-1,-1},},
		/* to D */
		{{2,6},{2,5},{2,4},{1,4},{0,4},{-1,-1},}
	},
	/* from D */ {
		/* to A */
		{{0,2},{1,2},{2,2},{2,1},{2,0},{-1,-1},},
		/* to B */
		{{0,2},{1,2},{2,2},{3,2},{4,2},{5,2},{6,2},{-1,-1},},
		/* to C */
		{{0,2},{1,2},{2,2},{3,2},{4,2},{4,3},{4,4},{4,5},{4,6},{-1,-1}},
		/* to D */
		{{-1,-1},}
	}
};

static int is_position_outside(struct position pos)
{
	return (pos.row == -1 || pos.col == -1);
}

static void is_circular(int start, struct vehicle_info *vi)
{
    if (start == 0)
    {
        lock_acquire(&vi->map_locks[3][2]);
        lock_release(&vi->map_locks[3][2]);
    }
    else if (start == 1)
    {
        lock_acquire(&vi->map_locks[4][3]);
        lock_release(&vi->map_locks[4][3]);
    }
    else if (start == 2)
    {
        lock_acquire(&vi->map_locks[3][4]);
        lock_release(&vi->map_locks[3][4]);
    }
    else if (start == 3)
    {
        lock_acquire(&vi->map_locks[2][3]);
        lock_release(&vi->map_locks[2][3]);
    }
}

/* return 0:termination, 1:success, -1:fail */
static int try_move(int start, int dest, int step, struct vehicle_info *vi)
{
	struct position pos_cur, pos_next;

	pos_next = vehicle_path[start][dest][step];
	pos_cur = vi->position;
    wait_cnt++;

	if (vi->state == VEHICLE_STATUS_RUNNING) {
		/* check termination */
		if (is_position_outside(pos_next)) {
			/* actual move */
			vi->position.row = vi->position.col = -1;
			return 0;
		}
	}

	/* lock next position */
    if (step < 2)
    {
	    //while (!lock_try_acquire(&vi->map_locks[pos_next.row][pos_next.col]))
        //    continue;
        lock_acquire(&vi->map_locks[pos_next.row][pos_next.col]);
    }
	if (vi->state == VEHICLE_STATUS_READY) {
		/* start this vehicle */
		vi->state = VEHICLE_STATUS_RUNNING;
	}
    else {
		/* release current position */
        if (step == 2)
        {
           int tmp_step = step;
           struct position tmp_pos = vehicle_path[start][dest][tmp_step++];
           is_circular(start, vi);
           while(tmp_pos.row > 1 && tmp_pos.row < 5 && tmp_pos.col > 1 && tmp_pos.col < 5)
           {
               if (lock_try_acquire(&vi->map_locks[tmp_pos.row][tmp_pos.col]))
                   tmp_pos = vehicle_path[start][dest][tmp_step++];
               //while (!lock_try_acquire(&vi->map_locks[tmp_pos.row][tmp_pos.col]))
               //    continue;
               //tmp_pos = vehicle_path[start][dest][tmp_step++];
           }
        }
        if(step <= 2 || (pos_cur.row > 1 && pos_cur.row < 5 && pos_cur.col > 1 && pos_cur.col < 5))
            lock_release(&vi->map_locks[pos_cur.row][pos_cur.col]);
	}
	/* update position */
	vi->position = pos_next;
    wait_cnt--;
	return 1;
}

void init_on_mainthread(int thread_cnt){
	/* Called once before spawning threads */
    cnt = (struct semaphore*)malloc(sizeof(struct semaphore));
    traffic = (struct semaphore*)malloc(sizeof(struct semaphore));
    sema_init(cnt, 0);
    sema_init(traffic, 3);
    total_cnt = thread_cnt;
}

void vehicle_loop(void *_vi)
{
	int res;
	int start, dest, step;

	struct vehicle_info *vi = _vi;

	start = vi->start - 'A';
	dest = vi->dest - 'A';

	vi->position.row = vi->position.col = -1;
	vi->state = VEHICLE_STATUS_READY;

	step = 0;
	while (1) {
        if (sema_try_down(cnt))
        {
		    /* vehicle main code */
            if (step == 2)
            {
                if (!sema_try_down(traffic))
                {
                    wait_cnt++;
                    sema_down(traffic);
                    wait_cnt--;
                }
                //sema_down(traffic);
            }
            if (step == 3)
                sema_up(traffic);
		    res = try_move(start, dest, step, vi);
		    if (res == 1) {
			    step++;
		    }
		    /* termination condition. */ 
		    if (res == 0) {
			    break;
		    }
            /* unitstep change! */
            unitstep_changed();
        }
        else
        {
            crossroads_step++;
            sema_init(cnt, total_cnt - wait_cnt);
        }
	}	

	/* status transition must happen before sema_up */
	vi->state = VEHICLE_STATUS_FINISHED;
}
