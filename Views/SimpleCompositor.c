#include "Compositor.h"
#include "device.h"

#include "Composition.h"


#include "basis/sdhDebug.h"
#include "basis/sdhError.h"

SimpCtor *signalSimpCtor;


static void DspViewData(void **ppvd, void *cl);

//��Ҫ�Ƕ����ͼԪ�ĳߴ�ȥȷ��
static void CalVdSize(void **ppvd, void *cl);
static void SumChldSize(void **ppvd, void *cl);
static void AdjustChildSeq(void **ppvd, void *cl);

static void Layout(void **ppvd, void *cl);

SimpCtor *Get_SimpCtor(void)
{
	
	if( signalSimpCtor == NULL)
	{
		signalSimpCtor = SimpCtor_new();
		
	}
	
	return signalSimpCtor;
}

//




static int Simp_VdLayout( Composition *ction)
{
	//�ⲽ����Ϊʹ�õ������ڲ�����Ԫ�ص�ʱ���Ǵ�ͷ������
	//����������Ҫ������
	List_map( ction->t_vd, AdjustChildSeq, ction);
	
	List_map( ction->t_vd, CalVdSize, ction);
	
	//�����е���Ԫ����x���y���ϵĳߴ�
	List_map( ction->t_vd, Layout, ction);
	
	
	
	return RET_OK;
}

static int Simp_Show( Composition *ction)
{
	
	
	//��ʾ����
	List_map( ction->t_vd, DspViewData, ction);
	
	return RET_OK;
}
static void AdjustChildSeq(void **ppvd, void *cl)
{
	ViewData_t *pvd = *ppvd;
	pvd->t_childen = List_reverse( pvd->t_childen);
}

static void CalVdSize(void **ppvd, void *cl)
{
	ViewData_t *pvd = *ppvd;
	ViewData_t *pPvd = pvd->paraent;
	Composition	*pction = ( Composition *)cl;
	short	numChild = 0;
	short	i = 0;
	
	
	
	//������ͼԪȥ�����Լ��ĳߴ�
	List_map( pvd->t_childen, CalVdSize, cl);
	

	
	
	
	
	//�����Ҫ����ͼԪ��ȷ���ߴ������ͼԪ������
	if( pvd->t_childen)
	{
		if( pvd->donotUseChldSize == 0)
		{
//			pvd->dspArea.sizeX = 0;
//			pvd->dspArea.sizeY = 0;
//			pvd->colcount = 0;
//			pvd->tmpSize = 0;
			List_map( pvd->t_childen, SumChldSize, cl);
//			pvd->dspArea.sizeY += pvd->lineSpacing;
			return;
		}
		
	}
	
	//�����Լ��ĳߴ�
	//�Ȳ����Ƿ��е����
//	if( pPvd->dspArea.boundary == NULL)
//	{
//		pPvd->dspArea.boundary = &pction->mySCI.scBoundary;
//	}
//	pvd->dspArea.sizeY = ( ( pvd->dspArea.sizeX- 1)/pvd->dspArea.sizeX + 1) * pvd->dspArea.sizeY;
	pvd->dspArea.sizeY = pvd->dspArea.sizeY;
	
	if( pvd->dspArea.sizeX == SIZE_BOUNDARY)
		pvd->dspArea.sizeX = pction->mySCI.scBoundary.x2;
	else
		pvd->dspArea.sizeX = pvd->dspArea.sizeX * pvd->dspCnt.len;
	
}

static void SumChldSize(void **ppvd, void *cl)
{
	ViewData_t *pvd = *ppvd;
	ViewData_t *pPvd = pvd->paraent;
	
	
	
	
	
	
	//��ͼԪ��һ���е�����û��ָ��������¾���Ϊһ��ֻ��ʾһ��
	//�����Ҫ�ڸ�ͼԪ�����Լ�������
	if( pPvd->cols)
	{
		
		pPvd->colcount ++;
		if( ( pPvd->colcount % pPvd->cols) == 0)
		{
			pPvd->dspArea.sizeY += pvd->dspArea.sizeY;
			pPvd->dspArea.sizeY += pPvd->lineSpacing;
			
			pPvd->tmpSize += pvd->dspArea.sizeX;
			
			if( pPvd->dspArea.sizeX < pPvd->tmpSize)
				pPvd->dspArea.sizeX = pPvd->tmpSize;
			
			pPvd->tmpSize = 0;
			
		}
		else
		{
			pPvd->tmpSize += pvd->dspArea.sizeX  + pPvd->columnGap;
//			sizex += pvd->dspArea.sizeX;
			
		}
		
	}
	else
	{
		pPvd->dspArea.sizeX += pvd->dspArea.sizeX ;
		if( pPvd->dspArea.sizeY < pvd->dspArea.sizeY)
			pPvd->dspArea.sizeY = pvd->dspArea.sizeY;
		pPvd->dspArea.sizeX += pPvd->columnGap;

//		pPvd->dspArea.sizeY += pPvd->lineSpacing;
		
	}
	
}

