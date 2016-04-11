/*
 * Created by Adam Mitchell on 4/2/16.
 *
 *
 * Part1 of the infamous elevator lab where elevator patrons are to enter elevator one at a time
 * and be delivered the floor they request.  The concepts involved here are p_thread synchronization and scheduling
 * algorithms.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "elevator.h"

// stole from elevator sim clean way of mallocing data
#define talloc(ty, sz) (ty *) malloc ((sz) * sizeof(ty))


// global list of users and a condition variable
typedef struct {
    Dllist users;
    pthread_cond_t *block_elevator;
    int elements_in_list;
} global_list;

void initialize_simulation(Elevator_Simulation *es)
{
    global_list * g_list = talloc(global_list, 1);
    g_list->users = new_dllist();
    g_list->block_elevator = talloc(pthread_cond_t, 1);
    pthread_cond_init(g_list->block_elevator, NULL);
    g_list->elements_in_list = 0;
    es->v = g_list;
}

void initialize_elevator(Elevator *e)
{}

void initialize_person(Person *p)
{}

void wait_for_elevator(Person *p)
{
    // get person's lock
    pthread_mutex_lock(p->es->lock);

    // add person to global list
    global_list *g_list = p->es->v;
    dll_append(g_list->users, new_jval_v(p));
    // increment elements_in_list
    g_list->elements_in_list++;

    // signal cond variable for "blocking elevtors"
    pthread_cond_signal(g_list->block_elevator);
    // unlock the mutex
    pthread_mutex_unlock(p->es->lock);

    // get the person's mutex lock
    pthread_mutex_lock(p->lock);
    // block on the person's condition variable
    pthread_cond_wait(p->cond, p->lock);
    // release the person's mutex lock
    pthread_mutex_unlock(p->lock);
}

void wait_to_get_off_elevator(Person *p)
{
    // get the elevator's mutex lock
    pthread_mutex_lock(p->e->lock);
    // unblock the elevator's condition variable
    pthread_cond_signal(p->e->cond);
    // unlock the elevator's mutex lock
    pthread_mutex_unlock(p->e->lock);

    // get the person's mutex lock
    pthread_mutex_lock(p->lock);
    // block on the person's condition variable
    pthread_cond_wait(p->cond, p->lock);
    // release the person's mutex lock
    pthread_mutex_unlock(p->lock);

}

void person_done(Person *p)
{
  // get the elevator's mutex lock
  pthread_mutex_lock(p->e->lock);
  // unblock the elevator's condition variable
  pthread_cond_signal(p->e->cond);
  // unlock the elevator's mutex lock
  pthread_mutex_unlock(p->e->lock);
}

void *elevator(void *arg)
{

    Elevator *e = (Elevator *) arg;

    while(1)
    {
        // global list struct
        global_list *g_list = e->es->v;

        /* start from floor 1 and work your way up continuously */
        int current_floor;
        for (current_floor = 1; current_floor <= e->es->nfloors; current_floor++)
        {
          move_to_floor(e, current_floor);

          // check if someone needs to be dropped off
          while () 
          {

          }

          // check if someone waiting on current_floor to be picked up




        }

        // pthread_mutex_lock(e->es->lock);
        // // get first person in global list
        // Person *p = (Person *) jval_v(dll_val(dll_first(g_list->users)));
        // // remove person being serviced from list
        // dll_delete_node(g_list->users->flink);
        // g_list->elements_in_list--;
        // pthread_mutex_unlock(e->es->lock);
        //
        // /** pick up person */
        // if (e->onfloor != p->from){}
        //     move_to_floor(e,p->from);
        // open_door(e);
        // p->e = e;   // put the elevator in the person's e field
        // pthread_mutex_lock(e->lock);
        // pthread_cond_signal(p->cond);
        // pthread_cond_wait(e->cond, e->lock);
        // pthread_mutex_unlock(e->lock);
        // close_door(e);
        //
        // /** move person to their destination floor */
        // move_to_floor(e,p->to);
        // open_door(e);
        // pthread_mutex_lock(e->lock);
        // pthread_cond_signal(p->cond);
        // pthread_cond_wait(e->cond, e->lock);
        // pthread_mutex_unlock(e->lock);
        // close_door(e);
    }
    return NULL;
}
