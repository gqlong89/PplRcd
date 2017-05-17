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
	char		c;
	uint8_t		dealNum = 0;
	uint8_t		dealIdx = 0;
	uint16_t	usableX, usableY;
	uint16_t	needX, needY;
	
	//һ���ַ�һ���ַ�������
	
	
	
	//�����������x1,y1,x2,y2����ʼ��Ϊ����λ��
	pvd->area_x1 = area->cursorX;
	pvd->area_y1 = area->cursorY;
	pvd->area_x2 = area->cursorX;
	pvd->area_y2 = area->cursorY;
	
	//���һ��x��һ���ַ�Ҳ�Ų��¾�ֱ���˳�
	if( pvd->size_x > area->LcdSizeX)
		return;

	//��ʼ�������ַ�����Ϊ0
	dealIdx = 0; 
	while( dealIdx < pvd->len)
	{
	
	//ȡ��һ���ַ�,���ô�����Ϊ1
		c = pvd->data[ dealIdx];
		dealNum = 1;
	//�ж��Ƿ�Ϊ���֣��Ǻ��ֵĻ�����������Ϊ2
		if( IS_CHINESE( c))
		{
			dealNum = 2;
			
		}
			
	//���㱾�δ�����Ҫ��x��ռ�
		needX = dealNum * pvd->size_x;
	//���㱾�δ�����Ҫ��y��ռ�
		needY =  pvd->size_y;
		while(1)
		{
			//ʹ�ù��λ�ú���Ļ�Ľ���������x���y���ϵĿ��пռ� A
		
			usableX = area->LcdSizeX - area->cursorX;
			usableY = area->LcdSizeY - area->cursorY;
			//���y�᷽��ŵ���
			if( needY < usableY)
			{
				//���x��ŵ��£������x���ƶ���Ӧ��λ�ã���������Ҳ�ƶ���Ӧ��λ�ã��˳����δ���
				//������ռ�
				if( needX < usableX)
				{
					area->cursorX += needX;
					dealIdx += dealNum;
					pvd->area_x2 += needX;
					
					break;
				}
				//���x��Ų��£�����һ��,�ƶ���꣬�ص�A�����ж�
				else
				{
					
					area->cursorY += needY;
					pvd->area_y2 += needY;
					area->cursorX = 0;
				}
			}
			else
			//���y��Ų��£�������Ҫ��ҳ��־��ֱ���˳�
			{
				pvd->more	= 1;
				return ;
			}
	
		}
	}
	

	
}

//static void VDapply(void **ppvd, void *cl)
//{
//	ViewData_t *pvd = *ppvd;
//	area_t	*area = ( area_t *)cl;

	
//	short 	num = 0;
//	short 	yLeft = 0;		//y���ϵĿ���ռ�
//	char	row = 0;
//	char	maxRow = 0;		//�ɷ��������
//	char	maxinrow = 0;
//	
//	
//	
//	
//	
//	//����������ʾ������
//	char	allowNum = 0;
//	
//	
//	
//	//������ʼ����Ϊ���ĵ�ǰλ��
//	pvd->area_x1 = area->cursorX;
//	pvd->area_y1 = area->cursorY;
//	pvd->area_x2 = area->cursorX;
//	pvd->area_y2 = area->cursorY;
//	yLeft = area->LcdSizeY - area->cursorY ;
//	
//	//ÿ���ܹ���ʾ������
//	maxinrow = area->LcdSizeX / pvd->size_x;
//	maxRow = ( area->LcdSizeY -  pvd->area_y1)/ pvd->size_y;
//	//�����������
//	num = pvd->len;
//	//���ǵ������ַ�������λ�ã�����һ��Ҫ����ż����λ��
//	if( num & 1)
//		num ++;
//	
//	
//	//�����ܹ����������
//	
//	//����1��
//	row = 1;
//	yLeft -= pvd->size_y;
//	if( yLeft < 0)
//		goto exit;
//	
//	allowNum =  ( area->LcdSizeX - pvd->area_x1) / pvd->size_x;
//	//���������ַ�������1�����Ͳ���������������ַ�����ʾ��һ�е��������
//	if( pvd->len > 1)
//	{
//		if( allowNum & 1)
//			allowNum --;
//		
//	} 
//	while(1)
//	{	
//		
//		//������ʾ�ģ��Ͷ��һ��
//		if(  ( num > allowNum) && ( yLeft >=0))
//		{

