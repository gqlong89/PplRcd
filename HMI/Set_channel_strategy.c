#include <stdint.h>
#include "Setting_HMI.h"
#include "ModelFactory.h"

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

static int ChnStrategy_entry(int row, int col, void *pp_text);
static int Cns_key_up(void *arg);
static int Cns_key_dn(void *arg);
static int Cns_key_lt(void *arg);
static int Cns_key_rt(void *arg);
static int Cns_key_er(void *arg);
static int Cns_init(void *arg);
static int Cns_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf);
strategy_t	g_chn_strategy = {
	ChnStrategy_entry,
	Cns_init,
	Cns_key_up,
	Cns_key_dn,
	Cns_key_lt,
	Cns_key_rt,
	Cns_key_er,
	Cns_get_focusdata,
};
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------
int g_set_weight = 1;			//��ֵ����ݰ����Ķ������仯
//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static char *const arr_p_chnnel_entry[11] = {"ͨ����", "λ��", "�ź�����", "���̵�λ", \
 "��������", "��������", "��¼����", "�˲�ʱ��", "С�ź��г�", "������ K", "������ B"
};

static char *arr_p_vram[11];
static char		cur_set_chn = 0;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void Cns_update_len(strategy_focus_t *p_syf);
static void Cns_update_content(int op, int weight);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int ChnStrategy_entry(int row, int col, void *pp_text)
{
	char **pp = (char **)pp_text;
	Model_chn		*p_mc = Get_Mode_chn(cur_set_chn);
	Model				*p_md = SUPER_PTR(p_mc, Model);
	if(col == 0) {
		
		if(row > 10)
			return 0;
		*pp = arr_p_chnnel_entry[row];
		return strlen(arr_p_chnnel_entry[row]);
	} else if(col == 1){
		switch(row) 
		{
			case 0:
				sprintf(arr_p_vram[row], "%d", cur_set_chn);
				break;
			case 1:		//λ��
				sprintf(arr_p_vram[row], "%d", p_mc->chni.tag_NO);
				break;
			case 2:		//�ź�����
				p_md->to_string(p_md, AUX_SIGNALTYPE, arr_p_vram[row]);
				break;
			case 3:		//��λ
				p_md->to_string(p_md, AUX_UNIT, arr_p_vram[row]);
				break;
			case 4:		//����
				p_md->to_string(p_md, chnaux_lower_limit, arr_p_vram[row]);
				break;
			case 5:		//����
				p_md->to_string(p_md, chnaux_upper_limit, arr_p_vram[row]);
				break;
			case 6:		//��¼����
				p_md->to_string(p_md, chnaux_record_mb, arr_p_vram[row]);
				break;
			case 7:		//�˲�ʱ��
				p_md->to_string(p_md, chnaux_filter_ts, arr_p_vram[row]);
				break;
			case 8:		//С�ź��г�
				p_md->to_string(p_md, chnaux_small_signal, arr_p_vram[row]);
				break;
			case 9:		//������
				p_md->to_string(p_md, chnaux_k, arr_p_vram[row]);
				
				break;
			case 10:		//������
				p_md->to_string(p_md, chnaux_b, arr_p_vram[row]);
				
				break;
			default:
				goto exit;
			
		}
		
		*pp = arr_p_vram[row];
		return strlen(arr_p_vram[row]);
		
		
	}
	exit:	
	return 0;
}

static int Cns_init(void *arg)
{
	int i = 0;
	memset(&g_chn_strategy.sf, 0, sizeof(g_chn_strategy.sf));
	g_chn_strategy.sf.f_col = 1;
	g_chn_strategy.sf.f_row = 0;
	g_chn_strategy.sf.start_byte = 0;
	g_chn_strategy.sf.num_byte = 1;

	VRAM_init();
	for(i = 0; i < 11; i++) {
		
		arr_p_vram[i] = VRAM_alloc(48);
		
	}
	
	g_set_weight = 1;
	return RET_OK;
}
static int Cns_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf)
{
	strategy_focus_t *p_syf = &g_chn_strategy.sf;
	char		**pp_vram = (char **)pp_data;
	int ret = 0;
	
	if(p_syf->f_row > 10) {
		return -1;
	}
	
	if(p_in_syf)
		p_syf = p_in_syf;
	ret = p_syf->num_byte;
	
	
	
	*pp_vram = arr_p_vram[p_syf->f_row] + p_syf->start_byte;
	

	
	
	
	
	return ret;
	
}



