#include <stdio.h>
#include "runOneSimulation.h"
#include <sys/time.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

/*
 * Vehicle is a struct that represents a vehicle at a light in a system
 * It Stores the 'light', a pointer to the lights struct that it is at
 * as well as an integer to store which iteration this vehicle was generated in
*/
struct Vehicle {
    struct Lights *light;
    int iterationGenerated;
};

/*
 * Lights is a struct which represents a light in the system
 * It stores the following
 * int lightPeriod - an integer to store the number of iterations that this light remains green for
    int timer - a timer integer to act as a timer when the light is green
    float avgTime - a float to store the average time that vehicles have waited at this light for
    int maxTime - an integer to store the maximum number of time a vehicle has waited at this light for
    int numOfVehicles - the number of vehicles that are waiting for OR passed through this light
    int clearanceTime - the number of iterations that have passed for the light to clear AFTER the cars have stopped arriving
    int status - status to show if this light is red(0) or green(1)
*/
struct Lights {
    int lightPeriod;
    int timer;
    float avgTime;
    int maxTime;
    int numOfVehicles;
    int clearanceTime;
    int status;
};

/*
 * The Node struct represents a Node in the system, each node holds a pointer to a vehicle as well as a pointer to the
 * next node. These nodes form a linked list to represent the queue of cars at each light
*/
typedef struct Node {
    struct Vehicle *vehicle;
    struct Node *next;
}NODE;

/**
 * The Function gets the last node in the linked list when passed a pointer to the head, and returns a pointer to it
 * @param node - pointer to the head node of the linked list
 * @return - a pointer to the last node in the list
 */
struct Node* get_last_node(struct Node *node){
    /* whilst a next node is present, move to the next node */
    while (node->next != NULL){
        node = node->next;
    }
    /* when there is no next node, the last node has been found, return this */
    return node;
}

/**
 * add_node takes details of the correct head node, lights and current iteration to add a new node to the linked list
 * @param head - a pointer to the head node of the linked list
 * @param light - a pointer to the lights that this vehicle will be at
 * @param iteration - the iteration value that this node is being created at
 * @return - an integer to state whether this addition was successful or not
 */
int add_node(NODE *head, struct Lights *light, int iteration){

    /* malloc the space for a new vehicle and store the pointer to this space */
    struct Vehicle *tmp_vehicle = (struct Vehicle*) malloc(sizeof(struct Vehicle));
    /* check that this malloc was successful */
    if (tmp_vehicle == NULL) {
        /* if the malloc is unsuccessful, then return 1 */
        return 1;
    }
    /* set the variables of the node's   vehicle to the correct values */
    tmp_vehicle->iterationGenerated = iteration;
    tmp_vehicle->light = light;

    /* malloc the space for a new node and store the pointer to this space */
    NODE *tmp_node = (struct Node*) malloc(sizeof(NODE));
    /* check that this malloc was successful */
    if (tmp_node == NULL) {
        /* if the malloc is unsuccessful, then return 1 */
        return 1;
    }

    /* set the variables of the node to the correct values */
    tmp_node->vehicle = tmp_vehicle;
    tmp_node->next = NULL;

    /* use the get_last_node function to add the pointer to the node to the 'next' variable of the last node
     * essentially adding this node to the end of the linked list*/
    NODE *last_item = get_last_node(head);
    last_item->next = tmp_node;

    /* increment the number of vehicles that have been added to this light */
    light->numOfVehicles++;
}

/**
 * add_stats takes a vehicle and an iteration and adds the statistics of this vehicle the the data of its corresponding light
 * @param vehicle  - pointer to the vehicle whost stats we are using
 * @param iteration - iteration that we are currently in when calling this function
 */
void add_stats(struct Vehicle *vehicle, int iteration){
    /* re-calculate the average waiting time for the lights this vehicle is at */
    vehicle->light->avgTime = ((float)(iteration - vehicle->iterationGenerated) + vehicle->light->avgTime)/2;

    /* re-calculate the maximum waiting time for the lights this vehicle is at */
    if(iteration - vehicle->iterationGenerated > vehicle->light->maxTime){
        vehicle->light->maxTime = iteration - vehicle->iterationGenerated;
    }
}

