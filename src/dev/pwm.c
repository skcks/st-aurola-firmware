#include "dev/pwm.h"

uint16_t curr_dutys[MAX_PWM_PINS];

TIM_HandleTypeDef pwm_dev0;
TIM_HandleTypeDef pwm_dev1;

volatile bool is_fading;

static void dev_setup() 
{

    //开启时钟
    __HAL_RCC_GPIOA_CLK_ENABLE(); //pwm1-4时钟
    __HAL_RCC_GPIOB_CLK_ENABLE(); //pwm5-6时钟

    //端口共用配置
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Alternate = TIM1_GPIO_AF_CHANNEL1;
    GPIO_InitStruct.Pin = PWM0_PIN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


}

static bool dev_init(TIM_HandleTypeDef *dev, TIM_TypeDef *tim) 
{
    //定时器初始化
    dev->Instance = tim;
    dev->Init.Prescaler = DEAFAULT_PRESCALER;
    dev->Init.Period = 0;
    dev->Init.ClockDivision = 0;
    dev->Init.CounterMode = TIM_COUNTERMODE_UP;
    dev->Init.RepetitionCounter = 0;
    dev->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    return HAL_TIM_Base_Init(dev) == HAL_OK &&
           HAL_TIM_PWM_Init(dev) == HAL_OK;
}

static uint8_t dev_config(TIM_HandleTypeDef *dev, uint16_t duty)
{
    uint8_t rst = 0;

    TIM_OC_InitTypeDef sConfig;
    sConfig.OCMode = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;
    sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;

    /* Set the pulse value for channel 1 */
    sConfig.Pulse = duty;
    if (HAL_TIM_PWM_ConfigChannel(dev, &sConfig, TIM_CHANNEL_1) == HAL_OK)
    {
        //记录失败通道
        rst |= 1;
    }

    if (HAL_TIM_PWM_ConfigChannel(dev, &sConfig, TIM_CHANNEL_2) == HAL_OK)
    {
        //记录失败通道
        rst |= 2;
    }

    if (HAL_TIM_PWM_ConfigChannel(dev, &sConfig, TIM_CHANNEL_3) == HAL_OK)
    {
        //记录失败通道
        rst |= 4;
    }

    if (HAL_TIM_PWM_ConfigChannel(dev, &sConfig, TIM_CHANNEL_4) == HAL_OK)
    {
        //记录失败通道
        rst |= 8;
    }

    return rst;
}

void pwm_init(uint16_t *dutys, uint8_t chn_num)
{

    //初始化起始占空比数据
    for (int i = 0; i < chn_num; i++)
    {
        curr_dutys[i] = dutys[i];
    }

    //初始化定时器1
    if (dev_init(&pwm_dev0, TIM1))
    {
        //设置pwm相关参数
        dev_config(&pwm_dev0, 0);
    }

    //初始化定时器3
    if (dev_init(&pwm_dev1, TIM3))
    {
        //设置pwm相关参数
        dev_config(&pwm_dev1, 0);
    }
}

void pwm_set_freq(uint16_t freq)
{
    uint16_t psr = CALC_PRESCALER(freq);
    //更新分频数值寄存器
    __HAL_TIM_SET_PRESCALER(&pwm_dev0, psr);
    __HAL_TIM_SET_PRESCALER(&pwm_dev1, psr);
}

void pwm_set_duty(uint8_t ch_idx, uint16_t duty)
{
    TIM_HandleTypeDef *dev = ch_idx < 4 ? &pwm_dev0 : &pwm_dev1;
    uint8_t channel = 0;

    if (ch_idx != 0 && ch_idx != 4)
    {
        channel = 2 << (ch_idx < 4 ? ch_idx : ch_idx - 4);
    }

    __HAL_TIM_SET_COMPARE(dev, channel, duty);
}

void pwm_set_all_dutys(uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(&pwm_dev0, TIM_CHANNEL_1, duty);
    __HAL_TIM_SET_COMPARE(&pwm_dev0, TIM_CHANNEL_2, duty);
    __HAL_TIM_SET_COMPARE(&pwm_dev0, TIM_CHANNEL_3, duty);
    __HAL_TIM_SET_COMPARE(&pwm_dev0, TIM_CHANNEL_4, duty);

    __HAL_TIM_SET_COMPARE(&pwm_dev1, TIM_CHANNEL_1, duty);
    __HAL_TIM_SET_COMPARE(&pwm_dev1, TIM_CHANNEL_2, duty);
    __HAL_TIM_SET_COMPARE(&pwm_dev1, TIM_CHANNEL_3, duty);
    __HAL_TIM_SET_COMPARE(&pwm_dev1, TIM_CHANNEL_4, duty);
}

void pwm_start(bool fade_in)
{
    if (fade_in)
    {
        pwm_set_all_dutys(0);
    }

    is_fading = fade_in;

    HAL_TIM_PWM_Start(&pwm_dev0, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&pwm_dev0, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&pwm_dev0, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&pwm_dev0, TIM_CHANNEL_4);

    HAL_TIM_PWM_Start(&pwm_dev1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&pwm_dev2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&pwm_dev3, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&pwm_dev4, TIM_CHANNEL_4);
}

void pwm_stop()
{
    HAL_TIM_PWM_Stop(&pwm_dev0, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&pwm_dev0, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&pwm_dev0, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(&pwm_dev0, TIM_CHANNEL_4);

    HAL_TIM_PWM_Stop(&pwm_dev1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&pwm_dev2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&pwm_dev3, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(&pwm_dev4, TIM_CHANNEL_4);
}