//
static void LayoutChld(void **ppvd, void *cl)
{
	ViewData_t *pvd = *ppvd;
	ViewData_t *pPvd = pvd->paraent;
	scArea_t	*bnd = pPvd->dspArea.boundary;
	short		boundaryX = 0;
	short		totalX = 0;
	
	//������һ���Ƿ�ֵ
	pvd->dspArea.useArea.x1 = 0;
	pvd->dspArea.useArea.y1 = 0;
	pvd->dspArea.useArea.x2 = 0;
	pvd->dspArea.useArea.y2 = 0;
	
	
	boundaryX = bnd->x2 - bnd->x1;
	if( pPvd->cols)
	{
		pPvd->colcount ++;
		boundaryX = boundaryX/( pPvd->cols );
	}
	
	totalX = pvd->dspArea.sizeX;
	
	//����Ҫ�������ģ�Ҫ������ʾ������ߴ���ȷ����ʼλ��
	
	if( pvd->notDealAli  || pvd->dspArea.ali == ALIGN_LEFT)
	{
	
		//�����������x1,y1,x2,y2����ʼ��Ϊ����λ��
		pvd->dspArea.useArea.x1 = pPvd->dspArea.cursorX;
		
	}
	else if(  pvd->dspArea.ali == ALIGN_MIDDLE)
	{
		pvd->dspArea.useArea.x1 = pPvd->dspArea.cursorX + ( ( boundaryX - totalX) >> 1);	
	}
	else if(  pvd->dspArea.ali == ALIGN_RIGHT)
	{
		pvd->dspArea.useArea.x1 = pPvd->dspArea.cursorX + boundaryX - totalX;
		
	}
//	pvd->dspArea.numRow = 1;
	if( pvd->dspArea.aliy == ALIGN_MIDDLE)
	{
		pvd->dspArea.useArea.y1 = pPvd->dspArea.cursorY + ( ( bnd->y2 - bnd->y1 - pvd->dspArea.sizeY) >> 1);
	}
	else
	{
		pvd->dspArea.useArea.y1 = pPvd->dspArea.cursorY;
	}
	
	pvd->dspArea.useArea.x2 = pvd->dspArea.useArea.x1 + pvd->dspArea.sizeX;
	pvd->dspArea.useArea.y2 = pvd->dspArea.useArea.y1 + pvd->dspArea.sizeY;
	
	//�ƶ����
	if( pPvd->cols == 0)
	{
		//����һ��
		pPvd->dspArea.cursorX = bnd->x1;
		pPvd->dspArea.cursorY += pvd->dspArea.sizeY;
		
	}
	else
	{
		if( ( pPvd->colcount % pPvd->cols) == 0)
		{
			pPvd->dspArea.cursorY += pvd->dspArea.sizeY + pPvd->lineSpacing;
			pPvd->dspArea.cursorX = bnd->x1;
		}
		else
		{
			pPvd->dspArea.cursorX += boundaryX + pPvd->columnGap;
			
		}
	}
	
	//׼��Ϊ��ͼԪ����ռ�
	//���Լ��ķ���Ŀռ���Ϊ�߽�
	pvd->colcount = 0;
	pvd->dspArea.cursorX = pvd->dspArea.useArea.x1;
	pvd->dspArea.cursorY = pvd->dspArea.useArea.y1;
	pvd->dspArea.boundary = &pvd->dspArea.useArea;
	List_map( pvd->t_childen, LayoutChld, cl);
	
	

	
	
}



//static void PlaceVd(void **ppvd, void *cl)


