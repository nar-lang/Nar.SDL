#include "_package.h"
#include <Nar.Program.h>

void map_Color(nar_runtime_t rt, nar_object_t obj, nar_cstring_t key, SDL_Color *result) {
    if (0 == strcmp(key, "r")) {
        result->r = (Uint8) nar->to_int(rt, obj);
    } else if (0 == strcmp(key, "g")) {
        result->g = (Uint8) nar->to_int(rt, obj);
    } else if (0 == strcmp(key, "b")) {
        result->b = (Uint8) nar->to_int(rt, obj);
    } else if (0 == strcmp(key, "a")) {
        result->a = (Uint8) nar->to_int(rt, obj);
    }
}

nar_object_t /* String */ native__Pixels_getPixelFormatName(nar_runtime_t rt,
        nar_object_t format /* PixelFormat */) {
    return nar->make_string(rt, SDL_GetPixelFormatName(nar->to_enum_option(rt, format)));
}

nar_object_t /* { bitsPerPixel: Int, rMask: Int, gMask: Int, bMask: Int, aMask: Int } */
native__Pixels_pixelFormatToMasks(nar_runtime_t rt,
        nar_object_t format /* PixelFormat */) {
    int bitsPerPixel;
    Uint32 rMask;
    Uint32 gMask;
    Uint32 bMask;
    Uint32 aMask;
    SDL_PixelFormatEnum fmt = nar->to_enum_option(rt, format);
    SDL_PixelFormatEnumToMasks(fmt, &bitsPerPixel, &rMask, &gMask, &bMask, &aMask);
    return nar->make_record(rt, 5,
            (nar_cstring_t[]) {"bitsPerPixel", "rMask", "gMask", "bMask", "aMask"},
            (nar_object_t[]) {
                    nar->make_int(rt, bitsPerPixel),
                    nar->make_int(rt, rMask),
                    nar->make_int(rt, gMask),
                    nar->make_int(rt, bMask),
                    nar->make_int(rt, aMask)
            });
}

nar_object_t /* PixelFormat */ native__Pixels_masksToPixelFormat(nar_runtime_t rt,
        nar_object_t bitsPerPixel /* Int */,
        nar_object_t rMask /* Int */,
        nar_object_t gMask /* Int */,
        nar_object_t bMask /* Int */,
        nar_object_t aMask /* Int */) {
    SDL_PixelFormatEnum fmt = SDL_MasksToPixelFormatEnum(
            (int) nar->to_int(rt, bitsPerPixel),
            nar->to_int(rt, rMask),
            nar->to_int(rt, gMask),
            nar->to_int(rt, bMask),
            nar->to_int(rt, aMask));
    return nar->make_enum_option(rt, Nar_SDL_Pixels_PixelFormat, fmt, 0, NULL);
}

void alloc_format(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_PixelFormat *result = SDL_AllocFormat(nar->to_enum_option(rt, payload));
    if (NULL == result) {
        reject(rt, nar->make_string(rt, SDL_GetError()), task_state);
    } else {
        resolve(rt, nar->make_native(rt, result, NULL), task_state);
    }
}

nar_object_t /* Task[String, PixelFormat] */ native__Pixels_allocFormat(nar_runtime_t rt,
        nar_object_t format /* PixelFormat */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, format, &alloc_format);
}

void free_format(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_FreeFormat(nar->to_native(rt, payload).ptr);
    resolve(rt, nar->make_unit(rt), task_state);
}

nar_object_t /* Task[String, ()] */ native__Pixels_freeFormat(nar_runtime_t rt,
        nar_object_t format /* PixelFormat */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, format, &free_format);
}

void alloc_palette(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Palette *result = SDL_AllocPalette((int) nar->to_int(rt, payload));
    if (NULL == result) {
        reject(rt, nar->make_string(rt, SDL_GetError()), task_state);
    } else {
        resolve(rt, nar->make_native(rt, result, NULL), task_state);
    }
}

