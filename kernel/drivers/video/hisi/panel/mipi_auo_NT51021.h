#include "hisi_fb.h"

#ifndef MIPI_AUO_NT51021
#define MIPI_AUO_NT51021



/*truly*/
char truly_control_reg[] = {
	0x8C,0x04,      
};

static char auo_global_reset[] = {
	0x01,0x00, 
};  
                  
char truly_set_v1v14[] = {
	0xFD,0x1A,      
};                

/*red*/                  
char select_page1_step0[] = {
	0x83,0xAA,      
};                
                  
char select_page1_step1[] = {
	0x84,0x11,      
};

/*green*/             
char select_page2_step0[] = {
	0x83,0xBB,      
};                
                  
char select_page2_step1[] = {
	0x84,0x22,      
};             

/*blue*/                  
char select_page3_step0[] = {
	0x83,0xCC,      
};                
                  
char select_page3_step1[] = {
	0x84,0x33,      
};                
                  
char mipi_rx[] = {
	0xA9,0x4B,      
};                

/*truly*/
char truly_gamma_red_param1[] = {
	0xC0,0x14,      
};                
                  
char truly_gamma_red_param2[] = {
	0xC1,0x15,      
};                
                  
char truly_gamma_red_param3[] = {
	0xC2,0x23,      
};                
                  
char truly_gamma_red_param4[] = {
	0xC3,0x31,      
};                
                  
char truly_gamma_red_param5[] = {
	0xC4,0x3D,      
};                
                  
char truly_gamma_red_param6[] = {
	0xC5,0x47,      
};                
                  
char truly_gamma_red_param7[] = {
	0xC6,0x50,      
};                
                  
char truly_gamma_red_param8[] = {
	0xC7,0x58,      
};                
                  
char truly_gamma_red_param9[] = {
	0xC8,0x5F,      
};                
                  
char truly_gamma_red_param10[] = {
	0xC9,0xCF,      
};                
                  
char truly_gamma_red_param11[] = {
	0xCA,0xD2,      
};                
                  
char truly_gamma_red_param12[] = {
	0xCB,0xE8,      
};                
                  
char truly_gamma_red_param13[] = {
	0xCC,0xF2,      
};                
                  
char truly_gamma_red_param14[] = {
	0xCD,0xF6,      
};                
                  
char truly_gamma_red_param15[] = {
	0xCE,0xF7,      
};                
                  
char truly_gamma_red_param16[] = {
	0xCF,0xF8,      
};                
                  
char truly_gamma_red_param17[] = {
	0xD0,0xFF,      
};                
                  
char truly_gamma_red_param18[] = {
	0xD1,0x15,      
};                
                  
char truly_gamma_red_param19[] = {
	0xD2,0x2A,      
};                
                  
char truly_gamma_red_param20[] = {
	0xD3,0x4C,      
};                
                  
char truly_gamma_red_param21[] = {
	0xD4,0x54,      
};                
                  
char truly_gamma_red_param22[] = {
	0xD5,0xB3,      
};                
                  
char truly_gamma_red_param23[] = {
	0xD6,0xBA,      
};                
                  
char truly_gamma_red_param24[] = {
	0xD7,0xC1,      
};                
                  
char truly_gamma_red_param25[] = {
	0xD8,0xC9,      
};                
                  
char truly_gamma_red_param26[] = {
	0xD9,0xD3,      
};                
                  
char truly_gamma_red_param27[] = {
	0xDA,0xDD,      
};                
                  
char truly_gamma_red_param28[] = {
	0xDB,0xE8,      
};                
                  
char truly_gamma_red_param29[] = {
	0xDC,0xF6,      
};                
                  
char truly_gamma_red_param30[] = {
	0xDD,0xFE,      
};                
                  
char truly_gamma_red_param31[] = {
	0xDE,0x00,      
};                
                  
char truly_gamma_red_param32[] = {
	0xDF,0x2F,      
};                
                  
char truly_gamma_red_param33[] = {
	0xE0,0x14,      
};                
                  
char truly_gamma_red_param34[] = {
	0xE1,0x15,      
};                
                  
char truly_gamma_red_param35[] = {
	0xE2,0x23,      
};                
                  
char truly_gamma_red_param36[] = {
	0xE3,0x31,      
};                
                  
char truly_gamma_red_param37[] = {
	0xE4,0x3D,      
};                
                  
char truly_gamma_red_param38[] = {
	0xE5,0x47,      
};                
                  
char truly_gamma_red_param39[] = {
	0xE6,0x50,      
};                
                  
char truly_gamma_red_param40[] = {
	0xE7,0x58,      
};                
                  
char truly_gamma_red_param41[] = {
	0xE8,0x5F,      
};                
                  