//����㷨���κβ�����һ������ʾ��ͼԪ��������
static void Layout(void **ppvd, void *cl)
{
	ViewData_t *pvd;
//	ViewData_t *pPvd = pvd->paraent;
	Composition	*pction = ( Composition *)cl;
	scArea_t	*bnd = NULL;
	short	boundaryX = 0;
	short	totalX;
	

	
	//����ͼԪ
	
 
	pvd = *ppvd;
	//������һ���Ƿ�ֵ
	pvd->dspArea.useArea.x1 = 0;
	pvd->dspArea.useArea.y1 = 0;
	pvd->dspArea.useArea.x2 = 0;
	pvd->dspArea.useArea.y2 = 0;
	
//	if(  pPvd == NULL)
	{
		bnd = &pction->mySCI.scBoundary;
//		pvd->dspArea.boundary = &pction->mySCI.scBoundary;
	}
	
	

	
	
	boundaryX = bnd->x2 - bnd->x1;
	if( pvd->dspArea.sizeY > ( bnd->y2 - bnd->y1) )
		return;
	//�����ܹ���Ҫ���п�, �Ȳ����Ƕ���
	totalX = pvd->dspArea.sizeX ;
	if( totalX > boundaryX)
	{
		return;
		
	}
	
	
	pvd->dspArea.curScInfo = &pction->mySCI;
	//�����к���
	if( pvd->dspArea.curScInfo->rowSize < pvd->dspArea.sizeY)
		pvd->dspArea.curScInfo->rowSize = pvd->dspArea.sizeY;
	if( pvd->dspArea.curScInfo->colSize < pvd->dspArea.sizeX)
		pvd->dspArea.curScInfo->colSize = pvd->dspArea.sizeX;
	
	
	
	//����Ҫ�������ģ�Ҫ������ʾ������ߴ���ȷ����ʼλ��
	if( pvd->notDealAli || pvd->dspArea.ali == ALIGN_LEFT)
	{
	
		//�����������x1,y1,x2,y2����ʼ��Ϊ����λ��
		pvd->dspArea.useArea.x1 = pction->mySCI.cursorX;
		
		
	}
	else if( pvd->dspArea.ali == ALIGN_MIDDLE)
	{
		pvd->dspArea.useArea.x1 = pction->mySCI.cursorX + ( ( boundaryX - totalX) >> 1);
	}
	else if( pvd->dspArea.ali == ALIGN_RIGHT)
	{
		pvd->dspArea.useArea.x1 = pction->mySCI.cursorX + boundaryX - totalX;
		
	}
	
//	pvd->dspArea.numRow = 1;
	pvd->dspArea.useArea.y1 = pction->mySCI.cursorY;
	pvd->dspArea.useArea.x2 = pvd->dspArea.useArea.x1 + pvd->dspArea.sizeX;
	pvd->dspArea.useArea.y2 = pction->mySCI.cursorY + pvd->dspArea.curScInfo->rowSize;
	
	
	
	
	
	//׼��Ϊ��ͼԪ����ռ�
	//���Լ��ķ���Ŀռ���Ϊ�߽�
	pvd->colcount = 0;
	pvd->dspArea.cursorX = pvd->dspArea.useArea.x1;
	pvd->dspArea.cursorY = pvd->dspArea.useArea.y1 ;
	pvd->dspArea.boundary = &pvd->dspArea.useArea;
	List_map( pvd->t_childen, LayoutChld, cl);
	

	//���������һ��
	pction->mySCI.cursorX = 0;
	pction->mySCI.cursorY += pvd->dspArea.curScInfo->rowSize;
	
	

	
	//�����м�� �м��
	pction->mySCI.cursorY += pction->lineSpacing;
	pction->mySCI.cursorX += pction->columnGap;
	
	
	
	
	

	
}

static void DspViewData(void **ppvd, void *cl)
{
	ViewData_t *pvd;
	Composition	*pction = ( Composition *)cl;
	
	if( ppvd == NULL)
		return ;
	
	pvd = *ppvd;
	
	if( pvd->gh)
		pvd->gh->draw( pvd->gh, &pvd->dspCnt, &pvd->dspArea);
	
	List_map( pvd->t_childen, DspViewData, cl);
	
}























//����

//static void VDShow(void **ppvd, void *cl)
//{
//	ViewData_t *pvd = *ppvd;
//	Composition	*pction = ( Composition *)cl;
//	//ÿ���ܹ���ʾ����	
//	short 	numInrow = 0;
//	
//	
//	short	displayed = 0;	//�Ѿ�����ʾ������
//	short	 y = 0;
//	short 	xend = 0 ;
//	short	xstart	= 0, ableNum;
//	char	c;
//	
//	int16_t		oldx1 = pvd->area_x1;
//	int16_t		oldy1 = pvd->area_y1;
//	
//	//�����������ɫ
//	pvd->gh->setClu( pvd->gh, pvd->colour);
//	pvd->gh->setFont( pvd->gh, pvd->font);
//	pvd->gh->insert( pvd->gh, pvd->data, pvd->len);
//	
//	pvd->gh->setWidth( pvd->gh, pvd->size_x);
//	pvd->gh->setHeight( pvd->gh, pvd->size_y);
//#if 1	
//	//�����к���
//	if( pction->rowSize[ pction->step] < pvd->size_y)
//		pction->rowSize[ pction->step] = pvd->size_y;
//	if( pction->colSize[ pction->step] < pvd->size_x)
//		pction->colSize[ pction->step] = pvd->size_x;
//	

