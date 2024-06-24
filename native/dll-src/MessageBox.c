#include "_package.h"
#include <Nar.Program.h>
#include <Nar.Base.h>

SDL_Window *to_window(nar_runtime_t rt, nar_object_t maybe_window_obj) {
    nar_option_t maybe_window = nar->to_option(rt, maybe_window_obj);
    SDL_Window *window = NULL;
    if (0 == strcmp(maybe_window.name, Nar_Base_Maybe_Maybe__Just)) {
        window = (SDL_Window *) nar->to_native(rt, maybe_window.values[0]).ptr;
    }
    return window;
}

void map_MessageBoxButtonData(
        nar_runtime_t rt, nar_cstring_t key, nar_object_t value, SDL_MessageBoxButtonData *mbbd) {
    if (0 == strcmp(key, "flag")) {
        mbbd->flags = nar->to_enum_option(rt, value);
    } else if (0 == strcmp(key, "buttonId")) {
        mbbd->buttonid = (int) nar->to_int(rt, value);
    } else if (0 == strcmp(key, "text")) {
        mbbd->text = nar->to_string(rt, value);
    }
}

SDL_MessageBoxButtonData to_button_data(nar_runtime_t rt, nar_object_t button_obj) {
    SDL_MessageBoxButtonData button;
    nar->map_record(rt, button_obj, &button, (nar_map_record_cb_fn_t) &map_MessageBoxButtonData);
    return button;
}

SDL_MessageBoxColor to_color(nar_runtime_t rt, nar_object_t tuple_obj) {
    nar_tuple_t tuple = nar->to_tuple(rt, tuple_obj);
    return (SDL_MessageBoxColor) {
            .r = (Uint8) nar->to_int(rt, tuple.values[0]),
            .g = (Uint8) nar->to_int(rt, tuple.values[1]),
            .b = (Uint8) nar->to_int(rt, tuple.values[2])
    };
}

void map_MessageBoxColorScheme(nar_runtime_t rt,
        nar_cstring_t key, nar_object_t value, SDL_MessageBoxColorScheme *scheme) {
    if (0 == strcmp(key, "background")) {
        scheme->colors[SDL_MESSAGEBOX_COLOR_BACKGROUND] = to_color(rt, value);
    } else if (0 == strcmp(key, "text")) {
        scheme->colors[SDL_MESSAGEBOX_COLOR_TEXT] = to_color(rt, value);
    } else if (0 == strcmp(key, "buttonBorder")) {
        scheme->colors[SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] = to_color(rt, value);
    } else if (0 == strcmp(key, "buttonBackground")) {
        scheme->colors[SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] = to_color(rt, value);
    } else if (0 == strcmp(key, "buttonSelected")) {
        scheme->colors[SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] = to_color(rt, value);
    }
}

SDL_MessageBoxColorScheme *to_color_scheme(nar_runtime_t rt, nar_object_t record_obj) {
    SDL_MessageBoxColorScheme *scheme = nar->alloc(sizeof(SDL_MessageBoxColorScheme));
    nar->map_record(rt, record_obj, scheme, (nar_map_record_cb_fn_t) &map_MessageBoxColorScheme);
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
            nar->to_enum_option(rt, icon),
            nar->to_string(rt, title),
            nar->to_string(rt, message),
            to_window(rt, x.values[3]));
    if (0 == result) {
        resolve(rt, nar->make_unit(rt), task_state);
    } else {
        reject(rt, nar->make_string(rt, SDL_GetError()), task_state);
    }
}