char truly_gamma_red_param42[] = {
	0xE9,0xCF,      
};                
                  
char truly_gamma_red_param43[] = {
	0xEA,0xD2,      
};                
                  
char truly_gamma_red_param44[] = {
	0xEB,0xE8,      
};                
                  
char truly_gamma_red_param45[] = {
	0xEC,0xF2,      
};                
                  
char truly_gamma_red_param46[] = {
	0xED,0xF6,      
};                
                  
char truly_gamma_red_param47[] = {
	0xEE,0xF7,      
};                
                  
char truly_gamma_red_param48[] = {
	0xEF,0xF8,      
};                
                  
char truly_gamma_red_param49[] = {
	0xF0,0xFF,      
};                
                  
char truly_gamma_red_param50[] = {
	0xF1,0x15,      
};                
                  
char truly_gamma_red_param51[] = {
	0xF2,0x2A,      
};                
                  
char truly_gamma_red_param52[] = {
	0xF3,0x4C,      
};                
                  
char truly_gamma_red_param53[] = {
	0xF4,0x54,      
};                
                  
char truly_gamma_red_param54[] = {
	0xF5,0xB3,      
};                
                  
char truly_gamma_red_param55[] = {
	0xF6,0xBA,      
};                
                  
char truly_gamma_red_param56[] = {
	0xF7,0xC1,      
};                
                  
char truly_gamma_red_param57[] = {
	0xF8,0xC9,      
};                
                  
char truly_gamma_red_param58[] = {
	0xF9,0xD3,      
};                
                  
char truly_gamma_red_param59[] = {
	0xFA,0xDD,      
};                
                  
char truly_gamma_red_param60[] = {
	0xFB,0xE8,      
};                
                  
char truly_gamma_red_param61[] = {
	0xFC,0xF6,      
};                
                  
char truly_gamma_red_param62[] = {
	0xFD,0xFE,      
};                
                  
char truly_gamma_red_param63[] = {
	0xFE,0x00,      
};                
                  
char truly_gamma_red_param64[] = {
	0xFF,0x2F,      
};         

char truly_gamma_green_param1[] = {
	0xC0,0x14,      
};                
                  
char truly_gamma_green_param2[] = {
	0xC1,0x15,      
};                
                  
char truly_gamma_green_param3[] = {
	0xC2,0x23,      
};                
                  
char truly_gamma_green_param4[] = {
	0xC3,0x31,      
};                
                  
char truly_gamma_green_param5[] = {
	0xC4,0x3D,      
};                
                  
char truly_gamma_green_param6[] = {
	0xC5,0x47,      
};                
                  
char truly_gamma_green_param7[] = {
	0xC6,0x50,      
};                
                  
char truly_gamma_green_param8[] = {
	0xC7,0x58,      
};                
                  
char truly_gamma_green_param9[] = {
	0xC8,0x5F,      
};                
                  
char truly_gamma_green_param10[] = {
	0xC9,0xCF,      
};                
                  
char truly_gamma_green_param11[] = {
	0xCA,0xD2,      
};                
                  
char truly_gamma_green_param12[] = {
	0xCB,0xE8,      
};                
                  
char truly_gamma_green_param13[] = {
	0xCC,0xF2,      
};                
                  
char truly_gamma_green_param14[] = {
	0xCD,0xF6,      
};                
                  
char truly_gamma_green_param15[] = {
	0xCE,0xF7,      
};                
                  
char truly_gamma_green_param16[] = {
	0xCF,0xF8,      
};                
                  
char truly_gamma_green_param17[] = {
	0xD0,0xFF,      
};                
                  
char truly_gamma_green_param18[] = {
	0xD1,0x15,      
};                
                  
char truly_gamma_green_param19[] = {
	0xD2,0x2A,      
};                
                  
char truly_gamma_green_param20[] = {
	0xD3,0x4C,      
};                
                  
char truly_gamma_green_param21[] = {
	0xD4,0x54,      
};                
                  
char truly_gamma_green_param22[] = {
	0xD5,0xB3,      
};                
                  
char truly_gamma_green_param23[] = {
	0xD6,0xBA,      
};                
                  
char truly_gamma_green_param24[] = {
	0xD7,0xC1,      
};                
                  
char truly_gamma_green_param25[] = {
	0xD8,0xC9,      
};                
                  
char truly_gamma_green_param26[] = {
	0xD9,0xD3,      
};                
                  
char truly_gamma_green_param27[] = {
	0xDA,0xDD,      
};                
                  
char truly_gamma_green_param28[] = {
	0xDB,0xE8,      
};                
                  
char truly_gamma_green_param29[] = {
	0xDC,0xF6,      
};                
                  
char truly_gamma_green_param30[] = {
	0xDD,0xFE,      
};                
                  
