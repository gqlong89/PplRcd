#include "testView.h"
#include "TxtExpr.h"

 
static const char testContext[] = \
" <body> <h3>���Java����</h3> </body>";



void View_test(void)
{
//	TestViewShow();
	char *pp;
	Expr *myexp = (Expr* )GetTxtExpr();
	myexp->setVar( myexp, "h3");		//����Context�еı���������
	myexp->interpret( myexp, (void *)testContext);
	
}



