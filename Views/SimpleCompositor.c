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
	short 	yLeft = 0;		//y���ϵĿ���ռ�
	char	row = 0;
	char	maxRow = 0;		//�ɷ��������
	char	maxinrow = 0;
	
	//����������ʾ������
	char	allowNum = 0;
	
	
	
	//������ʼ����Ϊ���ĵ�ǰλ��
	pvd->area_x1 = area->cursorX;
	pvd->area_y1 = area->cursorY;
	pvd->area_x2 = area->cursorX;
	pvd->area_y2 = area->cursorY;
	yLeft = area->LcdSizeY - area->cursorY ;
	
	//ÿ���ܹ���ʾ������
	maxinrow = area->LcdSizeX / pvd->size_x;
	maxRow = ( area->LcdSizeY -  pvd->area_y1)/ pvd->size_y;
	//�����������
	num = pvd->len;
	//���ǵ������ַ�������λ�ã�����һ��Ҫ����ż����λ��
	if( num & 1)
		num ++;
	
	
	//�����ܹ����������
	
	//����1��
	row = 1;
	yLeft -= pvd->size_y;
	if( yLeft < 0)
		goto exit;
	
	allowNum =  ( area->LcdSizeX - pvd->area_x1) / pvd->size_x;
	//���������ַ�������1�����Ͳ���������������ַ�����ʾ��һ�е��������
	if( pvd->len > 1)
	{
		if( allowNum & 1)
			allowNum --;
		
	} 
	while(1)
	{	
		
		//������ʾ�ģ��Ͷ��һ��
		if(  ( num > allowNum) && ( yLeft >=0))
		{

			num -= allowNum;
			//����һ��
			row ++;
			yLeft -= pvd->size_y;
			if( yLeft < 0)
			{
					//������һ��
				row --;
				num = allowNum;
				
			}
			else
			{
			
				allowNum = maxinrow;
			}
			
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
	
	exit:
	if( yLeft < 0)
	{
		
		pvd->more	= 1;
	}
	
	
	//�ƶ����
	area->cursorX = pvd->area_x2 ;
	area->cursorY = pvd->area_y2;
	
}
//�ڿ�����ʾ��ʱ��Ϊ�˷�ֹһ�����ֱ������ʾ�����Ի���һ��
static void VDShow(void **ppvd, void *cl)
{
	ViewData_t *pvd = *ppvd;
	Composition	*ction = ( Composition *)cl;
	//ÿ���ܹ���ʾ����	
	short 	numInrow = 0;
	char	numRow = 0;
	
	//��ȫ��Ҫ��ʾ������תת��gh�Ļ�����
	pvd->gh->setClu( pvd->gh, pvd->colour);
	pvd->gh->setFont( pvd->gh, pvd->font);
	pvd->gh->insert( pvd->gh, pvd->data, pvd->len);
	
	//������Ҫ��ʾ������
	numRow = ( pvd->area_y2 - pvd->area_y1) / pvd->size_y + 1;
	
	//��ʾ��1��
	numInrow = ( ction->lcdArea.LcdSizeX - pvd->area_x1) / pvd->size_x;
	if( numRow > 1)
	{
		if( numInrow & 1)
			numInrow --;
		
	}
	pvd->gh->draw( pvd->gh, pvd->area_x1, pvd->area_y1, numInrow);
	
	//����
	pvd->area_y1 += pvd->size_y;
	pvd->area_x1 = 0;
	
	numRow --;
	
	//��ʾ�м����
	numInrow = ( ction->lcdArea.LcdSizeX ) / pvd->size_x;
	//��Ҫ��ҳ��ʾ��ʱ�����һ�а����м�������ʾ
	while( numRow > 1 )
	{
		
		if( numInrow & 1)
			numInrow --;
			
		
		
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
		pvd->done = 0;
	}
	else
	{
		pvd->done = 1;
		
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

