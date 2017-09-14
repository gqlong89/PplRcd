#include "real_time_trendHmi.h"
#include "menuHMI.h"
#include "HMIFactory.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "format.h"
#include "chnInfoPic.h"
#include "ModelFactory.h"


//��״ͼ��y�����ϣ���100%��ʾ�Ļ���:71 -187 
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------



#define REAKTIMEHMI_BK_PIC				0		//����ͼƬ���
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI *g_p_RLT_trendHmi;
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------





//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

static const char RT_hmi_code_bkPic[] =  {"<bpic vx0=0 vy0=0 m=0 >23</>" };
static const char RT_hmi_code_title[] =  {"<text vx0=0 vy0=4 m=0 clr=white f=24>ʵʱ����</>" };

static const char RT_hmi_code_div[] = { "<text vx0=8 vy0=36 f=16 m=0 clr=red>1</>" };

static const char RT_hmi_code_data[] = { "<text f=16 m=0 mdl=test aux=0>100</>" };

static const char RT_hmi_code_point[] = { "<point></>" };

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


	
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_RT_trendHMI( HMI *self, void *arg);
static void RT_trendHmi_InitSheet( HMI *self );
static void RT_trendHmi_HideSheet( HMI *self );

static void	RT_trendHmi_Show( HMI *self);


static void	RT_trendHmi_HitHandle( HMI *self, char *s);

//����




//����
static void RT_trendHmi_EnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg);

static int RT_trendHmi_MdlUpdata( Observer *self, void *p_srcMdl);


static void Bulid_rtCurveSheet( RLT_trendHMI *self);

static void RT_trendHmi_update(RLT_trendHMI *self);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

RLT_trendHMI *Get_RLT_trendHMI(void)
{
	static RLT_trendHMI *singal_RTTHmi = NULL;
	if( singal_RTTHmi == NULL)
	{
		singal_RTTHmi = RLT_trendHMI_new();
		g_p_RLT_trendHmi = SUPER_PTR( singal_RTTHmi, HMI);
		
	}
	
	return singal_RTTHmi;
	
}

CTOR( RLT_trendHMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_RT_trendHMI);
FUNCTION_SETTING( HMI.initSheet, RT_trendHmi_InitSheet);
FUNCTION_SETTING( HMI.hide, RT_trendHmi_HideSheet);
FUNCTION_SETTING( HMI.show, RT_trendHmi_Show);

FUNCTION_SETTING( HMI.hitHandle, RT_trendHmi_HitHandle);



FUNCTION_SETTING( shtCmd.shtExcute, RT_trendHmi_EnterCmdHdl);
FUNCTION_SETTING( Observer.update, RT_trendHmi_MdlUpdata);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_RT_trendHMI( HMI *self, void *arg)
{
	RLT_trendHMI		*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	
	p_shtctl = GetShtctl();
	
	//��ʼ������ͼƬ
	p_exp = ExpCreate( "pic");
	cthis->p_bkg = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)RT_hmi_code_bkPic, cthis->p_bkg) ;
	
	p_exp = ExpCreate( "text");
	cthis->p_title = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)RT_hmi_code_title, cthis->p_title) ;
	
	p_exp = ExpCreate( "text");
	cthis->p_div = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)RT_hmi_code_div, cthis->p_div) ;
	
	Bulid_rtCurveSheet(cthis);

	//��ʼ������
	self->init_focus(self);
	
	
	return RET_OK;

}



static void RT_trendHmi_InitSheet( HMI *self )
{
	RLT_trendHMI			*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	int i,  h = 0;

	Sheet_updown( cthis->p_bkg, h++);
	Sheet_updown( cthis->p_title, h++);
	Sheet_updown( cthis->p_div, h++);
//	for( i = 0; i < BARHMI_NUM_BARS; i++) {
//		Sheet_updown( cthis->arr_p_sht_data[i], h++);
//	}
//	
	
}

static void RT_trendHmi_HideSheet( HMI *self )
{
	RLT_trendHMI			*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	
	int i;
	

	
//	for( i = BARHMI_NUM_BARS - 1; i >= 0; i--) {
//		Sheet_updown( cthis->arr_p_sht_data[i], -1);
//	}
	Sheet_updown( cthis->p_div, -1);
	Sheet_updown( cthis->p_title, -1);
	Sheet_updown( cthis->p_bkg, -1);
//	self->clear_focus(self, 0, 0);
}	





static void	RT_trendHmi_Show( HMI *self )
{
	RLT_trendHMI		*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	g_p_curHmi = self;
	
	
	RT_trendHmi_update( cthis);
	Sheet_refresh( cthis->p_bkg);
//	self->show_focus( self, 0, 0);
}

static void	RT_trendHmi_HitHandle( HMI *self, char *s)
{
//	RLT_trendHMI		*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
//	shtCmd		*p_cmd;


	if( !strcmp( s, HMIKEY_ENTER))
	{
		self->switchHMI(self, g_p_HMI_menu);
		
	}
	if( !strcmp( s, HMIKEY_ESC))
	{
		self->switchBack(self);
	}
	
}



