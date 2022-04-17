eglib_SetIndexColor(&eglib, 0, 0, 255, 0);
eglib_SetIndexColor(&eglib, 1, 255, 255, 255);

eglib_DrawGradientLine(&eglib, 0, 0, 99, 0);
eglib_DrawGradientLine(&eglib, 0, 0, 50, 50);
eglib_DrawGradientLine(&eglib, 0, 0, 0, 99);

eglib_DrawGradientLine(&eglib, 99, 99, 99, 0);
eglib_DrawGradientLine(&eglib, 99, 99, 50, 50);
eglib_DrawGradientLine(&eglib, 99, 99, 0, 99);

eglib_DrawGradientLine(&eglib, 0, 99, 50, 50);
eglib_DrawGradientLine(&eglib, 99, 0, 50, 50);