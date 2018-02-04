//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "Component_curve.h"
#include "ExpFactory.h"
#include "HMI.h"
#include "commHMI.h"
#include "sdhDef.h"
#include "arithmetic/bit.h"
#include "mem/CiiMem.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------


#define CRV_FLAG_HIDE				1
#define CRV_FLAG_FULL_CLEAN	2		//�����������������ʱ�����ȫ���������ݱ�־


#define	CRV_BEEK		5		//����ƽ����
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

static const char CRV_code_line[] = { "<line></>" };
static const char CRV_code_bk_pic[] =  {"<bpic vx0=0 vy0=50 m=1 >15</>" };
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

		
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static Curve 	*p_CRV_self = NULL;
static sheet	*p_CRV_line;
static sheet	*p_CRV_bk_pic;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void 	CRV_Init(Curve *self, uint8_t	num_curve);
static int 		CRV_Alloc(curve_att_t  *c);
static void		CRV_Free(uint8_t  crv_fd);
static void		CRV_Reset(uint8_t  crv_fd);
static void		CRV_Add_point(uint8_t  crv_fd, crv_val_t *cv);
static void		CRV_Ctl(uint8_t  crv_fd, uint8_t	ctl, uint16_t val);
static void		CRV_Show_bkg(void);
static void		CRV_Show_curve(uint8_t  crv_fd, uint8_t show_ctl);



static void CRV_Alloc_bkg_id(uint8_t crv_fd);
static void CRV_Set_dirty_by_bkg_id(uint8_t bkg_id, uint8_t val);
static void CRV_Copy_att(curve_att_t *dst, curve_att_t *src);
static void CRV_Deal_full(uint8_t crv_fd);
static void CRV_Exc_display(uint8_t  crv_fd, uint8_t show_ctl);
static void CRV_Clean_bkg(uint8_t	crv_fd);
static void CRV_Draw_left_to_right(uint8_t  crv_fd, uint8_t show_ctl);
static void CRV_Draw_right_to_left(uint8_t  crv_fd, uint8_t show_ctl);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
Curve	*CRV_Get_Sington(void)
{
	
	if(p_CRV_self == NULL)
	{
		p_CRV_self = Curve_new();
		if(p_CRV_self  == NULL) while(1);
	}
	
	return p_CRV_self;
	
}




CTOR(Curve)
FUNCTION_SETTING(init, CRV_Init);
FUNCTION_SETTING(alloc, CRV_Alloc);
FUNCTION_SETTING(free, CRV_Free);
FUNCTION_SETTING(reset, CRV_Reset);
FUNCTION_SETTING(add_point, CRV_Add_point);
FUNCTION_SETTING(crv_ctl, CRV_Ctl);
FUNCTION_SETTING(crv_show_bkg, CRV_Show_bkg);
FUNCTION_SETTING(crv_show_curve, CRV_Show_curve);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static void 	CRV_Init(Curve *self, uint8_t	num_curve)
{
	Expr 			*p_exp ;
	shtctl 		*p_shtctl = NULL;
	
	p_shtctl = GetShtctl();
	p_exp = ExpCreate( "line");
	p_CRV_line = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)CRV_code_line, p_CRV_line) ;
	
	p_exp = ExpCreate( "pic");
	p_CRV_bk_pic = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)CRV_code_bk_pic, p_CRV_bk_pic);
	
	
	
	self->num_curve = num_curve;
	self->set_free_curve = 0xff;
	self->set_vaild_curve = 0;
	self->p_crv_att = CALLOC(num_curve, sizeof(curve_att_t));
	self->p_run_info = CALLOC(num_curve, sizeof(crv_run_info_t));
	self->p_bkg_info = CALLOC(num_curve, sizeof(crv_back_ground_info_t));
}

//ʧ�ܷ���-1
static int CRV_Alloc(curve_att_t  *c)
{
	uint16_t		i = 0;

	
	for(i = 0; i < p_CRV_self->num_curve; i ++)
	{
		if(Check_bit(&p_CRV_self->set_free_curve, i))
		{
			Clear_bit(&p_CRV_self->set_free_curve, i);
			Set_bit(&p_CRV_self->set_vaild_curve, i);
			CRV_Copy_att(p_CRV_self->p_crv_att + i, c);
			p_CRV_self->p_run_info[i].crv_size = (c->crv_x1 - c->crv_x0 + 1) / c->crv_step_pix;
			p_CRV_self->p_run_info[i].p_vals_y = VRAM_alloc(p_CRV_self->p_run_info[i].crv_size);
			p_CRV_self->p_run_info[i].cur_index = 0;
			p_CRV_self->p_run_info[i].crv_start_index = 0;
			p_CRV_self->p_crv_att[i].crv_flag |= CRV_FLAG_FULL_CLEAN;		//Ĭ������£��������˾����
			CRV_Alloc_bkg_id(i);
			return i;
			
		}
	}	
	
	return -1;
	
}

