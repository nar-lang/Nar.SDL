#include "_package.h"
#include <SDL_blendmode.h>

SDL_BlendMode to_blend_mode(nar_runtime_t rt, nar_object_t option) {
    nar_int_t result;
    if (!nar->to_enum_option_s(rt, option, &result)) {
        nar_option_t blend_mode = nar->to_option(rt, option);
        if (0 == strcmp(blend_mode.name, Nar_SDL_Blend_BlendMode__BlendModeCustom)) {
            result = nar->to_int(rt, blend_mode.values[0]);
        }
    }
    return result;
}

nar_object_t /*BlendMode*/ native__BlendMode_composeCustomBlendMode(
        nar_runtime_t rt,
        nar_object_t srcColorFactor/* BlendFactor */,
        nar_object_t dstColorFactor/* BlendFactor */,
        nar_object_t colorOperation/* BlendOperation */,
        nar_object_t srcAlphaFactor/* BlendFactor */,
        nar_object_t dstAlphaFactor/* BlendFactor */,
        nar_object_t alphaOperation/* BlendOperation */) {
    SDL_BlendMode mode = SDL_ComposeCustomBlendMode(
            nar->to_enum_option(rt, srcColorFactor),
            nar->to_enum_option(rt, dstColorFactor),
            nar->to_enum_option(rt, colorOperation),
            nar->to_enum_option(rt, srcAlphaFactor),
            nar->to_enum_option(rt, dstAlphaFactor),
            nar->to_enum_option(rt, alphaOperation));
    switch (mode) {
        case SDL_BLENDMODE_NONE:
        case SDL_BLENDMODE_BLEND:
        case SDL_BLENDMODE_ADD:
        case SDL_BLENDMODE_MOD:
        case SDL_BLENDMODE_MUL:
        case SDL_BLENDMODE_INVALID: {
            return nar->make_enum_option(rt, Nar_SDL_Blend_BlendMode, mode, 0, NULL);
        }
        default: {
            nar_object_t value = nar->make_int(rt, mode);
            return nar->make_option(rt, Nar_SDL_Blend_BlendMode__BlendModeCustom, 1, &value);
        }
    }
}

void init_blend_mode(nar_runtime_t rt) {
    nar->register_def(rt, "Nar.SDL.BlendMode", "composeCustomBlendMode",
            &native__BlendMode_composeCustomBlendMode, 6);

    nar->enum_def(Nar_SDL_Blend_BlendMode, Nar_SDL_Blend_BlendMode__BlendModeNone, SDL_BLENDMODE_NONE);
    nar->enum_def(Nar_SDL_Blend_BlendMode, Nar_SDL_Blend_BlendMode__BlendModeBlend, SDL_BLENDMODE_BLEND);
    nar->enum_def(Nar_SDL_Blend_BlendMode, Nar_SDL_Blend_BlendMode__BlendModeAdd, SDL_BLENDMODE_ADD);
    nar->enum_def(Nar_SDL_Blend_BlendMode, Nar_SDL_Blend_BlendMode__BlendModeMod, SDL_BLENDMODE_MOD);
    nar->enum_def(Nar_SDL_Blend_BlendMode, Nar_SDL_Blend_BlendMode__BlendModeMul, SDL_BLENDMODE_MUL);
    nar->enum_def(Nar_SDL_Blend_BlendMode, Nar_SDL_Blend_BlendMode__BlendModeInvalid, SDL_BLENDMODE_INVALID);

    nar->enum_def(Nar_SDL_Blend_BlendOperation, Nar_SDL_Blend_BlendOperation__BlendOperationAdd,
            SDL_BLENDOPERATION_ADD);
    nar->enum_def(Nar_SDL_Blend_BlendOperation, Nar_SDL_Blend_BlendOperation__BlendOperationSubtract,
            SDL_BLENDOPERATION_SUBTRACT);
    nar->enum_def(Nar_SDL_Blend_BlendOperation, Nar_SDL_Blend_BlendOperation__BlendOperationRevSubtract,
            SDL_BLENDOPERATION_REV_SUBTRACT);
    nar->enum_def(Nar_SDL_Blend_BlendOperation, Nar_SDL_Blend_BlendOperation__BlendOperationMinimum,
            SDL_BLENDOPERATION_MINIMUM);
    nar->enum_def(Nar_SDL_Blend_BlendOperation, Nar_SDL_Blend_BlendOperation__BlendOperationMaximum,
            SDL_BLENDOPERATION_MAXIMUM);

    nar->enum_def(Nar_SDL_Blend_BlendFactor, Nar_SDL_Blend_BlendFactor__BlendFactorZero,
            SDL_BLENDFACTOR_ZERO);
    nar->enum_def(Nar_SDL_Blend_BlendFactor, Nar_SDL_Blend_BlendFactor__BlendFactorOne,
            SDL_BLENDFACTOR_ONE);
    nar->enum_def(Nar_SDL_Blend_BlendFactor, Nar_SDL_Blend_BlendFactor__BlendFactorSrcColor,
            SDL_BLENDFACTOR_SRC_COLOR);
    nar->enum_def(Nar_SDL_Blend_BlendFactor, Nar_SDL_Blend_BlendFactor__BlendFactorOneMinusSrcColor,
            SDL_BLENDFACTOR_ONE_MINUS_SRC_COLOR);
    nar->enum_def(Nar_SDL_Blend_BlendFactor, Nar_SDL_Blend_BlendFactor__BlendFactorSrcAlpha,
            SDL_BLENDFACTOR_SRC_ALPHA);
    nar->enum_def(Nar_SDL_Blend_BlendFactor, Nar_SDL_Blend_BlendFactor__BlendFactorOneMinusSrcAlpha,
            SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA);
    nar->enum_def(Nar_SDL_Blend_BlendFactor, Nar_SDL_Blend_BlendFactor__BlendFactorDstColor,
            SDL_BLENDFACTOR_DST_COLOR);
    nar->enum_def(Nar_SDL_Blend_BlendFactor, Nar_SDL_Blend_BlendFactor__BlendFactorOneMinusDstColor,
            SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR);
    nar->enum_def(Nar_SDL_Blend_BlendFactor, Nar_SDL_Blend_BlendFactor__BlendFactorDstAlpha,
            SDL_BLENDFACTOR_DST_ALPHA);
    nar->enum_def(Nar_SDL_Blend_BlendFactor, Nar_SDL_Blend_BlendFactor__BlendFactorOneMinusDstAlpha,
            SDL_BLENDFACTOR_ONE_MINUS_DST_ALPHA);
}