char truly_gamma_green_param31[] = {
	0xDE,0x00,      
};                
                  
char truly_gamma_green_param32[] = {
	0xDF,0x2F,      
};                
                  
char truly_gamma_green_param33[] = {
	0xE0,0x14,      
};                
                  
char truly_gamma_green_param34[] = {
	0xE1,0x15,      
};                
                  
char truly_gamma_green_param35[] = {
	0xE2,0x23,      
};                
                  
char truly_gamma_green_param36[] = {
	0xE3,0x31,      
};                
                  
char truly_gamma_green_param37[] = {
	0xE4,0x3D,      
};                
                  
char truly_gamma_green_param38[] = {
	0xE5,0x47,      
};                
                  
char truly_gamma_green_param39[] = {
	0xE6,0x50,      
};                
                  
char truly_gamma_green_param40[] = {
	0xE7,0x58,      
};                
                  
char truly_gamma_green_param41[] = {
	0xE8,0x5F,      
};                
                  
char truly_gamma_green_param42[] = {
	0xE9,0xCF,      
};                
                  
char truly_gamma_green_param43[] = {
	0xEA,0xD2,      
};                
                  
char truly_gamma_green_param44[] = {
	0xEB,0xE8,      
};                
                  
char truly_gamma_green_param45[] = {
	0xEC,0xF2,      
};                
                  
char truly_gamma_green_param46[] = {
	0xED,0xF6,      
};                
                  
char truly_gamma_green_param47[] = {
	0xEE,0xF7,      
};                
                  
char truly_gamma_green_param48[] = {
	0xEF,0xF8,      
};                
                  
char truly_gamma_green_param49[] = {
	0xF0,0xFF,      
};                
                  
char truly_gamma_green_param50[] = {
	0xF1,0x15,      
};                
                  
char truly_gamma_green_param51[] = {
	0xF2,0x2A,      
};                
                  
char truly_gamma_green_param52[] = {
	0xF3,0x4C,      
};                
                  
char truly_gamma_green_param53[] = {
	0xF4,0x54,      
};                
                  
char truly_gamma_green_param54[] = {
	0xF5,0xB3,      
};                
                  
char truly_gamma_green_param55[] = {
	0xF6,0xBA,      
};                
                  
char truly_gamma_green_param56[] = {
	0xF7,0xC1,      
};                
                  
char truly_gamma_green_param57[] = {
	0xF8,0xC9,      
};                
                  
char truly_gamma_green_param58[] = {
	0xF9,0xD3,      
};                
                  
char truly_gamma_green_param59[] = {
	0xFA,0xDD,      
};                
                  
char truly_gamma_green_param60[] = {
	0xFB,0xE8,      
};                
                  
char truly_gamma_green_param61[] = {
	0xFC,0xF6,      
};                
                  
char truly_gamma_green_param62[] = {
	0xFD,0xFE,      
};                
                  
char truly_gamma_green_param63[] = {
	0xFE,0x00,      
};                
                  
char truly_gamma_green_param64[] = {
	0xFF,0x2F,      
}; 

char truly_gamma_blue_param1[] = {
	0xC0,0x14,      
};                
                  
char truly_gamma_blue_param2[] = {
	0xC1,0x15,      
};                
                  
char truly_gamma_blue_param3[] = {
	0xC2,0x23,      
};                
                  
char truly_gamma_blue_param4[] = {
	0xC3,0x31,      
};                
                  
char truly_gamma_blue_param5[] = {
	0xC4,0x3D,      
};                
                  
char truly_gamma_blue_param6[] = {
	0xC5,0x47,      
};                
                  
char truly_gamma_blue_param7[] = {
	0xC6,0x50,      
};                
                  
char truly_gamma_blue_param8[] = {
	0xC7,0x58,      
};                
                  
char truly_gamma_blue_param9[] = {
	0xC8,0x5F,      
};                
                  
char truly_gamma_blue_param10[] = {
	0xC9,0xCF,      
};                
                  
char truly_gamma_blue_param11[] = {
	0xCA,0xD2,      
};                
                  
char truly_gamma_blue_param12[] = {
	0xCB,0xE8,      
};                
                  
char truly_gamma_blue_param13[] = {
	0xCC,0xF2,      
};                
                  
char truly_gamma_blue_param14[] = {
	0xCD,0xF6,      
};                
                  
char truly_gamma_blue_param15[] = {
	0xCE,0xF7,      
};                
                  
char truly_gamma_blue_param16[] = {
	0xCF,0xF8,      
};                
                  
char truly_gamma_blue_param17[] = {
	0xD0,0xFF,      
};                
                  
char truly_gamma_blue_param18[] = {
	0xD1,0x15,      
};                
                  
char truly_gamma_blue_param19[] = {
	0xD2,0x2A,      
};                
                  