static void		CRV_Free(uint8_t  crv_fd)
{
	int	i = 0;
	
	for(i = 0; i < p_CRV_self->num_curve; i++)
	{
		if((i == crv_fd) || (crv_fd == HMI_CMP_ALL))
		{
			Clear_bit(&p_CRV_self->set_vaild_curve, i);
			Set_bit(&p_CRV_self->set_free_curve, i);
			p_CRV_self->p_crv_att[i].crv_flag = 0;
		}	
	}
}
static void		CRV_Reset(uint8_t  crv_fd)
{
	if(crv_fd >= p_CRV_self->num_curve)
			return;
	
	p_CRV_self->p_run_info[crv_fd].cur_index = 0;
	p_CRV_self->p_run_info[crv_fd].crv_num_points = 0;
	
}


static void		CRV_Add_point(uint8_t  crv_fd, crv_val_t *cv)
{
	uint16_t		val_y = 0;
	uint16_t		range;
	uint16_t		height;
			
	if(crv_fd >= p_CRV_self->num_curve)
			return;
	

	
	
	CRV_Deal_full(crv_fd);
		
	if(cv->val <= cv->lower_limit)
		cv->prc = 0;
	else if(cv->val >= cv->up_limit)
		cv->prc = 100;
	else
	{
		cv->prc = (cv->prc - cv->lower_limit) / (cv->up_limit - cv->lower_limit);
		
	}
	
	range = p_CRV_self->p_crv_att[crv_fd].crv_y1 - p_CRV_self->p_crv_att[crv_fd].crv_y0;
	height = cv->prc * 100 / range;
	val_y = p_CRV_self->p_crv_att[crv_fd].crv_y0 + height;
	
	
	
	p_CRV_self->p_run_info[crv_fd].p_vals_y[p_CRV_self->p_run_info[crv_fd].cur_index] = val_y;
	
	p_CRV_self->p_run_info[crv_fd].cur_index += 1;
	p_CRV_self->p_run_info[crv_fd].cur_index %= p_CRV_self->p_run_info[crv_fd].crv_size;
	
	p_CRV_self->p_run_info[crv_fd].crv_num_points ++;
	
	if(p_CRV_self->p_crv_att[crv_fd].crv_direction == HMI_DIR_LEFT)
		CRV_Set_dirty_by_bkg_id(p_CRV_self->p_bkg_info[crv_fd].crv_bkg_id, 1);

}
static void		CRV_Ctl(uint8_t  crv_fd, uint8_t	ctl, uint16_t val)
{
//	crv_run_info_t					*p_run = p_CRV_self->p_run_info + crv_fd;
	curve_att_t							*p_att = p_CRV_self->p_crv_att + crv_fd;
	crv_back_ground_info_t	*p_bkg =  p_CRV_self->p_bkg_info + crv_fd;
	switch(ctl)
	{
		case CRV_CTL_HIDE:
			if(val)
				p_att->crv_flag |= CRV_FLAG_HIDE;
			else
				p_att->crv_flag &= ~CRV_FLAG_HIDE;
			CRV_Set_dirty_by_bkg_id(p_bkg->crv_bkg_id, 1);
			break;
		case CRV_CTL_STEP_PIX:
			if(val)
			{
				p_att->crv_step_pix = val;
				CRV_Set_dirty_by_bkg_id(p_bkg->crv_bkg_id, 1);
			}
			break;
		
		default:
			break;
		
		
	}
	
}
static void		CRV_Show_bkg(void)
{
	
	p_CRV_bk_pic->p_gp->vdraw(p_CRV_bk_pic->p_gp, &p_CRV_bk_pic->cnt, &p_CRV_bk_pic->area);
}


static void CRV_Alloc_bkg_id(uint8_t crv_fd)
{
//	crv_run_info_t					*p_run = p_CRV_self->p_run_info + crv_fd;
//	curve_att_t							*p_att = p_CRV_self->p_crv_att + crv_fd;
//	crv_back_ground_info_t	*p_bkg;
//	short	i;
//	short	used_id = 0;
//	for(i = 0; i < p_CRV_self->num_curve; i++)
//	{
//		if(Check_bit(&p_CRV_self->set_vaild_curve, i) == 0)
//			continue;
//		
//		
//		
//	}
	
	//todo: Ŀǰ�����ߵı����϶���һ���ģ���˾Ͳ�������������ˣ�����Ϊ0,�Ժ�����Ҫ��������ʾ�ڲ�ͬ������ʱ����ʵ���㷨
	
	p_CRV_self->p_bkg_info[crv_fd].crv_bkg_id = 0;
	
}

