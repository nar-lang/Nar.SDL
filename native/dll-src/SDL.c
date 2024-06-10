#include "_package.h"
#include <Nar.Program.h>
#include <SDL.h>

Uint32 subsystem_to_flag(nar_runtime_t rt, nar_object_t subSystem) {
    nar_option_t opt = nar->to_option(rt, subSystem);
    nar_cstring_t name = opt.name;

    if (0 == strcmp(Nar_SDL_InitSubSystem__InitTimer, name)) {
        return SDL_INIT_TIMER;
    }
    if (0 == strcmp(Nar_SDL_InitSubSystem__InitAudio, name)) {
        return SDL_INIT_AUDIO;
    }
    if (0 == strcmp(Nar_SDL_InitSubSystem__InitVideo, name)) {
        return SDL_INIT_VIDEO;
    }
    if (0 == strcmp(Nar_SDL_InitSubSystem__InitJoystick, name)) {
        return SDL_INIT_JOYSTICK;
    }
    if (0 == strcmp(Nar_SDL_InitSubSystem__InitHaptic, name)) {
        return SDL_INIT_HAPTIC;
    }
    if (0 == strcmp(Nar_SDL_InitSubSystem__InitGameController, name)) {
        return SDL_INIT_GAMECONTROLLER;
    }
    if (0 == strcmp(Nar_SDL_InitSubSystem__InitEvents, name)) {
        return SDL_INIT_EVENTS;
    }
    if (0 == strcmp(Nar_SDL_InitSubSystem__InitSensor, name)) {
        return SDL_INIT_SENSOR;
    }
    return 0;
}

nar_bool_t update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (SDL_QUIT == event.type) {
            return nar_false;
        }
    }
    return nar_true;
}

Uint32 subsystem_list_to_flag(nar_runtime_t rt, nar_object_t subSystems) {
    Uint32 flags = 0;
    nar_object_t it = subSystems;
    while (nar->index_is_valid(rt, it)) {
        nar_list_item_t item = nar->to_list_item(rt, it);
        flags |= subsystem_to_flag(rt, item.value);
        it = item.next;
    }
    return flags;
}

void init_sdl(nar_runtime_t rt, nar_object_t payload, nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject, nar_task_state_t *task_state) {
    Uint32 flags = subsystem_list_to_flag(rt, payload);
    if (0 == SDL_Init(flags)) {
        nar_program_updater_add_t updater_add =
                nar->get_metadata(rt, NAR_META__Nar_Program_updater_add);
        updater_add(rt, update, 0);
        resolve(rt, payload, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t native__init(nar_runtime_t rt, nar_object_t subSystems) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, subSystems, init_sdl);
}

void init_subsystem_exec(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    Uint32 flags = subsystem_to_flag(rt, payload);
    if (0 == SDL_InitSubSystem(flags)) {
        resolve(rt, payload, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t native__initSubSystem(nar_runtime_t rt, nar_object_t subSystem) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, subSystem, init_subsystem_exec);
}

void quit_subsystem(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    Uint32 flags = subsystem_to_flag(rt, payload);
    SDL_QuitSubSystem(flags);
    resolve(rt, payload, task_state);
}

nar_object_t native__quitSubSystem(nar_runtime_t rt, nar_object_t subSystem) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, subSystem, quit_subsystem);
}

void was_init(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_cmd_resolve_fn_t resolve,
        nar_cmd_state_t *cmd_state) {
    Uint32 flags = subsystem_list_to_flag(rt, payload);
    Uint32 initialized = SDL_WasInit(flags);
    nar_object_t list = nar->make_list(rt, 0, NULL);
    if (0 != (initialized & SDL_INIT_TIMER)) {
        nar_object_t value = nar->make_option(rt, Nar_SDL_InitSubSystem__InitTimer, 0, NULL);
        list = nar->make_list_cons(rt, value, list);
    }
    if (0 != (initialized & SDL_INIT_AUDIO)) {
        nar_object_t value = nar->make_option(rt, Nar_SDL_InitSubSystem__InitAudio, 0, NULL);
        list = nar->make_list_cons(rt, value, list);
    }
    if (0 != (initialized & SDL_INIT_VIDEO)) {
        nar_object_t value = nar->make_option(rt, Nar_SDL_InitSubSystem__InitVideo, 0, NULL);
        list = nar->make_list_cons(rt, value, list);
    }
    if (0 != (initialized & SDL_INIT_JOYSTICK)) {
        nar_object_t value = nar->make_option(rt, Nar_SDL_InitSubSystem__InitJoystick, 0, NULL);
        list = nar->make_list_cons(rt, value, list);
    }
    if (0 != (initialized & SDL_INIT_HAPTIC)) {
        nar_object_t value = nar->make_option(rt, Nar_SDL_InitSubSystem__InitHaptic, 0, NULL);
        list = nar->make_list_cons(rt, value, list);
    }
    if (0 != (initialized & SDL_INIT_GAMECONTROLLER)) {
        nar_object_t value = nar->make_option(
                rt, Nar_SDL_InitSubSystem__InitGameController, 0, NULL);
        list = nar->make_list_cons(rt, value, list);
    }
    if (0 != (initialized & SDL_INIT_EVENTS)) {
        nar_object_t value = nar->make_option(rt, Nar_SDL_InitSubSystem__InitEvents, 0, NULL);
        list = nar->make_list_cons(rt, value, list);
    }
    if (0 != (initialized & SDL_INIT_SENSOR)) {
        nar_object_t value = nar->make_option(rt, Nar_SDL_InitSubSystem__InitSensor, 0, NULL);
        list = nar->make_list_cons(rt, value, list);
    }
    resolve(rt, list, cmd_state);
}

nar_object_t native__wasInit(nar_runtime_t rt, nar_object_t subSystems, nar_object_t toMsg) {
    nar_program_cmd_new_fn_t cmd_new = nar->get_metadata(rt, NAR_META__Nar_Program_cmd_new);
    return cmd_new(rt, toMsg, subSystems, was_init);
}

void quit_sdl(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Quit();
    nar_program_updater_remove_t updater_remove =
            nar->get_metadata(rt, NAR_META__Nar_Program_updater_remove);
    updater_remove(rt, update);
    resolve(rt, nar->make_unit(rt), task_state);
}

nar_object_t native__quit(nar_runtime_t rt) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, nar->make_unit(rt), quit_sdl);
}
