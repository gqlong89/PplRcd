#include "testView.h"
#include "Composition.h"
#include "Compositor.h"
#include "Reader.h"
#include "ExpFactory.h"



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
static const char testContext[] = \
"<title bkc=black  f=24 ali=l>����</> \
<input ali=m cg=2 id=0x01> <text f=24 clr=black >Passwd:</> <rct bkc=black x=96 y=30></></input>";

#endif

//����ɫ "none" ��ĳ����ɫ
#define SCREENBKC		"white"
#define LINESPACING		2
#define COLUMGRAP		0

void View_test(void)
{
//	TestViewShow();
	char *pp = (void *)testContext;
	char	name[8];
	int		nameLen;
	Composition *ct = Get_Composition();
	Compositor *ctor = (Compositor *)Get_SimpCtor();
	Expr *myexp ;
	
	//�����Ű���Ű��㷨
	ct->lineSpacing = LINESPACING;
	ct->columnGap = COLUMGRAP;
	ct->setCtor( ct, ctor);
	ct->setSCBkc( ct, SCREENBKC);
	ct->clean( ct);
	

	
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
		
	ct->flush( ct);
	

}



