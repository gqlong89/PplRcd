
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_Component_Button_H__
#define __INC_Component_Button_H__

#include <stdint.h>
#include <string.h>
#include "lw_oopc.h"

#include "sdhDef.h"

//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define		NUM_BUTTON			4

#define		BTN_TYPE_MENU		0
#define		BTN_TYPE_COPY		1
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
INTERFACE(Button_receive)
{
	void	(*btn_recv)(Button_receive *self, uint8_t	btn_id);
	
};


CLASS(Button)
{
	void 		*p_self;
	uint8_t		btn_id;
	uint8_t		set_act_btn;
	uint8_t		cur_focush_btn;
	uint8_t		none;
	void 		(*init)(Button *self);
	int			(*build_each_btn)(uint8_t	seq, uint8_t btn_type, Button_receive *br);
	void		(*clean_btn)(void);
	void		(*clean_focus)(void);
	void		(*show_focus)(void);
	int			(*move_focus)(uint8_t	position);		//direct 0��ʾ�ڵ�һ����Ծ��ť 1 ��ʾ����һ�� 2 ��ʾ��ǰһ��
	void		(*deal_enter)(void);
};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
Button	*BTN_Get_Sington(void);


#endif