/**
 * remove_first_node removes the node which is at the front of the linked list (the noed acting as the car nearest to the lights)
 * @param head - the head node of the linked list
 * @param iteration - iteration that we are currently in when calling this function
 * @return - an integer to whether ths function was successful
 */
int remove_first_node(NODE *head, int iteration){
    /* if the head's next parameter is null, the list is empty, so return 1 */
    if (head->next == NULL){
        return 1;
    }
     /* get the first node in the list and store it as in tmp_node */
    NODE* tmp_node = head->next;
    /* set the heads' next parameter to the node after the node we are removing */
    head->next = head->next->next;

    /* add the stats of the node we are removing's vehicle to the lights it passed */
    add_stats(tmp_node->vehicle, iteration);

    /* free the memory belonging to this vehicle's struct */
    free(tmp_node->vehicle);
    /* free the memory belonging to this node's struct */
    free(tmp_node);

}

/**
 * Takes the head node and checks if the linked list is empty
 * @param node - the head node of the linked list
 * @return - an integer to specify whether the linked list is empty(1) of not(0)
 */
int is_empty(NODE *node){
    /* if the head nodes' has no next node, the linked list is empty */
    if (node->next == NULL) {
        /* return 1 */
        return 1;
    }else{
        /* otherwise, the list is not empty, return 0 */
        return 0;
    }
}

/**
 * update_light takes in in both light instances and updates the values as requierd
 * @param light - the light that is currently green
 * @param other - the light that is currently red
 * @return - an integer to signify if the light were changed(1) or not(0)
 */
int update_light(struct Lights *light, struct Lights *other){
    /* if the green lights timer has reached 0 */
    if(light->timer == 0){
        /* set this timer to the lightPeriod variable (reset the timer) */
        light->timer = light->lightPeriod;
        /* set this lights status to 0 (Red) */
        light->status = 0;
        /* set the red lights' status to 1(Green) */
        other->status = 1;
        /* return 1 as the light changed color */
        return 1;
    }else{
        /* the light is to remain green */
        /* decrement the timer */
        light->timer --;
        /* return 0 as the light remained green */
        return 0;
    }
}

/**
 * function to get a random value between 0 and 100
 * @return a random float between 0 and 100
 */
float get_random_val() {
    /* initialise the variables belonging to the GSL library */
    const gsl_rng_type *T;
    gsl_rng *r;
    gsl_rng_env_setup();
    /* get information based on the current time */
    struct timeval tv;
    gettimeofday(&tv, 0);
    /* seed the random function based on the current time */
    unsigned long mySeed = tv.tv_sec + tv.tv_usec;
    T = gsl_rng_default;
    r = gsl_rng_alloc(T);
    gsl_rng_set(r, mySeed);
    /* ger a random float between 0 and 1 */
    double u = gsl_rng_uniform(r);
    gsl_rng_free(r);
    /* return theis value multiplied by 100 */
    return ((float)u * 100);
}

/**
 * the runOneSimulation function takes in the 4 required parameters adn runs 500 loops of the lights where cars can be
 * randomly added and light changed, then after 100 iterations, the light continue changing to empty all the traffic
 * @param arrivalRateLHS - The rate of arrival for the Left light (a integer percentage between 0 and 100)
 * @param lightPeriodLHS - The period that the Left light stays active for (an integer for the number of iterations)
 * @param arrivalRateRHS - The rate of arrival for the Right light (a percentage between 0 and 100)
 * @param lightPeriodRHS - The period that the Left light stays active for (an integer for the number of iterations)
 * @return - a ReturnData struct containing statistics about the vehicles at each light
 */
