#include "testView.h"
#include "Composition.h"
#include "Compositor.h"
#include "Reader.h"
#include "ExpFactory.h"
 
static const char testContext[] = \
"<h5>screen char test1!</> <h4>screen char test2!</>\
<h3>screen char test3!</><h2>screen char test4!</><h1>screen char test5!</>";



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



