
#include "testView.h"
#include "Composition.h"
#include "Compositor.h"
#include "Reader.h"
#include "ExpFactory.h"
#include "Model.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define  TEST_CONTEXT  4



#if TEST_CONTEXT == 1
static const char testContext[] = \
"<h1 c=red >12����ȫ�ֿ�!</><h2 c=blue>16����ȫ�ֿ�!</>\
<h3 c=yellow>2������ɽ��!</><h4 c=gren>�ƺ��뺣������</><h3>��2��ǧ��Ŀ</><h6 c=purple>����һ��¥¥!</><h1 c=red >12����ȫ�ֿ�!</>";
#elif TEST_CONTEXT == 2
static const char testContext[] = \
"<bu><h2 c=red >ȷ��</></><bu><h2 c=red >����</></>";
#elif TEST_CONTEXT == 3

static const char testContext[] = \
"<h2 c=red >��̬</> <br/>\
<br/>\
<bu c=blue ><h2 c=red >ϵͳ��̬</></bu><bu c=blue ><h2 c=red> ��ʾ��̬</></bu><br/>\
<bu c=blue ><h2 c=red >������̬</></bu><bu c=blue ><h2 c=red> �����̬</></bu><br/>\
<bu c=blue ><h2 c=red >��¼��̬</></bu><bu c=blue ><h2 c=red> ������̬</></bu><br/>\
<bu c=blue ><h2 c=red >����̬</></bu><bu c=blue ><h2 c=red> ��ӡ��̬</></bu><br/>\
<bu c=blue ><h2 c=red >ͨ����̬</></bu><bu c=blue ><h2 c=red> ϵͳ��Ϣ</></bu><br/>\
<br/>\
<bu c=yellow><h2 c=purple>��̬�ļ�</></bu><bu c=yellow><h2 c=purple>�˳�</></bu><br/>";
#elif TEST_CONTEXT == 4
//static const char testContext[] = \
//"<title bkc=black  f=24 ali=l>����</> \
//<gr ali=m cg=2  id=0x01 > <text f=24 clr=black >Passwd:</> <rct bkc=black x=96 y=30></></gr> \
//<gr cols=2 cg=2 ls=2 f=24 bkc=black ali=m x=96 y=30 > <bu ><text >System</></bu><bu  ><text > View</></bu>\
//<bu ><text >Input</></bu><bu ><text > Output</></bu></gr>";

static   char* const testContext[2] = {	\
"<title bkc=black clr=blue f=24 xali=l>����</> \
<gr cols=2 xali=m   id=0x01 > <text f=16  yali=m clr=blue >����:</> <rct bkc=black x=126 y=30></></gr> \
<gr cols=2 cg=2 ls=2 f=16 bkc=black clr=blue xali=m x=126 y=30 >\
<bu ><text yali=m >ϵͳ����</></bu><bu  ><text yali=m > ͨ������</></bu>\
<bu ><text yali=m >��������</></bu><bu ><text yali=m > �㷨����</></bu> \
<bu ><text yali=m >��ʾ����</></bu><bu ><text yali=m > ���ݱ���</></bu>\
<bu ><text yali=m >���ݴ�ӡ</></bu><bu ><text yali=m > �˳�</></bu>\
</gr> \
<text f=16 bkc=white clr=red xali=m>��Ҫ���ݣ���רҵ��ʿ���˳�</> ", \
//��̬���沿��
"<time bndx1=180 bndx2=320 bndy1=0  bndy2=24 f=24 xali=m bkc=black clr=yellow id=0 spr=/> </time>" \


};

#endif

//ͨ��id�󶨵�ָ��
static  void * const dynId_ptr[2] = { &g_SysTime, NULL};

//����ɫ "none" ��ĳ����ɫ
#define SCREENBKC		"white"
#define LINESPACING		4
#define COLUMGRAP		0
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

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
/* Cycle/Sync Callback functions */


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

void View_test(void)
{
//	TestViewShow();
	char *pp = (void *)testContext[0];
	char	name[8];
	short		nameLen;
	short		i = 0;
	Composition *ct = Get_Composition();
	Compositor *ctor = (Compositor *)Get_SimpCtor();
	Expr *myexp ;
	
	//�����Ű���Ű��㷨
	ct->lineSpacing = LINESPACING;
	ct->columnGap = COLUMGRAP;
	ct->setCtor( ct, ctor);
	ct->setSCBkc( ct, SCREENBKC);
	ct->clean( ct);
	
	ct->dynPtr = (void *)dynId_ptr;
	
	i = 0;
	while( i < 2)
	{
		pp = (char *)testContext[i ++];
		while(1)
		{
			nameLen = 8;
			nameLen = GetName( pp, name, nameLen);
			if( nameLen == 0)
				break;
			
			myexp = ExpCreate( name);
			if( myexp == NULL)
				break;
			
				//�����Ű�
			myexp->setCtion( myexp, ct);
			myexp->setVar( myexp, name);		//����Context�еı���������
			pp = myexp->interpret( myexp, NULL, pp);
			
		}
		
	}
	
	
	
		
	ct->flush( ct);
	

}


//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static int TestView_init( View *self, void *arg)
{
	
	View_test();
	
	return RET_OK;
}

static int		TestView_show( View *self, void *arg)
{
	Model *m = ( Model *)( self->myModel);
	Composition *ct = Get_Composition();
	
	m->getMdlData( m, 0, &g_SysTime);
	
	ct->dynShow( ct);
	return RET_OK;
}

CTOR( TestView)
SUPER_CTOR( View);
FUNCTION_SETTING( View.init, TestView_init);
FUNCTION_SETTING( View.show, TestView_show);
END_CTOR