void map_MessageBoxData(
        nar_runtime_t rt, nar_cstring_t key, nar_object_t value, SDL_MessageBoxData *data) {
    if ((0 == strcmp(key, "icon")) || (0 == strcmp(key, "buttonsLayout"))) {
        data->flags |= nar->to_enum_option(rt, value);
    } else if (0 == strcmp(key, "window")) {
        data->window = to_window(rt, value);
    } else if (0 == strcmp(key, "title")) {
        data->title = nar->to_string(rt, value);
    } else if (0 == strcmp(key, "message")) {
        data->message = nar->to_string(rt, value);
    } else if (0 == strcmp(key, "buttons")) {
        nar_list_t button_list = nar->to_list(rt, value);
        SDL_MessageBoxButtonData *buttons =
                nar->alloc(sizeof(SDL_MessageBoxButtonData) * button_list.size);
        for (nar_size_t j = 0; j < button_list.size; j++) {
            buttons[j] = to_button_data(rt, button_list.items[j]);
        }
        data->numbuttons = (int) button_list.size;
        data->buttons = buttons;
    } else if (0 == strcmp(key, "colorScheme")) {
        nar_option_t maybe_color_scheme = nar->to_option(rt, value);
        if (0 == strcmp(maybe_color_scheme.name, Nar_Base_Maybe_Maybe__Just)) {
            data->colorScheme = to_color_scheme(rt, maybe_color_scheme.values[0]);
        }
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
    nar->map_record(rt, payload, &data, (nar_map_record_cb_fn_t) &map_MessageBoxData);
    Uint32 result = SDL_ShowMessageBox(&data, &buttonId);
    nar->free((void *) data.colorScheme);
    nar->free((void *) data.buttons);

    if (0 == result) {
        resolve(rt, nar->make_int(rt, buttonId), task_state);
    } else {
        reject(rt, nar->make_string(rt, SDL_GetError()), task_state);
    }
}

nar_object_t /* Task[String, ()] */ native__MessageBox_showSimpleMessageBox(nar_runtime_t rt,
        nar_object_t icon /* MessageBoxIcon */,
        nar_object_t title /* String */,
        nar_object_t message /* String */,
        nar_object_t window /* Video.Window */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt,
            nar->make_tuple(rt, 4, (nar_object_t[]) {icon, title, message, window}),
            &show_simple_message_box);
}

nar_object_t /* Task[String, Int] */ native__MessageBox_showMessageBox(nar_runtime_t rt,
        nar_object_t data /* MessageBoxData */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, data, &show_message_box);
}

void init_message_box(nar_runtime_t rt) {
    nar->register_def(rt, "Nar.SDL.MessageBox", "showSimpleMessageBox",
            &native__MessageBox_showSimpleMessageBox, 4);
    nar->register_def(rt, "Nar.SDL.MessageBox", "showMessageBox",
            &native__MessageBox_showMessageBox, 1);

    nar->enum_def(Nar_SDL_MessageBox_MessageBoxIcon, Nar_SDL_MessageBox_MessageBoxIcon__MessageBoxIconError,
            SDL_MESSAGEBOX_ERROR);
    nar->enum_def(Nar_SDL_MessageBox_MessageBoxIcon,
            Nar_SDL_MessageBox_MessageBoxIcon__MessageBoxIconWarning,
            SDL_MESSAGEBOX_WARNING);
    nar->enum_def(Nar_SDL_MessageBox_MessageBoxIcon,
            Nar_SDL_MessageBox_MessageBoxIcon__MessageBoxIconInformation,
            SDL_MESSAGEBOX_INFORMATION);

    nar->enum_def(Nar_SDL_MessageBox_MessageBoxButtonsLayout,
            Nar_SDL_MessageBox_MessageBoxButtonsLayout__MessageBoxButtonsLayoutLeftToRight,
            SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT);
    nar->enum_def(Nar_SDL_MessageBox_MessageBoxButtonsLayout,
            Nar_SDL_MessageBox_MessageBoxButtonsLayout__MessageBoxButtonsLayoutRightToLeft,
            SDL_MESSAGEBOX_BUTTONS_RIGHT_TO_LEFT);

    nar->enum_def(Nar_SDL_MessageBox_MessageBoxButtonFlag,
            Nar_SDL_MessageBox_MessageBoxButtonFlag__MessageBoxButtonFlagNone, 0);
    nar->enum_def(Nar_SDL_MessageBox_MessageBoxButtonFlag,
            Nar_SDL_MessageBox_MessageBoxButtonFlag__MessageBoxButtonFlagReturnKeyDefault,
            SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT);
    nar->enum_def(Nar_SDL_MessageBox_MessageBoxButtonFlag,
            Nar_SDL_MessageBox_MessageBoxButtonFlag__MessageBoxButtonFlagEscapeKeyDefault,
            SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT);
}