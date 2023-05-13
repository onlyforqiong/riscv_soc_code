#include <pid_alg.h>
#include <rtthread.h>

PID_PIDparam_Typedef PID_ChassisMotorpidparam = {fixedpt_fromint(5),fixedpt_rconst(0.01),fixedpt_fromint(0.01),fixedpt_fromint(10000),fixedpt_fromint(10000)};


void PID_InitPIDParam(PID_PIDparam_Typedef* ppram, fixedpt kp, fixedpt ki, fixedpt kd, fixedpt max_pid, fixedpt max_sumi) {

    ppram->kp = kp;
    ppram->ki = ki;
    ppram->kd = kd;
    ppram->max_output = max_pid;
    ppram->max_sumi = max_sumi;
}

void PID_InitPID(PID_PIDTypeDef *ppid) {

    ppid->err[0]  = fixedpt_fromint(0);
    ppid->err[1]  = fixedpt_fromint(0);
    ppid->err[2]  = fixedpt_fromint(0);
    ppid->err_lim = fixedpt_fromint(0);
    ppid->sumi    = fixedpt_fromint(0);
    ppid->output  = fixedpt_fromint(0);
}

fixedpt PID_GetPIDOutput(PID_PIDTypeDef* ppid) {

    return ppid->output;
}

fixedpt PID_GetPIDRef(PID_PIDTypeDef* ppid) {
	
	return ppid->ref ;
}

void PID_SetPIDRef(PID_PIDTypeDef* ppid, fixedpt ref) {

    ppid->ref = ref;
}

void PID_SetPIDFdb(PID_PIDTypeDef* ppid, fixedpt fdb) {

    ppid->fdb = fdb;
}


void PID_ClearPID(PID_PIDTypeDef* ppid) {

//    ppid->ref = 0;
    ppid->fdb    = fixedpt_fromint(0);
    ppid->err[0] = fixedpt_fromint(0);
    ppid->err[1] = fixedpt_fromint(0);
    ppid->err[2] = fixedpt_fromint(0);
    ppid->err_lim = fixedpt_fromint(0);
    // ppid->err_fdf[0] = 0;
    // ppid->err_fdf[1] = 0;
    // ppid->err_fdf[2] = 0;
    // ppid->out_fdf    = 0;
    ppid->sumi        = fixedpt_fromint(0);
    ppid->output      = fixedpt_fromint(0);
 
}

void PID_SetPIDMax(PID_PIDparam_Typedef *pparam,fixedpt PID_Max) {
	
	pparam->max_output = PID_Max;
	
}
void PID_ClacPID(PID_PIDTypeDef* ppid, PID_PIDparam_Typedef* pparam) {

    fixedpt dError,Error;
    Error = ppid->ref - ppid->fdb;
    // char out_string[20];
    // // char sum_string[20];
    // fixedpt_str(ppid->sumi ,sum_string,-1);
	// fixedpt_str(Error,out_string,-1);
    // rt_kprintf("error %s sum %s \n",out_string,sum_string);
    ppid->err[2] = ppid->err[1];
    ppid->err[1] = ppid->err[0];
    ppid->err[0] = Error;

    dError = ppid->err[0] - ppid->err[1];
    ppid->sumi += Error;
    if(ppid->sumi >= pparam->max_sumi)
        ppid->sumi = pparam->max_sumi;
		else if(ppid->sumi <= -pparam -> max_sumi)
				ppid->sumi = -pparam->max_sumi;
			
    fixedpt temp_output;
    char out_string[20];
    // temp_output = fixedpt_mul(Error , pparam->kp) + fixedpt_mul(pparam->ki ,ppid-> sumi) +fixedpt_mul( pparam->kd , dError);
     temp_output = (Error >> FIXEDPT_FBITS)* pparam->kp + fixedpt_mul(pparam->ki ,ppid-> sumi) +fixedpt_mul( pparam->kd , dError);
    if(Error > fixedpt_fromint(4000) || Error < fixedpt_fromint(-4000)) {
         
        //    fixedpt_str(Error,out_string,-1); 
        //     rt_kprintf(" error %s ",out_string);
        //     fixedpt_str(temp_output,out_string,-1);
        //     rt_kprintf("temp_output %s \n",out_string);
        //     rt_kprintf("p %d i %d d %d\n",fixedpt_toint(pparam->kp),fixedpt_toint(pparam->ki),fixedpt_toint(pparam->kd));
    }
    if(temp_output > pparam->max_output) {
        temp_output = pparam->max_output;
        //  fixedpt_str(temp_output,out_string,-1);
            // rt_kprintf("after change temp_output %s \n",out_string);
    }
    else if(temp_output<-pparam->max_output) {
        temp_output = -pparam->max_output;
        //  fixedpt_str(temp_output,out_string,-1);
        //     rt_kprintf("after change temp_output %s \n",out_string);
    }
    
    ppid->output = temp_output;
}