//			num -= allowNum;
//			//����һ��
//			row ++;
//			yLeft -= pvd->size_y;
//			if( yLeft < 0)
//			{
//					//������һ��
//				row --;
//				num = allowNum;
//				
//			}
//			else
//			{
//			
//				allowNum = maxinrow;
//			}
//			
//		}	
//		else
//		{
//			//���˵Ļ����ͼ���x����Ľ�β
//			
//			//����ʼλ����ͬһ�еĻ���Ҫ��x1��ƫ��
//			//��ͬ�оͲ���Ҫ��
//			
//			
//			if( row == 1)
//				pvd->area_x2 = pvd->area_x1 + num * pvd->size_x ;
//			else
//				pvd->area_x2 = num * pvd->size_x ;
//			break;
//		}
//		
//		
//		
//	}
//	
//	
//	//����y�����ϵĽ�β
//	pvd->area_y2 = pvd->area_y1 + (row -1 )* pvd->size_y;
//	
//	exit:
//	if( yLeft < 0)
//	{
//		
//		pvd->more	= 1;
//	}
//	
//	
//	//�ƶ����
//	area->cursorX = pvd->area_x2 ;
//	area->cursorY = pvd->area_y2;
	
//}
//�ڿ�����ʾ��ʱ��Ϊ�˷�ֹһ�����ֱ������ʾ�����Ի���һ��
static void VDShow(void **ppvd, void *cl)
{
	ViewData_t *pvd = *ppvd;
	Composition	*ction = ( Composition *)cl;
	//ÿ���ܹ���ʾ����	
	short 	numInrow = 0;
	
	
	short	displayed = 0;	//�Ѿ�����ʾ������
	short	 y = 0;
	short 	xend = 0 ;
	short	xstart	= 0, ableNum;
	char	c;
	
	int16_t		oldx1 = pvd->area_x1;
	int16_t		oldy1 = pvd->area_y1;
	
	//�����������ɫ
	pvd->gh->setClu( pvd->gh, pvd->colour);
	pvd->gh->setFont( pvd->gh, pvd->font);
	pvd->gh->insert( pvd->gh, pvd->data, pvd->len);

	displayed = 0;
	for( y = pvd->area_y1; y <= pvd->area_y2; y += pvd->size_y)
	{
		//�ڵ�һ�У�x����ʼλ���Ƿ��������x����ʼλ��
		if( y == pvd->area_y1)
			xstart = pvd->area_x1;
		else
			xstart = 0;
		//�����һ�У�x��Ľ���λ���Ƿ�������Ľ���λ��
		if( y == pvd->area_y2 || pvd->area_y1 == pvd->area_y2)
			xend = pvd->area_x2;
		else
			xend = ction->lcdArea.LcdSizeX;
		
		ableNum = ( xend - xstart) / pvd->size_x;
		
		//������һ������ʾ�����ַ�
		numInrow  = 0;
		while(1)
		{
			c = pvd->data[ displayed];
			
			if( IS_CHINESE( c))
			{
				if( ableNum > 1)
				{
					numInrow += 2;
					displayed += 2;
					ableNum -= 2;
				}
				
				
				
			}
			else
			{
				if( ableNum >  0)
				{
					numInrow ++;
					displayed ++;
					ableNum -= 1;
				}	
				
			}
			//�����ʾ���˾��˳�
			if( displayed >= pvd->len)
				break;
			//��������Ѿ�û�пռ��ˣ�����ʾ����
			if( ableNum == 0)
				break;
			//������л�����ʾһ���ַ���������һ���ǲ����ܷŽ�ȥ��
			if( ableNum == 1)
			{		
				c = pvd->data[ displayed];
				if( !IS_CHINESE( c))
				{
					displayed ++;
					numInrow ++;
				}
				break;
			}
				
		}
		pvd->gh->draw( pvd->gh, pvd->area_x1, pvd->area_y1, numInrow);
		pvd->area_x1 = 0;
		pvd->area_y1 += pvd->size_y;
		
	}
	
	
	pvd->done = displayed;
	
	
	//�ָ���ʼλ�ã������´���ʾ
	pvd->area_x1 = oldx1;
	pvd->area_y1 = oldy1;

	
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

