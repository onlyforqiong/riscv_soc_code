#include<filter_alg.h>


fixedptd lowpass_filter(lowpass_filterdata * pdata ,fixedptd pvalue) {
	
	fixedptd temp_value ;
	pdata->value = pvalue;
	temp_value =fixedpt_mul( pdata->lowpass_param ,pdata->value) + fixedpt_mul((fixedpt_fromint(1)-pdata->lowpass_param), pdata->last_value) ;
	pdata->last_value = temp_value ;
	return temp_value ;
}
fixedptd kalman_filter(kalman_filterdata* kfp,fixedptd input){
	
	fixedptd x_mid = kfp->x_last;
	kfp->now_P = kfp->last_P + kfp->Q;
	kfp->Kg = (kfp->now_P )/(kfp->now_P + kfp->R);
	kfp->x_now = x_mid+kfp->Kg *(input - x_mid);
	//kfp->out = kfp->out + kfp->Kg *(input- kfp->out);
	kfp->last_P = (1-kfp->Kg) * kfp->now_P;
	kfp->out = kfp->x_now;
	kfp->x_last = kfp->x_now;
	return kfp->out;
}


