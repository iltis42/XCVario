coordinate_t x, y;

x = 50 - 20;
y = 50 + 14 / 2;

eglib_SetIndexColor(&eglib, 0, 0, 0, 128);
eglib_DrawLine(&eglib, 0, y, 99, y);
eglib_DrawLine(&eglib, x, 0, x, 99);

eglib_SetFont(&eglib, &font_Liberation_SansRegular_20px);
eglib_SetIndexColor(&eglib, 0, 255, 255, 255);
eglib_DrawTextCentered(&eglib, x, y, "Hello!");