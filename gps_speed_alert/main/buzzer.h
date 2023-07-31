
//2023-07-30 13:29:38 - works for ESP32
static void init_hw_c3()
{ 
        //adc1_config_width(ADC_WIDTH_BIT_10);
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(adc_channel, ADC_ATTEN_DB_11);
        ledc_timer_config_t ledc_timer = {
            .duty_resolution = LEDC_TIMER_10_BIT,
            .freq_hz = 900,
            .speed_mode = LEDC_LOW_SPEED_MODE,              
            .timer_num = LEDC_TIMER_0,
            .clk_cfg = LEDC_AUTO_CLK,
        };

        ledc_timer_config(&ledc_timer);
        ledc_channel.channel = LEDC_CHANNEL_0;
        ledc_channel.duty = 0;
        ledc_channel.gpio_num = LEDC_GPIO;
        ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;   
        ledc_channel.hpoint = 0;
        ledc_channel.timer_sel = LEDC_TIMER_0;
        ledc_channel_config(&ledc_channel)
 
}//init_hw
