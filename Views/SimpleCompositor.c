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


static void VDapply(void **ppvd, void *cl)
{
	ViewData_t *pvd = *ppvd;
	area_t	*area = ( area_t *)cl;
	short 	num = 0;
	

	char	row = 0;
	
	char	maxinrow = 0;
	
	//����������ʾ������
	char	allowNum = 0;
	
	
	
	//������ʼ����Ϊ���ĵ�ǰλ��
	pvd->area_x1 = area->cursorX;
	pvd->area_y1 = area->cursorY;
	
	//ÿ���ܹ���ʾ������
	maxinrow = area->LcdSizeX / pvd->size_x;
	
	//�����������
	num = pvd->len;
	//���ǵ������ַ�������λ�ã�����һ��Ҫ����ż����λ��
	if( num & 1)
		num ++;
	
	
	//�����ܹ����������
	row = 1;
	allowNum =  ( area->LcdSizeX - pvd->area_x1) / pvd->size_x;
	while(1)
	{	
		
		//������ʾ�ģ��Ͷ��һ��
		if(  num > allowNum)
		{

			num -= allowNum;
			//����һ��
			row ++;
			allowNum = maxinrow;
			
		}	
		else
		{
			//���˵Ļ����ͼ���x����Ľ�β
			
			//����ʼλ����ͬһ�еĻ���Ҫ��x1��ƫ��
			//��ͬ�оͲ���Ҫ��
			if( row == 1)
				pvd->area_x2 = pvd->area_x1 + num * pvd->size_x ;
			else
				pvd->area_x2 = num * pvd->size_x ;
			break;
		}
		
		
		
	}
	
	
	//����y�����ϵĽ�β
	pvd->area_y2 = pvd->area_y1 + (row -1 )* pvd->size_y;
	if( pvd->area_y2 > area->LcdSizeY)
	{
		pvd->area_y2 = area->LcdSizeY;
		pvd->more	= 1;
	}
	
	
	//�ƶ����
	area->cursorX = pvd->area_x2 + pvd->size_x;
	area->cursorY = pvd->area_y2;
	
}

static void VDShow(void **ppvd, void *cl)
{
	ViewData_t *pvd = *ppvd;
	Composition	*ction = ( Composition *)cl;
	//ÿ���ܹ���ʾ����	
	short 	numInrow = 0;
	char	numRow = 0;
	
	//��ȫ��Ҫ��ʾ������תת��gh�Ļ�����
	pvd->gh->setFont( pvd->gh, pvd->font);
	pvd->gh->insert( pvd->gh, pvd->data, pvd->len);
	
	//������Ҫ��ʾ������
	numRow = ( pvd->area_y2 - pvd->area_y1) / pvd->size_y + 1;
	
	//��ʾ��1��
	numInrow = ( ction->lcdArea.LcdSizeX - pvd->area_x1) / pvd->size_x;
	pvd->gh->draw( pvd->gh, pvd->area_x1, pvd->area_y1, numInrow);
	
	//����
	pvd->area_y1 += pvd->size_y;
	pvd->area_x1 = 0;
	
	numRow --;
	
	//��ʾ�м����
	numInrow = ( ction->lcdArea.LcdSizeX ) / pvd->size_x;
	//��Ҫ��ҳ��ʾ��ʱ�����һ�а����м�������ʾ
	while( numRow > 1 || pvd->more)
	{
		
		pvd->gh->draw( pvd->gh, pvd->area_x1, pvd->area_y1, numInrow);
		
		//����
		pvd->area_y1 += pvd->size_y;
		pvd->area_x1 = 0;
		
		numRow --;
	}
	
	//��ʾ���һ��
	if( numRow == 1)
	{
		numInrow = ( pvd->area_x2 ) / pvd->size_x;
		pvd->gh->draw( pvd->gh, pvd->area_x1, pvd->area_y1, numInrow);
	}
	
	
	//����Ƿ���Ҫ��ҳ��ʾ
	if( pvd->more)
	{
		
	}
	
	
}

static int Simp_VdLayout( Composition *ction, List_T t_vd)
{
	//�����е���Ԫ����x���y���ϵĳߴ�
	List_map( ction->t_vd, VDapply, &ction->lcdArea);
	
	//��ʾ����
	List_map( ction->t_vd, VDShow, ction);
	
	return RET_OK;
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
			{
				if( ction->addRow( ction) != RET_OK)
				{
					//��ʾ��ҳ��ʶ
					
					goto exit;
				}
					
				
				
			}
				
			
		}
		
		
		gh->draw( gh, ction->x, ction->y, num);	
		ction->x += num  * ction->ghWidth;
		
		num = gh->getNum( gh);
		
		//��Ļ�������һ��
		if( num)
		{
			if( ction->addRow( ction) != RET_OK)
			{
				//��ʾ��ҳ��ʶ
				
				goto exit;
			}
			
		}
			
	}
	
	
	exit:	
	return RET_OK;
}


CTOR( SimpCtor)
FUNCTION_SETTING( Compositor.vdLayout, Simp_VdLayout);



FUNCTION_SETTING( Compositor.compose, Simp_Compose);


END_CTOR