static void CRV_Copy_att(curve_att_t *dst, curve_att_t *src)
{
	
	dst->crv_col = src->crv_col;
	dst->crv_direction = src->crv_direction;
	dst->crv_step_pix = src->crv_step_pix;
	dst->crv_flag = src->crv_flag;
	dst->crv_x0 = src->crv_x0;
	dst->crv_x1 = src->crv_x1;
	dst->crv_y0 = src->crv_y0;
	dst->crv_y1 = src->crv_y0;
	
}

static void CRV_Deal_full(uint8_t crv_fd)
{
	
	if(p_CRV_self->p_run_info[crv_fd].crv_num_points ==  p_CRV_self->p_run_info[crv_fd].crv_size)
		return;
	
	//���ߵ������Ժ������ȫ���ĵ����»��ƻ���������ǵ���
	if(p_CRV_self->p_crv_att[crv_fd].crv_flag & CRV_FLAG_FULL_CLEAN)
	{
		CRV_Set_dirty_by_bkg_id(p_CRV_self->p_bkg_info[crv_fd].crv_bkg_id, 1);
		//���������
		p_CRV_self->p_run_info[crv_fd].cur_index = 0;
		p_CRV_self->p_run_info[crv_fd].crv_num_points = 0;
	}
	else
	{
		//ֻ�Ƴ�������Ǹ���,��������
		p_CRV_self->p_run_info[crv_fd].cur_index += 1;
		p_CRV_self->p_run_info[crv_fd].cur_index %= p_CRV_self->p_run_info[crv_fd].crv_size;
		//�ƶ������ߵ���ʼ����
		p_CRV_self->p_run_info[crv_fd].crv_start_index ++;
		p_CRV_self->p_run_info[crv_fd].crv_start_index %= p_CRV_self->p_run_info[crv_fd].crv_size;
	}
	
	
}

static void		CRV_Show_curve(uint8_t  crv_fd, uint8_t show_ctl)
{
	
	int	i = 0;
	
	if(show_ctl == CRV_SHOW_WHOLE)		
		CRV_Set_dirty_by_bkg_id(HMI_CMP_ALL, 1);
	
	for(i = 0; i < p_CRV_self->num_curve; i++)
	{
		if((i != crv_fd) && (crv_fd != HMI_CMP_ALL))
		{
			continue;
			
		}
		if(Check_bit(&p_CRV_self->set_vaild_curve, i) == 0)
			continue;
		if(p_CRV_self->p_crv_att[i].crv_flag & CRV_FLAG_HIDE)
			continue;
		
		//��ʾ������
		CRV_Exc_display(i, show_ctl);
		
		
	}
	
		
	
	
}
static void CRV_Exc_display(uint8_t  crv_fd, uint8_t show_ctl)
{
	if((p_CRV_self->p_bkg_info[crv_fd].crv_bkg_dirty))
	{
		CRV_Clean_bkg(crv_fd);
		
	}
	
	p_CRV_line->cnt.colour = p_CRV_self->p_crv_att[crv_fd].crv_col;
	
	//���ֻ��һ����ͻ�һ����
	if(p_CRV_self->p_run_info[crv_fd].crv_num_points == 1) {
		p_CRV_line->area.x0 = p_CRV_self->p_crv_att[crv_fd].crv_x0;
		p_CRV_line->area.x1 = p_CRV_self->p_crv_att[crv_fd].crv_x0;
		p_CRV_line->area.y0 = p_CRV_self->p_run_info[crv_fd].p_vals_y[p_CRV_self->p_run_info[crv_fd].crv_start_index];
		p_CRV_line->area.y1 = p_CRV_line->area.y0;
		
		p_CRV_line->p_gp->vdraw(p_CRV_line->p_gp, &p_CRV_line->cnt, &p_CRV_line->area);
		
		return;
	}
	
	if(p_CRV_self->p_crv_att[crv_fd].crv_direction == HMI_DIR_LEFT)
	{
		CRV_Draw_right_to_left(crv_fd, show_ctl);
	}
	else if(p_CRV_self->p_crv_att[crv_fd].crv_direction == HMI_DIR_RIGHT)
	{
		
		CRV_Draw_left_to_right(crv_fd, show_ctl);
		
	}
	
	
}

static void CRV_Set_dirty_by_bkg_id(uint8_t bkg_id, uint8_t val)
{
	int		i;
	for(i = 0; i < p_CRV_self->num_curve; i++)
	{
		if(Check_bit(&p_CRV_self->set_vaild_curve, i) == 0)
			continue;
		
		if((i != bkg_id) && (bkg_id != HMI_CMP_ALL))
			continue;
		p_CRV_self->p_bkg_info[i].crv_bkg_dirty = val;
	}
	
	
}


