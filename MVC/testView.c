#include "testView.h"
#include "Composition.h"
#include "Compositor.h"
#include "Reader.h"
#include "ExpFactory.h"



#define  TEST_CONTEXT  2



#if TEST_CONTEXT == 1
static const char testContext[] = \
"<h1 c=red >12����ȫ�ֿ�!</> <h2 c=blue>16����ȫ�ֿ�!</>\
<h3 c=yellow>2������ɽ��!</><h4 c=gren>�ƺ��뺣������</><h5>��2��ǧ��Ŀ</><h6 c=purple>����һ��¥¥!</><h1 c=red >12����ȫ�ֿ�!</>";
#elif TEST_CONTEXT == 2
static const char testContext[] = \
"<bu><h2 c=red >ȷ��</></><bu><h2 c=red >����</></>";


#endif




void View_test(void)
{
//	TestViewShow();
	void *pp = (void *)testContext;
	char	name[8];
	int		nameLen;
	Composition *ct = Get_Composition();
	Compositor *ctor = (Compositor *)Get_SimpCtor();
	Expr *myexp ;
	
	//�����Ű���Ű��㷨
	ct->setCtor( ct, ctor);
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



