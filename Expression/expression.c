#include "expression.h"
#include <string.h>
/*
expression :: = 

variable = [ A-Z, a-z]*



*/

/*��д
clr : colour
f : font
bu : button
UDT��д
pa	: 	paragraph����
rct :	rectangle ����

scr : 	Screen region ��Ļ����
sel	:	selection 	ѡ������
ali :  aligning ���뷽ʽ

cg	:	columnGap �м��
ls	:	lineSpacing	�м��

pmx/y : permille x/y�᳤��ռ��Ļ��x/y��ߴ��ǧ�ֱ�
*/


//ÿ�ֱ��ʽ�ڽ���ʱʹ�ã���������ͱ���ָ�Ϊ0
char expTempBUf[TEMPBUF_LEN];


static void SetVar( Expr *self, char *variable)
{
	//�ض�
	if( strlen( variable) > VAR_LEN)
		variable[ VAR_LEN] = 0;
		
	strcpy( self->variable, variable);
}

//static void SetFather( Expr *self, Expr *fa)
//{
//	
//	self->father = fa;
//}
static void SetCtion( Expr *self, Composition *ct)
{
	self->ction = ct;
	
}













ABS_CTOR( Expr)
FUNCTION_SETTING( setVar, SetVar);
FUNCTION_SETTING( setCtion, SetCtion);
//FUNCTION_SETTING( setFather, SetFather);
//FUNCTION_SETTING( getBgc, ExpGetBackgroudColor);



//FUNCTION_SETTING( str2colour, ExpStr2colour);
//FUNCTION_SETTING( str2font, ExpStr2font);

END_ABS_CTOR