char truly_gamma_blue_param20[] = {
	0xD3,0x4C,      
};                
                  
char truly_gamma_blue_param21[] = {
	0xD4,0x54,      
};                
                  
char truly_gamma_blue_param22[] = {
	0xD5,0xB3,      
};                
                  
char truly_gamma_blue_param23[] = {
	0xD6,0xBA,      
};                
                  
char truly_gamma_blue_param24[] = {
	0xD7,0xC1,      
};                
                  
char truly_gamma_blue_param25[] = {
	0xD8,0xC9,      
};                
                  
char truly_gamma_blue_param26[] = {
	0xD9,0xD3,      
};                
                  
char truly_gamma_blue_param27[] = {
	0xDA,0xDD,      
};                
                  
char truly_gamma_blue_param28[] = {
	0xDB,0xE8,      
};                
                  
char truly_gamma_blue_param29[] = {
	0xDC,0xF6,      
};                
                  
char truly_gamma_blue_param30[] = {
	0xDD,0xFE,      
};                
                  
char truly_gamma_blue_param31[] = {
	0xDE,0x00,      
};                
                  
char truly_gamma_blue_param32[] = {
	0xDF,0x2F,      
};                
                  
char truly_gamma_blue_param33[] = {
	0xE0,0x14,      
};                
                  
char truly_gamma_blue_param34[] = {
	0xE1,0x15,      
};                
                  
char truly_gamma_blue_param35[] = {
	0xE2,0x23,      
};                
                  
char truly_gamma_blue_param36[] = {
	0xE3,0x31,      
};                
                  
char truly_gamma_blue_param37[] = {
	0xE4,0x3D,      
};                
                  
char truly_gamma_blue_param38[] = {
	0xE5,0x47,      
};                
                  
char truly_gamma_blue_param39[] = {
	0xE6,0x50,      
};                
                  
char truly_gamma_blue_param40[] = {
	0xE7,0x58,      
};                
                  
char truly_gamma_blue_param41[] = {
	0xE8,0x5F,      
};                
                  
char truly_gamma_blue_param42[] = {
	0xE9,0xCF,      
};                
                  
char truly_gamma_blue_param43[] = {
	0xEA,0xD2,      
};                
                  
char truly_gamma_blue_param44[] = {
	0xEB,0xE8,      
};                
                  
char truly_gamma_blue_param45[] = {
	0xEC,0xF2,      
};                
                  
char truly_gamma_blue_param46[] = {
	0xED,0xF6,      
};                
                  
char truly_gamma_blue_param47[] = {
	0xEE,0xF7,      
};                
                  
char truly_gamma_blue_param48[] = {
	0xEF,0xF8,      
};                
                  
char truly_gamma_blue_param49[] = {
	0xF0,0xFF,      
};                
                  
char truly_gamma_blue_param50[] = {
	0xF1,0x15,      
};                
                  
char truly_gamma_blue_param51[] = {
	0xF2,0x2A,      
};                
                  
char truly_gamma_blue_param52[] = {
	0xF3,0x4C,      
};                
                  
char truly_gamma_blue_param53[] = {
	0xF4,0x54,      
};                
                  
char truly_gamma_blue_param54[] = {
	0xF5,0xB3,      
};                
                  
char truly_gamma_blue_param55[] = {
	0xF6,0xBA,      
};                
                  
char truly_gamma_blue_param56[] = {
	0xF7,0xC1,      
};                
                  
char truly_gamma_blue_param57[] = {
	0xF8,0xC9,      
};                
                  
char truly_gamma_blue_param58[] = {
	0xF9,0xD3,      
};                
                  
char truly_gamma_blue_param59[] = {
	0xFA,0xDD,      
};                
                  
char truly_gamma_blue_param60[] = {
	0xFB,0xE8,      
};                
                  
char truly_gamma_blue_param61[] = {
	0xFC,0xF6,      
};                
                  
char truly_gamma_blue_param62[] = {
	0xFD,0xFE,      
};                
                  
char truly_gamma_blue_param63[] = {
	0xFE,0x00,      
};                
                  
char truly_gamma_blue_param64[] = {
	0xFF,0x2F,      
};  

char display_on[] = {
	0x29,
};

//stbyb low                                                                                                                            
char exit_sleep[] = {                                       
	//0x11,    
	0x89,0x01,
};

char sleep_in[] = {                                       
	//0x10, 
	0x89,0x03
};
//stbyb hi
//char exit_sleep[] = {                                       
//	//0x11,    
//	0x89,0x03,
//};
//
//char sleep_in[] = {                                       
//	//0x10, 
//	0x89,0x01
//};
char sel_page_step0[] = {
	0x83, 0x00,                                                            
};

char sel_page_step1[] = {
	0x84, 0x00,                                                            
};