nar_object_t /* Task[String, Palette] */ native__Pixels_allocPalette(nar_runtime_t rt,
        nar_object_t nColors /* Int */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, nColors, &alloc_palette);
}

void set_format_palette(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    nar_object_t format = data.values[0];
    nar_object_t palette = data.values[1];
    int result = SDL_SetPixelFormatPalette(
            nar->to_native(rt, format).ptr, nar->to_native(rt, palette).ptr);
    if (0 != result) {
        reject(rt, nar->make_string(rt, SDL_GetError()), task_state);
    } else {
        resolve(rt, palette, task_state);
    }
}

nar_object_t /* Task[String, Palette] */ native__Pixels_setPixelFormatPalette(nar_runtime_t rt,
        nar_object_t format /* PixelFormat */,
        nar_object_t palette /* Palette */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {format, palette});
    return task_new(rt, data, &set_format_palette);
}

void set_palette_colors(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    nar_object_t palette = data.values[0];
    int firstColor = (int) nar->to_int(rt, data.values[2]);
    int nColors = (int) nar->to_int(rt, data.values[3]);
    SDL_Color *rawColors = nar->alloc(sizeof(SDL_Color) * nColors);
    int n = firstColor;
    nar_object_t it = data.values[1];
    while (n > 0 && nar->index_is_valid(rt, it)) {
        nar_list_item_t item = nar->to_list_item(rt, it);
        it = item.next;
        n--;
    }
    if (n != 0) {
        reject(rt, nar->make_string(rt, "Array out of bounds"), task_state);
        goto cleanup;
    }
    while (n < nColors && nar->index_is_valid(rt, it)) {
        nar_list_item_t item = nar->to_list_item(rt, it);
        SDL_Color color;
        nar->map_record(rt, item.value, &color, (nar_map_record_cb_fn_t) &map_Color);
        rawColors[n] = color;
        it = item.next;
        n++;
    }
    if (n != nColors) {
        reject(rt, nar->make_string(rt, "Array out of bounds"), task_state);
        goto cleanup;
    } else {
        int result = SDL_SetPaletteColors(nar->to_native(rt, palette).ptr, rawColors, 0, nColors);
        if (0 != result) {
            reject(rt, nar->make_string(rt, SDL_GetError()), task_state);
        } else {
            resolve(rt, palette, task_state);
        }
    }
    cleanup:
    nar->free(rawColors);
}

nar_object_t /* Task[String, Palette] */ native__Pixels_setPaletteColors(nar_runtime_t rt,
        nar_object_t palette /* Palette */,
        nar_object_t colors /* List[Color] */,
        nar_object_t firstColor /* Int */,
        nar_object_t nColors /* Int */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 4,
            (nar_object_t[]) {palette, colors, firstColor, nColors});
    return task_new(rt, data, &set_palette_colors);
}

void free_palette(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_FreePalette(nar->to_native(rt, payload).ptr);
    resolve(rt, nar->make_unit(rt), task_state);
}

nar_object_t /* Task[String, ()] */ native__Pixels_freePalette(nar_runtime_t rt,
        nar_object_t palette /* Palette */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, palette, &free_palette);
}

nar_object_t /* Int */ native__Pixels_mapRGB(nar_runtime_t rt,
        nar_object_t format /* PixelFormat */,
        nar_object_t r /* Int */,
        nar_object_t g /* Int */,
        nar_object_t b /* Int */) {
    return nar->make_int(rt, SDL_MapRGB(
            nar->to_native(rt, format).ptr,
            nar->to_int(rt, r),
            nar->to_int(rt, g),
            nar->to_int(rt, b)));
}

nar_object_t /* Int */ native__Pixels_mapRGBA(nar_runtime_t rt,
        nar_object_t format /* PixelFormat */,
        nar_object_t r /* Int */,
        nar_object_t g /* Int */,
        nar_object_t b /* Int */,
        nar_object_t a /* Int */) {
    return nar->make_int(rt, SDL_MapRGBA(
            nar->to_native(rt, format).ptr,
            nar->to_int(rt, r),
            nar->to_int(rt, g),
            nar->to_int(rt, b),
            nar->to_int(rt, a)));
}

