//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_system_H_
#define __INC_system_H_
#include <stdint.h>
#include "utils/time.h"
#include "utils/PHN_string.h"
#include "model_conf.h"

//#include "HMI/HMI.h"
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

#define HRD_VER						10		//1位小数

//系统配置类宏定义
#define CONF_KEYSCAN_POLL		1		//按键扫描：轮询方式,该值为0，则为中断触发式
#define	CONF_KEYSCAN_CYCLEMS	100
#define CURVE_POINT			240			//曲线点数最多240点,但是数据的长度要加上1个起始点


#define OP_ADD				0
#define OP_SUB				1
#define OP_MUX				2
#define OP_DIV				3

#define FSH_FM25_NUM			0
#define FSH_W25Q_NUM			1
#define FSH_OPT_SECTOR			0
#define FSH_OPT_BLOCK			1
#define FSH_OPT_CHIP			2
#define NUM_FSH					2
#define FSH_FLAG_READBACK_CHECK		2			//


#define	CHG_SYSTEM_CONF				1	
#define	CHG_MODCHN_CONF(n)			(1 << (n + 1))

#define SYSFLAG_CHN_ERR				1			//向通道板读取数据无回复时，认为通道板未连接
#define SYSFLAG_EFS_NOTREADY		2		//文件系统未就绪，可能正在擦除文件
#define SYSFLAG_POWEON				4
#define SYSFLAG_POWEROFF			8

#define SYSFLAG_ERR					0x10
#define SYSFLAG_URGENCY				0x20
#define SYSFLAG_READ_CET			0x40		//要求读取冷端温度
#define SYSFLAG_LOWSPACE			0x80		//存储历史数据的空间即将耗尽


#define STG_DEFAULT_ALARM			86400		//一天的秒数


#define SYS_TIME				phn_sys.sys_time
//#define	STSFLAG_TIME_CHANGE		8
//#define	STSFLAG_PARAM_SYSTEM_CHANGE		0x10
//#define	STSFLAG_PARAM_CHANNEL_CHANGE		0x20
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef enum {
	es_psd = 0,
	es_rcd_t_s,
	es_brk_cpl,
	es_brk_rss,
	es_cmn_md,
	es_baud,
	es_id,
	es_mdfy_prm,
	es_cold_end_way,
	es_CJC,
	es_vcs,
	es_beep,
}e_system_t;

typedef struct {
	uint8_t		num_chn;		//可以在超级设置界面中设置，但不得大于NUM_CHANNEL
	uint8_t		none;
	uint8_t		password[3];
	uint8_t		super_psd[3];
	
	//record_gap_s:sec 0:1 1:2 2:4 3:8 4:12 5:24 6:36 7:60 8:120 9:180 10:240 
	uint8_t		record_gap_s;		
	uint8_t		break_couple;		//断偶处理方式: 始点，保持，终点
	uint8_t		break_resistor;		//断阻处理
	uint8_t		communication_mode;			//仪表与pc连接： 通讯； 仪表与打印机连接: 打印
	
	uint8_t		id;											// 1 - 63
	uint8_t		baud_idx;
	uint8_t		sys_flag;
	uint8_t		cold_end_way;				//0 外部， 1 设定
	
	uint8_t		CJC;								//冷端补偿 0-99 为设定模式
	uint8_t		disable_modify_adjust_paramter;		//禁止修改调节参数
	uint8_t		show_chn_status;					//禁止通道状态显示
	uint8_t		enable_beep;											//按键声音允许
	
	int 			baud_rate;
	uint32_t		storage_alarm;			//历史数据存储容量快耗尽时会产生警告信号，这个值作为阀值
}system_conf_t;

//-----------HMI -----------------------------------------------
typedef struct {
	int			hmi_sem;
}hmi_mgr_t;
//---------- flash驱动的定义 --------------------------------------

typedef struct {
	uint16_t		num_sct;
	uint16_t		num_blk;
	uint32_t		total_pagenum;					///整个存储器的页数量
	
	uint16_t		page_size;						///一页的长度
	
	
	uint8_t			fnf_flag;
	uint8_t			none;
}fsh_info_t;

