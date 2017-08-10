

/*
expression :: = 

variable = [ A-Z, a-z]*



*/




/*��д
���棡 name=val �ĵȺ�ǰ�������пո񣡣�����
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

gr	:	grid
bx/by : ͼ����x/y���ϵĳ���

bndx1/y1/x2/y2 : bonduary x1/y1/x2/y2
spr: separator�ָ���

pmx/y : permille x/y�᳤��ռ��Ļ��x/y��ߴ��ǧ�ֱ�
*/

/*

��ͼԪ���Լ̳и�ͼԪ�����ԣ�����ɫ������ɫ������
�����ͼԪ�Լ�ָ�������ԣ�������ʹ���Լ��ƶ���
����Լ�û��ָ���������Ҹ�ͼԪ�����ԣ������ͼԪû���ṩ����ʹ��ϵͳĬ�ϵ�����

*/



















//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "expression.h"
#include <string.h>
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
const Except_T Exp_Failed = { "Exp Failed" };

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------
//ÿ�ֱ��ʽ�ڽ���ʱʹ�ã���������ͱ���ָ�Ϊ0
char expTempBUf[TEMPBUF_LEN];



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
static void SetVar( Expr *self, char *variable);
static void SetCtion( Expr *self, Composition *ct);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

int Set_shtContextAtt( char *p_att, sheet *p_sht)
{
	
	p_sht->cnt.colour = String2Clr( p_att);
	p_sht->cnt.bkc  = String2Bkc( p_att);
	p_sht->cnt.font = String2Font( p_att);
	
	return SET_ATT_OK;
}

int Set_shtAreaAtt( char *p_att, sheet *p_sht)
{
	int 	ret = SET_ATT_OK;
	
	char 	tmpbuf[4] = {0};


	GetKeyVal( p_att, "xali", tmpbuf, 4);
	p_sht->area.alix = String2Align( tmpbuf);
	GetKeyVal( p_att, "yali", tmpbuf, 4);
	p_sht->area.aliy = String2Align( tmpbuf);
	
	if( GetKeyVal( p_att, "vx0", tmpbuf, 4))
	{
		p_sht->area.x0 = atoi( tmpbuf);
	}
	if( GetKeyVal( p_att, "vy0", tmpbuf, 4))
	{
		p_sht->area.y0 = atoi( tmpbuf);
	}
	
	if( GetKeyVal( p_att, "bx", tmpbuf, 4))
	{
		p_sht->bxsize = atoi( tmpbuf);
	}
	else
	{
		ret |= SET_ATT_BSIZEFAIL;
	}
	if( GetKeyVal( p_att, "by", tmpbuf, 4))
	{
		p_sht->bysize = atoi( tmpbuf);
	}
	else
	{
		ret |= SET_ATT_BSIZEFAIL;
	}
	
	
	return ret;
}

ABS_CTOR( Expr)
FUNCTION_SETTING( setVar, SetVar);
FUNCTION_SETTING( setCtion, SetCtion);
//FUNCTION_SETTING( setFather, SetFather);
//FUNCTION_SETTING( getBgc, ExpGetBackgroudColor);



//FUNCTION_SETTING( str2colour, ExpStr2colour);
//FUNCTION_SETTING( str2font, ExpStr2font);

END_ABS_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static void SetVar( Expr *self, char *variable)
{
	//�ض�
	if( strlen( variable) > VAR_LEN)
		variable[ VAR_LEN] = 0;
		
	strcpy( self->variable, variable);
}

static void SetCtion( Expr *self, Composition *ct)
{
	self->ction = ct;
	
}