nar_object_t /* (Int, Int, Int) */ native__Pixels_getRGB(nar_runtime_t rt,
        nar_object_t pixel /* Int */,
        nar_object_t format /* PixelFormat */) {
    Uint8 r, g, b;
    SDL_GetRGB(nar->to_int(rt, pixel), nar->to_native(rt, format).ptr, &r, &g, &b);
    return nar->make_tuple(rt, 3, (nar_object_t[]) {
            nar->make_int(rt, r),
            nar->make_int(rt, g),
            nar->make_int(rt, b)
    });
}

nar_object_t /* (Int, Int, Int, Int) */ native__Pixels_getRGBA(nar_runtime_t rt,
        nar_object_t pixel /* Int */,
        nar_object_t format /* PixelFormat */) {
    Uint8 r, g, b, a;
    SDL_GetRGBA(nar->to_int(rt, pixel), nar->to_native(rt, format).ptr, &r, &g, &b, &a);
    return nar->make_tuple(rt, 4, (nar_object_t[]) {
            nar->make_int(rt, r),
            nar->make_int(rt, g),
            nar->make_int(rt, b),
            nar->make_int(rt, a)
    });
}

nar_object_t /* List[Int] */ native__Pixels_calculateGammaRamp(nar_runtime_t rt,
        nar_object_t gamma /* Float */) {
    Uint16 *ramp = nar->alloc(sizeof(Uint16) * 256);
    SDL_CalculateGammaRamp((float) nar->to_float(rt, gamma), ramp);
    nar_object_t result = nar->make_list(rt, 0, NULL);
    for (int i = 255; i >= 0; i--) {
        result = nar->make_list_cons(rt, nar->make_int(rt, ramp[i]), result);
    }
    nar->free(ramp);
    return result;
}

