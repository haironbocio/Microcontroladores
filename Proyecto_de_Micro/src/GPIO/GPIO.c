#include<GPIO/GPIO.h>


gpio_config_t Mode_config = {

		.pin_bit_mask = (1ULL <<Mode),
		.mode = GPIO_MODE_INPUT,
		.intr_type = GPIO_INTR_DISABLE,
		.pull_up_en = 1,
		.pull_down_en = 0,
		}
;
gpio_config_t LED1_config = {

		.pin_bit_mask = (1ULL <<LED1),
		.mode = GPIO_MODE_OUTPUT,
		.intr_type = GPIO_INTR_DISABLE,
		.pull_up_en = 0,
		.pull_down_en = 0,
		}
;
gpio_config_t LED2_config = {

		.pin_bit_mask = (1ULL <<LED2),
		.mode = GPIO_MODE_OUTPUT,
		.intr_type = GPIO_INTR_DISABLE,
		.pull_up_en = 0,
		.pull_down_en = 0,
		}
;
gpio_config_t LED3_config = {

		.pin_bit_mask = (1ULL <<LED3),
		.mode = GPIO_MODE_OUTPUT,
		.intr_type = GPIO_INTR_DISABLE,
		.pull_up_en = 0,
		.pull_down_en = 0,
		}
;

gpio_config_t LED4_config = {

		.pin_bit_mask = (1ULL <<LED4),
		.mode = GPIO_MODE_OUTPUT,
		.intr_type = GPIO_INTR_DISABLE,
		.pull_up_en = 0,
		.pull_down_en = 0,
		}
;

gpio_config_t Buzzer_config = {

		.pin_bit_mask = (1ULL <<Buzzer),
		.mode = GPIO_MODE_OUTPUT,
		.intr_type = GPIO_INTR_DISABLE,
		.pull_up_en = 0,
		.pull_down_en = 0,
		}
;
void GPIO_INIT(void)
{
gpio_config(&Mode_config);
gpio_config(&LED1_config);
gpio_config(&LED2_config);
gpio_config(&LED3_config);
gpio_config(&LED4_config);
gpio_config(&Buzzer_config);
}