char sel_mipi_mode[] = {
	0x8F,0xA5,
};

char op_adjust[] = {
	0xA9,0x4B,
};

char bl_level[] = {                                       
	0x9f, 0x00,                                                            
};

//ALS input control  page2   C0:PWM signal input;  40:8-bit value input;
char als_input_control[] = {                                       
	0x93, 0x40,                                                            
};

//SRE: ALS_DUTY value = 128 or PWM signal duty = 50% means indoors


//truly cabc mode
char truly_cabc_off[] = {
	0x90, 0x00,
};
char truly_cabc_STILL[] = {
	0x90, 0x40,
};
char truly_cabc_MOVING[] = {
	0x90, 0xC0,
};

//auo cabc mode
char auo_cabc_off[] = {
	0x90, 0xC0,
};
char auo_cabc_STILL[] = {
	0x90, 0x40,
};
char auo_cabc_MOVING[] = {
	0x90, 0x00,
};

//close adl dimming bit[8]:1 ; pwm fre bit[2:1:0]=110; 
//when cabc cloesed, bl setting will be sensitived.
char close_als_dimming[] = {
	0x91, 0xA0,
	//0x91, 0x97,
};

// 2steps
char moving_dimming_steps[] = {
	0x94, 0x18,
};

// 2steps
char still_dimming_steps[] = {
	0x95,0x00,
};

char auto_moving_mode_detect[] = {
	0x9B, 0x8C,
};

char min_duty_clamp[] = {
	0x96, 0x8,
};

/*minimum duty = 80% begin*/
//cabc pwm register control
char cabc_pwm0_reg[] = {
	0xA1, 0xFF,
};

char cabc_pwm1_reg[] = {
	0xA2, 0xFA,
};

char cabc_pwm2_reg[] = {
	0xA3, 0xF3,
};

char cabc_pwm3_reg[] = {
	0xA4, 0xED,
};

char cabc_pwm4_reg[] = {
	0xA5, 0xE7,
};

char cabc_pwm5_reg[] = {
	0xA6, 0xE2,
};

char cabc_pwm6_reg[] = {
	0xA7, 0xDC,
};

char cabc_pwm7_reg[] = {
	0xA8, 0xD7,
};

char cabc_pwm8_reg[] = {
	0xA9, 0xD1,
};

char cabc_pwm9_reg[] = {
	0xAA, 0xCC,
};

//cabc data enhanced control
char cabc_delta_gray64[] = {
	0xB4,0x1C,
};

char cabc_delta_gray128[] = {
	0xB5,0x38,
};

char cabc_delta_gray192[] = {
	0xB6,0x30,
};
/*minimum duty = 80% end*/

char auo_ce_enable[] = {
	0x90, 0x13,
};

char auo_ce_disable[] = {
	0x90, 0x10,
};

//vivid color: control the whole ce  [5--0]  0~63
char auo_vivid_color[] = {
	0x91, 0x04,
};

//smart color: adjust a part of ce
char auo_red_hue_ratio[] = {
	0x92, 0x07,
};

char auo_yellow_hue_ratio[] = {
	0x93, 0x04,
};

char auo_green_hue_ratio[] = {
	0x94, 0x05,
};

char auo_cyan_hue_ratio[] = {
	0x95, 0x00,
};

char auo_blue_hue_ratio[] = {
	0x96, 0x04,
};

char auo_magenta_hue_ratio[] = {
	0x97, 0x06,
};

//ce enable register  page 3 90H
/*-------------------------------------------------------------------------------
| D[7] D[6] D[5] |           D[4]            |    D[3]     |        D[2]       |     D[1]            |     D[0]     |
---------------------------------------------------------------------------------
|          -            |Text_protection_en  | Edge_en  |Skin_hold_en  | Smt_hue_en     |  Allce_en  |
---------------------------------------------------------------------------------*/
char ce_enable[] = {
	0x90, 0x13,
};

char ce_disable[] = {
	0x90, 0x10,
};

//vivid color: control the whole ce  [5--0]  0~63
static char truly_vivid_color[] = {
	0x91, 0x20,
};

//smart color: adjust a part of ce
/*ce level control begin*/
char truly_red_hue_ratio[] = {
	0x92, 0x20,
};

char truly_yellow_hue_ratio[] = {
	0x93, 0x20,
};

char truly_green_hue_ratio[] = {
	0x94, 0x20,
};

char truly_cyan_hue_ratio[] = {
	0x95, 0x20,
};

char truly_blue_hue_ratio[] = {
	0x96, 0x20,
};

char truly_magenta_hue_ratio[] = {
	0x97, 0x20,
};
/*ce level control end*/

struct dsi_cmd_desc truly_cabc_off_cmds[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step0), select_page2_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step1), select_page2_step1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(truly_cabc_off), truly_cabc_off},
};

