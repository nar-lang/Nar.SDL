#include "_package.h"
#include <SDL_video.h>
#include <Nar.Program.h>

Uint32 object_to_window_flags(nar_runtime_t rt, nar_object_t flags_list) {
    Uint32 flags = 0;
    nar_list_t list = nar->to_list(rt, flags_list);
    for (nar_size_t i = 0; i < list.size; i++) {
        nar_cstring_t name = nar->to_option(rt, list.items[i]).name;
        if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagFullscreen, name)) {
            flags |= SDL_WINDOW_FULLSCREEN;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagOpengl, name)) {
            flags |= SDL_WINDOW_OPENGL;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagShown, name)) {
            flags |= SDL_WINDOW_SHOWN;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagHidden, name)) {
            flags |= SDL_WINDOW_HIDDEN;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagBorderless, name)) {
            flags |= SDL_WINDOW_BORDERLESS;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagResizable, name)) {
            flags |= SDL_WINDOW_RESIZABLE;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagMinimized, name)) {
            flags |= SDL_WINDOW_MINIMIZED;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagMaximized, name)) {
            flags |= SDL_WINDOW_MAXIMIZED;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagMouseGrabbed, name)) {
            flags |= SDL_WINDOW_MOUSE_GRABBED;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagInputFocus, name)) {
            flags |= SDL_WINDOW_INPUT_FOCUS;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagMouseFocus, name)) {
            flags |= SDL_WINDOW_MOUSE_FOCUS;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagFullscreenDesktop, name)) {
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagForeign, name)) {
            flags |= SDL_WINDOW_FOREIGN;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagAllowHighdpi, name)) {
            flags |= SDL_WINDOW_ALLOW_HIGHDPI;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagMouseCapture, name)) {
            flags |= SDL_WINDOW_MOUSE_CAPTURE;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagAlwaysOnTop, name)) {
            flags |= SDL_WINDOW_ALWAYS_ON_TOP;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagSkipTaskbar, name)) {
            flags |= SDL_WINDOW_SKIP_TASKBAR;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagUtility, name)) {
            flags |= SDL_WINDOW_UTILITY;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagTooltip, name)) {
            flags |= SDL_WINDOW_TOOLTIP;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagPopupMenu, name)) {
            flags |= SDL_WINDOW_POPUP_MENU;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagKeyboardGrabbed, name)) {
            flags |= SDL_WINDOW_KEYBOARD_GRABBED;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagVulkan, name)) {
            flags |= SDL_WINDOW_VULKAN;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagMetal, name)) {
            flags |= SDL_WINDOW_METAL;
        } else if (0 == strcmp(Nar_SDL_Video_WindowFlag__WindowFlagInputGrabbed, name)) {
            flags |= SDL_WINDOW_INPUT_GRABBED;
        }
    }

    return flags;
}

void object_to_position(nar_runtime_t rt, nar_object_t pos, int *x, int *y) {
    nar_option_t opt = nar->to_option(rt, pos);
    if (0 == strcmp(opt.name, Nar_SDL_Video_WindowPosition__WindowPositionCentered)) {
        *x = SDL_WINDOWPOS_CENTERED;
        *y = SDL_WINDOWPOS_CENTERED;
    } else if (0 == strcmp(opt.name, Nar_SDL_Video_WindowPosition__WindowPositionAt)) {
        *x = (int) nar->to_int(rt, opt.values[0]);
        *y = (int) nar->to_int(rt, opt.values[1]);
    } else {
        *x = SDL_WINDOWPOS_UNDEFINED;
        *y = SDL_WINDOWPOS_UNDEFINED;
    }
}

void create_window(nar_runtime_t rt, nar_object_t payload,
        nar_program_task_resolve_fn_t resolve, nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    int x, y;
    object_to_position(rt, data.values[1], &x, &y);

    SDL_Window *window = SDL_CreateWindow(
            nar->to_string(rt, data.values[0]),
            x, y,
            (int) nar->to_int(rt, data.values[2]),
            (int) nar->to_int(rt, data.values[3]),
            object_to_window_flags(rt, data.values[4]));

    if (NULL != window) {
        resolve(rt, nar->make_native(rt, window, NULL), task_state);
    } else {
        reject(rt, nar->make_string(rt, SDL_GetError()), task_state);
    }
}

nar_object_t native__createWindow(nar_runtime_t rt, nar_object_t title, nar_object_t position,
        nar_object_t width, nar_object_t height, nar_object_t flags) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 5, (nar_object_t[]) {
            title, position, width, height, flags
    });
    return task_new(rt, data, &create_window);
}

void destroy_window(nar_runtime_t rt, nar_object_t payload,
        nar_program_task_resolve_fn_t resolve, nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_DestroyWindow(nar->to_native(rt, payload).ptr);
    resolve(rt, nar->make_unit(rt), task_state);
}

nar_object_t native__destroyWindow(nar_runtime_t rt, nar_object_t window) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, window, &destroy_window);
}