#include "_package.h"
#include <Nar.Program.h>
#include <SDL_messagebox.h>
#include <Nar.Base.h>

Uint32 icon_to_flags(nar_runtime_t rt, nar_object_t icon) {
    Uint32 flags = 0;
    nar_cstring_t icon_name = nar->to_option(rt, icon).name;
    if (0 == strcmp(icon_name, Nar_SDL_MessageBox_Icon__IconError)) {
        flags |= SDL_MESSAGEBOX_ERROR;
    } else if (0 == strcmp(icon_name, Nar_SDL_MessageBox_Icon__IconWarning)) {
        flags |= SDL_MESSAGEBOX_WARNING;
    } else if (0 == strcmp(icon_name, Nar_SDL_MessageBox_Icon__IconInformation)) {
        flags |= SDL_MESSAGEBOX_INFORMATION;
    }
    return flags;
}

Uint32 buttons_layout_to_flags(nar_runtime_t rt, nar_object_t layout) {
    Uint32 flags = 0;
    nar_cstring_t layout_name = nar->to_option(rt, layout).name;
    if (0 == strcmp(layout_name, Nar_SDL_MessageBox_ButtonsLayout__ButtonsLayoutLeftToRight)) {
        flags |= SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;
    } else if (0 == strcmp(
            layout_name, Nar_SDL_MessageBox_ButtonsLayout__ButtonsLayoutRightToLeft)) {
        flags |= SDL_MESSAGEBOX_BUTTONS_RIGHT_TO_LEFT;
    }
    return flags;
}

SDL_Window *maybe_window_to_window(nar_runtime_t rt, nar_object_t maybe_window_obj) {
    nar_option_t maybe_window = nar->to_option(rt, maybe_window_obj);
    SDL_Window *window = NULL;
    if (0 == strcmp(maybe_window.name, Nar_Base_Maybe_Maybe__Just)) {
        window = (SDL_Window *) nar->to_native(rt, maybe_window.values[0]).ptr;
    }
    return window;
}

SDL_MessageBoxButtonData record_to_button_data(nar_runtime_t rt, nar_object_t button_obj) {
    SDL_MessageBoxButtonData button;
    nar_record_t record = nar->to_record(rt, button_obj);
    for (nar_size_t i = 0; i < record.size; i++) {
        nar_cstring_t key = record.keys[i];
        nar_object_t value = record.values[i];
        if (0 == strcmp(key, "flag")) {
            nar_option_t flag = nar->to_option(rt, value);
            if (0 == strcmp(flag.name, Nar_SDL_MessageBox_ButtonFlag__ButtonFlagNone)) {
                button.flags = 0;
            } else if (0 == strcmp(
                    flag.name, Nar_SDL_MessageBox_ButtonFlag__ButtonFlagReturnKeyDefault)) {
                button.flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            } else if (0 == strcmp(
                    flag.name, Nar_SDL_MessageBox_ButtonFlag__ButtonFlagEscapeKeyDefault)) {
                button.flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            }
        } else if (0 == strcmp(key, "buttonId")) {
            button.buttonid = (int) nar->to_int(rt, value);
        } else if (0 == strcmp(key, "text")) {
            button.text = nar->to_string(rt, value);
        }
    }
    return button;
}

SDL_MessageBoxColor tuple_to_color(nar_runtime_t rt, nar_object_t tuple_obj) {
    nar_tuple_t tuple = nar->to_tuple(rt, tuple_obj);
    return (SDL_MessageBoxColor) {
            .r = (Uint8) nar->to_int(rt, tuple.values[0]),
            .g = (Uint8) nar->to_int(rt, tuple.values[1]),
            .b = (Uint8) nar->to_int(rt, tuple.values[2])
    };
}