struct dsi_cmd_desc truly_cabc_still_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step0), select_page2_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step1), select_page2_step1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(truly_cabc_STILL), truly_cabc_STILL},
};

struct dsi_cmd_desc truly_cabc_moving_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step0), select_page2_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step1), select_page2_step1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(truly_cabc_MOVING), truly_cabc_MOVING},
};

struct dsi_cmd_desc auo_cabc_off_cmds[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step0), select_page2_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step1), select_page2_step1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(auo_cabc_off), auo_cabc_off},
};

struct dsi_cmd_desc auo_cabc_still_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step0), select_page2_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step1), select_page2_step1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(auo_cabc_STILL), auo_cabc_STILL},
};

struct dsi_cmd_desc auo_cabc_moving_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step0), select_page2_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step1), select_page2_step1},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(auo_cabc_MOVING), auo_cabc_MOVING},
};


struct dsi_cmd_desc sleep_in_cmds[] = {
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
    	sizeof(sel_page_step0), sel_page_step0},
    {DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
    	sizeof(sel_page_step1), sel_page_step1},	    	
    {DTYPE_DCS_WRITE1, 0, 120, WAIT_TYPE_MS,
    	sizeof(sleep_in), sleep_in},
};

struct dsi_cmd_desc elusion_high_bl[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(sel_page_step0), sel_page_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(sel_page_step1), sel_page_step1},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(bl_level), bl_level},
};

struct dsi_cmd_desc truly_cabc_initial_cmd[] = { 
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step0), select_page2_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step1), select_page2_step1},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_cabc_STILL), truly_cabc_STILL},
	//{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(auto_moving_mode_detect), auto_moving_mode_detect},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(still_dimming_steps), still_dimming_steps},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(moving_dimming_steps), moving_dimming_steps},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(min_duty_clamp), min_duty_clamp},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(close_als_dimming), close_als_dimming},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm0_reg), cabc_pwm0_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm1_reg), cabc_pwm1_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm2_reg), cabc_pwm2_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm3_reg), cabc_pwm3_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm4_reg), cabc_pwm4_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm5_reg), cabc_pwm5_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm6_reg), cabc_pwm6_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm7_reg), cabc_pwm7_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm8_reg), cabc_pwm8_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm9_reg), cabc_pwm9_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_delta_gray64), cabc_delta_gray64},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_delta_gray128), cabc_delta_gray128},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_delta_gray192), cabc_delta_gray192},
};

struct dsi_cmd_desc auo_cabc_initial_cmd[] = { 
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step0), select_page2_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step1), select_page2_step1},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(auo_cabc_STILL), auo_cabc_STILL},
	//{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(auto_moving_mode_detect), auto_moving_mode_detect},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(still_dimming_steps), still_dimming_steps},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(moving_dimming_steps), moving_dimming_steps},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(min_duty_clamp), min_duty_clamp},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(close_als_dimming), close_als_dimming},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm0_reg), cabc_pwm0_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm1_reg), cabc_pwm1_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm2_reg), cabc_pwm2_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm3_reg), cabc_pwm3_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm4_reg), cabc_pwm4_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm5_reg), cabc_pwm5_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm6_reg), cabc_pwm6_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm7_reg), cabc_pwm7_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm8_reg), cabc_pwm8_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_pwm9_reg), cabc_pwm9_reg},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_delta_gray64), cabc_delta_gray64},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_delta_gray128), cabc_delta_gray128},
    //{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(cabc_delta_gray192), cabc_delta_gray192},
};

struct dsi_cmd_desc auo_ce_initial_cmd[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page3_step0), select_page3_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page3_step1), select_page3_step1},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(auo_ce_enable), auo_ce_enable},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(auo_vivid_color), auo_vivid_color},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(auo_red_hue_ratio), auo_red_hue_ratio},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(auo_yellow_hue_ratio), auo_yellow_hue_ratio},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(auo_green_hue_ratio), auo_green_hue_ratio},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(auo_cyan_hue_ratio), auo_cyan_hue_ratio},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(auo_blue_hue_ratio), auo_blue_hue_ratio},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(auo_magenta_hue_ratio), auo_magenta_hue_ratio},
};

struct dsi_cmd_desc truly_ce_initial_cmd[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page3_step0), select_page3_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page3_step1), select_page3_step1},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(ce_disable), ce_disable},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_vivid_color), truly_vivid_color},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_red_hue_ratio), truly_red_hue_ratio},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_yellow_hue_ratio), truly_yellow_hue_ratio},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_green_hue_ratio), truly_green_hue_ratio},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_cyan_hue_ratio), truly_cyan_hue_ratio},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_blue_hue_ratio), truly_blue_hue_ratio},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_magenta_hue_ratio), truly_magenta_hue_ratio},
};

