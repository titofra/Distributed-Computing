/**
 * @brief Mutex implementation with a distinction between getters and setters (for performance reasons)
 * @author Titouan ABADIE - http://github.com/titofra - 04/23
*/

#ifndef MUTEX_H
#define MUTEX_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mutex {
    pthread_mutex_t themutex;
    pthread_cond_t cond_getters;
    pthread_cond_t cond_setters;
    int nb_getters;
    int nb_setters_waiting;
    int is_setting;
} mutex_t;

/**
 * @brief Initialise the mutex
 * @param mutex_t* mut, Pointer to the mutex
*/
void InitMutex (mutex_t* mut);

/**
 * @brief Get an access. Note that this is a blocking function
 * @param mutex_t* mut, Pointer to the mutex
 * @param bool wSet, true if you need a get/set access, else false (only get access)
*/
void AcquireMutex (mutex_t* mut, bool wSet);

/**
 * @brief Release an access
 * @param mutex_t* mut, Pointer to the mutex
 * @param bool wSet, true if it was a get/set access, else false (only get access)
*/
void ReleaseMutex (mutex_t* mut, bool wSet);

/**
 * @brief Destroy a mutex
 * @param mutex_t* mut, Pointer to the mutex
*/
void DestroyMutex(mutex_t* mut);

#ifdef __cplusplus
}
#endif

#endif  //MUTEX_H