void init_pixels(nar_runtime_t rt) {
    nar->register_def(rt, "Nar.SDL.Pixels", "getPixelFormatName",
            &native__Pixels_getPixelFormatName, 1);
    nar->register_def(rt, "Nar.SDL.Pixels", "pixelFormatToMasks",
            &native__Pixels_pixelFormatToMasks, 1);
    nar->register_def(rt, "Nar.SDL.Pixels", "masksToPixelFormat",
            &native__Pixels_masksToPixelFormat, 5);
    nar->register_def(rt, "Nar.SDL.Pixels", "allocFormat",
            &native__Pixels_allocFormat, 1);
    nar->register_def(rt, "Nar.SDL.Pixels", "freeFormat",
            &native__Pixels_freeFormat, 1);
    nar->register_def(rt, "Nar.SDL.Pixels", "allocPalette",
            &native__Pixels_allocPalette, 1);
    nar->register_def(rt, "Nar.SDL.Pixels", "setPixelFormatPalette",
            &native__Pixels_setPixelFormatPalette, 2);
    nar->register_def(rt, "Nar.SDL.Pixels", "setPaletteColors",
            &native__Pixels_setPaletteColors, 4);
    nar->register_def(rt, "Nar.SDL.Pixels", "freePalette",
            &native__Pixels_freePalette, 1);
    nar->register_def(rt, "Nar.SDL.Pixels", "mapRGB",
            &native__Pixels_mapRGB, 4);
    nar->register_def(rt, "Nar.SDL.Pixels", "mapRGBA",
            &native__Pixels_mapRGBA, 5);
    nar->register_def(rt, "Nar.SDL.Pixels", "getRGB",
            &native__Pixels_getRGB, 2);
    nar->register_def(rt, "Nar.SDL.Pixels", "getRGBA",
            &native__Pixels_getRGBA, 2);
    nar->register_def(rt, "Nar.SDL.Pixels", "calculateGammaRamp",
            &native__Pixels_calculateGammaRamp, 1);

    nar->enum_def(Nar_SDL_Pixels_PixelType, Nar_SDL_Pixels_PixelType__PixelTypeIndex1,
            SDL_PIXELTYPE_INDEX1);
    nar->enum_def(Nar_SDL_Pixels_PixelType, Nar_SDL_Pixels_PixelType__PixelTypeIndex4,
            SDL_PIXELTYPE_INDEX4);
    nar->enum_def(Nar_SDL_Pixels_PixelType, Nar_SDL_Pixels_PixelType__PixelTypeIndex8,
            SDL_PIXELTYPE_INDEX8);
    nar->enum_def(Nar_SDL_Pixels_PixelType, Nar_SDL_Pixels_PixelType__PixelTypePacked8,
            SDL_PIXELTYPE_PACKED8);
    nar->enum_def(Nar_SDL_Pixels_PixelType, Nar_SDL_Pixels_PixelType__PixelTypePacked16,
            SDL_PIXELTYPE_PACKED16);
    nar->enum_def(Nar_SDL_Pixels_PixelType, Nar_SDL_Pixels_PixelType__PixelTypePacked32,
            SDL_PIXELTYPE_PACKED32);
    nar->enum_def(Nar_SDL_Pixels_PixelType, Nar_SDL_Pixels_PixelType__PixelTypeArrayU8,
            SDL_PIXELTYPE_ARRAYU8);
    nar->enum_def(Nar_SDL_Pixels_PixelType, Nar_SDL_Pixels_PixelType__PixelTypeArrayU16,
            SDL_PIXELTYPE_ARRAYF16);
    nar->enum_def(Nar_SDL_Pixels_PixelType, Nar_SDL_Pixels_PixelType__PixelTypeArrayU32,
            SDL_PIXELTYPE_ARRAYU32);
    nar->enum_def(Nar_SDL_Pixels_PixelType, Nar_SDL_Pixels_PixelType__PixelTypeArrayF16,
            SDL_PIXELTYPE_ARRAYF16);
    nar->enum_def(Nar_SDL_Pixels_PixelType, Nar_SDL_Pixels_PixelType__PixelTypeArrayF32,
            SDL_PIXELTYPE_ARRAYF32);

    nar->enum_def(Nar_SDL_Pixels_BitmapOrder, Nar_SDL_Pixels_BitmapOrder__BitmapOrderNone,
            SDL_BITMAPORDER_NONE);
    nar->enum_def(Nar_SDL_Pixels_BitmapOrder, Nar_SDL_Pixels_BitmapOrder__BitmapOrder4321,
            SDL_BITMAPORDER_4321);
    nar->enum_def(Nar_SDL_Pixels_BitmapOrder, Nar_SDL_Pixels_BitmapOrder__BitmapOrder1234,
            SDL_BITMAPORDER_1234);

    nar->enum_def(Nar_SDL_Pixels_PackedOrder, Nar_SDL_Pixels_PackedOrder__PackedOrderNone,
            SDL_PACKEDORDER_NONE);
    nar->enum_def(Nar_SDL_Pixels_PackedOrder, Nar_SDL_Pixels_PackedOrder__PackedOrderXRGB,
            SDL_PACKEDORDER_XRGB);
    nar->enum_def(Nar_SDL_Pixels_PackedOrder, Nar_SDL_Pixels_PackedOrder__PackedOrderRGBX,
            SDL_PACKEDORDER_RGBX);
    nar->enum_def(Nar_SDL_Pixels_PackedOrder, Nar_SDL_Pixels_PackedOrder__PackedOrderARGB,
            SDL_PACKEDORDER_ARGB);
    nar->enum_def(Nar_SDL_Pixels_PackedOrder, Nar_SDL_Pixels_PackedOrder__PackedOrderRGBA,
            SDL_PACKEDORDER_RGBA);
    nar->enum_def(Nar_SDL_Pixels_PackedOrder, Nar_SDL_Pixels_PackedOrder__PackedOrderXBGR,
            SDL_PACKEDORDER_XBGR);
    nar->enum_def(Nar_SDL_Pixels_PackedOrder, Nar_SDL_Pixels_PackedOrder__PackedOrderBGRX,
            SDL_PACKEDORDER_BGRX);
    nar->enum_def(Nar_SDL_Pixels_PackedOrder, Nar_SDL_Pixels_PackedOrder__PackedOrderABGR,
            SDL_PACKEDORDER_ABGR);
    nar->enum_def(Nar_SDL_Pixels_PackedOrder, Nar_SDL_Pixels_PackedOrder__PackedOrderBGRA,
            SDL_PACKEDORDER_BGRA);

    nar->enum_def(Nar_SDL_Pixels_ArrayOrder, Nar_SDL_Pixels_ArrayOrder__ArrayOrderNone,
            SDL_ARRAYORDER_NONE);
    nar->enum_def(Nar_SDL_Pixels_ArrayOrder, Nar_SDL_Pixels_ArrayOrder__ArrayOrderRGB,
            SDL_ARRAYORDER_RGB);
    nar->enum_def(Nar_SDL_Pixels_ArrayOrder, Nar_SDL_Pixels_ArrayOrder__ArrayOrderRGBA,
            SDL_ARRAYORDER_RGBA);
    nar->enum_def(Nar_SDL_Pixels_ArrayOrder, Nar_SDL_Pixels_ArrayOrder__ArrayOrderARGB,
            SDL_ARRAYORDER_ARGB);
    nar->enum_def(Nar_SDL_Pixels_ArrayOrder, Nar_SDL_Pixels_ArrayOrder__ArrayOrderBGR,
            SDL_ARRAYORDER_BGR);
    nar->enum_def(Nar_SDL_Pixels_ArrayOrder, Nar_SDL_Pixels_ArrayOrder__ArrayOrderBGRA,
            SDL_ARRAYORDER_BGRA);
    nar->enum_def(Nar_SDL_Pixels_ArrayOrder, Nar_SDL_Pixels_ArrayOrder__ArrayOrderABGR,
            SDL_ARRAYORDER_ABGR);

    nar->enum_def(Nar_SDL_Pixels_PackedLayout, Nar_SDL_Pixels_PackedLayout__PackedLayoutNone,
            SDL_PACKEDLAYOUT_NONE);
    nar->enum_def(Nar_SDL_Pixels_PackedLayout, Nar_SDL_Pixels_PackedLayout__PackedLayout332,
            SDL_PACKEDLAYOUT_332);
    nar->enum_def(Nar_SDL_Pixels_PackedLayout, Nar_SDL_Pixels_PackedLayout__PackedLayout4444,
            SDL_PACKEDLAYOUT_4444);
    nar->enum_def(Nar_SDL_Pixels_PackedLayout, Nar_SDL_Pixels_PackedLayout__PackedLayout1555,
            SDL_PACKEDLAYOUT_1555);
    nar->enum_def(Nar_SDL_Pixels_PackedLayout, Nar_SDL_Pixels_PackedLayout__PackedLayout5551,
            SDL_PACKEDLAYOUT_5551);
    nar->enum_def(Nar_SDL_Pixels_PackedLayout, Nar_SDL_Pixels_PackedLayout__PackedLayout565,
            SDL_PACKEDLAYOUT_565);
    nar->enum_def(Nar_SDL_Pixels_PackedLayout, Nar_SDL_Pixels_PackedLayout__PackedLayout8888,
            SDL_PACKEDLAYOUT_8888);
    nar->enum_def(Nar_SDL_Pixels_PackedLayout, Nar_SDL_Pixels_PackedLayout__PackedLayout2101010,
            SDL_PACKEDLAYOUT_2101010);
    nar->enum_def(Nar_SDL_Pixels_PackedLayout, Nar_SDL_Pixels_PackedLayout__PackedLayout1010102,
            SDL_PACKEDLAYOUT_1010102);

    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatIndex1LSB,
            SDL_PIXELFORMAT_INDEX1LSB);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatIndex1MSB,
            SDL_PIXELFORMAT_INDEX1MSB);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatIndex2LSB,
            SDL_PIXELFORMAT_INDEX2LSB);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatIndex2MSB,
            SDL_PIXELFORMAT_INDEX2MSB);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatIndex4LSB,
            SDL_PIXELFORMAT_INDEX4LSB);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatIndex4MSB,
            SDL_PIXELFORMAT_INDEX4MSB);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatIndex8,
            SDL_PIXELFORMAT_INDEX8);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatRGB332,
            SDL_PIXELFORMAT_RGB332);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatXRGB4444,
            SDL_PIXELFORMAT_XRGB4444);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatRGB444,
            SDL_PIXELFORMAT_RGB444);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatXBGR4444,
            SDL_PIXELFORMAT_XBGR4444);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatBGR444,
            SDL_PIXELFORMAT_BGR444);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatXRGB1555,
            SDL_PIXELFORMAT_XRGB1555);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatRGB555,
            SDL_PIXELFORMAT_RGB555);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatXBGR1555,
            SDL_PIXELFORMAT_XBGR1555);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatBGR555,
            SDL_PIXELFORMAT_BGR555);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatARGB4444,
            SDL_PIXELFORMAT_ARGB4444);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatRGBA4444,
            SDL_PIXELFORMAT_RGBA4444);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatABGR4444,
            SDL_PIXELFORMAT_ABGR4444);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatBGRA4444,
            SDL_PIXELFORMAT_BGRA4444);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatARGB1555,
            SDL_PIXELFORMAT_ARGB1555);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatRGBA5551,
            SDL_PIXELFORMAT_RGBA5551);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatABGR1555,
            SDL_PIXELFORMAT_ABGR1555);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatBGRA5551,
            SDL_PIXELFORMAT_BGRA5551);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatRGB565,
            SDL_PIXELFORMAT_RGB565);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatBGR565,
            SDL_PIXELFORMAT_BGR565);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatRGB24,
            SDL_PIXELFORMAT_RGB24);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatBGR24,
            SDL_PIXELFORMAT_BGR24);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatXRGB8888,
            SDL_PIXELFORMAT_XRGB8888);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatRGB888,
            SDL_PIXELFORMAT_RGB888);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatRGBX8888,
            SDL_PIXELFORMAT_RGBX8888);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatXBGR8888,
            SDL_PIXELFORMAT_XBGR8888);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatBGR888,
            SDL_PIXELFORMAT_BGR888);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatBGRX8888,
            SDL_PIXELFORMAT_BGRX8888);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatARGB8888,
            SDL_PIXELFORMAT_ARGB8888);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatRGBA8888,
            SDL_PIXELFORMAT_RGBA8888);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatABGR8888,
            SDL_PIXELFORMAT_ABGR8888);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatBGRA8888,
            SDL_PIXELFORMAT_BGRA8888);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatARGB2101010,
            SDL_PIXELFORMAT_ARGB2101010);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatRGBA32,
            SDL_PIXELFORMAT_RGBA32);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatARGB32,
            SDL_PIXELFORMAT_ARGB32);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatBGRA32,
            SDL_PIXELFORMAT_BGRA32);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatABGR32,
            SDL_PIXELFORMAT_ABGR32);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatRGBX32,
            SDL_PIXELFORMAT_RGBX32);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatXRGB32,
            SDL_PIXELFORMAT_XRGB32);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatBGRX32,
            SDL_PIXELFORMAT_BGRX32);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatXBGR32,
            SDL_PIXELFORMAT_XBGR32);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatYV12,
            SDL_PIXELFORMAT_YV12);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatIYUV,
            SDL_PIXELFORMAT_IYUV);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatYUY2,
            SDL_PIXELFORMAT_YUY2);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatUYVY,
            SDL_PIXELFORMAT_UYVY);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatYVYU,
            SDL_PIXELFORMAT_YVYU);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatNV12,
            SDL_PIXELFORMAT_NV12);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatNV21,
            SDL_PIXELFORMAT_NV21);
    nar->enum_def(Nar_SDL_Pixels_PixelFormat, Nar_SDL_Pixels_PixelFormat__PixelFormatExternalOES,
            SDL_PIXELFORMAT_EXTERNAL_OES);
}