SDL_MessageBoxColorScheme *record_to_color_scheme(nar_runtime_t rt, nar_object_t record_obj) {
    SDL_MessageBoxColorScheme *scheme = nar->alloc(sizeof(SDL_MessageBoxColorScheme));
    nar_record_t record = nar->to_record(rt, record_obj);
    for (nar_size_t i = 0; i < record.size; i++) {
        nar_cstring_t key = record.keys[i];
        nar_object_t value = record.values[i];
        if (0 == strcmp(key, "background")) {
            scheme->colors[SDL_MESSAGEBOX_COLOR_BACKGROUND] = tuple_to_color(rt, value);
        } else if (0 == strcmp(key, "text")) {
            scheme->colors[SDL_MESSAGEBOX_COLOR_TEXT] = tuple_to_color(rt, value);
        } else if (0 == strcmp(key, "buttonBorder")) {
            scheme->colors[SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] = tuple_to_color(rt, value);
        } else if (0 == strcmp(key, "buttonBackground")) {
            scheme->colors[SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] = tuple_to_color(rt, value);
        } else if (0 == strcmp(key, "buttonSelected")) {
            scheme->colors[SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] = tuple_to_color(rt, value);
        }
    }
    return scheme;
}

void show_simple_message_box(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t x = nar->to_tuple(rt, payload);
    nar_object_t icon = x.values[0];
    nar_object_t title = x.values[1];
    nar_object_t message = x.values[2];

    Uint32 result = SDL_ShowSimpleMessageBox(
            icon_to_flags(rt, icon),
            nar->to_string(rt, title),
            nar->to_string(rt, message),
            maybe_window_to_window(rt, x.values[3]));
    if (0 == result) {
        resolve(rt, nar->make_unit(rt), task_state);
    } else {
        reject(rt, nar->make_string(rt, SDL_GetError()), task_state);
    }
}

void show_message_box(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    int buttonId;
    SDL_MessageBoxData data = {0};
    nar_record_t record = nar->to_record(rt, payload);
    for (nar_size_t i = 0; i < record.size; i++) {
        nar_cstring_t key = record.keys[i];
        nar_object_t value = record.values[i];
        if (0 == strcmp(key, "icon")) {
            data.flags |= icon_to_flags(rt, value);
        } else if (0 == strcmp(key, "buttonsLayout")) {
            data.flags |= buttons_layout_to_flags(rt, value);
        } else if (0 == strcmp(key, "window")) {
            data.window = maybe_window_to_window(rt, value);
        } else if (0 == strcmp(key, "title")) {
            data.title = nar->to_string(rt, value);
        } else if (0 == strcmp(key, "message")) {
            data.message = nar->to_string(rt, value);
        } else if (0 == strcmp(key, "buttons")) {
            nar_list_t button_list = nar->to_list(rt, value);
            SDL_MessageBoxButtonData *buttons =
                    nar->alloc(sizeof(SDL_MessageBoxButtonData) * button_list.size);
            for (nar_size_t j = 0; j < button_list.size; j++) {
                buttons[j] = record_to_button_data(rt, button_list.items[j]);
            }
            data.numbuttons = (int) button_list.size;
            data.buttons = buttons;
        } else if (0 == strcmp(key, "colorScheme")) {
            nar_option_t maybe_color_scheme = nar->to_option(rt, value);
            if (0 == strcmp(maybe_color_scheme.name, Nar_Base_Maybe_Maybe__Just)) {
                data.colorScheme = record_to_color_scheme(rt, maybe_color_scheme.values[0]);
            }
        }
    }
    Uint32 result = SDL_ShowMessageBox(&data, &buttonId);
    nar->free((void *) data.colorScheme);
    nar->free((void *) data.buttons);

    if (0 == result) {
        resolve(rt, nar->make_int(rt, buttonId), task_state);
    } else {
        reject(rt, nar->make_string(rt, SDL_GetError()), task_state);
    }
}

nar_object_t native__MessageBox_showSimpleMessageBox(
        nar_runtime_t rt,
        nar_object_t icon,
        nar_object_t title,
        nar_object_t message,
        nar_object_t window) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt,
            nar->make_tuple(rt, 4, (nar_object_t[]) {icon, title, message, window}),
            &show_simple_message_box);
}

nar_object_t native__MessageBox_showMessageBox(
        nar_runtime_t rt,
        nar_object_t data) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, data, &show_message_box);
}