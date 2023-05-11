#ifndef PID_ALG_H
#define PID_ALG_H

#ifdef __cplusplus
extern "C" {
#endif

#include<rtfixpoint.h>


typedef struct {
    fixedpt ref;
    fixedpt fdb;
    fixedpt err[3];
    fixedpt err_lim;          // Integral anti-windup 
    // fixedpt err_fdf[3];       // Feedforard
    // fixedpt out_fdf;          // Feedforard out put
    fixedpt sumi;
    fixedpt output;

    // Filter_LowPassTypeDef d_fil;
    // Filter_LowPassTypeDef delta_fil;

    // Filter_LowPassTypeDef kf1_fil;
    // Filter_LowPassTypeDef kf2_fil;
    fixedpt err_watch;
} PID_PIDTypeDef;

typedef struct {
    fixedpt kp;
    fixedpt ki;
    fixedpt kd;

    fixedpt max_output;
    fixedpt max_sumi;   
} PID_PIDparam_Typedef;

void PID_InitPIDParam(PID_PIDparam_Typedef* ppram, fixedpt kp, fixedpt ki, fixedpt kd, fixedpt max_pid, fixedpt max_sumi);
fixedpt PID_GetPIDOutput(PID_PIDTypeDef* ppid);
void PID_SetPIDRef(PID_PIDTypeDef* ppid, fixedpt ref);
void PID_ClearPID(PID_PIDTypeDef* ppid);
void PID_ClacPID(PID_PIDTypeDef* ppid, PID_PIDparam_Typedef* pparam);
void PID_InitPID(PID_PIDTypeDef *ppid);
void PID_SetPIDFdb(PID_PIDTypeDef* ppid, fixedpt fdb);
fixedpt PID_GetPIDRef(PID_PIDTypeDef* ppid) ;
void PID_SetPIDMax(PID_PIDparam_Typedef *pparam,fixedpt PID_Max);

extern PID_PIDparam_Typedef PID_ChassisMotorpidparam ;

#endif
