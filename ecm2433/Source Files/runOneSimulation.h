#ifndef ECM2433___CW_RUNONESIMULATION_H
#define ECM2433___CW_RUNONESIMULATION_H

/* If this file hasn't already been imported, define the struct Res*/

/* The struct Res, aka ReturnData creates a neat and usable datatype to allow the
 * required data to be returned by the runOneSimulation in runOneSimulation.c*/
typedef struct Res {
    float avgTimeRHS;
    float maxTimeRHS;
    float numOfVehiclesRHS;
    float clearanceTimeRHS;
    float avgTimeLHS;
    float maxTimeLHS;
    float numOfVehiclesLHS;
    float clearanceTimeLHS;
    float status;
}ReturnData;

#endif

/* Declare the runOneSimulation functions of runOneSimulation.c and specify its return type  */
ReturnData runOneSimulation(int arrivalRateLHS,
                     int lightPeriodLHS,
                     int arrivalRateRHS,
                     int lightPeriodRHS);