static int Cns_key_up(void *arg)
{
	
//	Model_chn			*p_mc = Get_Mode_chn(cur_set_chn);
//	Model				*p_md = SUPER_PTR(p_mc, Model);
	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
//	strategy_focus_t 	*p_syf = &g_chn_strategy.sf;
//	char			*p;
	int 			ret = RET_OK;
	
	if(arg) {
		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
		
	}
	
	//
	if(kt.key_type == SY_KEYTYPE_LONGPUSH) {
		g_set_weight += 10;
		
	} else {
		g_set_weight = 1;
	}
	Cns_update_content(OP_ADD, g_set_weight);
	
	
	return ret;
}

static int Cns_key_dn(void *arg)
{
	
//	Model_chn			*p_mc = Get_Mode_chn(cur_set_chn);
//	Model				*p_md = SUPER_PTR(p_mc, Model);
	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
//	strategy_focus_t 	*p_syf = &g_chn_strategy.sf;
	int 				ret = RET_OK;
	
	if(arg) {
		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
		
	}
	
	//
	if(kt.key_type == SY_KEYTYPE_LONGPUSH) {
		g_set_weight += 10;
		
	} else {
		g_set_weight = 1;
	}
	
	Cns_update_content(OP_SUB, g_set_weight);
	
		
	return ret;
}

static int Cns_key_rt(void *arg)
{
	strategy_focus_t *p_syf = &g_chn_strategy.sf;
	int ret = RET_OK;
//	switch(p_syf->f_row) {
//		case 0:
//			
//			
//			break;
//		default:
//			ret = ERR_OPT_FAILED;
//			break;
//		
//		
//	}
	
	
	if(p_syf->f_row < 10)
		p_syf->f_row ++;
	else {
		p_syf->f_row = 0;
		p_syf->f_col = 1;
		ret = -1;
	}
		
	 
	Cns_update_len(p_syf);
	return ret;
}


static int Cns_key_lt(void *arg)
{
	strategy_focus_t *p_syf = &g_chn_strategy.sf;
	int ret = RET_OK;
//	switch(p_syf->f_row) {
//		case 0:
//			p_syf->num_byte = 1;
//			if(p_syf->start_byte == 0)
//				p_syf->start_byte = 17;
//			else {
//				p_syf->start_byte -= 1;
//			}
//			break;
//		default:
//			ret = ERR_OPT_FAILED;
//			break;
//		
//		
//	}
	if(p_syf->f_row )
		p_syf->f_row --;
	else {
		p_syf->f_row = 10;
		ret = -1;
		
	}
	
	Cns_update_len(p_syf);
	return ret;
}



static int Cns_key_er(void *arg)
{
	
	return -1;
}


static void Cns_update_len(strategy_focus_t *p_syf)
{
	p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	
	//�ѵ�λ�޳���
	switch(p_syf->f_row)
	{
		case 6:		//x M
		case 7:		//x S
		case 8:		//x %
			p_syf->num_byte -= 2;
			break;
		
	}
	
	
}

static void Cns_update_content(int op, int weight)
{
	Model_chn			*p_mc = Get_Mode_chn(cur_set_chn);
	Model				*p_md = SUPER_PTR(p_mc, Model);
	strategy_focus_t 	*p_syf = &g_chn_strategy.sf;
	
	
	
	
	switch(p_syf->f_row) 
	{
		case 0:
			cur_set_chn = Operate_in_tange(cur_set_chn, op, 1, 0, NUM_CHANNEL);
			g_chn_strategy.cmd_hdl(g_chn_strategy.p_cmd_rcv, sycmd_reflush, NULL);
//			Str_Calculations(arr_p_vram[p_syf->f_row], 1,  op, weight, 0, NUM_CHANNEL);
			break;
		case 1:		//λ��
			Str_Calculations(arr_p_vram[p_syf->f_row], 1,  op, weight, 0, 9);
			break;
		case 2:		//�ź�����
			p_md->modify_str_conf(p_md, AUX_SIGNALTYPE, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 3:		//��λ
			p_md->modify_str_conf(p_md, AUX_UNIT, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 4:		//����
			p_md->modify_str_conf(p_md, chnaux_lower_limit, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 5:		//����
			p_md->modify_str_conf(p_md, chnaux_upper_limit, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 6:		//��¼����
			p_md->modify_str_conf(p_md, chnaux_record_mb, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 7:		//�˲�ʱ��
			p_md->modify_str_conf(p_md, chnaux_filter_ts, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 8:		//С�ź��г�
			p_md->modify_str_conf(p_md, chnaux_small_signal, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 9:		//������
			p_md->modify_str_conf(p_md, chnaux_k, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 10:		//������
			p_md->modify_str_conf(p_md, chnaux_b, arr_p_vram[p_syf->f_row], op, weight);
			break;
		default:
			break;
		
	}
	
	Cns_update_len(p_syf);
	
	
}


