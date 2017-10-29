#ifndef _INC_winHMI_H_
#define _INC_winHMI_H_
#include "HMI.h"
#include "commHMI.h"

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdint.h>
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//����������arg[0]����
#define	WINTYPE_TIPS					0			//��ʾ����
#define	WINTYPE_ALARM					1			
#define	WINTYPE_ERROR					2	
#define	WINTYPE_MUS_BND				3
#define	WINTYPE_MUS_UNTIL			4			//��λ��ѡ��
#define	WINTYPE_TIME_SET			5			//

//���ڱ�־��arg[1]�д���
#define WINFLAG_RETURN				1			//�ñ�־���ƴ�����ȷ�ϰ����з���ԭ����		
 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
CLASS(winHmi)
{
	EXTENDS(HMI);
	sheet  		*p_sht_bkpic;
	sheet  		*p_sht_title;
	sheet  		*p_sht_tips;
	sheet  		*p_sht_cur;
	
	void				*p_cmd_rcv;
	stategy_cmd			cmd_hdl;
	

//	sheet  		*arr_p_sht_data[NUM_CHANNEL];
//	sheet  			**pp_shts;
	uint8_t		f_row;
	uint8_t		f_col;
	uint8_t		win_type;
	uint8_t		none;
	
	
};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
winHmi *Get_winHmi(void) ;


#endif
