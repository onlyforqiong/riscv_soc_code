#ifndef __FILTER_ALG_H
#define __FILTER_ALG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtthread.h>
#include<rtfixpoint.h>


typedef struct 
	{
		fixedptd lowpass_param;
		fixedptd last_value;
		fixedptd value;
	} lowpass_filterdata;
	
typedef struct{
	
	fixedptd last_P;//上次估算协方差，初始化值为0.02
	fixedptd now_P;//档次估算协方差，初始化值为0

	fixedptd out;//滤波器输出，初始化值为0
	fixedptd Kg;//卡尔曼增益，初始化为0
	fixedptd Q;//过程噪声协方差，初始化值为0.001
	fixedptd R;//观测噪声协方差，初始化值为0.543
		fixedptd x_last;
	fixedptd x_now;
}kalman_filterdata;

fixedptd lowpass_filter(lowpass_filterdata * pdata ,fixedptd pvalue) ;
fixedptd kalman_filter(kalman_filterdata* kfp,fixedptd input);


#ifdef __cplusplus
}
#endif

#endif



