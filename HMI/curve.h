#ifndef _INC_CURVE_H__
#define _INC_CURVE_H__
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdint.h>
#include "HMI.h"
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define	CURVE_BEEK		5		//����ƽ����
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
//���߹�����
typedef struct {
	uint8_t		num_points;
	uint8_t		current_idx;
	uint8_t		start_idx;
	uint8_t		flags;		//�Ƿ���ʾ������
	uint8_t		colour;
	uint8_t		start_x;	
	uint8_t		direct;		//0:������ 1�����ҵ���
	uint8_t		step;		//ÿ�������ƶ��ľ���
	uint8_t		points[240];
}curve_ctl_t;
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
extern curve_ctl_t		g_curve[6];
extern sheet  		*g_p_curve_bkg;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
void Curve_init(void);
void Curve_clean(curve_ctl_t *p_cctl);
void Curve_add_point(curve_ctl_t *p_cctl, int val);
void Curve_draw(curve_ctl_t *p_cctl);
void Curve_set(curve_ctl_t *p_cctl, int num, int clr, int start_x, int step);
#endif