//	displayed = 0;
//	//һ������ʾ
//	for( y = pvd->area_y1; y <= pvd->area_y2; y += pction->rowSize[ pction->step])
//	{
//		//�ڵ�һ�У�x����ʼλ���Ƿ��������x����ʼλ��
//		if( y == pvd->area_y1)
//			xstart = pvd->area_x1;
//		else
//			xstart = 0;
//		//�����һ�У�x��Ľ���λ���Ƿ�������Ľ���λ��
//		if( y == pvd->area_y2 || pvd->area_y1 == pvd->area_y2)
//			xend = pvd->area_x2;
//		else
//			xend = pction->lcdArea.LcdSizeX;
//		
//		ableNum = ( xend - xstart) / pvd->size_x;
//		
//		//������һ������ʾ�����ַ�
//		numInrow  = 0;
//		while(1)
//		{
//			c = pvd->data[ displayed];
//			
//			if( IS_CHINESE( c))
//			{
//				if( ableNum > 1)
//				{
//					numInrow += 2;
//					displayed += 2;
//					ableNum -= 2;
//				}
//				
//				
//				
//			}
//			else
//			{
//				if( ableNum >  0)
//				{
//					numInrow ++;
//					displayed ++;
//					ableNum -= 1;
//				}	
//				
//			}
//			//�����ʾ���˾��˳�
//			if( displayed >= pvd->len)
//				break;
//			//��������Ѿ�û�пռ��ˣ�����ʾ����
//			if( ableNum == 0)
//				break;
//			//������л�����ʾһ���ַ���������һ���ǲ����ܷŽ�ȥ��
//			if( ableNum == 1)
//			{		
//				c = pvd->data[ displayed];
//				if( !IS_CHINESE( c))
//				{
//					displayed ++;
//					numInrow ++;
//				}
//				break;
//			}
//				
//		}
//		pvd->gh->draw( pvd->gh, pvd->area_x1, pvd->area_y1, numInrow);
//		
//		//����һ��
//		pvd->area_x1 = 0;
//		pvd->area_y1 += pction->rowSize[ pction->step];
//		
//	}
//	
//	
//	pvd->done = displayed;
//	
//	
//	//�ָ���ʼλ�ã������´���ʾ
//	pvd->area_x1 = oldx1;
//	pvd->area_y1 = oldy1;

//#endif	
//	//��ʾ��ͼԪ
//	List_map( pvd->t_childen, ShowChild, cl);
//}



//static void DealChild(void **ppvd, void *cl)
//{
//	ViewData_t *pvd = *ppvd;
//	ViewData_t *pPvd = pvd->paraent;
//	
//	
//	
//	//Ϊ�˶Գ����Ը�Ԫ�صĳߴ�/2
////	pvd->area_x1 = pPvd->area_x1 + pPvd->size_x/2;
////	pvd->area_y1 = pPvd->area_y1 + pPvd->size_y/2;
//	
//	
//	if( SET_CHILDASWHOLE( pPvd->childAttr))
//	{
//		
//		pPvd->size_x += pvd->size_x * pvd->len;
//		
//	}
//	else
//	{
////		pPvd->size_x += pvd->size_x;
//		
//		
//	}
//	pPvd->size_y += pvd->size_y;
//	
//}

//static void VDapply(void **ppvd, void *cl)
//{
//	ViewData_t *pvd = *ppvd;
//	Composition *pction = ( Composition *)cl;
//	area_t	*area = &pction->lcdArea;
//	char		c;
//	uint8_t		dealNum = 0;
//	uint8_t		dealIdx = 0;
//	uint8_t		rowNum = 0;
//	uint16_t	usableX, usableY;
//	uint16_t	needX, needY;
//	
//	List_map( pvd->t_childen, DealChild, NULL);

