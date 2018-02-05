
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_Component_curve_H__
#define __INC_Component_curve_H__

#include <stdint.h>
#include <string.h>
#include "lw_oopc.h"

#include "sdhDef.h"
#include "dev_lcd.h"
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define CRV_CTL_HIDE					0
#define CRV_CTL_STEP_PIX				1
//#define CRV_CTL_DIRTY					2

//������ʾ����
#define	CRV_SHOW_WHOLE				0		//�������߶���ʾ
#define	CRV_SHOW_LATEST				1		//ֻ��ʾ������ӵĵ�
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------


typedef struct
{
	uint8_t					crv_col;
	uint8_t					crv_direction;
	uint8_t					crv_step_pix;		//
	uint8_t					crv_flag;
	uint16_t				crv_x0,crv_y0;
	uint16_t				crv_x1,crv_y1;
}curve_att_t;

typedef struct {
	uint16_t		up_limit;
	uint16_t		lower_limit;
	uint16_t		val;
	uint16_t		prc;
}crv_val_t;
typedef struct {
	uint8_t			*p_vals_y;
	uint16_t		crv_size;
	uint16_t		crv_num_points;		//���еĵ���
	uint16_t		cur_index;
	//��ǰ���ߵ���ʼ�����������������Ժ����������£���ʵ�����Żᷢ���仯
	//����������ߵ�����£�curve_start_indexʼ��Ϊ0
	uint16_t		crv_start_index;		
}crv_run_info_t;

typedef struct {
	uint8_t				crv_bkg_dirty;
	uint8_t				crv_bkg_id;		//�������������һ������ô���ֵ�Ͳ�һ��
	uint8_t				none[2];
}crv_back_ground_info_t;

CLASS(Curve)
{
	uint8_t		set_free_curve;
	uint8_t		set_vaild_curve;
	uint8_t		num_curve;
	uint8_t		total_pix;

	curve_att_t							*p_crv_att;
	crv_run_info_t					*p_run_info;
	crv_back_ground_info_t	*p_bkg_info;
	void 		(*init)(Curve *self, uint8_t	num_curve);
	int			(*alloc)(curve_att_t  *c);
	void		(*free)(uint8_t  crv_fd);
	void		(*reset)(uint8_t  crv_fd);
	void		(*add_point)(uint8_t  crv_fd, crv_val_t *cv);
	void		(*crv_ctl)(uint8_t  crv_fd, uint8_t	ctl, uint16_t val);
	void		(*crv_show_bkg)(void);
	void		(*crv_show_curve)(uint8_t  crv_fd, uint8_t show_ctl);

};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
Curve	*CRV_Get_Sington(void);


#endif

