#include "expression.h"
#include <string.h>
/*
expression :: = 




*/


static void SetVar( Expr *self, char *variable)
{
	//�ض�
	if( strlen( variable) > VAR_LEN)
		variable[ VAR_LEN] = 0;
		
	strcpy( self->variable, variable);
}

ABS_CTOR( Expr)
FUNCTION_SETTING( setVar, SetVar);


END_ABS_CTOR





