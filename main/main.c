#include "stdio.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "include/OLEDFont.h"
esp_err_t I2C_Init(){

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .scl_io_num = 17 ,
        .sda_io_num = 18 ,
        .scl_pullup_en = GPIO_PULLUP_ENABLE ,
        .sda_pullup_en = GPIO_PULLUP_ENABLE ,
        .master.clk_speed = 400000 
    };
    i2c_param_config(0,&conf);
    return i2c_driver_install(0,conf.mode,0,0,0);
}

esp_err_t OLED_WR_Byte(uint8_t data,uint8_t cmd){

    uint8_t write_buff[2] ={((cmd == 1) ? (0x40) : (0x00)), data};

    //printf("wr1:%x\nwr2:%x\n",write_buff[0],write_buff[1]); //串口打印write_buff内容
    return i2c_master_write_to_device(0,0X3C,write_buff,sizeof(write_buff),100);
}
void OLED_Clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WR_Byte(0xb0 + i, 0);
        OLED_WR_Byte(0x00, 0);
        OLED_WR_Byte(0x10, 0);
        for (n = 0; n < 128; n++){
            OLED_WR_Byte(0, 1);
        }
    }
}
void OLED_Init(void)
{
    OLED_WR_Byte(0xAE, 0); //--display off
    OLED_WR_Byte(0x00, 0); //---set low column address
    OLED_WR_Byte(0x10, 0); //---set high column address
    OLED_WR_Byte(0x40, 0); //--set start line address
    OLED_WR_Byte(0xB0, 0); //--set page address
    OLED_WR_Byte(0x81, 0); // contract control
    OLED_WR_Byte(0xFF, 0); //--128
    OLED_WR_Byte(0xA1, 0); // set segment remap
    OLED_WR_Byte(0xA6, 0); //--normal / reverse
    OLED_WR_Byte(0xA8, 0); //--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3F, 0); //--1/32 duty
    OLED_WR_Byte(0xC8, 0); // Com scan direction
    OLED_WR_Byte(0xD3, 0); //-set display offset
    OLED_WR_Byte(0x00, 0); //
    OLED_WR_Byte(0xD5, 0); // set osc division
    OLED_WR_Byte(0x80, 0); //
    OLED_WR_Byte(0xD8, 0); // set area color mode off
    OLED_WR_Byte(0x05, 0); //
    OLED_WR_Byte(0xD9, 0); // Set Pre-Charge Period
    OLED_WR_Byte(0xF1, 0); //
    OLED_WR_Byte(0xDA, 0); // set com pin configuartion
    OLED_WR_Byte(0x12, 0); //
    OLED_WR_Byte(0xDB, 0); // set Vcomh
    OLED_WR_Byte(0x30, 0); //
    OLED_WR_Byte(0x8D, 0); // set charge pump enable
    OLED_WR_Byte(0x14, 0); //
    OLED_WR_Byte(0xAF, 0); //--turn on oled panel

    OLED_Clear();
}

void OLED_Set_Pos(uint8_t x, uint8_t y)
{
    OLED_WR_Byte(0xb0 + y, 0);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, 0);
    OLED_WR_Byte((x & 0x0f), 0);
}
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size)
{
    uint8_t c = 0;
    uint8_t i = 0;
    c = chr - ' ';
    if (x > 127)
    {
        x = 0;
        y = y + 2;
    }
    if (Char_Size == 16)
    {
        OLED_Set_Pos(x, y);
        for (i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i], 1);
        OLED_Set_Pos(x, y + 1);
        for (i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i + 8], 1);
    }
    else
    {
        OLED_Set_Pos(x, y);
        for (i = 0; i < 6; i++)
            OLED_WR_Byte(F6x8[c][i], 1);
    }
}

void app_main(void){
    printf(esp_err_to_name(I2C_Init()));
    printf("\n");
    printf(esp_err_to_name(OLED_WR_Byte(0x6f,1))); 
    printf("\n");
    OLED_Init();        
    printf("ok\n");
    for (uint8_t i = 33; i < 59; i++)
    {
        OLED_Clear();
        OLED_ShowChar(i,2,i,8);
            
        vTaskDelay(500/portTICK_PERIOD_MS);
            }
    
   
}