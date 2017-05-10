#include "Compositor.h"
#include "device.h"

#include "Composition.h"


#include "basis/sdhDebug.h"
#include "basis/sdhError.h"

SimpCtor *signalSimpCtor;

SimpCtor *Get_SimpCtor(void)
{
	
	if( signalSimpCtor == NULL)
	{
		signalSimpCtor = SimpCtor_new();
		
	}
	
	return signalSimpCtor;
}




static int Simp_Compose( Composition *ction, Glyph *gh)
{
	int num;
	
	ction->ghWidth = gh->getWidth( gh);
	ction->ghHeight = gh->getHeight( gh);
	num = gh->getNum( gh);
	while( num)
	{
		//����Ƿ��ӿ���ϳ�����Ļ
		while( 1)
		{
			if( ( ction->x + num * ction->ghWidth ) <= ction->lcdWidth )
				break;
			if( num > 2)
				num -= 2;
			else 
				ction->addRow( ction);
				
			
		}
		gh->draw( gh, ction->x, ction->y, num);
		num = gh->getNum( gh);
		//��Ļ�������һ��
		if( num)
			ction->addRow( ction);
	}
	
	//����Ƿ������ϳ�����Ļ
	
	//��Ļ���Ҫ���ҳ��ʾ
	
	return RET_OK;
}


CTOR( SimpCtor)
FUNCTION_SETTING( Compositor.compose, Simp_Compose);


END_CTOR