ReturnData runOneSimulation(int arrivalRateLHS,
                     int lightPeriodLHS,
                     int arrivalRateRHS,
                     int lightPeriodRHS){

    /* create two Lights structs for each light and initialise the values accordingly */
    struct Lights leftLight = {lightPeriodLHS, lightPeriodLHS, 0, 0, 0, 0, 0};
    struct Lights rightLight = {lightPeriodRHS, lightPeriodRHS, 0, 0, 0, 0, 1};

    /* malloc the space for the right linked list's head node and store the pointer to this space */
    NODE *rightLightHead = (NODE*) malloc(sizeof(NODE));
    /* check that this malloc was successful */
    if (rightLightHead == NULL) {
        /* if the malloc is unsuccessful, then return an empty ReturnData instance with the status 1 */
        ReturnData tmp = {0,0,0,0,0,0,0,0,0};
        return tmp;
    }
    /* set the variables for the head node to NULL */
    rightLightHead->next = NULL;
    rightLightHead->vehicle = NULL;

    /* malloc the space for the left linked list's head node and store the pointer to this space */
    NODE *leftLightHead = (NODE*) malloc(sizeof(NODE));
    /* check that this malloc was successful */
    if (leftLightHead == NULL) {
        /* if the malloc is unsuccessful, then return an empty ReturnData instance with the status 1 */
        ReturnData tmp = {0,0,0,0,0,0,0,0,0};
        return tmp;
    }
    /* set the variables for the head node to NULL */
    leftLightHead->next = NULL;
    leftLightHead->vehicle = NULL;

    /* declare and instantiate the variables to control the running of the while loop below */
    int iteration = 0;
    int max = 500;

    while(1){

        /* if we have exceeded the number of iterations where vehicles are allowed to arrive(500) */
        if(iteration>max){
            /* if the right light is not empty, then increment the right lights clearance time */
            if(is_empty(rightLightHead) == 0){
                rightLight.clearanceTime++;
            }
            /* if the left light is not empty, then increment the left lights clearance time */
            if(is_empty(leftLightHead) == 0){
                leftLight.clearanceTime++;
            }
            /* if both the right and left lights are empty, then break out of the while loop as we have finished iterating */
            if (is_empty(rightLightHead) == 1 && is_empty(leftLightHead) == 1){
                break;
            }
        }

        /* update the status of the lights, passing in the green light as the first parameter */
        /* store the result in light_changed, which stores if a light was changed(1) or not(0) */
        int light_changed = 0;
        if(rightLight.status == 1){
            light_changed = update_light(&rightLight, &leftLight);
        }else if(leftLight.status == 1){
            light_changed = update_light(&leftLight, &rightLight);
        }



        /* if the lights didn't change AND we have not exceeded the maximum number of iterations where cars can spawn */
        if(light_changed == 0){

            if(iteration < max) {
                /* get two random values between 0 and 100  */
                float left_rand = get_random_val();
                float right_rand = get_random_val();

                /* if this random value for the left light is less than the arrival rate passed in (probability) add a vehicle to the left queue*/
                if ((int) left_rand <= arrivalRateLHS) {
                    add_node(leftLightHead, &leftLight, iteration);
                }
                /* if this random value for the right light is less than the arrival rate passed in (probability) add a vehicle to the right queue*/
                if (((int) right_rand <= arrivalRateRHS)) {
                    add_node(rightLightHead, &rightLight, iteration);
                }
            }

            /* if one of the lights is green, remove a node from this light */
            if(rightLight.status==1){
                remove_first_node(rightLightHead, iteration);
            }else if(leftLight.status==1){
                remove_first_node(leftLightHead, iteration);
            }
        }
        /* increment the value of iteration as an iteration has completed */
        iteration++;
    }

    /* free the two malloced heads for the linked list */
    free(rightLightHead);
    free(leftLightHead);

    /* Return the ReturnData struct with the stats of each of the lights and the status 1 as the function completed successfully */
    ReturnData res = {rightLight.avgTime,
                    rightLight.maxTime,
                    rightLight.numOfVehicles,
                    rightLight.clearanceTime,
                    leftLight.avgTime,
                    leftLight.maxTime,
                    leftLight.numOfVehicles,
                    leftLight.clearanceTime,
                    1};

    return res;

}
