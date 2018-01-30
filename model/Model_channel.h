#ifndef __INC_Model_channel_H_
#define __INC_Model_channel_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include "Model.h"
#include "system.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
//#define AUX_CHN_record_MB  					0x10
//#define AUX_CHN_lower_limit  				0x10
//#define AUX_CHN_upper_limit  				0x11
//#define AUX_CHN_small_signal  				0x12
//#define AUX_CHN_K							0x13
//#define AUX_CHN_B							0x14

#define MDHCHN_CHN_NUM		0x20
#define MAX_TOUCHSPOT		3

#define ALM_HH					0x40
#define ALM_HI					0x20
#define ALM_LO					0x10
#define ALM_LL					0x08
#define ALM_CODE_HH			1
#define ALM_CODE_HI			2
#define ALM_CODE_LO			3
#define ALM_CODE_LL			4


//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef enum {
	chnaux_record_mb = 0x10,
	chnaux_tag_NO,
	chnaux_filter_ts,
	chnaux_lower_limit,
	chnaux_upper_limit,
	chnaux_small_signal,
	chnaux_k,
	chnaux_b,
	alarm_hh,
	alarm_hi,
	alarm_lo,
	alarm_ll,
	tchspt_hh,
	tchspt_hi,
	tchspt_lo,
	tchspt_ll,
	alarm_backlash,
	DO_output
}e_chn_aux_t;
typedef enum {
	 
	
	
	
	
	
	AI_B,
	AI_E,
	AI_J,
	AI_K,	
	AI_S,
	AI_T,
	AI_Pt100,
	AI_Cu50,
	AI_0_10_mA,
	AI_4_20_mA,
	AI_0_5_V,
	AI_1_5_V,
	AI_0_20_mV,
	AI_0_100_mV,
	
	AI_0_400_ohm,
	PI_0_30_kHz,
	DI_8_30_V,
	DI_0_5_V,
	AO_4_20_mA,
	es_max,
}e_signal_t;

typedef struct {
	uint8_t			chn_NO;
	uint8_t			tag_NO;
	uint8_t			signal_type;
	uint8_t			unit;
	uint8_t			MB;			//记录容量，M
	uint8_t			filter_time_s;
	uint8_t			decimal;
	uint8_t			flag_err;		//0 无错误， 1 采样失败    8
	short				small_signal;					//10
	uint16_t		lower_limit, upper_limit;		//	14

	//K的小数点固定为2，B的小数点与采样信号类型的小数点一致
	short				k,b;				//18
	/******************************************/
	int16_t			value;					//20
	uint16_t		sample_value;				//	22
	uint8_t			smp_flag;				//23	0 采样值无效  1 采样值有效
	uint8_t			none;
}chn_info_t;

typedef struct {
	uint16_t				alarm_hh;
	uint16_t				alarm_hi;
	uint16_t				alarm_lo;
	uint16_t				alarm_ll;
	
	//报警输出触点
	uint8_t				touch_spot_hh;
	uint8_t				touch_spot_hi;
	uint8_t				touch_spot_lo;
	uint8_t				touch_spot_ll;
	
	uint8_t				alarm_backlash;		//报警回差  0 - 10.0%
	
	uint8_t				alm_flag;
	/*
	0x40:高高报
	0x20:高报
	0x10:低报
	0x08:低低报
	*/
	
	
	
	uint8_t				num_alms_in_stg;		//存储器中的报警数量
	uint8_t				none;
}chn_alarm_t;
	
typedef struct {
	uint8_t			do_chn;
	uint8_t			val;
	uint8_t			none[2];
}do_out_t;
	

typedef struct
{
	uint8_t			alarm_backlash;		//报警回差  0 - 10.0%			29
	uint8_t			tag_NO;
	uint8_t			signal_type;
	uint8_t			unit;
	uint8_t			MB;			//记录容量，M
	uint8_t			filter_time_s;
	uint8_t			decimal;
	uint8_t			none;		//0 无错误， 1 采样失败
	short				small_signal;
	int16_t			lower_limit, upper_limit;		//
	short				k,b;													//16
	
	short				alarm_hh;
	short				alarm_hi;
	short				alarm_lo;
	short				alarm_ll;											//24
	
	//报警输出触点
	uint8_t				touch_spot_hh;
	uint8_t				touch_spot_hi;
	uint8_t				touch_spot_lo;
	uint8_t				touch_spot_ll;						//28
	
//	uint8_t				alarm_backlash;		//报警回差  0 - 10.0%			29
//	uint8_t				none[3];							//32
	
}mdl_chn_save_t;		//要保存的信息

CLASS(Model_chn)
{
		
	EXTENDS( Model);
	chn_info_t		chni;
	chn_alarm_t		alarm;
	struct {
		uint8_t		alm_hh_index;
		uint8_t		alm_hi_index;
		uint8_t		alm_lo_index;
		uint8_t		alm_ll_index;
	}alarm_mgr;
	
	char 	*str_buf;
	char 	*unit_buf;
	char 	*alarm_buf;
};

//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------
extern Model		*arr_p_mdl_chn[NUM_CHANNEL];
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
Model_chn *Get_Mode_chn(int n);
int	MdlChn_save_data(uint8_t chn_num, mdl_chn_save_t *p);		//把要存储的数据放入p，返回长度
void MdlChn_default_conf(int chn_num);
void MdlChn_default_alarm(int chn_num);





#endif