static void RT_trendHmi_EnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg)
{
	RLT_trendHMI	*cthis = SUB_PTR( self, shtCmd, RLT_trendHMI);
	HMI		*selfHmi = SUPER_PTR( cthis, HMI);
	HMI		*srcHmi = ( HMI *)arg;
	
	srcHmi->switchHMI( srcHmi, selfHmi);
	
}

//������ʾ��������������ʵ�λ��
//��������
static void RT_trendHmi_update(RLT_trendHMI *self)
{
//	uint8_t		up_y = 30;
//	uint8_t		right_x = 160;
//	uint8_t		box_sizey = 70;		
//	uint8_t		box_sizex = 160;		
//	
//	//�����ܱ߽�Ŀ�϶
//	uint8_t		space_to_up = 		2;	
//	uint8_t		space_to_bottom = 	2;
//	uint8_t		space_to_left = 	8;	
//	uint8_t		space_to_right = 	8;
//	
//	char 			i = 0, j = 0;
//	uint16_t 		sizex = 0;
//	uint16_t 		sizey = 0;
//	sheet			*p_sht;
//	
//	//ˢ��һ������,����0 - 10000�������, ����ʱʹ��
////	self->arr_p_sht_data[i]->p_mdl->getMdlData( self->arr_p_sht_data[i]->p_mdl, 10000, NULL);		
//	
//	for( i = 0; i < 3; i++) { 
//		for( j = 0; j < 2; j++) {
//			
//			//������ֵ
//			p_sht = self->arr_p_sht_data[i * 2 + j];
//			p_sht->cnt.data = \
//				p_sht->p_mdl->to_string( p_sht->p_mdl, p_sht->cnt.mdl_aux, NULL);
//			p_sht->cnt.len = strlen( p_sht->cnt.data);
//			p_sht->p_gp->getSize( p_sht->p_gp, p_sht->cnt.font, &sizex, &sizey);
//			sizex = sizex * p_sht->cnt.len;	

//			p_sht->area.x0 = right_x +  (j ) * box_sizex - space_to_right - sizex;
//			p_sht->area.y0 = up_y + i * box_sizey + space_to_up;
//			
//			
//			
//			//���㵥λ������
//			p_sht = self->arr_p_sht_unit[i * 2 + j];
//			p_sht->cnt.data = \
//				p_sht->p_mdl->to_string( p_sht->p_mdl, p_sht->cnt.mdl_aux, NULL);
//			p_sht->cnt.len = strlen( p_sht->cnt.data);
//			p_sht->p_gp->getSize( p_sht->p_gp, p_sht->cnt.font, &sizex, &sizey);
//			sizex = sizex * p_sht->cnt.len;	

//			p_sht->area.x0 = right_x +  j * box_sizex - space_to_right - sizex;
//			p_sht->area.y0 = up_y + ( i + 1)* box_sizey - space_to_bottom - sizey;
//			
//			//���㱨����Ϣ������
//			
//			p_sht = self->arr_p_sht_alarm[i * 2 + j];
//			p_sht->cnt.data = \
//				p_sht->p_mdl->to_string( p_sht->p_mdl, p_sht->cnt.mdl_aux, NULL);
//			p_sht->cnt.len = strlen( p_sht->cnt.data);
//			p_sht->p_gp->getSize( p_sht->p_gp, p_sht->cnt.font, &sizex, &sizey);
//			sizex = sizex * p_sht->cnt.len;	
//			//������Ϣ������ʾ
//			p_sht->area.x0 = j * box_sizex + space_to_left;
//			p_sht->area.y0 = up_y + ( i + 1)* box_sizey - space_to_bottom - sizey;
//		}
//			
//	}
	
}

// 

static void Bulid_rtCurveSheet( RLT_trendHMI *self)
{
	
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	Model			*p_mdl = NULL;
	short 			i;
	
	
	
	p_shtctl = GetShtctl();

	for( i = 0; i < BARHMI_NUM_BARS; i++) {
		
		
		p_exp = ExpCreate("text");
		self->arr_p_sht_data[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)RT_hmi_code_data, self->arr_p_sht_data[i]) ;
		
//		self->arr_p_sht_data[i]->p_mdl->attach( self->arr_p_sht_data[i]->p_mdl, ( Observer *)self->arr_p_sht_data[i]);		
		self->arr_p_sht_data[i]->cnt.colour = arr_clrs[i];
		
	}
//	p_exp = ExpCreate("point");
//	self->p_point = Sheet_alloc( p_shtctl);
//	p_exp->inptSht( p_exp, (void *)RT_hmi_code_point, self->p_point) ;
//	
//	//todo: �ĳ�ͨ��
//	p_mdl = ModelCreate("test");
//	p_mdl->attach(p_mdl, &self->Observer);
	
}


static int RT_trendHmi_MdlUpdata( Observer *self, void *p_srcMdl)
{
//	RLT_trendHMI *cthis = SUB_PTR( self, Observer, RLT_trendHMI);
//	Model	*mdl = (Model *)p_srcMdl;
//	
//	
//	DataHmi_update( cthis);
//	
//	Sheet_refresh( cthis->arr_p_sht_data[0]);
//	
	return RET_OK;
	
}


