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
#include <stdbool.h>
#include <pthread.h>
#include "elevator.h"

// stole from elevator sim clean way of mallocing data
#define talloc(ty, sz) (ty *) malloc ((sz) * sizeof(ty))

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct {
  Dllist users_up [1000];
  Dllist users_down [1000];
  Dllist users_leaving [1000];
  pthread_cond_t *block_elevator;
} user_list_struct_t;

bool DEBUG = 0;

void let_people_on (Elevator * e, int going_up, int current_floor, user_list_struct_t *g_list);
void let_people_off (Elevator * e, int going_up, int current_floor, user_list_struct_t *g_list);


void initialize_simulation(Elevator_Simulation *es)
{
  // global list of users and a condition variable

  user_list_struct_t * g_list = talloc(user_list_struct_t, 1);
  g_list->block_elevator = talloc(pthread_cond_t, 1);
  pthread_cond_init(g_list->block_elevator, NULL);

  int i = 1;
  for(; i <= es->nfloors; i++)
  {
    g_list->users_up[i] = new_dllist();
    g_list->users_down[i] = new_dllist();
  }

  es->v = g_list;
}

void initialize_elevator(Elevator *e)
{
  user_list_struct_t * g_list = talloc(user_list_struct_t, 1);
  g_list->block_elevator = talloc(pthread_cond_t, 1);
  pthread_cond_init(g_list->block_elevator, NULL);

  int i = 1;
  for(; i <= e->es->nfloors; i++)
  {
    g_list->users_leaving[i] = new_dllist();
  }

  e->v = g_list;
}

void initialize_person(Person *p)
{
  // get person's lock
  pthread_mutex_lock(p->lock);
  // add person to global list
  user_list_struct_t *g_list = p->es->v;

  if (p->from < p->to)
  {dll_append(g_list->users_up[p->from], new_jval_v(p));}
  else
  {dll_append(g_list->users_down[p->from], new_jval_v(p));}

  // unlock the mutex
  pthread_mutex_unlock(p->lock);
}

void wait_for_elevator(Person *p)
{
  // get the person's mutex lock
  pthread_mutex_lock(p->lock);
  // block on the person's condition variable
  pthread_cond_wait(p->cond, p->lock);
  // release the person's mutex lock
  pthread_mutex_unlock(p->lock);
}

void wait_to_get_off_elevator(Person *p)
{
  pthread_mutex_lock(p->e->lock);
  // unblock the elevator's condition variable
  // so it can service another person
  pthread_cond_signal(p->e->cond);
  pthread_cond_wait(p->cond, p->e->lock);
  pthread_mutex_unlock(p->e->lock);
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
  user_list_struct_t *floor_list = e->es->v;
  user_list_struct_t *e_list = e->v;

  int going_up = 1;
  int current_floor = 1;

  for(;;)
  {

    /* start from floor 1 and work your way up then back down continuously */
    move_to_floor(e, current_floor);

    // pthread_mutex_lock(e->es->lock);
    let_people_off (e, going_up, current_floor, e_list);
    // pthread_mutex_unlock(e->es->lock);

    // let people on
    pthread_mutex_lock(e->es->lock);
    let_people_on (e, going_up, current_floor, floor_list);
    pthread_mutex_unlock(e->lock);
    pthread_mutex_unlock(e->es->lock);

    if (e->door_open)
    close_door(e);
    if (e->onfloor == e->es->nfloors)
    {
      going_up = 0;
    }
    else if (e->onfloor == 1)
    {
      going_up = 1;
    }

    if (going_up)
    {
      current_floor++;
    }
    else if (!going_up)
    {
      current_floor--;
    }
  }
  return NULL;
}

void let_people_on (Elevator * e, int going_up, int current_floor, user_list_struct_t *g_list)
{
  Dllist tmp;
  user_list_struct_t *e_list = e->v;

  if (g_list->users_up[e->onfloor]->flink == g_list->users_up[e->onfloor]
    && g_list->users_down[e->onfloor]->flink == g_list->users_down[e->onfloor])
    {
      return;
    }

    if (going_up)
    {
      while (g_list->users_up[current_floor]->flink != g_list->users_up[current_floor])
      {
        Person *p = (Person *)(jval_v(g_list->users_up[current_floor]->flink->val));
        if(DEBUG)
        printf(ANSI_COLOR_CYAN "%s, %d, %d\n" ANSI_COLOR_RESET, p->fname, p->from, p->to);

        dll_delete_node(g_list->users_up[current_floor]->flink);

        pthread_mutex_unlock(e->es->lock);
        if (!e->door_open)
        {
          open_door(e);
        }

        p->e = e;   // put the elevator in the person's e field
        pthread_mutex_lock(e_list->block_elevator);
        dll_append(e_list->users_leaving[p->to], new_jval_v(p));
        pthread_mutex_unlock(e_list->block_elevator);

        /////get elevators lock, signal person, sleep//////////
        pthread_mutex_lock(p->lock);
        pthread_cond_signal(p->cond);
        pthread_mutex_unlock(p->lock);

        pthread_mutex_lock(e->lock);
        pthread_cond_wait(e->cond, e->lock);
        pthread_mutex_unlock(e->lock);
      }
    }

    if (!going_up)
    {
      while (g_list->users_down[current_floor]->flink != g_list->users_down[current_floor])
      {
        Person *p = (Person *)(jval_v(g_list->users_down[current_floor]->flink->val));
        if(DEBUG)
        printf(ANSI_COLOR_BLUE "%s, %d, %d\n" ANSI_COLOR_RESET, p->fname, p->from, p->to);

        dll_delete_node(g_list->users_down[current_floor]->flink);

        pthread_mutex_unlock(e->es->lock);
        if (!e->door_open)
        {
          open_door(e);
        }

        p->e = e;   // put the elevator in the person's e field
        pthread_mutex_lock(e_list->block_elevator);
        dll_append(e_list->users_leaving[p->to], new_jval_v(p));
        pthread_mutex_unlock(e_list->block_elevator);

        /////get elevators lock, signal person, sleep//////////
        pthread_mutex_lock(p->lock);
        pthread_cond_signal(p->cond);
        pthread_mutex_unlock(p->lock);

        pthread_mutex_lock(e->lock);
        pthread_cond_wait(e->cond, e->lock);
        pthread_mutex_unlock(e->lock);
      }
    }
  }

  void let_people_off (Elevator * e, int going_up, int current_floor, user_list_struct_t *g_list)
  {
    Dllist tmp;

    if (g_list->users_leaving[e->onfloor]->flink == g_list->users_leaving[e->onfloor])
    {
      return;
    }

    while (g_list->users_leaving[current_floor]->flink != g_list->users_leaving[current_floor])
    {
      Person *p = (Person *)(jval_v(g_list->users_leaving[current_floor]->flink->val));

      if(DEBUG)
      printf(ANSI_COLOR_RED "%s, %d, %d\n" ANSI_COLOR_RESET, p->fname, p->from, p->to);

      if (!e->door_open)
      {
        open_door(e);
      }
      // remove person from list
      dll_delete_node(g_list->users_leaving[current_floor]->flink);

      pthread_mutex_lock(e->lock);
      pthread_cond_signal(p->cond);
      pthread_cond_wait(e->cond, e->lock);
      pthread_mutex_unlock(e->lock);
    }
  }
