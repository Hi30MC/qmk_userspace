RGB_MATRIX_EFFECT(scrolling_trans_flag)
RGB_MATRIX_EFFECT(scrolling_lesbian_flag)
RGB_MATRIX_EFFECT(scrolling_system_flag)
RGB_MATRIX_EFFECT(snake_of_order)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static bool snake_of_order(effect_params_t* params){
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint16_t scroll_offset = (g_rgb_timer >> 8) % led_max;
    rgb_matrix_set_color((scroll_offset > 0) ? (scroll_offset - 1): led_max - 1, 0, 0, 0);
    rgb_matrix_set_color(scroll_offset, 0x10, 0x10, 0x10);

    return rgb_matrix_check_finished_leds(led_max);
}

static bool scrolling_pride_flag(effect_params_t* params, const hsv_t* flag_colors, uint8_t num_colors){
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    // Stripe width in pixels - adjust this to change stripe size
    const uint8_t stripe_width = 224 / num_colors;

    // Calculate scroll offset based on time
    uint16_t scroll_offset = scale16by8(g_rgb_timer, qadd8(rgb_matrix_config.speed / 4, 1));
    // uint16_t scroll_offset = 0;

    for (uint8_t i = led_min; i < led_max; i++) {
        // Add scroll offset to create movement
        uint16_t scrolled_x = (g_led_config.point[i].x + scroll_offset) % (stripe_width * num_colors);
        HSV color = flag_colors[scrolled_x / stripe_width];

        color.v = 50 * color.v / 255;
        RGB rgb = rgb_matrix_hsv_to_rgb(color);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    return rgb_matrix_check_finished_leds(led_max);
}

static bool scrolling_trans_flag(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    const hsv_t trans_colors[4] = {
        {197*255/360, 63*255/100, 98*255/100},
        {348*255/360, 45*255/100, 96*255/100},
        {0*255/360, 0*255/100, 75*255/100},
        {348*255/360, 45*255/100, 96*255/100}
    };

    return scrolling_pride_flag(params, trans_colors, 4);
}

static bool scrolling_lesbian_flag(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    const hsv_t lesbian_colors[5] = {
        {324*255/360, 100*255/100, 65*255/100},
        {324*255/360, 54*255/100, 83*255/100},
        {0*255/360, 0, 75*255/100},
        {25*255/360, 67*255/100, 100*255/100},
        {11*255/360, 100*255/100, 84*255/100}
    };

    return scrolling_pride_flag(params, lesbian_colors, 5);
}

static bool scrolling_system_flag(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    const hsv_t system_colors[5] = {
        {312*255/360, 90*255/100, 20*255/100},
        {280*255/360, 45*255/100, 52*255/100},
        {243*255/360, 34*255/100, 78*255/100},
        {150*255/360, 24*255/100, 76*255/100},
        {53*255/360, 55*255/100, 95*255/100}
    };

    return scrolling_pride_flag(params, system_colors, 5);
}

#endif