//	
//	
//	//һ���ַ�һ���ַ�������
//	
//	
//	
//	//�����������x1,y1,x2,y2����ʼ��Ϊ����λ��
//	pvd->area_x1 = area->cursorX;
//	pvd->area_y1 = area->cursorY;
//	pvd->area_x2 = area->cursorX;
//	pvd->area_y2 = area->cursorY;
//	
//	pvd->dspArea.x1 = area->cursorX;
//	pvd->dspArea.y1 = area->cursorY;
//	pvd->dspArea.x2 = area->cursorX;
//	pvd->dspArea.y2 = area->cursorY;
//	
//	  
//	
//	//�����к���
//	if( pction->rowSize[ pction->step] < pvd->size_y)
//		pction->rowSize[ pction->step] = pvd->size_y;
//	if( pction->colSize[ pction->step] < pvd->size_x)
//		pction->colSize[ pction->step] = pvd->size_x;
//	
//	//���һ��x��һ���ַ�Ҳ�Ų��¾�ֱ���˳�
//	if( pvd->size_x > area->LcdSizeX)
//		return;
//	
//	
//	

//	//��ʼ�������ַ�����Ϊ0
//	dealIdx = 0; 
//	while( dealIdx < pvd->len)
//	{
//	
//	//ȡ��һ���ַ�,���ô�����Ϊ1
//		c = pvd->data[ dealIdx];
//		dealNum = 1;
//	//�ж��Ƿ�Ϊ���֣��Ǻ��ֵĻ�����������Ϊ2
//		if( IS_CHINESE( c))
//		{
//			dealNum = 2;
//			
//		}
//		//�ǻ��з��Ļ���ֱ�ӻ���
//		if( IS_BR( c))
//		{
//			pction->addRow( pction);
//			
//			rowNum ++;
//			dealIdx ++;
//			continue;
//		}
//		
//			
//	//���㱾�δ�����Ҫ��x��ռ�
//		if( IS_TAB( c))
//		{
//			
//			needX = 4 * pvd->size_x;
//		}
//		else
//		{
//			needX = dealNum * pvd->size_x;
//		}
//		
//	//���㱾�δ�����Ҫ��y��ռ�
//		needY =  pvd->size_y;
//		while(1)
//		{
//			//ʹ�ù��λ�ú���Ļ�Ľ���������x���y���ϵĿ��пռ� A
//		
//			usableX = area->LcdSizeX - area->cursorX;
//			usableY = area->LcdSizeY - area->cursorY;
//			//���y�᷽��ŵ���
//			if( needY < usableY)
//			{
//				//���x��ŵ��£������x���ƶ���Ӧ��λ�ã���������Ҳ�ƶ���Ӧ��λ�ã��˳����δ���
//				//������ռ�
//				if( needX < usableX)
//				{
//					area->cursorX += needX;
//					dealIdx += dealNum;					
//					break;
//				}
//				//���x��Ų��£�����һ��,�ƶ���꣬�ص�A�����ж�
//				else
//				{
//					pction->addRow( pction);
//					rowNum ++;
//				}
//			}
//			else
//			//���y��Ų��£�������Ҫ��ҳ��־��ֱ���˳�
//			{
//				pvd->more	= 1;
//				goto exit;
//			}
//	
//		}
//	}
//	
//	exit:
//		pvd->area_y2 += pction->rowSize[ pction->step] * rowNum;
//		pvd->area_x2 = area->cursorX;
//	
//		pvd->dspArea.x2 = area->cursorX;
//		pvd->dspArea.y2 += pction->rowSize[ pction->step] * rowNum;
//		pvd->dspArea.numRow = rowNum;
//		rowNum = 0;
//		return;
//	
//}


//static void ShowChild(void **ppvd, void *cl)
//{
//	ViewData_t *pvd = *ppvd;
//	ViewData_t *pPvd = pvd->paraent;
////	Composition	*ction = ( Composition *)cl;
////	short		x,y;
//	
//	if( SET_CHILDASWHOLE( pPvd->childAttr))
//	{

//		pvd->area_x1 = pPvd->area_x1 + 2;
//		pvd->area_y1 = pPvd->area_y1 + 2;
//		
//		pvd->dspArea.x1 = pPvd->area_x1 + 2;
//		pvd->dspArea.y1 = pPvd->area_y1 + 2;
//		
//		
//	}
//	else
//	{
//		
//		
//	}
//	
//	//�����������ɫ
//	pvd->gh->setClu( pvd->gh, pvd->colour);
//	pvd->gh->setFont( pvd->gh, pvd->font);
//	pvd->gh->insert( pvd->gh, pvd->data, pvd->len);
//	
//	pvd->gh->draw( pvd->gh, pvd->area_x1, pvd->area_y1, pvd->len);
//	
//}





CTOR( SimpCtor)
FUNCTION_SETTING( Compositor.vdLayout, Simp_VdLayout);



FUNCTION_SETTING( Compositor.show, Simp_Show);


END_CTOR

