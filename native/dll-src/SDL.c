#include "_package.h"
#include <Nar.Program.h>
#include <SDL.h>

nar_bool_t update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (SDL_QUIT == event.type) {
            return nar_false;
        }
    }
    return nar_true;
}

void init_sdl_task(nar_runtime_t rt, nar_object_t payload, nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject, nar_task_state_t *task_state) {
    Uint32 flags = nar->to_enum_option_flags(rt, payload);
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

nar_object_t /* Task[String, List[InitSubSystem]] */ native__init(nar_runtime_t rt,
        nar_object_t subSystems /* List[InitSubSystem] */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, subSystems, init_sdl_task);
}

void init_subsystem_exec(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    Uint32 flags = nar->to_enum_option(rt, payload);
    if (0 == SDL_InitSubSystem(flags)) {
        resolve(rt, payload, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, InitSubSystem] */ native__initSubSystem(nar_runtime_t rt,
        nar_object_t subSystem /* InitSubSystem */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, subSystem, init_subsystem_exec);
}

void quit_subsystem(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    Uint32 flags = nar->to_enum_option(rt, payload);
    SDL_QuitSubSystem(flags);
    resolve(rt, payload, task_state);
}

nar_object_t /* Task[String, InitSubSystem] */ native__quitSubSystem(nar_runtime_t rt,
        nar_object_t subSystem /* InitSubSystem */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, subSystem, quit_subsystem);
}

void was_init(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    Uint32 flags = nar->to_enum_option_flags(rt, payload);
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
    resolve(rt, list, task_state);
}

nar_object_t /* Task[String, List[InitSubSystem]] */ native__wasInit(nar_runtime_t rt,
        nar_object_t subSystems /* List[InitSubSystem] */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, subSystems, was_init);
}

typedef struct {
    nar_program_sub_trigger_fn_t trigger;
    nar_sub_state_t *sub_state;
} sub_quit_t;

void quit_sdl(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    sub_quit_t *sub_quit = (sub_quit_t *) nar->get_metadata(rt, NAR_META__Nar_SDL_onQuit_sub);
    if (NULL != sub_quit) {
        sub_quit->trigger(rt, nar->make_unit(rt), sub_quit->sub_state);
    }

    SDL_Quit();
    nar_program_updater_remove_t updater_remove =
            nar->get_metadata(rt, NAR_META__Nar_Program_updater_remove);
    updater_remove(rt, update);
    resolve(rt, nar->make_unit(rt), task_state);
}

nar_object_t /* Task[String, ()] */ native__quit(nar_runtime_t rt) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, nar->make_unit(rt), quit_sdl);
}

void quit_on(nar_runtime_t rt, nar_object_t payload, nar_program_sub_trigger_fn_t trigger,
        nar_sub_state_t *sub_state) {
    sub_quit_t *sub_quit = (sub_quit_t *) nar->get_metadata(rt, NAR_META__Nar_SDL_onQuit_sub);
    if (NULL != sub_quit) {
        nar->free(sub_quit);
    }
    sub_quit = nar->alloc(sizeof(sub_quit_t));
    sub_quit->trigger = trigger;
    sub_quit->sub_state = sub_state;
    nar->set_metadata(rt, NAR_META__Nar_SDL_onQuit_sub, sub_quit);
}

void quit_off(nar_runtime_t rt, nar_object_t payload) {
    sub_quit_t *sub_quit = (sub_quit_t *) nar->get_metadata(rt, NAR_META__Nar_SDL_onQuit_sub);
    if (NULL != sub_quit) {
        nar->free(sub_quit);
    }
    nar->set_metadata(rt, NAR_META__Nar_SDL_onQuit_sub, NULL);
}

nar_object_t /* Sub[msg] */ native__onQuit(nar_runtime_t rt,
        nar_object_t toMsg /* ( () ): msg */) {
    nar_program_sub_new_fn_t sub_new = nar->get_metadata(rt, NAR_META__Nar_Program_sub_new);
    return sub_new(rt, toMsg, NAR_INVALID_OBJECT, &quit_on, &quit_off);
}

void init_sdl(nar_runtime_t rt) {
    nar->register_def(rt, "Nar.SDL", "init", &native__init, 1);
    nar->register_def(rt, "Nar.SDL", "initSubSystem", &native__initSubSystem, 1);
    nar->register_def(rt, "Nar.SDL", "quitSubSystem", &native__quitSubSystem, 1);
    nar->register_def(rt, "Nar.SDL", "wasInit", &native__wasInit, 1);
    nar->register_def(rt, "Nar.SDL", "quit", &native__quit, 0);
    nar->register_def(rt, "Nar.SDL", "onQuit", &native__onQuit, 1);

    nar->enum_def(Nar_SDL_InitSubSystem, Nar_SDL_InitSubSystem__InitTimer, SDL_INIT_TIMER);
    nar->enum_def(Nar_SDL_InitSubSystem, Nar_SDL_InitSubSystem__InitAudio, SDL_INIT_AUDIO);
    nar->enum_def(Nar_SDL_InitSubSystem, Nar_SDL_InitSubSystem__InitVideo, SDL_INIT_VIDEO);
    nar->enum_def(Nar_SDL_InitSubSystem, Nar_SDL_InitSubSystem__InitJoystick, SDL_INIT_JOYSTICK);
    nar->enum_def(Nar_SDL_InitSubSystem, Nar_SDL_InitSubSystem__InitHaptic, SDL_INIT_HAPTIC);
    nar->enum_def(Nar_SDL_InitSubSystem, Nar_SDL_InitSubSystem__InitGameController,
            SDL_INIT_GAMECONTROLLER);
    nar->enum_def(Nar_SDL_InitSubSystem, Nar_SDL_InitSubSystem__InitEvents, SDL_INIT_EVENTS);
    nar->enum_def(Nar_SDL_InitSubSystem, Nar_SDL_InitSubSystem__InitSensor, SDL_INIT_SENSOR);
}