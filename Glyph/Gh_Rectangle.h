#ifndef __GH_RECTANGLE_H_
#define __GH_RECTANGLE_H_
#include "Glyph.h"
#include "lw_oopc.h"
#include <stdint.h>
#include "arithmetic/cycQueue.h"


CLASS(GhRectangle)
{
	EXTENDS( Glyph);
	
	//ֻ��ͨ��font�����Ա�����ÿ�͸���
	uint16_t	width;
	uint16_t	height;
	
	
};


GhRectangle *Get_GhRectangle(void);
#endif
