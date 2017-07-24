#include "TxtExpr.h"
//#include "basis/sdhDebug.h"
//#include "basis/sdhError.h"
#include "sdhDef.h"
#include "dev_lcd.h"
#include "Gh_txt.h"

#include <string.h>

#include "Reader.h"

static TxtExpr *signalTxtExpr;


TxtExpr *GetTxtExpr(void)
{
	if( signalTxtExpr == NULL)
	{
		signalTxtExpr = TxtExpr_new();
	}
	return signalTxtExpr;
}


static void * TxtInterpret( Expr *self, void *faVd, void *context)
{
//	TxtExpr *cthis = ( TxtExpr *)self;
	GhTxt *mytxt = Get_GhTxt();
	Glyph	*myGp = (Glyph *)mytxt;
	int len = 0;
	char	*pp;
	char 	*pnewPosition;
	char	*att = expTempBUf;
	char 	tmpbuf[4] = {0};
	
	ViewData_t	*vd;


	GetAttribute( context, att, TEMPBUF_LEN);
	
	
	pnewPosition = GetNameVale( context, self->variable, &pp, &len);
	if( len == 0)
		goto exit;
	
	vd = self->ction->allocVD( self->ction);
	
	if( vd == NULL)
		goto exit;
	
	
	
	
	//�������ȼ�:�Լ���˽�����ԣ� �����ԣ� ϵͳĬ������
	
	
	
	vd->dspCnt.colour = String2Clr( att);
	
	
//	vd->dspCnt.effects = String2CntEff( att);
	vd->dspCnt.bkc  = String2Bkc( att);
	
	GetKeyVal( att, "xali", tmpbuf, 4);
	vd->dspArea.ali = String2Align( tmpbuf);
	GetKeyVal( att, "yali", tmpbuf, 4);
	vd->dspArea.aliy = String2Align( tmpbuf);
	
	
//	vd->dspArea.ali = String2Align( att);
	vd->dspCnt.font = String2Font( att);
	
	UsePrntAttIfNeed( ( ViewData_t	*)faVd, vd);
	
	if( vd->dspCnt.colour == ERR_COLOUR)
	{
		
		vd->dspCnt.colour = DefaultColour(NULL);
		
	}
	if( vd->dspArea.ali == ALIGN_ERR)
	{
		
		vd->dspArea.ali = ALIGN_DEFAULT;
		
	}
	
	vd->dspCnt.data = pp;
	vd->dspCnt.len = len;
	
	vd->gh = myGp;
	myGp->getSize( myGp,  vd->dspCnt.font, &vd->dspArea.sizeX, &vd->dspArea.sizeY);
	
	//�Ա���������⴦��
	if( !strcasecmp( self->variable, "title"))
	{
		vd->notDealAli = 1;
		if( vd->dspCnt.bkc == ERR_COLOUR)
		{
			
			vd->dspCnt.bkc = COLOUR_BLUE;
		}
		if( vd->dspCnt.font < FONT_16)
			vd->dspCnt.font = FONT_16;
		vd->dspCnt.subType = TEXT_ST_LABLE;
		vd->dspArea.sizeX = SIZE_BOUNDARY;
		
	}
	
	
	
	
	
	
	self->ction->insertVD( self->ction, faVd, vd);
	pnewPosition = RemoveTail( pnewPosition, att, 16);
	exit:
	memset( expTempBUf, 0, sizeof( expTempBUf));
	
	return pnewPosition;
}


static void * TxtInptSht( Expr *self, void *context, sheet *p_sht)
{
	GhTxt 	*mytxt = Get_GhTxt();
	Glyph	*myGp = (Glyph *)mytxt;
	char	name[8];
	int 	len = 0;
	char	*pp;
	char 	*pnewPosition;
	char	*att = expTempBUf;
	char 	tmpbuf[4] = {0};
	int 	bx_by_correct = 1;


	GetAttribute( context, att, TEMPBUF_LEN);
	
	len = 8;
	len = GetName( pp, name, len);
	
	pnewPosition = GetNameVale( context, name, &pp, &len);
	if( len == 0)
		goto exit;
	
	
	
	
	
	
	//�������ȼ�:�Լ���˽�����ԣ� �����ԣ� ϵͳĬ������
	
	
	
	p_sht->cnt.colour = String2Clr( att);
	p_sht->cnt.bkc  = String2Bkc( att);
	p_sht->cnt.font = String2Font( att);
	p_sht->cnt.data = pp;
	p_sht->cnt.len = len;
	
	GetKeyVal( att, "xali", tmpbuf, 4);
	p_sht->alix = String2Align( tmpbuf);
	GetKeyVal( att, "yali", tmpbuf, 4);
	p_sht->aliy = String2Align( tmpbuf);
	
	if( GetKeyVal( att, "vx0", tmpbuf, 4))
	{
		p_sht->area.x0 = atoi( tmpbuf);
	}
	if( GetKeyVal( att, "vy0", tmpbuf, 4))
	{
		p_sht->area.y0 = atoi( tmpbuf);
	}
	
	if( GetKeyVal( att, "bxSz", tmpbuf, 4))
	{
		p_sht->bxsize = atoi( tmpbuf);
	}
	else
	{
		bx_by_correct = 0;
	}
	if( GetKeyVal( att, "bySz", tmpbuf, 4))
	{
		p_sht->bysize = atoi( tmpbuf);
	}
	else
	{
		bx_by_correct = 0;
	}
	
	if( bx_by_correct == 0)
	{
		myGp->getSize( myGp,  p_sht->cnt.font, &p_sht->bxsize, &p_sht->bysize);
	}
	
	
	p_sht->p_gp = myGp;
	
	
	
	
	
	
	
	
	//�Ա���������⴦��
	if( !strcasecmp( self->variable, "title"))
	{
		if( p_sht->cnt.bkc == ERR_COLOUR)
		{
			
			p_sht->cnt.bkc = COLOUR_BLUE;
		}
		if( p_sht->cnt.font < FONT_16)
			p_sht->cnt.font = FONT_16;
		p_sht->cnt.subType = TEXT_ST_LABLE;
		p_sht->bxsize = SIZE_BOUNDARY;
		
	}
	
	pnewPosition = RemoveTail( pnewPosition, att, 16);
	exit:
	memset( expTempBUf, 0, sizeof( expTempBUf));
	
	return pnewPosition;
}


CTOR( TxtExpr)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.interpret, TxtInterpret);
FUNCTION_SETTING( Expr.inptSht, TxtInptSht);

END_CTOR