static void CRV_Clean_bkg(uint8_t	crv_fd)
{
	p_CRV_bk_pic->p_gp->vdraw(p_CRV_bk_pic->p_gp, &p_CRV_bk_pic->cnt, &p_CRV_bk_pic->area);
	
	//��������ͬid�ı��������־����
	CRV_Set_dirty_by_bkg_id(p_CRV_self->p_bkg_info[crv_fd].crv_bkg_id, 0);
}

static int CRV_Rle(uint8_t  crv_fd, int cur_idx)
{
	crv_run_info_t	*p_run = p_CRV_self->p_run_info + crv_fd;
	int 	len = 0;
	short 	i = 0;
	short	points = 0;
	short	idx = 0;
	
	points = p_run->crv_num_points;
	
	//��cur_idx֮�����еĵ���м���
	for(i = 1; i < (points); i+= 1) {
		
		idx =cur_idx + i;
		idx %= p_run->crv_size;
		
		if(abs(p_run->p_vals_y[cur_idx] - p_run->p_vals_y[idx]) < CRV_BEEK)
			len ++;
		else
			break;
		
		
		
		
	}
	if(len == 0)
		len = 1;
	return len;
}


//����������ʾ�����ߣ���Ҫ�����ػ��
static void CRV_Draw_left_to_right(uint8_t  crv_fd, uint8_t show_ctl)
{
	short 	i = 0;
	short		points = 0;
	short		idx = 0;
	short		len = 0;
	crv_run_info_t	*p_run = p_CRV_self->p_run_info + crv_fd;
	curve_att_t			*p_att = p_CRV_self->p_crv_att + crv_fd;
	
	points = p_run->crv_num_points;
	
	for(i = 0; i < (points - 1); i+= len) {
		//����x0,y0
		p_CRV_line->area.x0 = p_att->crv_x1 - (points - 1 - i) * p_att->crv_step_pix;
		idx = p_run->crv_start_index + i;
		idx %= p_run->crv_size;
		p_CRV_line->area.y0 = p_run->p_vals_y[idx];
		
		//�������x1,y1������

		len = CRV_Rle(crv_fd, idx);
		//����x1, y1
		p_CRV_line->area.x1 = p_CRV_line->area.x0 + p_att->crv_step_pix * (len + 0);
		idx += len;
		idx %= p_run->crv_size;
		p_CRV_line->area.y1 = p_run->p_vals_y[idx];
		
		p_CRV_line->p_gp->vdraw(p_CRV_line->p_gp, &p_CRV_line->cnt, &p_CRV_line->area);
	}
}


//�����������1��ʱ���ܵ��ã���������
//���������������Ƿ����1
static void CRV_Draw_right_to_left(uint8_t  crv_fd, uint8_t show_ctl)
{
	crv_run_info_t	*p_run = p_CRV_self->p_run_info + crv_fd;
	curve_att_t			*p_att = p_CRV_self->p_crv_att + crv_fd;
	short 	i = 0;
	short		points = 0;
	short		idx = 0;
	short		len = 0;
	
	
	if(show_ctl == CRV_SHOW_LATEST)
	{
		
	
		p_CRV_line->cnt.colour = p_att->crv_col;
		
		p_CRV_line->area.x0 = p_att->crv_x0 + p_att->crv_step_pix * (p_run->cur_index - 2);
		p_CRV_line->area.x1 = p_CRV_line->area.x0 + p_att->crv_step_pix;
		
		p_CRV_line->area.y0 = p_run->p_vals_y[p_run->cur_index - 2];
		p_CRV_line->area.y1 = p_run->p_vals_y[p_run->cur_index - 1];
		p_CRV_line->p_gp->vdraw(p_CRV_line->p_gp, &p_CRV_line->cnt, &p_CRV_line->area);		
		return;
	}
	
	
	points = p_run->crv_num_points;
	
	for(i = 0; i < (points - 1); i+= len) {
		//����x0,y0
		p_CRV_line->area.x0 = p_att->crv_x0 +  i * p_att->crv_step_pix;
		idx = p_run->crv_start_index + i;
		idx %= p_run->crv_size;
		p_CRV_line->area.y0 = p_run->p_vals_y[idx];
		
		//�������x1,y1������

		len = CRV_Rle(crv_fd, idx);
		//����x1, y1
		p_CRV_line->area.x1 = p_CRV_line->area.x0 + p_att->crv_step_pix * (len + 0);
		idx += len;
		idx %= p_run->crv_size;
		p_CRV_line->area.y1 = p_run->p_vals_y[idx];
		p_CRV_line->p_gp->vdraw(p_CRV_line->p_gp, &p_CRV_line->cnt, &p_CRV_line->area);
		
	}
	
	
}





