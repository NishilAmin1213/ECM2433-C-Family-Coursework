/* Include the required files */
#include "runSimulations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

/**
 * Updates the averages of the res variable using the new values of the tmp variable
 * @param res - pointer to a ReturnData structure which holds the averaged results so far
 * @param tmp - ReturnData structure which holds the most recent results to be added to the average stored in res
 */
void update_res(ReturnData *res, ReturnData tmp){
    /* calculate the new average for the average waiting time in the right light and store it to the res struct  */
    res->avgTimeRHS = (res->avgTimeRHS + tmp.avgTimeRHS)/2;
    /* calculate the new average for the maximum waiting time in the right light and store it to the res struct  */
    res->maxTimeRHS = (res->maxTimeRHS + tmp.maxTimeRHS)/2;
    /* calculate the new average for the total number of vehicles in the right light and store it to the res struct  */
    res->numOfVehiclesRHS = (res->numOfVehiclesRHS + tmp.numOfVehiclesRHS)/2;
    /* calculate the new average for the clearance time of the right light and store it to the res struct  */
    res->clearanceTimeRHS = (res->clearanceTimeRHS + tmp.clearanceTimeRHS)/2;

    /* calculate the new average for the average waiting time in the left light and store it to the res struct  */
    res->avgTimeLHS = (res->avgTimeLHS + tmp.avgTimeLHS)/2;
    /* calculate the new average for the maximum waiting time in the left light and store it to the res struct  */
    res->maxTimeLHS = (res->maxTimeLHS + tmp.maxTimeLHS)/2;
    /* calculate the new average for the total number of vehicles in the left light and store it to the res struct  */
    res->numOfVehiclesLHS = (res->numOfVehiclesLHS + tmp.numOfVehiclesLHS)/2;
    /* calculate the new average for the clearance time of the left light and store it to the res struct  */
    res->clearanceTimeLHS = (res->clearanceTimeLHS + tmp.clearanceTimeLHS)/2;
}

/**
 * Displays the desired information and text to the stdout stream
 * @param arrivalRateLHS - The rate of arrival for the Left light (a integer percentage between 0 and 100)
 * @param lightPeriodLHS - The period that the Left light stays active for (an integer for the number of iterations)
 * @param arrivalRateRHS - The rate of arrival for the Right light (a percentage between 0 and 100)
 * @param lightPeriodRHS - The period that the Left light stays active for (an integer for the number of iterations)
 * @param res - The averaged result over the 100 calls of the runOneSimulation function
 */
void display(int arrivalRateLHS,
             int lightPeriodLHS,
             int arrivalRateRHS,
             int lightPeriodRHS,
             ReturnData res){
    printf("Parameter Values:\n"
           "    from left:\n"
           "        traffic arrival rate: %d\n"
           "        traffic light period %d\n"
           "    from right:\n"
           "        traffic arrival rate: %d\n"
           "        traffic light period %d\n"
           "Results (averaged over 100 runs):\n"
           "    from left:\n"
           "        number of vehicles: %f\n"
           "        average waiting time: %f\n"
           "        maximum waiting time: %f\n"
           "        clearance time: %f\n"
           "    from right:\n"
           "        number of vehicles: %f\n"
           "        average waiting time: %f\n"
           "        maximum waiting time: %f\n"
           "        clearance time: %f\n",
           arrivalRateLHS,
           lightPeriodLHS,
           arrivalRateRHS,
           lightPeriodRHS,
           res.numOfVehiclesLHS,
           res.avgTimeLHS,
           res.maxTimeLHS,
           res.clearanceTimeLHS,
           res.numOfVehiclesRHS,
           res.avgTimeRHS,
           res.maxTimeRHS,
           res.clearanceTimeRHS);
}

/**
 * Runs the runOneSimulation function 100 times using the input passed into this function, returns the averaged results
 * @param arrivalRateLHS - The rate of arrival for the Left light (a integer percentage between 0 and 100)
 * @param lightPeriodLHS - The period that the Left light stays active for (an integer for the number of iterations)
 * @param arrivalRateRHS - The rate of arrival for the Right light (a percentage between 0 and 100)
 * @param lightPeriodRHS - The period that the Left light stays active for (an integer for the number of iterations)
 * @return - A ReturnData struct to hold all relevant data required by the calling function
 */
ReturnData runSimulations(int arrivalRateLHS, int lightPeriodLHS, int arrivalRateRHS, int lightPeriodRHS){
    /* calls the function once to get the initial ResultData struct */
    ReturnData res = runOneSimulation(arrivalRateLHS, lightPeriodLHS, arrivalRateRHS, lightPeriodRHS);

    /* for another 100 times, do the code below */
    int test_no = 2;
    while(test_no<=100){
        /* increment the test_no counter */
        test_no++;
        /* call runOneSimulation once and store the result in tmp*/
        ReturnData tmp = runOneSimulation(arrivalRateLHS, lightPeriodLHS, arrivalRateRHS, lightPeriodRHS);
        /* if the status of the result is 0, an error occurred, so discard this run */
        if(tmp.status == 0){
                continue;
        }
        /* as we didn't discard this run, update the overall res using the new tmp variable and initial res */
        update_res(&res, tmp);
    }

    /* once complete, res can be returned */
    return res;
}

/**
 * Main function to handle incoming inputs and call the runSimulations function
 * @param argc - number of arguements being passed in (this param does not need to be passed in by the user)
 * @param argv - array of parameters passed in by the user in the command line
 * @return - integer to show successful or errors in the run
 */
int main(int argc, char *argv[]){

    /* call the runSimulations function with the passed in inputs in integer format */
    ReturnData dat = runSimulations(atoi(argv[1]),
                                    atoi(argv[2]),
                                    atoi(argv[3]),
                                    atoi(argv[4]));

    /* pass the passed in inputs as well as the result from the runs, into the display function */
    display(atoi(argv[1]),
            atoi(argv[2]),
            atoi(argv[3]),
            atoi(argv[4]),
            dat);

    /* return 1 to show a successful run of the code */
    return 1;
}
