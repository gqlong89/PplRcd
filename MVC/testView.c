#include "testView.h"
#include "Composition.h"
#include "Compositor.h"
#include "Reader.h"
#include "ExpFactory.h"

static const char testContext[] = \
"<h1 c=red >12����ȫ�ֿ�!</> <h2 c=blue>16����ȫ�ֿ�!</>\
<h3 c=yellow>2������ɽ��!</><h4 c=gren>�ƺ��뺣������</><h5>��2��ǧ��Ŀ</><h6 c=purple>����һ��¥¥!</><h1 c=red >12����ȫ�ֿ�!</>";



void View_test(void)
{
//	TestViewShow();
	void *pp = (void *)testContext;
	void *pnew;
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
		pp = myexp->interpret( myexp, pp);
		
	}
		
	ct->flush( ct);
	
//	myexp->setVar( myexp, "h2");		//����Context�еı���������
//	pnew = myexp->interpret( myexp, pp);
//	
//	myexp->setVar( myexp, "h2");		
//	pnew = myexp->interpret( myexp, pnew);
//	
//	myexp->setVar( myexp, "h3");		
//	pnew = myexp->interpret( myexp, pnew);
//	
//	myexp->setVar( myexp, "h2");		
//	pnew = myexp->interpret( myexp, pnew);
}



