#ifndef __MODEL_H_
#define __MODEL_H_
#include <stdint.h>
#include "arithmetic/list.h"
#include "lw_oopc.h"
#include "Glyph.h"


#define SET_CHILDASWHOLE(att)		(	att | 1)	//��Ԫ�ر�����Ϊһ������
#define IS_CHILDASWHOLE(att)		(	att & 1)	//
#define SET_CHILDFIRST(att)		(	att | 2)	//�ڷ�����Ļ�ռ�ʱ����Ԫ�����ȣ����谷��Ҫȥ�����Ԫ��
#define IS_CHILDFIRST(att)		(	att & 2)	//



typedef struct {
	
	int16_t		LcdSizeX;
	int16_t		LcdSizeY;
	//���λ��
	int16_t		cursorX;
	int16_t		cursorY;

}area_t;

typedef struct {
	void 		*paraent;
	List_T		t_childen;
	
	char		*data;
	uint16_t 	len;
	int8_t		font;
	int8_t		colour;
	//ÿ����ʾԪ�صĳߴ�
	uint16_t		size_x;
	uint16_t		size_y;
	
	//��¼��ʾ�����ݱ����������ʾ����
	int16_t		area_x1;
	int16_t		area_y1;
	int16_t		area_x2;
	int16_t		area_y2;
	
	Glyph		*gh;
	
	char		more;		//��Ҫ��ҳ��ʾ
	short		done;		//�Ƿ���ʾ����
	uint8_t		childAttr;		//��Ԫ�ص����ԣ����Ƿ���Ա��ֵ�������ʾ���ߴ�����Ƿ�����Ԫ������
	
}ViewData_t;

CLASS( Composition)
{
	List_T				t_vd;
	uint16_t			x;
	uint16_t			y;
	void				*ctor;
	
	uint16_t	lcdWidth, lcdHeight;
	uint16_t	ghWidth, ghHeight;
	
	area_t		lcdArea;
	
	
	void ( *setCtor)( Composition *self, void *ctor);
	
	ViewData_t	*( *allocVD)( Composition *self);
	int ( *insertVD)( Composition *self, ViewData_t *faVd, ViewData_t *vd);
	
	int ( *clean)( Composition *self);
	int ( *flush)( Composition *self);
	
	
	int ( *insert)( Composition *self, Glyph *gh);
	int ( *addRow)( Composition *self);
};


Composition *Get_Composition(void);

#endif