struct dsi_cmd_desc shift_to_mipi_cmd[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(sel_mipi_mode), sel_mipi_mode},
	{DTYPE_DCS_WRITE1, 0,20, WAIT_TYPE_MS, sizeof(auo_global_reset), auo_global_reset},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(sel_mipi_mode), sel_mipi_mode},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page1_step0), select_page1_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page1_step1), select_page1_step1},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(op_adjust), op_adjust},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(sel_page_step0), sel_page_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(sel_page_step1), sel_page_step1},
};

struct dsi_cmd_desc auo_display_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(sel_page_step0), sel_page_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(sel_page_step1), sel_page_step1},
	{DTYPE_DCS_WRITE1, 0, 180, WAIT_TYPE_MS,sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_MS,sizeof(display_on), display_on},
};

struct dsi_cmd_desc truly_gamma_initial_cmd[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_control_reg), truly_control_reg},
	//{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_set_v1v14), truly_set_v1v14},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page1_step0), select_page1_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page1_step1), select_page1_step1},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(mipi_rx), mipi_rx},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param1), truly_gamma_red_param1},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param2), truly_gamma_red_param2},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param3), truly_gamma_red_param3},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param4), truly_gamma_red_param4},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param5), truly_gamma_red_param5},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param6), truly_gamma_red_param6},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param7), truly_gamma_red_param7},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param8), truly_gamma_red_param8},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param9), truly_gamma_red_param9},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param10), truly_gamma_red_param10},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param11), truly_gamma_red_param11},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param12), truly_gamma_red_param12},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param13), truly_gamma_red_param13},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param14), truly_gamma_red_param14},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param15), truly_gamma_red_param15},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param16), truly_gamma_red_param16},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param17), truly_gamma_red_param17},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param18), truly_gamma_red_param18},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param19), truly_gamma_red_param19},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param20), truly_gamma_red_param20},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param21), truly_gamma_red_param21},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param22), truly_gamma_red_param22},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param23), truly_gamma_red_param23},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param24), truly_gamma_red_param24},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param25), truly_gamma_red_param25},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param26), truly_gamma_red_param26},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param27), truly_gamma_red_param27},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param28), truly_gamma_red_param28},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param29), truly_gamma_red_param29},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param30), truly_gamma_red_param30},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param31), truly_gamma_red_param31},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param32), truly_gamma_red_param32},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param33), truly_gamma_red_param33},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param34), truly_gamma_red_param34},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param35), truly_gamma_red_param35},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param36), truly_gamma_red_param36},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param37), truly_gamma_red_param37},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param38), truly_gamma_red_param38},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param39), truly_gamma_red_param39},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param40), truly_gamma_red_param40},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param41), truly_gamma_red_param41},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param42), truly_gamma_red_param42},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param43), truly_gamma_red_param43},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param44), truly_gamma_red_param44},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param45), truly_gamma_red_param45},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param46), truly_gamma_red_param46},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param47), truly_gamma_red_param47},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param48), truly_gamma_red_param48},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param49), truly_gamma_red_param49},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param50), truly_gamma_red_param50},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param51), truly_gamma_red_param51},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param52), truly_gamma_red_param52},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param53), truly_gamma_red_param53},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param54), truly_gamma_red_param54},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param55), truly_gamma_red_param55},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param56), truly_gamma_red_param56},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param57), truly_gamma_red_param57},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param58), truly_gamma_red_param58},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param59), truly_gamma_red_param59},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param60), truly_gamma_red_param60},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param61), truly_gamma_red_param61},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param62), truly_gamma_red_param62},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param63), truly_gamma_red_param63},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_red_param64), truly_gamma_red_param64},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step0), select_page2_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page2_step1), select_page2_step1},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param1), truly_gamma_green_param1},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param2), truly_gamma_green_param2},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param3), truly_gamma_green_param3},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param4), truly_gamma_green_param4},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param5), truly_gamma_green_param5},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param6), truly_gamma_green_param6},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param7), truly_gamma_green_param7},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param8), truly_gamma_green_param8},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param9), truly_gamma_green_param9},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param10), truly_gamma_green_param10},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param11), truly_gamma_green_param11},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param12), truly_gamma_green_param12},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param13), truly_gamma_green_param13},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param14), truly_gamma_green_param14},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param15), truly_gamma_green_param15},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param16), truly_gamma_green_param16},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param17), truly_gamma_green_param17},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param18), truly_gamma_green_param18},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param19), truly_gamma_green_param19},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param20), truly_gamma_green_param20},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param21), truly_gamma_green_param21},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param22), truly_gamma_green_param22},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param23), truly_gamma_green_param23},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param24), truly_gamma_green_param24},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param25), truly_gamma_green_param25},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param26), truly_gamma_green_param26},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param27), truly_gamma_green_param27},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param28), truly_gamma_green_param28},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param29), truly_gamma_green_param29},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param30), truly_gamma_green_param30},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param31), truly_gamma_green_param31},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param32), truly_gamma_green_param32},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param33), truly_gamma_green_param33},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param34), truly_gamma_green_param34},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param35), truly_gamma_green_param35},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param36), truly_gamma_green_param36},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param37), truly_gamma_green_param37},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param38), truly_gamma_green_param38},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param39), truly_gamma_green_param39},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param40), truly_gamma_green_param40},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param41), truly_gamma_green_param41},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param42), truly_gamma_green_param42},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param43), truly_gamma_green_param43},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param44), truly_gamma_green_param44},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param45), truly_gamma_green_param45},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param46), truly_gamma_green_param46},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param47), truly_gamma_green_param47},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param48), truly_gamma_green_param48},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param49), truly_gamma_green_param49},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param50), truly_gamma_green_param50},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param51), truly_gamma_green_param51},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param52), truly_gamma_green_param52},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param53), truly_gamma_green_param53},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param54), truly_gamma_green_param54},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param55), truly_gamma_green_param55},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param56), truly_gamma_green_param56},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param57), truly_gamma_green_param57},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param58), truly_gamma_green_param58},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param59), truly_gamma_green_param59},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param60), truly_gamma_green_param60},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param61), truly_gamma_green_param61},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param62), truly_gamma_green_param62},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param63), truly_gamma_green_param63},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_green_param64), truly_gamma_green_param64},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page3_step0), select_page3_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(select_page3_step1), select_page3_step1},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param1), truly_gamma_blue_param1},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param2), truly_gamma_blue_param2},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param3), truly_gamma_blue_param3},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param4), truly_gamma_blue_param4},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param5), truly_gamma_blue_param5},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param6), truly_gamma_blue_param6},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param7), truly_gamma_blue_param7},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param8), truly_gamma_blue_param8},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param9), truly_gamma_blue_param9},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param10), truly_gamma_blue_param10},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param11), truly_gamma_blue_param11},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param12), truly_gamma_blue_param12},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param13), truly_gamma_blue_param13},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param14), truly_gamma_blue_param14},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param15), truly_gamma_blue_param15},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param16), truly_gamma_blue_param16},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param17), truly_gamma_blue_param17},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param18), truly_gamma_blue_param18},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param19), truly_gamma_blue_param19},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param20), truly_gamma_blue_param20},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param21), truly_gamma_blue_param21},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param22), truly_gamma_blue_param22},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param23), truly_gamma_blue_param23},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param24), truly_gamma_blue_param24},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param25), truly_gamma_blue_param25},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param26), truly_gamma_blue_param26},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param27), truly_gamma_blue_param27},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param28), truly_gamma_blue_param28},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param29), truly_gamma_blue_param29},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param30), truly_gamma_blue_param30},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param31), truly_gamma_blue_param31},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param32), truly_gamma_blue_param32},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param33), truly_gamma_blue_param33},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param34), truly_gamma_blue_param34},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param35), truly_gamma_blue_param35},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param36), truly_gamma_blue_param36},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param37), truly_gamma_blue_param37},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param38), truly_gamma_blue_param38},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param39), truly_gamma_blue_param39},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param40), truly_gamma_blue_param40},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param41), truly_gamma_blue_param41},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param42), truly_gamma_blue_param42},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param43), truly_gamma_blue_param43},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param44), truly_gamma_blue_param44},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param45), truly_gamma_blue_param45},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param46), truly_gamma_blue_param46},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param47), truly_gamma_blue_param47},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param48), truly_gamma_blue_param48},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param49), truly_gamma_blue_param49},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param50), truly_gamma_blue_param50},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param51), truly_gamma_blue_param51},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param52), truly_gamma_blue_param52},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param53), truly_gamma_blue_param53},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param54), truly_gamma_blue_param54},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param55), truly_gamma_blue_param55},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param56), truly_gamma_blue_param56},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param57), truly_gamma_blue_param57},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param58), truly_gamma_blue_param58},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param59), truly_gamma_blue_param59},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param60), truly_gamma_blue_param60},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param61), truly_gamma_blue_param61},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param62), truly_gamma_blue_param62},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param63), truly_gamma_blue_param63},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(truly_gamma_blue_param64), truly_gamma_blue_param64},
};

struct dsi_cmd_desc truly_display_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(sel_page_step0), sel_page_step0},
	{DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US, sizeof(sel_page_step1), sel_page_step1},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_MS,sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE1, 0, 5, WAIT_TYPE_MS,sizeof(display_on), display_on},
};
#endif



