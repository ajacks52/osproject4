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
} Sim_List;

void initialize_simulation(Elevator_Simulation *es)
{
    Sim_List * sim = talloc(Sim_List, 1);
    sim->users = new_dllist();
    // set the elevator sim (*void) to my (*sim list struct)
    es->v = sim;
}

void initialize_elevator(Elevator *e)
{

}

void initialize_person(Person *p)
{
    Jval person = new_jval_v(p);
    Sim_List * temp = p->es->v;
    dll_append(temp->users, person);
}

void wait_for_elevator(Person *p)
{
    // 1. add person to global list
    // Sim_List * sim = p->es->v;
    // lock ?
    // pthread_mutex_lock(p->es->lock);
    // dll_append(sim->users, new_jval_v(p));
    // pthread_mutex_unlock(p->es->lock);
    // unlock

    // 2. signal cond variable to block elevator
    // 3. block on person's cond variable
    pthread_cond_wait(p->cond, p->lock);
}

void wait_to_get_off_elevator(Person *p)
{
    // unblock e's cond variable
    // block on p's cond variable
    pthread_cond_signal(p->e->cond);
    pthread_cond_wait(p->cond, p->lock);
}

void person_done(Person *p)
{
    // unblock the elevator's cond variable
    pthread_cond_signal(p->e->cond);
}

void *elevator(void *arg)
{

    Elevator *e = (Elevator *) arg;

    while(1)
    {
      printf("Nut sack!\n" );
      fflush(stdout);
        // global struct
        Sim_List *sim = e->es->v;

        // pull person from global list
        Person *p = (Person *)(jval_v(sim->users->flink->val));
        // if (!p) // check global list if it's empty elevator waits
        //     pthread_cond_wait(e->cond ,e->lock);

        // remove person being serviced from list
        dll_delete_node(sim->users->flink);
        p->e = e;

        if (e->onfloor != p->from){}
            move_to_floor(e,p->from);
        open_door(e);
        pthread_cond_signal(p->cond);
        pthread_cond_wait(e->cond, e->lock);
        close_door(e);
        pthread_mutex_unlock(e->lock);
    }
    return NULL;
}
