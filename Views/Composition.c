#include "Composition.h"
#include "Compositor.h"
#include "device.h"
#include <string.h>

#include "Gh_txt.h"

#include "basis/sdhDebug.h"
#include "basis/sdhError.h"

#include "mem/CiiMem.h"

static void Ction_FreeVD( void **vd, void *c1);

Composition *signalComposition;

Composition *Get_Composition(void)
{
	
	if( signalComposition == NULL)
	{
		signalComposition = Composition_new();
		
		//todo:暂时写死
		signalComposition->mySCI.scBoundary.x1 = 0;
		signalComposition->mySCI.scBoundary.y1 = 0;
		signalComposition->mySCI.scBoundary.x2 = 320;
		signalComposition->mySCI.scBoundary.y2 = 240;
		
	}
	
	return signalComposition;
}

static void SetCtor( Composition *self, void *ctor)
{
	self->ctor = ctor;
	
}
//设置屏幕背景色
void SetSCBkc( Composition *self, char* bkc)
{
	
	self->mySCI.scBkc = String2Colour( bkc);
}
static int Ction_Clean( Composition *self)
{
	GhTxt *mytxt = Get_GhTxt();
	Glyph	*myGp = (Glyph *)mytxt;
	I_dev_lcd *lcd;
	
	Dev_open( LCD_DEVID, (void *)&lcd);
	
	//回收版面中的显示数据
	List_map( self->t_vd, Ction_FreeVD, NULL);
	List_free( &self->t_vd);
	
	self->mySCI.cursorX = 0;
	self->mySCI.cursorY = 0;
	self->mySCI.colSize = 0;
	self->mySCI.rowSize = 0;
	

	
	lcd->Clear( self->mySCI.scBkc);
	myGp->clean(myGp);
	
	return RET_OK;
}


//将版面中的数据全部显示出来
//为每个要显示的数据分配界面区域


static int Ction_flush( Composition *self)
{

	Compositor *thisCtor = ( Compositor *)self->ctor;
	self->t_vd = List_reverse( self->t_vd);
	
	self->mySCI.scBoundary.x1 = 0;
	self->mySCI.scBoundary.y1 = 0;
	self->mySCI.scBoundary.x2 = 320;
	self->mySCI.scBoundary.y2 = 240;
	

	thisCtor->vdLayout( self);
	thisCtor->show( self);
	
	return RET_OK;
}

ViewData_t	* Ction_AllocVD( Composition *self)
{
	ViewData_t *vd =(ViewData_t *) CALLOC( 1, sizeof( ViewData_t));
	return vd;
	
}

int Ction_InsertVD( Composition *self, ViewData_t *faVd, ViewData_t *vd)
{
	if( faVd)
	{
		faVd->t_childen = List_push( faVd->t_childen, vd);
		vd->paraent = faVd;
	}
	else
	{
		self->t_vd = List_push( self->t_vd, vd);
	}

	return RET_OK;
}













//static int Ction_Insert( Composition *self, Glyph *gh)
//{
//	
//	
//	Compositor *thisCtor = ( Compositor *)self->ctor;
//	

//	
//	
//	self->lcdWidth = 320;
//	self->lcdHeight = 240;
//	
//	
//	thisCtor->compose( self, gh);
//	
//	
//	
//	return RET_OK;
//}

int Ction_AddRow( Composition *self)
{
	//如果再加一行就超过屏幕就返回错误
//	if( ( self->rowSize[ self->step] + self->lcdArea.cursorY) > self->lcdArea.LcdSizeY)
//		return ERR_RES_UNAVAILABLE;
//	//将光标指向下一行起始位置
//	self->lcdArea.cursorY += self->rowSize[ self->step];
//	self->lcdArea.cursorX = 0;
	
	
	return RET_OK;
}

static void Ction_FreeVD( void **vd, void *c1)
{
	FREE( *vd);
	
	
	
}

CTOR( Composition)


FUNCTION_SETTING( setCtor, SetCtor);

FUNCTION_SETTING( clean, Ction_Clean);
FUNCTION_SETTING( setSCBkc, SetSCBkc);
FUNCTION_SETTING( addRow, Ction_AddRow);

FUNCTION_SETTING( allocVD, Ction_AllocVD);
FUNCTION_SETTING( insertVD, Ction_InsertVD);
FUNCTION_SETTING( flush, Ction_flush);


END_CTOR

