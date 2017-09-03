#include "commHMI.h"
#include <string.h>
#include "ExpFactory.h"
#include "HMIFactory.h"

#include "utils/time.h"
#include "format.h"



//�ṩ �������¼�����Ϣ�����ڣ�������ʱ�䣬��ѡ���ͼ��
//��Щͼ����ܻᱻ����������ʹ��
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------
sheet			*g_p_shtTime;
sheet			*g_p_ico_memu;
sheet			*g_p_ico_bar;
sheet			*g_p_ico_digital;
sheet			*g_p_ico_trend;

hmiAtt_t CmmHmiAtt = { 10,1, COLOUR_BLACK, 4, 2};
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define TIME_BUF_LEN		16


const char timeCode[] = { "<time vx0=240 vy0=0 bx=60  by=24 f=24 xali=m bkc=black clr=yellow spr=/> </time>" };

const char ico_memu[] = { "<bu vx0=10 vy0=206 bx=33 by=33 bkc=black clr=black><pic  bx=32  by=32 >1</></bu>" };
//�����ͼͼ��
const char ico_bar[] = { "<bu vx0=50 vy0=206 bx=33 by=33 bkc=black clr=black><pic  bx=32  by=32 >2</></bu>" };
//�������Ի���ͼ��
const char ico_digital[] = { "<bu vx0=90 vy0=206 bx=33 by=33bkc=black clr=black><pic  bx=32  by=32 >3</></bu>" };
//�������ƻ���ͼ��
const char ico_trend[] = { "<bu vx0=130 vy0=206 bx=33 by=33 bkc=black clr=black><pic  bx=32  by=32 >4</></bu>" };

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static cmmHmi *singalCmmHmi;
static char s_timer[TIME_BUF_LEN];
static struct  tm time;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_cmmHmi( HMI *self, void *arg);
static void Timer2str( struct  tm *p_tm, char *s, int n);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
cmmHmi *GetCmmHMI(void)
{
	if( singalCmmHmi == NULL)
	{
		singalCmmHmi = cmmHmi_new();
		
	}
	
	return singalCmmHmi;
	
}


CTOR( cmmHmi)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_cmmHmi);
//FUNCTION_SETTING( View.show, TestView_show);
END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_cmmHmi( HMI *self, void *arg)
{
//	cmmHmi	*cthis = SUB_PTR( self, HMI, cmmHmi);
	HMI 		*p_hmi;
	menuHMI		*menuHmi ;
	barGhHMI	*barHmi ;
	shtctl *p_shtctl = NULL;
	Expr *p_exp ;
	
	
	//��ʼ������ͼ��
	p_shtctl = GetShtctl();
	g_p_shtTime = Sheet_alloc( p_shtctl);
	
	//timer
	p_exp = ExpCreate( "text");
	p_exp->inptSht( p_exp, (void *)timeCode, g_p_shtTime) ;
	
	g_p_shtTime->p_mdl = ModelCreate("time");
	g_p_shtTime->p_mdl->attach( g_p_shtTime->p_mdl, (Observer *)g_p_shtTime);
	g_p_shtTime->p_mdl->getMdlData( g_p_shtTime->p_mdl, 0, &time);

	Timer2str( &time, s_timer, TIME_BUF_LEN);
	g_p_shtTime->cnt.data = s_timer;
	g_p_shtTime->cnt.len = strlen( s_timer);
	
	//ͼ���ʼ��
	p_exp = ExpCreate( "bu");

	
	//��ʼ������ͼ��
	g_p_ico_memu  = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)ico_memu, g_p_ico_memu) ;
	g_p_ico_memu->area.x1 = g_p_ico_memu->area.x0 + g_p_ico_memu->bxsize;
	g_p_ico_memu->area.y1 = g_p_ico_memu->area.y0 + g_p_ico_memu->bysize;
	FormatSheetSub( g_p_ico_memu);
	
	
	g_p_ico_bar  = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)ico_bar, g_p_ico_bar) ;
	g_p_ico_bar->area.x1 = g_p_ico_bar->area.x0 + g_p_ico_bar->bxsize;
	g_p_ico_bar->area.y1 = g_p_ico_bar->area.y0 + g_p_ico_bar->bysize;
	FormatSheetSub( g_p_ico_bar);
	
	
	g_p_ico_digital  = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)ico_digital, g_p_ico_digital) ;
	g_p_ico_digital->area.x1 = g_p_ico_digital->area.x0 + g_p_ico_bar->bxsize;
	g_p_ico_digital->area.y1 = g_p_ico_digital->area.y0 + g_p_ico_bar->bysize;
	FormatSheetSub( g_p_ico_digital);
	
	g_p_ico_trend  = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)ico_trend, g_p_ico_trend) ;
	g_p_ico_trend->area.x1 = g_p_ico_trend->area.x0 + g_p_ico_trend->bxsize;
	g_p_ico_trend->area.y1 = g_p_ico_trend->area.y0 + g_p_ico_trend->bysize;
	FormatSheetSub( g_p_ico_trend);
	
	
	//�����빫��ͼ����صĽ���
	p_hmi = CreateHMI( HMI_MENU);
	p_hmi->init( p_hmi, NULL);
	menuHmi = SUB_PTR( p_hmi, HMI, menuHMI);
	
	p_hmi = CreateHMI( HMI_BAR);
	p_hmi->init( p_hmi, NULL);
	barHmi = SUB_PTR( p_hmi, HMI, barGhHMI);
	
	
	//��ͼ�궯������ؽ��洦���
	g_p_ico_memu->p_enterCmd = &menuHmi->shtCmd;
	g_p_ico_bar->p_enterCmd = &barHmi->shtCmd;
	
	return RET_OK;
}



static void Timer2str( struct  tm *p_tm, char *s, int n)
{
	
	
	snprintf( s, n, "%02d:%02d:%02d", p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
	
}













