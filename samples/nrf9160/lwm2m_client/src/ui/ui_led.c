/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>

#include "ui_led.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ui_led, CONFIG_UI_LOG_LEVEL);

#define PWM_PERIOD_USEC				(USEC_PER_SEC / 100U)

struct pwm_dt_spec {
	const struct device *dev;
	uint32_t channel;
	pwm_flags_t flags;
};

#define PWM_DT_SPEC_GET(node_id)					\
	{								\
		.dev = DEVICE_DT_GET(DT_PWMS_CTLR(node_id)),		\
		.channel = DT_PWMS_CHANNEL(node_id),			\
		.flags = DT_PWMS_FLAGS(node_id),			\
	}

#define PWM_DT_SPEC_GET_OR(node_id, default_value)			\
	COND_CODE_1(DT_NODE_HAS_PROP(node_id, pwms),			\
		    (PWM_DT_SPEC_GET(node_id)),				\
		    (default_value))

static const struct pwm_dt_spec pwm_leds[] = {
	PWM_DT_SPEC_GET_OR(DT_ALIAS(pwm_led0), {}),
	PWM_DT_SPEC_GET_OR(DT_ALIAS(pwm_led1), {}),
	PWM_DT_SPEC_GET_OR(DT_ALIAS(pwm_led2), {}),
};

static uint32_t pulse_width[ARRAY_SIZE(pwm_leds)];
static bool state[ARRAY_SIZE(pwm_leds)];

static const struct gpio_dt_spec leds[] = {
	GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {}),
	GPIO_DT_SPEC_GET_OR(DT_ALIAS(led1), gpios, {}),
	GPIO_DT_SPEC_GET_OR(DT_ALIAS(led2), gpios, {}),
	GPIO_DT_SPEC_GET_OR(DT_ALIAS(led3), gpios, {}),
};

int ui_led_pwm_on_off(uint8_t led_num, bool new_state)
{
	int ret;

	if (led_num >= ARRAY_SIZE(pwm_leds) || pwm_leds[led_num].dev == NULL) {
		return -EINVAL;
	}

	state[led_num] = new_state;

	ret = pwm_pin_set_usec(pwm_leds[led_num].dev, pwm_leds[led_num].channel,
			       PWM_PERIOD_USEC,
			       pulse_width[led_num] * new_state,
			       pwm_leds[led_num].flags);
	if (ret) {
		LOG_ERR("Set LED PWM pin %u failed  (%d)", led_num, ret);
		return ret;
	}

	return 0;
}

static uint32_t calculate_pulse_width(uint8_t led_intensity)
{
	return PWM_PERIOD_USEC * led_intensity / UINT8_MAX;
}

int ui_led_pwm_set_intensity(uint8_t led_num, uint8_t led_intensity)
{
	int ret;

	if (led_num >= ARRAY_SIZE(pwm_leds) || pwm_leds[led_num].dev == NULL) {
		return -EINVAL;
	}

	pulse_width[led_num] = calculate_pulse_width(led_intensity);

	ret = pwm_pin_set_usec(pwm_leds[led_num].dev, pwm_leds[led_num].channel,
			       PWM_PERIOD_USEC,
			       pulse_width[led_num] * state[led_num],
			       pwm_leds[led_num].flags);
	if (ret) {
		LOG_ERR("Set LED PWM pin %u failed (%d)", led_num, ret);
		return ret;
	}

	return 0;
}

int ui_led_pwm_init(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(pwm_leds); ++i) {
		if (pwm_leds[i].dev != NULL && !device_is_ready(pwm_leds[i].dev)) {
			return -ENODEV;
		}
	}

	return 0;
}

int ui_led_gpio_on_off(uint8_t led_num, bool new_state)
{
	int ret;

	if (led_num >= ARRAY_SIZE(leds) || leds[led_num].port == NULL) {
		return -EINVAL;
	}

	ret = gpio_pin_set_dt(&leds[led_num], new_state);
	if (ret) {
		LOG_ERR("Set LED %u pin failed (%d)", led_num, ret);
		return ret;
	}

	return 0;
}

int ui_led_gpio_init(void)
{
	int ret;

	for (size_t i = 0; i < ARRAY_SIZE(leds); ++i) {
		if (leds[i].port != NULL && !device_is_ready(leds[i].port)) {
			return -ENODEV;
		}

		ret = gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
		if (ret) {
			LOG_ERR("Configure LED %d failed (%d)", i, ret);
			return ret;
		}
	}

	return 0;
}