typedef struct {
	
	fsh_info_t	fnf;
	
//	int (*fsh_init)(void);
	void (*fsh_wp)(int p);
	void (*fsh_info)(fsh_info_t *nf);


	int	(*fsh_ersse)(int opt, uint32_t	num);
	void	(*fsh_ersse_addr)(uint32_t	start_addr, uint32_t size);
//	int	(*fsh_wr_sector)(uint8_t *wr_buf, uint16_t num_sector);
//	int	(*fsh_rd_sector)(uint8_t *rd_buf, uint16_t num_sector);
	int (*fsh_write)(uint8_t *wr_buf, uint32_t wr_addr, uint32_t num_bytes);
	int (*fsh_raw_write)(uint8_t *wr_buf, uint32_t wr_addr, uint32_t num_bytes);
	int (*fsh_read)(uint8_t *wr_buf, uint32_t rd_addr, uint32_t num_bytes);
	int (*fsh_raw_read)(uint8_t *wr_buf, uint32_t rd_addr, uint32_t num_bytes);
	int	(*fsh_lock)(uint32_t ms);
	int	(*fsh_unlock)(void);

	void (*fsh_flush)(void);
}flash_t;

//----------------文件系统的定义 --------------------------------

typedef enum {
	WR_SEEK_SET = 0,
	WR_SEEK_CUR = 1,
	WR_SEEK_END = 2,
	RD_SEEK_SET = 3,
	RD_SEEK_CUR = 4,
	RD_SEEK_END = 5,
	GET_WR_END = 6,
	GET_RD_END = 7,
}lseek_whence_t;
typedef struct {
	uint8_t			fsh_No;				//对应的存储器编号
	uint8_t			opt_mode;			//0  只读  1 读写
	uint8_t			file_flag;
	uint8_t			none;
	
	int				file_sem;
	
	uint16_t		start_page;
	uint16_t		num_page;
	
	uint32_t		file_size;
	uint32_t		read_position;
	uint32_t		write_position;

	char			*p_name;
}file_info_t;

typedef struct {
	uint8_t		num_partitions;
	
	//可靠性等级, 0 一般，在写文件时不回读判断 1 高，写文件时，要回读判断
	uint8_t		reliable_level;		
	uint16_t	err_code;
	
	//file_size在文件不存在时，需要创建时使用
	int		(*fs_open)(uint8_t		prt, char *path, char *mode, int	file_size);	
	int		(*fs_close)(int fd);
	int		(*fs_delete)(int fd, char *path);	//fd < 0的时候，通过path来查找文件
	int		(*fs_write)(int fd, uint8_t *p, int len);
	int		(*fs_raw_write)(int fd, uint8_t *p, int len);		//直接写入到硬件，不使用缓存
	int		(*fs_read)(int fd, uint8_t *p, int len);
	int		(*fs_raw_read)(int fd, uint8_t *p, int len);
	int		(*fs_resize)(int fd, char *name, int new_size);
	int 	(*fs_lseek)(int fd, int whence, int32_t offset);
	void 	(*fs_erase_file)(int fd, uint32_t start, uint32_t size);
	void 	(*fs_shutdown)(void);
	file_info_t*		(*fs_file_info)(int fd);
			
	
}fs_t;

//--------------------------------------------------------------------------

typedef struct {
	uint8_t				major_ver;
	uint8_t				minor_ver;
	uint8_t				usb_ready;		
	uint8_t				usb_device;		//0 无usb设备 1 有usb设�
	uint8_t				sys_flag;
	uint8_t				pwr_rcd_index;
	
	
	//显示相关
	uint16_t			lcd_cmd_bytes;
//	uint32_t			lcd_sem_wait_ms;
	
	struct  tm		sys_time;
	
	//通道板子上的信息
	uint8_t				DO_val;				//DO的实时值
	uint8_t				DO_err;
	uint16_t			code_end_temperature;
	
	
	
	
	
	//按键
	uint16_t				key_weight;
	uint16_t				hit_count;
	
	hmi_mgr_t				hmi_mgr;
	system_conf_t		sys_conf;
	flash_t					arr_fsh[NUM_FSH];
	fs_t						fs;
}system_t;
	

//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
extern	char 				*arr_p_vram[18];
extern	uint16_t		time_smp;			//下次记录的时间
//extern 	char				g_setting_chn;
extern	char				flush_flag;

extern system_t			phn_sys;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
int	Operate_in_range(int	arg1, int op, int arg2, int rangel, int rangeh);
int	Operate_in_range_keep(int	arg1, int op, int arg2, int rangel, int rangeh);
void System_init(void);
void System_power_off(void);

void 			System_time(struct  tm *stime);
uint32_t  SYS_time_sec(void);
int 			System_set_time(struct  tm *stime);


extern void System_default(void);
extern void SYS_Reset(void);




int SYS_Commit(void);
#endif
