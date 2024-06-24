#include "_package.h"
#include <Nar.Base.h>
#include <Nar.Program.h>

nar_object_t make_DisplayMode(nar_runtime_t *rt, const SDL_DisplayMode *dm) {
    return nar->make_record(rt, 5,
            (nar_cstring_t[]) {"format", "w", "h", "refreshRate", "driverData"},
            (nar_object_t[]) {
                    nar->make_int(rt, dm->format),
                    nar->make_int(rt, dm->w),
                    nar->make_int(rt, dm->h),
                    nar->make_int(rt, dm->refresh_rate),
                    nar->make_native(rt, dm->driverdata, NULL)});
}

int to_window_position(nar_runtime_t *rt, nar_object_t p) {
    nar_option_t opt = nar->to_option(rt, p);
    if (0 == strcmp(opt.name, Nar_SDL_Video_WindowPosition__WindowPositionUndefined)) {
        return SDL_WINDOWPOS_UNDEFINED;
    } else if (0 == strcmp(opt.name, Nar_SDL_Video_WindowPosition__WindowPositionCentered)) {
        return SDL_WINDOWPOS_CENTERED;
    } else {
        return (int) nar->to_int(rt, opt.values[0]);
    }
}

void map_DisplayMode(nar_runtime_t rt,
        nar_cstring_t key, nar_object_t value, SDL_DisplayMode *rect) {
    if (0 == strcmp(key, "format")) {
        rect->format = (Uint32) nar->to_int(rt, value);
    } else if (0 == strcmp(key, "w")) {
        rect->w = (int) nar->to_int(rt, value);
    } else if (0 == strcmp(key, "h")) {
        rect->h = (int) nar->to_int(rt, value);
    } else if (0 == strcmp(key, "refreshRate")) {
        rect->refresh_rate = (int) nar->to_int(rt, value);
    } else if (0 == strcmp(key, "driverData")) {
        rect->driverdata = nar->to_native(rt, value).ptr;
    }
}

nar_object_t /* Int */ native__Video_numVideoDrivers(nar_runtime_t rt) {
    int n = SDL_GetNumRenderDrivers();
    return nar->make_int(rt, n);
}

nar_object_t /* Maybe[String] */ native__Video_getVideoDriver(nar_runtime_t rt,
        nar_object_t index /* Int */) {
    const char *name = SDL_GetVideoDriver((int) nar->to_int(rt, index));
    if (NULL == name) {
        nar_object_t result = nar->make_string(rt, name);
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Just, 1, &result);
    } else {
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL);
    }
}

void video_init(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    const char *driverName = NULL;
    nar_option_t opt = nar->to_option(rt, payload);
    if (0 == strcmp(opt.name, Nar_Base_Maybe_Maybe__Just)) {
        driverName = nar->to_string(rt, opt.values[0]);
    }
    if (0 == SDL_VideoInit(driverName)) {
        resolve(rt, payload, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, ()] */ native__Video_videoInit(nar_runtime_t rt,
        nar_object_t driverName /* Maybe[String] */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, driverName, video_init);
}

void video_quit(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_VideoQuit();
    resolve(rt, NAR_INVALID_OBJECT, task_state);
}

nar_object_t /* Task[Never, ()] */ native__Video_videoQuit(nar_runtime_t rt) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, NAR_INVALID_OBJECT, video_quit);
}

void current_video_driver(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    const char *name = SDL_GetCurrentVideoDriver();
    nar_object_t result = nar->make_string(rt, name);
    resolve(rt, result, task_state);
}

nar_object_t /* Task[Never, String] */ native__Video_currentVideoDriver(nar_runtime_t rt) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, NAR_INVALID_OBJECT, current_video_driver);
}

nar_object_t /* Int */ native__Video_numVideoDisplays(nar_runtime_t rt) {
    int n = SDL_GetNumVideoDisplays();
    return nar->make_int(rt, n);
}

nar_object_t /* Maybe[String] */ native__Video_getDisplayName(nar_runtime_t rt,
        nar_object_t displayIndex /* Int */) {
    const char *name = SDL_GetDisplayName((int) nar->to_int(rt, displayIndex));
    if (NULL == name) {
        nar_object_t result = nar->make_string(rt, name);
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Just, 1, &result);
    } else {
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL);
    }
}

nar_object_t /* Maybe[Rect] */ native__Video_getDisplayBounds(nar_runtime_t rt,
        nar_object_t displayIndex /* Int */) {
    SDL_Rect rect;
    if (0 == SDL_GetDisplayBounds((int) nar->to_int(rt, displayIndex), &rect)) {
        nar_object_t result = make_Rect(rt, &rect);
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Just, 1, &result);
    } else {
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL);
    }
}

nar_object_t /* Maybe[Rect] */ native__Video_getDisplayUsableBounds(nar_runtime_t rt,
        nar_object_t displayIndex /* Int */) {
    SDL_Rect rect;
    if (0 == SDL_GetDisplayUsableBounds((int) nar->to_int(rt, displayIndex), &rect)) {
        nar_object_t result = make_Rect(rt, &rect);
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Just, 1, &result);
    } else {
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL);
    }
}

nar_object_t /* Maybe[(Float, Float, Float)] */ native__Video_getDisplayDpi(nar_runtime_t rt,
        nar_object_t displayIndex /* Int */) {
    float dd, hd, vd;
    if (0 == SDL_GetDisplayDPI((int) nar->to_int(rt, displayIndex), &dd, &hd, &vd)) {
        nar_object_t result = nar->make_tuple(rt, 3, (nar_object_t[]) {
                nar->make_float(rt, dd), nar->make_float(rt, hd), nar->make_float(rt, vd)});
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Just, 1, &result);
    } else {
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL);
    }
}

nar_object_t /* Maybe[Orientation] */ native__Video_getDisplayOrientation(nar_runtime_t rt,
        nar_object_t displayIndex /* Int  */) {
    int index = (int) nar->to_int(rt, displayIndex);
    SDL_DisplayOrientation orientation = SDL_GetDisplayOrientation(index);
    if (orientation != SDL_ORIENTATION_UNKNOWN) {
        nar_object_t result = nar->make_enum_option(rt, Nar_SDL_Video_Orientation, orientation, 0,
                NULL);
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Just, 1, &result);
    } else {
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL);
    }
}

nar_object_t /* Result[String, Int] */ native__Video_getNumDisplayModes(nar_runtime_t rt,
        nar_object_t displayIndex /* Int */) {
    int index = (int) nar->to_int(rt, displayIndex);
    int n = SDL_GetNumDisplayModes(index);
    if (n >= 0) {
        nar_object_t result = nar->make_int(rt, n);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Maybe[DisplayMode] */ native__Video_getDisplayMode(nar_runtime_t rt,
        nar_object_t displayIndex /* Int */,
        nar_object_t modeIndex /* Int  */) {
    int index = (int) nar->to_int(rt, displayIndex);
    int mode = (int) nar->to_int(rt, modeIndex);
    SDL_DisplayMode dm;
    if (0 == SDL_GetDisplayMode(index, mode, &dm)) {
        nar_object_t result = make_DisplayMode(rt, &dm);
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Just, 1, &result);
    } else {
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL);
    }
}

nar_object_t /* Result[String,  DisplayMode] */
native__Video_getCurrentDisplayMode(nar_runtime_t rt,
        nar_object_t displayIndex /* Int  */) {
    int index = (int) nar->to_int(rt, displayIndex);
    SDL_DisplayMode dm;
    if (0 == SDL_GetCurrentDisplayMode(index, &dm)) {
        nar_object_t result = make_DisplayMode(rt, &dm);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, DisplayMode] */ native__Video_getClosestDisplayMode(nar_runtime_t rt,
        nar_object_t displayIndex /* Int */,
        nar_object_t mode /* DisplayMode  */) {
    int index = (int) nar->to_int(rt, displayIndex);
    SDL_DisplayMode dm;
    nar->map_record(rt, mode, &dm, (nar_map_record_cb_fn_t) map_DisplayMode);
    SDL_DisplayMode closest;
    if (0 == SDL_GetClosestDisplayMode(index, &dm, &closest)) {
        nar_object_t result = make_DisplayMode(rt, &closest);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Int] */ native__Video_getPointDisplayIndex(nar_runtime_t rt,
        nar_object_t point /* Point */) {
    SDL_Point pt;
    nar->map_record(rt, point, &pt, (nar_map_record_cb_fn_t) map_Point);
    int index = SDL_GetPointDisplayIndex(&pt);
    if (index >= 0) {
        nar_object_t result = nar->make_int(rt, index);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Int] */ native__Video_getRectDisplayIndex(nar_runtime_t rt,
        nar_object_t rect /* Rect */) {
    SDL_Rect r;
    nar->map_record(rt, rect, &r, (nar_map_record_cb_fn_t) map_Rect);
    int index = SDL_GetRectDisplayIndex(&r);
    if (index >= 0) {
        nar_object_t result = nar->make_int(rt, index);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Int] */ native__Video_getWindowDisplayIndex(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    int index = SDL_GetWindowDisplayIndex(nar->to_native(rt, window).ptr);
    if (index >= 0) {
        nar_object_t result = nar->make_int(rt, index);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Window] */ native__Video_setWindowDisplayMode(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t mode /* DisplayMode  */) {
    SDL_DisplayMode dm;
    nar->map_record(rt, mode, &dm, (nar_map_record_cb_fn_t) map_DisplayMode);
    SDL_Window *win = nar->to_native(rt, window).ptr;
    if (0 == SDL_SetWindowDisplayMode(win, &dm)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &window);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, DisplayMode] */ native__Video_getWindowDisplayMode(nar_runtime_t rt,
        nar_object_t window /* Window  */) {
    SDL_DisplayMode dm;
    SDL_Window *win = nar->to_native(rt, window).ptr;
    if (0 == SDL_GetWindowDisplayMode(win, &dm)) {
        nar_object_t result = make_DisplayMode(rt, &dm);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, List[Int]] */ native__Video_getWindowICCProfile(nar_runtime_t rt,
        nar_object_t window /* Window  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    size_t size;
    const void *data = SDL_GetWindowICCProfile(win, &size);
    if (NULL != data) {
        nar_object_t *items = nar->alloc(size * sizeof(nar_object_t));
        for (size_t i = 0; i < size; i++) {
            items[i] = nar->make_int(rt, ((char *) data)[i]);
        }
        nar_object_t result = nar->make_list(rt, size, items);
        nar->free(items);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, PixelFormat] */ native__Video_getWindowPixelFormat(nar_runtime_t rt,
        nar_object_t window /* Window  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    Uint32 format = SDL_GetWindowPixelFormat(win);
    if (format != SDL_PIXELFORMAT_UNKNOWN) {
        nar_object_t result = nar->make_enum_option(rt, Nar_SDL_Pixels_PixelFormat, format, 0,
                NULL);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    };
}

nar_object_t /* Task[String, Window] */ native__Video_createWindow(nar_runtime_t rt,
        nar_object_t title /* String */,
        nar_object_t posX /* WindowPosition */,
        nar_object_t posY /* WindowPosition */,
        nar_object_t width /* Int */,
        nar_object_t height /* Int */,
        nar_object_t flags /* List[WindowFlag]  */) {
    SDL_Window *window = SDL_CreateWindow(
            nar->to_string(rt, title),
            to_window_position(rt, posX),
            to_window_position(rt, posY),
            (int) nar->to_int(rt, width),
            (int) nar->to_int(rt, height),
            nar->to_enum_option_flags(rt, flags));
    if (NULL != window) {
        nar_object_t result = nar->make_native(rt, window, NULL);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Task[String, Window] */ native__Video_createWindowFrom(nar_runtime_t rt,
        nar_object_t data /* Ptr */) {
    SDL_Window *window = SDL_CreateWindowFrom(nar->to_native(rt, data).ptr);
    if (NULL != window) {
        nar_object_t result = nar->make_native(rt, window, NULL);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Int] */ native__Video_getWindowID(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    Uint32 id = SDL_GetWindowID(win);
    if (id != 0) {
        nar_object_t result = nar->make_int(rt, id);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Window] */ native__Video_getWindowFromID(nar_runtime_t rt,
        nar_object_t id /* Int */) {
    SDL_Window *win = SDL_GetWindowFromID((Uint32) nar->to_int(rt, id));
    if (NULL != win) {
        nar_object_t result = nar->make_native(rt, win, NULL);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* List[WindowFlag] */ native__Video_getWindowFlags(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    Uint32 flags = SDL_GetWindowFlags(win);
    return nar->make_enum_option_flags(rt, Nar_SDL_Video_WindowFlag, flags);
}

nar_object_t /* Window */ native__Video_setWindowTitle(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t title /* String */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_SetWindowTitle(win, nar->to_string(rt, title));
    return window;
}

nar_object_t /* Result[String, String] */ native__Video_getWindowTitle(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    const char *title = SDL_GetWindowTitle(win);
    if (NULL != title) {
        nar_object_t result = nar->make_string(rt, title);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Window */ native__Video_setWindowIcon(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t icon /* Surface */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_Surface *surf = nar->to_native(rt, icon).ptr;
    SDL_SetWindowIcon(win, surf);
    return window;
}

nar_object_t /* (Window, Ptr) */ native__Video_setWindowData(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t name /* String */,
        nar_object_t data /* Ptr  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    void *prev = SDL_SetWindowData(win, nar->to_string(rt, name), nar->to_native(rt, data).ptr);
    return nar->make_tuple(rt, 2, (nar_object_t[]) {window, nar->make_native(rt, prev, NULL)});
}

nar_object_t /* Ptr */ native__Video_getWindowData(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t name /* String */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    void *data = SDL_GetWindowData(win, nar->to_string(rt, name));
    return nar->make_native(rt, data, NULL);
}

nar_object_t /* Window */ native__Video_setWindowPosition(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t x /* Int */,
        nar_object_t y /* Int  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_SetWindowPosition(win, (int) nar->to_int(rt, x), (int) nar->to_int(rt, y));
    return window;
}

nar_object_t /* (Int, Int) */ native__Video_getWindowPosition(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    int x, y;
    SDL_GetWindowPosition(win, &x, &y);
    return nar->make_tuple(rt, 2,
            (nar_object_t[]) {nar->make_int(rt, x), nar->make_int(rt, y)});
}

nar_object_t /* Window */ native__Video_setWindowSize(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t width /* Int */,
        nar_object_t height /* Int  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_SetWindowSize(win, (int) nar->to_int(rt, width), (int) nar->to_int(rt, height));
    return window;
}

nar_object_t /* (Int, Int) */ native__Video_getWindowSize(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    int w, h;
    SDL_GetWindowSize(win, &w, &h);
    return nar->make_tuple(rt, 2,
            (nar_object_t[]) {nar->make_int(rt, w), nar->make_int(rt, h)});
}

nar_object_t /* Result[String, (Int, Int, Int, Int)] */
native__Video_getWindowBordersSize(nar_runtime_t rt,
        nar_object_t window /* Window  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    int top, left, bottom, right;
    if (0 == SDL_GetWindowBordersSize(win, &top, &left, &bottom, &right)) {
        nar_object_t result = nar->make_tuple(rt, 4,
                (nar_object_t[]) {
                        nar->make_int(rt, top),
                        nar->make_int(rt, left),
                        nar->make_int(rt, bottom),
                        nar->make_int(rt, right)});
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* (Int, Int) */ native__Video_getWindowSizeInPixels(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    int w, h;
    SDL_GetWindowSize(win, &w, &h);
    return nar->make_tuple(rt, 2, (nar_object_t[]) {nar->make_int(rt, w), nar->make_int(rt, h)});
}

nar_object_t /* Window */ native__Video_setWindowMinimumSize(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t width /* Int */,
        nar_object_t height /* Int  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_SetWindowMinimumSize(win, (int) nar->to_int(rt, width), (int) nar->to_int(rt, height));
    return window;
}

nar_object_t /* (Int, Int) */ native__Video_getWindowMinimumSize(nar_runtime_t rt,
        nar_object_t window /* Window  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    int w, h;
    SDL_GetWindowMinimumSize(win, &w, &h);
    return nar->make_tuple(rt, 2, (nar_object_t[]) {nar->make_int(rt, w), nar->make_int(rt, h)});
}

nar_object_t /* Window */ native__Video_setWindowMaximumSize(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t width /* Int */,
        nar_object_t height /* Int  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_SetWindowMaximumSize(win, (int) nar->to_int(rt, width), (int) nar->to_int(rt, height));
    return window;
}

nar_object_t /* (Int, Int) */ native__Video_getWindowMaximumSize(nar_runtime_t rt,
        nar_object_t window /* Window  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    int w, h;
    SDL_GetWindowMaximumSize(win, &w, &h);
    return nar->make_tuple(rt, 2, (nar_object_t[]) {nar->make_int(rt, w), nar->make_int(rt, h)});
}

nar_object_t /* Window */ native__Video_setWindowBordered(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t bordered /* Bool  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_SetWindowBordered(win, nar->to_bool(rt, bordered));
    return window;
}

nar_object_t /* Window */ native__Video_setWindowResizable(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t resizable /* Bool  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_SetWindowResizable(win, nar->to_bool(rt, resizable));
    return window;
}

nar_object_t /* Window */ native__Video_setWindowAlwaysOnTop(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t alwaysOnTop /* Bool  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_SetWindowAlwaysOnTop(win, nar->to_bool(rt, alwaysOnTop));
    return window;
}

nar_object_t /* Window */ native__Video_showWindow(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_ShowWindow(win);
    return window;
}

nar_object_t /* Window */ native__Video_hideWindow(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_HideWindow(win);
    return window;
}

nar_object_t /* Window */ native__Video_raiseWindow(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_RaiseWindow(win);
    return window;
}

nar_object_t /* Window */ native__Video_maximizeWindow(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_MaximizeWindow(win);
    return window;
}

nar_object_t /* Window */ native__Video_minimizeWindow(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_MinimizeWindow(win);
    return window;
}

nar_object_t /* Window */ native__Video_restoreWindow(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_RestoreWindow(win);
    return window;
}

nar_object_t /* Result[String, Window] */ native__Video_setWindowFullscreen(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t mode /* FullscreenMode */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    if (0 == SDL_SetWindowFullscreen(win, nar->to_enum_option(rt, mode))) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &window);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Bool */ native__Video_hasWindowSurface(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    return nar->make_bool(rt, SDL_HasWindowSurface(win));
}

nar_object_t /* Result[String, Surface] */ native__Video_getWindowSurface(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_Surface *surf = SDL_GetWindowSurface(win);
    if (NULL != surf) {
        nar_object_t result = nar->make_native(rt, surf, NULL);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

void update_window_surface(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Window *win = nar->to_native(rt, payload).ptr;
    if (0 == SDL_UpdateWindowSurface(win)) {
        resolve(rt, payload, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Window] */ native__Video_updateWindowSurface(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, window, update_window_surface);
}

void update_window_surface_rects(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Window *win = nar->to_native(rt, data.values[0]).ptr;
    nar_list_t rect_objs = nar->to_list(rt, data.values[1]);
    SDL_Rect *rects = nar->alloc(rect_objs.size * sizeof(SDL_Rect));
    for (size_t i = 0; i < rect_objs.size; i++) {
        nar->map_record(rt, rect_objs.items[i], rects + i, (nar_map_record_cb_fn_t) map_Rect);
    }
    if (0 == SDL_UpdateWindowSurfaceRects(win, rects, (int) rect_objs.size)) {
        resolve(rt, payload, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
    nar->free(rects);
}

nar_object_t /* Task[String, Window] */ native__Video_updateWindowSurfaceRects(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t rects /* List[Rect]  */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {window, rects});
    return task_new(rt, data, update_window_surface_rects);
}

void destroy_window_surface(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Window *win = nar->to_native(rt, payload).ptr;
    SDL_DestroyWindow(win);
    resolve(rt, payload, task_state);
}

nar_object_t /* Task[String, Window] */ native__Video_destroyWindowSurface(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, window, destroy_window_surface);
}

nar_object_t /* Window */ native__Video_setWindowGrab(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t grabbed /* Bool */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_SetWindowGrab(win, nar->to_bool(rt, grabbed));
    return window;
}

nar_object_t /* Window */ native__Video_setWindowKeyboardGrab(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t grabbed /* Bool  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_SetWindowKeyboardGrab(win, nar->to_bool(rt, grabbed));
    return window;
}

nar_object_t /* Window */ native__Video_setWindowMouseGrab(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t grabbed /* Bool  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_SetWindowMouseGrab(win, nar->to_bool(rt, grabbed));
    return window;
}

nar_object_t /* Bool */ native__Video_getWindowGrab(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    return nar->make_bool(rt, SDL_GetWindowGrab(win));
}

nar_object_t /* Bool */ native__Video_getWindowKeyboardGrab(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    return nar->make_bool(rt, SDL_GetWindowKeyboardGrab(win));
}

nar_object_t /* Bool */ native__Video_getWindowMouseGrab(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    return nar->make_bool(rt, SDL_GetWindowMouseGrab(win));
}

void grabbed_window(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Window *win = SDL_GetGrabbedWindow();
    if (NULL != win) {
        nar_object_t result = nar->make_native(rt, win, NULL);
        nar_object_t just = nar->make_option(rt, Nar_Base_Maybe_Maybe__Just, 1, &result);
        resolve(rt, just, task_state);
    } else {
        resolve(rt, nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL), task_state);
    }
}

nar_object_t /* Task[Maybe[Window]] */ native__Video_grabbedWindow(nar_runtime_t rt) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, NAR_INVALID_OBJECT, grabbed_window);
}

nar_object_t /* Result[String, Window] */ native__Video_setWindowMouseRect(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t rect /* Rect  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_Rect r;
    nar->map_record(rt, rect, &r, (nar_map_record_cb_fn_t) map_Rect);
    if (0 == SDL_SetWindowHitTest(win, NULL, &r)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &window);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Window] */ native__Video_setWindowBrightness(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t brightness /* Float  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    if (0 == SDL_SetWindowBrightness(win, (float) nar->to_float(rt, brightness))) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &window);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Float */ native__Video_getWindowBrightness(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    return nar->make_float(rt, SDL_GetWindowBrightness(win));
}

nar_object_t /* Result[String, Window] */ native__Video_setWindowOpacity(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t opacity /* Float  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    if (0 == SDL_SetWindowOpacity(win, (float) nar->to_float(rt, opacity))) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &window);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Float] */ native__Video_getWindowOpacity(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    float opacity;
    if (0 == SDL_GetWindowOpacity(win, &opacity)) {
        nar_object_t result = nar->make_float(rt, opacity);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Window] */ native__Video_setWindowModalFor(nar_runtime_t rt,
        nar_object_t modalWindow /* Window */,
        nar_object_t parentWindow /* Window  */) {
    SDL_Window *modal = nar->to_native(rt, modalWindow).ptr;
    SDL_Window *parent = nar->to_native(rt, parentWindow).ptr;
    if (0 == SDL_SetWindowModalFor(modal, parent)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &modalWindow);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

void set_window_input_focus(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Window *win = nar->to_native(rt, payload).ptr;
    if (0 == SDL_SetWindowInputFocus(win)) {
        resolve(rt, payload, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Window] */ native__Video_setWindowInputFocus(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, window, set_window_input_focus);
}

nar_object_t /* Result[String, Window] */ native__Video_setWindowGammaRamp(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t red /* List[Int] */,
        nar_object_t green /* List[Int] */,
        nar_object_t blue /* List[Int]  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    nar_list_t red_list = nar->to_list(rt, red);
    nar_list_t green_list = nar->to_list(rt, green);
    nar_list_t blue_list = nar->to_list(rt, blue);

    Uint16 *r = nar->alloc(red_list.size * sizeof(Uint16));
    Uint16 *g = nar->alloc(green_list.size * sizeof(Uint16));
    Uint16 *b = nar->alloc(blue_list.size * sizeof(Uint16));

    for (size_t i = 0; i < red_list.size; i++) {
        r[i] = (Uint16) nar->to_int(rt, red_list.items[i]);
    }
    for (size_t i = 0; i < green_list.size; i++) {
        g[i] = (Uint16) nar->to_int(rt, green_list.items[i]);
    }
    for (size_t i = 0; i < blue_list.size; i++) {
        b[i] = (Uint16) nar->to_int(rt, blue_list.items[i]);
    }

    int status = SDL_SetWindowGammaRamp(win, r, g, b);
    nar->free(r);
    nar->free(g);
    nar->free(b);

    if (0 == status) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &window);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, (List[Int], List[Int], List[Int])] */
native__Video_getWindowGammaRamp(nar_runtime_t rt,
        nar_object_t window /* Window  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    Uint16 r[256], g[256], b[256];
    if (0 == SDL_GetWindowGammaRamp(win, r, g, b)) {
        nar_object_t red = nar->make_list(rt, 0, NULL);
        nar_object_t green = nar->make_list(rt, 0, NULL);
        nar_object_t blue = nar->make_list(rt, 0, NULL);
        for (int i = 255; i >= 0; i--) {
            red = nar->make_list_cons(rt, red, nar->make_int(rt, r[i]));
            green = nar->make_list_cons(rt, green, nar->make_int(rt, g[i]));
            blue = nar->make_list_cons(rt, blue, nar->make_int(rt, b[i]));
        }
        nar_object_t result = nar->make_tuple(rt, 3, (nar_object_t[]) {red, green, blue});
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

typedef struct {
    nar_runtime_t rt;
    nar_serialized_object_t callback;
} hit_test_data_t;

SDL_HitTestResult hit_test(SDL_Window *win, const SDL_Point *area, void *data) {
    hit_test_data_t *htd = data;
    nar_runtime_t rt = htd->rt;
    nar_object_t callback = nar->deserialize_object(rt, htd->callback);
    nar_object_t window = nar->make_native(rt, win, NULL);
    nar_object_t point = make_Point(rt, area);
    nar_object_t result = nar->apply_func(rt, callback, 2, (nar_object_t[]) {window, point});
    return nar->make_enum_option(rt, Nar_SDL_Video_HitTestResult, result, 0, NULL);
}

nar_object_t /* Result[String, Window] */ native__Video_setWindowHitTest(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t callback /* Maybe[(Window, Point): HitTestResult]  */) {
    nar_serialized_object_t prev_data = nar->get_metadata(rt,
            NAR_META__Nar_SDL_Video_windowHitTest_data);
    if (NULL != prev_data) {
        nar->free(prev_data);
    }

    SDL_Window *win = nar->to_native(rt, window).ptr;
    hit_test_data_t *data = nar->alloc(sizeof(hit_test_data_t));
    if (0 == SDL_SetWindowHitTest(win, hit_test, data)) {
        nar->set_metadata(rt, NAR_META__Nar_SDL_Video_windowHitTest_data, data);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &window);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        nar->free(data);
        nar->set_metadata(rt, NAR_META__Nar_SDL_Video_windowHitTest_data, NULL);
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

void flash_window(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Window *win = nar->to_native(rt, data.values[0]).ptr;
    SDL_FlashOperation op = nar->to_enum_option(rt, data.values[1]);
    if (0 == SDL_FlashWindow(win, op)) {
        resolve(rt, payload, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Window] */ native__Video_flashWindow(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t operation /* FlashOperation  */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {window, operation});
    return task_new(rt, data, flash_window);
}

void destroy_window(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Window *win = nar->to_native(rt, payload).ptr;
    SDL_DestroyWindow(win);
    resolve(rt, nar->make_unit(rt), task_state);
}

nar_object_t /* Task[Never, ()] */ native__Video_destroyWindow(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, window, destroy_window);
}

void is_screen_saver_enabled(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    resolve(rt, nar->make_bool(rt, SDL_IsScreenSaverEnabled()), task_state);
}

nar_object_t /* Task[Never, Bool] */ native__Video_isScreenSaverEnabled(nar_runtime_t rt) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, NAR_INVALID_OBJECT, is_screen_saver_enabled);
}

void enable_screen_saver(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_EnableScreenSaver();
    resolve(rt, nar->make_unit(rt), task_state);
}

nar_object_t /* Task[Never, ()] */ native__Video_enableScreenSaver(nar_runtime_t rt) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, NAR_INVALID_OBJECT, enable_screen_saver);
}

void disable_screen_saver(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_DisableScreenSaver();
    resolve(rt, nar->make_unit(rt), task_state);
}

nar_object_t /* Task[Never, ()] */ native__Video_disableScreenSaver(nar_runtime_t rt) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, NAR_INVALID_OBJECT, disable_screen_saver);
}

void gl_load_library(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    if (0 == SDL_GL_LoadLibrary(nar->to_string(rt, payload))) {
        resolve(rt, nar->make_unit(rt), task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, ()] */ native__Video_glLoadLibrary(nar_runtime_t rt,
        nar_object_t path /* String */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, path, gl_load_library);
}

nar_object_t /* Result[String, Ptr] */ native__Video_glGetProcAddress(nar_runtime_t rt,
        nar_object_t proc /* String */) {
    void *ptr = SDL_GL_GetProcAddress(nar->to_string(rt, proc));
    if (NULL != ptr) {
        nar_object_t result = nar->make_native(rt, ptr, NULL);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

void gl_unload_library(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_GL_UnloadLibrary();
    resolve(rt, nar->make_unit(rt), task_state);
}

nar_object_t /* Task[Never, ()] */ native__Video_glUnloadLibrary(nar_runtime_t rt) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, NAR_INVALID_OBJECT, gl_unload_library);
}

nar_object_t /* Bool */ native__Video_glExtensionSupported(nar_runtime_t rt,
        nar_object_t extension /* String */) {
    return nar->make_bool(rt, SDL_GL_ExtensionSupported(nar->to_string(rt, extension)));
}

void gl_reset_attributes(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_GL_ResetAttributes();
    resolve(rt, nar->make_unit(rt), task_state);
}

nar_object_t /* Task[Never, ()] */ native__Video_glResetAttributes(nar_runtime_t rt) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, NAR_INVALID_OBJECT, gl_reset_attributes);
}

void gl_set_attribute(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    Uint32 attr = nar->to_enum_option(rt, data.values[0]);
    int value = (int) nar->to_int(rt, data.values[1]);
    if (0 == SDL_GL_SetAttribute(attr, value)) {
        resolve(rt, nar->make_unit(rt), task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, ()] */ native__Video_glSetAttribute(nar_runtime_t rt,
        nar_object_t attr /* GLAttr */,
        nar_object_t value /* Int */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {attr, value});
    return task_new(rt, data, gl_set_attribute);
}

void gl_get_attribute(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_object_t attr = nar->to_tuple(rt, payload).values[0];
    Uint32 attr_enum = nar->to_enum_option(rt, attr);
    int value;
    if (0 == SDL_GL_GetAttribute(attr_enum, &value)) {
        resolve(rt, nar->make_int(rt, value), task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Int] */ native__Video_glGetAttribute(nar_runtime_t rt,
        nar_object_t attr /* GLAttr */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, attr, gl_get_attribute);
}

void create_gl_context(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Window *win = nar->to_native(rt, payload).ptr;
    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    if (NULL != ctx) {
        nar_object_t result = nar->make_native(rt, ctx, NULL);
        resolve(rt, result, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, GLContext] */ native__Video_glCreateContext(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, window, create_gl_context);
}

nar_object_t /* Result[String, Window] */ native__Video_glMakeCurrent(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t context /* GLContext  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    void *ctx = nar->to_native(rt, context).ptr;
    if (0 == SDL_GL_MakeCurrent(win, ctx)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &window);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

void gl_get_current_window(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Window *win = SDL_GL_GetCurrentWindow();
    if (NULL != win) {
        nar_object_t result = nar->make_native(rt, win, NULL);
        resolve(rt, result, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        resolve(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Window] */ native__Video_glGetCurrentWindow(nar_runtime_t rt) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, NAR_INVALID_OBJECT, gl_get_current_window);
}

void gl_get_current_context(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_GLContext ctx = SDL_GL_GetCurrentContext();
    if (NULL != ctx) {
        nar_object_t result = nar->make_native(rt, ctx, NULL);
        resolve(rt, result, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        resolve(rt, error, task_state);
    }
}

nar_object_t /* Task[String, GLContext] */ native__Video_glGetCurrentContext(nar_runtime_t rt) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, NAR_INVALID_OBJECT, gl_get_current_context);
}

nar_object_t /* (Int, Int) */ native__Video_glGetDrawableSize(nar_runtime_t rt,
        nar_object_t window /* Window  */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    int w, h;
    SDL_GL_GetDrawableSize(win, &w, &h);
    return nar->make_tuple(rt, 2, (nar_object_t[]) {nar->make_int(rt, w), nar->make_int(rt, h)});
}

void set_swap_interval(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    int interval = (int) nar->to_int(rt, payload);
    if (0 == SDL_GL_SetSwapInterval(interval)) {
        resolve(rt, nar->make_unit(rt), task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, ()] */ native__Video_glSetSwapInterval(nar_runtime_t rt,
        nar_object_t interval /* Int */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, interval, set_swap_interval);
}

void get_swap_interval(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    int interval = SDL_GL_GetSwapInterval();
    if (interval >= 0) {
        resolve(rt, nar->make_int(rt, interval), task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Int] */ native__Video_glGetSwapInterval(nar_runtime_t rt) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, NAR_INVALID_OBJECT, get_swap_interval);
}

void gl_swap_window(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Window *win = nar->to_native(rt, payload).ptr;
    SDL_GL_SwapWindow(win);
    resolve(rt, payload, task_state);
}

nar_object_t /* Task[String, Window] */ native__Video_glSwapWindow(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, window, gl_swap_window);
}

void gl_delete_context(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_GLContext ctx = nar->to_native(rt, payload).ptr;
    SDL_GL_DeleteContext(ctx);
    resolve(rt, nar->make_unit(rt), task_state);
}

nar_object_t /* Task[String, ()] */ native__Video_glDeleteContext(nar_runtime_t rt,
        nar_object_t context /* GLContext */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, context, gl_delete_context);
}

void init_video(nar_runtime_t rt) {
    nar_cstring_t module = "Nar.SDL.Video";
    nar->register_def(rt, module, "numVideoDrivers", &native__Video_numVideoDrivers, 0);
    nar->register_def(rt, module, "getVideoDriver", &native__Video_getVideoDriver, 1);
    nar->register_def(rt, module, "videoInit", &native__Video_videoInit, 1);
    nar->register_def(rt, module, "videoQuit", &native__Video_videoQuit, 0);
    nar->register_def(rt, module, "currentVideoDriver", &native__Video_currentVideoDriver, 0);
    nar->register_def(rt, module, "numVideoDisplays", &native__Video_numVideoDisplays, 0);
    nar->register_def(rt, module, "getDisplayName", &native__Video_getDisplayName, 1);
    nar->register_def(rt, module, "getDisplayBounds", &native__Video_getDisplayBounds, 1);
    nar->register_def(rt, module, "getDisplayUsableBounds",
            &native__Video_getDisplayUsableBounds, 1);
    nar->register_def(rt, module, "getDisplayDpi", &native__Video_getDisplayDpi, 1);
    nar->register_def(rt, module, "getDisplayOrientation", &native__Video_getDisplayOrientation, 1);
    nar->register_def(rt, module, "getNumDisplayModes", &native__Video_getNumDisplayModes, 1);
    nar->register_def(rt, module, "getDisplayMode", &native__Video_getDisplayMode, 2);
    nar->register_def(rt, module, "getCurrentDisplayMode", &native__Video_getCurrentDisplayMode, 1);
    nar->register_def(rt, module, "getClosestDisplayMode", &native__Video_getClosestDisplayMode, 2);
    nar->register_def(rt, module, "getPointDisplayIndex", &native__Video_getPointDisplayIndex, 1);
    nar->register_def(rt, module, "getRectDisplayIndex", &native__Video_getRectDisplayIndex, 1);
    nar->register_def(rt, module, "getWindowDisplayIndex", &native__Video_getWindowDisplayIndex, 1);
    nar->register_def(rt, module, "setWindowDisplayMode", &native__Video_setWindowDisplayMode, 2);
    nar->register_def(rt, module, "getWindowDisplayMode", &native__Video_getWindowDisplayMode, 1);
    nar->register_def(rt, module, "getWindowICCProfile", &native__Video_getWindowICCProfile, 1);
    nar->register_def(rt, module, "getWindowPixelFormat", &native__Video_getWindowPixelFormat, 1);
    nar->register_def(rt, module, "createWindow", &native__Video_createWindow, 6);
    nar->register_def(rt, module, "createWindowFrom", &native__Video_createWindowFrom, 1);
    nar->register_def(rt, module, "getWindowID", &native__Video_getWindowID, 1);
    nar->register_def(rt, module, "getWindowFromID", &native__Video_getWindowFromID, 1);
    nar->register_def(rt, module, "getWindowFlags", &native__Video_getWindowFlags, 1);
    nar->register_def(rt, module, "setWindowTitle", &native__Video_setWindowTitle, 2);
    nar->register_def(rt, module, "getWindowTitle", &native__Video_getWindowTitle, 1);
    nar->register_def(rt, module, "setWindowIcon", &native__Video_setWindowIcon, 2);
    nar->register_def(rt, module, "setWindowData", &native__Video_setWindowData, 3);
    nar->register_def(rt, module, "getWindowData", &native__Video_getWindowData, 2);
    nar->register_def(rt, module, "setWindowPosition", &native__Video_setWindowPosition, 3);
    nar->register_def(rt, module, "getWindowPosition", &native__Video_getWindowPosition, 1);
    nar->register_def(rt, module, "setWindowSize", &native__Video_setWindowSize, 3);
    nar->register_def(rt, module, "getWindowSize", &native__Video_getWindowSize, 1);
    nar->register_def(rt, module, "getWindowBordersSize", &native__Video_getWindowBordersSize, 1);
    nar->register_def(rt, module, "getWindowSizeInPixels", &native__Video_getWindowSizeInPixels, 1);
    nar->register_def(rt, module, "setWindowMinimumSize", &native__Video_setWindowMinimumSize, 3);
    nar->register_def(rt, module, "getWindowMinimumSize", &native__Video_getWindowMinimumSize, 1);
    nar->register_def(rt, module, "setWindowMaximumSize", &native__Video_setWindowMaximumSize, 3);
    nar->register_def(rt, module, "getWindowMaximumSize", &native__Video_getWindowMaximumSize, 1);
    nar->register_def(rt, module, "setWindowBordered", &native__Video_setWindowBordered, 2);
    nar->register_def(rt, module, "setWindowResizable", &native__Video_setWindowResizable, 2);
    nar->register_def(rt, module, "setWindowAlwaysOnTop", &native__Video_setWindowAlwaysOnTop, 2);
    nar->register_def(rt, module, "showWindow", &native__Video_showWindow, 1);
    nar->register_def(rt, module, "hideWindow", &native__Video_hideWindow, 1);
    nar->register_def(rt, module, "raiseWindow", &native__Video_raiseWindow, 1);
    nar->register_def(rt, module, "maximizeWindow", &native__Video_maximizeWindow, 1);
    nar->register_def(rt, module, "minimizeWindow", &native__Video_minimizeWindow, 1);
    nar->register_def(rt, module, "restoreWindow", &native__Video_restoreWindow, 1);
    nar->register_def(rt, module, "setWindowFullscreen", &native__Video_setWindowFullscreen, 2);
    nar->register_def(rt, module, "hasWindowSurface", &native__Video_hasWindowSurface, 1);
    nar->register_def(rt, module, "getWindowSurface", &native__Video_getWindowSurface, 1);
    nar->register_def(rt, module, "updateWindowSurface", &native__Video_updateWindowSurface, 1);
    nar->register_def(rt, module, "updateWindowSurfaceRects",
            &native__Video_updateWindowSurfaceRects, 2);
    nar->register_def(rt, module, "destroyWindowSurface", &native__Video_destroyWindowSurface, 1);
    nar->register_def(rt, module, "setWindowGrab", &native__Video_setWindowGrab, 2);
    nar->register_def(rt, module, "setWindowKeyboardGrab", &native__Video_setWindowKeyboardGrab, 2);
    nar->register_def(rt, module, "setWindowMouseGrab", &native__Video_setWindowMouseGrab, 2);
    nar->register_def(rt, module, "getWindowGrab", &native__Video_getWindowGrab, 1);
    nar->register_def(rt, module, "getWindowKeyboardGrab", &native__Video_getWindowKeyboardGrab, 1);
    nar->register_def(rt, module, "getWindowMouseGrab", &native__Video_getWindowMouseGrab, 1);
    nar->register_def(rt, module, "grabbedWindow", &native__Video_grabbedWindow, 0);
    nar->register_def(rt, module, "setWindowMouseRect", &native__Video_setWindowMouseRect, 2);
    nar->register_def(rt, module, "setWindowBrightness", &native__Video_setWindowBrightness, 2);
    nar->register_def(rt, module, "getWindowBrightness", &native__Video_getWindowBrightness, 1);
    nar->register_def(rt, module, "setWindowOpacity", &native__Video_setWindowOpacity, 2);
    nar->register_def(rt, module, "getWindowOpacity", &native__Video_getWindowOpacity, 1);
    nar->register_def(rt, module, "setWindowModalFor", &native__Video_setWindowModalFor, 2);
    nar->register_def(rt, module, "setWindowInputFocus", &native__Video_setWindowInputFocus, 1);
    nar->register_def(rt, module, "setWindowGammaRamp", &native__Video_setWindowGammaRamp, 4);
    nar->register_def(rt, module, "getWindowGammaRamp", &native__Video_getWindowGammaRamp, 1);
    nar->register_def(rt, module, "setWindowHitTest", &native__Video_setWindowHitTest, 2);
    nar->register_def(rt, module, "flashWindow", &native__Video_flashWindow, 2);
    nar->register_def(rt, module, "destroyWindow", &native__Video_destroyWindow, 1);
    nar->register_def(rt, module, "isScreenSaverEnabled", &native__Video_isScreenSaverEnabled, 0);
    nar->register_def(rt, module, "enableScreenSaver", &native__Video_enableScreenSaver, 0);
    nar->register_def(rt, module, "disableScreenSaver", &native__Video_disableScreenSaver, 0);
    nar->register_def(rt, module, "glLoadLibrary", &native__Video_glLoadLibrary, 1);
    nar->register_def(rt, module, "glGetProcAddress", &native__Video_glGetProcAddress, 1);
    nar->register_def(rt, module, "glUnloadLibrary", &native__Video_glUnloadLibrary, 0);
    nar->register_def(rt, module, "glExtensionSupported", &native__Video_glExtensionSupported, 1);
    nar->register_def(rt, module, "glResetAttributes", &native__Video_glResetAttributes, 0);
    nar->register_def(rt, module, "glSetAttribute", &native__Video_glSetAttribute, 2);
    nar->register_def(rt, module, "glGetAttribute", &native__Video_glGetAttribute, 1);
    nar->register_def(rt, module, "glCreateContext", &native__Video_glCreateContext, 1);
    nar->register_def(rt, module, "glMakeCurrent", &native__Video_glMakeCurrent, 2);
    nar->register_def(rt, module, "glGetCurrentWindow", &native__Video_glGetCurrentWindow, 0);
    nar->register_def(rt, module, "glGetCurrentContext", &native__Video_glGetCurrentContext, 0);
    nar->register_def(rt, module, "glGetDrawableSize", &native__Video_glGetDrawableSize, 1);
    nar->register_def(rt, module, "glSetSwapInterval", &native__Video_glSetSwapInterval, 1);
    nar->register_def(rt, module, "glGetSwapInterval", &native__Video_glGetSwapInterval, 0);
    nar->register_def(rt, module, "glSwapWindow", &native__Video_glSwapWindow, 1);
    nar->register_def(rt, module, "glDeleteContext", &native__Video_glDeleteContext, 1);

    nar->enum_def(Nar_SDL_Video_WindowPosition,
            Nar_SDL_Video_WindowPosition__WindowPositionUndefined, 0);
    nar->enum_def(Nar_SDL_Video_WindowPosition,
            Nar_SDL_Video_WindowPosition__WindowPositionCentered, 1);
    nar->enum_def(Nar_SDL_Video_WindowPosition, Nar_SDL_Video_WindowPosition__WindowPositionAt, 2);

    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagFullscreen,
            SDL_WINDOW_FULLSCREEN);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagOpengl,
            SDL_WINDOW_OPENGL);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagShown,
            SDL_WINDOW_SHOWN);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagHidden,
            SDL_WINDOW_HIDDEN);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagBorderless,
            SDL_WINDOW_BORDERLESS);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagResizable,
            SDL_WINDOW_RESIZABLE);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagMinimized,
            SDL_WINDOW_MINIMIZED);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagMaximized,
            SDL_WINDOW_MAXIMIZED);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagMouseGrabbed,
            SDL_WINDOW_MOUSE_GRABBED);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagInputFocus,
            SDL_WINDOW_INPUT_FOCUS);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagMouseFocus,
            SDL_WINDOW_MOUSE_FOCUS);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagFullscreenDesktop,
            SDL_WINDOW_FULLSCREEN_DESKTOP);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagForeign,
            SDL_WINDOW_FOREIGN);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagAllowHighdpi,
            SDL_WINDOW_ALLOW_HIGHDPI);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagMouseCapture,
            SDL_WINDOW_MOUSE_CAPTURE);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagAlwaysOnTop,
            SDL_WINDOW_ALWAYS_ON_TOP);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagSkipTaskbar,
            SDL_WINDOW_SKIP_TASKBAR);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagUtility,
            SDL_WINDOW_UTILITY);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagTooltip,
            SDL_WINDOW_TOOLTIP);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagPopupMenu,
            SDL_WINDOW_POPUP_MENU);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagKeyboardGrabbed,
            SDL_WINDOW_KEYBOARD_GRABBED);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagVulkan,
            SDL_WINDOW_VULKAN);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagMetal,
            SDL_WINDOW_METAL);
    nar->enum_def(Nar_SDL_Video_WindowFlag, Nar_SDL_Video_WindowFlag__WindowFlagInputGrabbed,
            SDL_WINDOW_INPUT_GRABBED);

    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventNone,
            SDL_WINDOWEVENT_NONE);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventShown,
            SDL_WINDOWEVENT_SHOWN);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventHidden,
            SDL_WINDOWEVENT_HIDDEN);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventExposed,
            SDL_WINDOWEVENT_EXPOSED);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventMoved,
            SDL_WINDOWEVENT_MOVED);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventResized,
            SDL_WINDOWEVENT_RESIZED);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventSizeChanged,
            SDL_WINDOWEVENT_SIZE_CHANGED);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventMinimized,
            SDL_WINDOWEVENT_MINIMIZED);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventMaximized,
            SDL_WINDOWEVENT_MAXIMIZED);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventRestored,
            SDL_WINDOWEVENT_RESTORED);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventEnter,
            SDL_WINDOWEVENT_ENTER);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventLeave,
            SDL_WINDOWEVENT_LEAVE);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventFocusGained,
            SDL_WINDOWEVENT_FOCUS_GAINED);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventFocusLost,
            SDL_WINDOWEVENT_FOCUS_LOST);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventClose,
            SDL_WINDOWEVENT_CLOSE);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventTakeFocus,
            SDL_WINDOWEVENT_TAKE_FOCUS);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventHitTest,
            SDL_WINDOWEVENT_HIT_TEST);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventIccProfChanged,
            SDL_WINDOWEVENT_ICCPROF_CHANGED);
    nar->enum_def(Nar_SDL_Video_WindowEvent, Nar_SDL_Video_WindowEvent__WindowEventDisplayChanged,
            SDL_WINDOWEVENT_DISPLAY_CHANGED);

    nar->enum_def(Nar_SDL_Video_DisplayEvent, Nar_SDL_Video_DisplayEvent__DisplayEventNone,
            SDL_DISPLAYEVENT_NONE);
    nar->enum_def(Nar_SDL_Video_DisplayEvent, Nar_SDL_Video_DisplayEvent__DisplayEventOrientation,
            SDL_DISPLAYEVENT_ORIENTATION);
    nar->enum_def(Nar_SDL_Video_DisplayEvent, Nar_SDL_Video_DisplayEvent__DisplayEventConnected,
            SDL_DISPLAYEVENT_CONNECTED);
    nar->enum_def(Nar_SDL_Video_DisplayEvent, Nar_SDL_Video_DisplayEvent__DisplayEventDisconnected,
            SDL_DISPLAYEVENT_DISCONNECTED);
    nar->enum_def(Nar_SDL_Video_DisplayEvent, Nar_SDL_Video_DisplayEvent__DisplayEventMoved,
            SDL_DISPLAYEVENT_MOVED);

    nar->enum_def(Nar_SDL_Video_Orientation, Nar_SDL_Video_Orientation__OrientationLandscape,
            SDL_ORIENTATION_LANDSCAPE);
    nar->enum_def(Nar_SDL_Video_Orientation, Nar_SDL_Video_Orientation__OrientationLandscapeFlipped,
            SDL_ORIENTATION_LANDSCAPE_FLIPPED);
    nar->enum_def(Nar_SDL_Video_Orientation, Nar_SDL_Video_Orientation__OrientationPortrait,
            SDL_ORIENTATION_PORTRAIT);
    nar->enum_def(Nar_SDL_Video_Orientation, Nar_SDL_Video_Orientation__OrientationPortraitFlipped,
            SDL_ORIENTATION_PORTRAIT_FLIPPED);

    nar->enum_def(Nar_SDL_Video_FlashOperation, Nar_SDL_Video_FlashOperation__FlashOperationCancel,
            SDL_FLASH_CANCEL);
    nar->enum_def(Nar_SDL_Video_FlashOperation, Nar_SDL_Video_FlashOperation__FlashOperationBriefly,
            SDL_FLASH_BRIEFLY);
    nar->enum_def(Nar_SDL_Video_FlashOperation,
            Nar_SDL_Video_FlashOperation__FlashOperationUntilFocused,
            SDL_FLASH_UNTIL_FOCUSED);

    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrRedSize,
            SDL_GL_RED_SIZE);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrGreenSize,
            SDL_GL_GREEN_SIZE);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrBlueSize,
            SDL_GL_BLUE_SIZE);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrAlphaSize,
            SDL_GL_ALPHA_SIZE);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrBufferSize,
            SDL_GL_BUFFER_SIZE);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrDoublebuffer,
            SDL_GL_DOUBLEBUFFER);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrDepthSize,
            SDL_GL_DEPTH_SIZE);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrStencilSize,
            SDL_GL_STENCIL_SIZE);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrAccumRedSize,
            SDL_GL_ACCUM_RED_SIZE);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrAccumGreenSize,
            SDL_GL_ACCUM_GREEN_SIZE);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrAccumBlueSize,
            SDL_GL_ACCUM_BLUE_SIZE);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrAccumAlphaSize,
            SDL_GL_ACCUM_ALPHA_SIZE);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrStereo,
            SDL_GL_STEREO);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrMultisamplebuffers,
            SDL_GL_MULTISAMPLEBUFFERS);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrMultisamplesamples,
            SDL_GL_MULTISAMPLESAMPLES);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrAcceleratedVisual,
            SDL_GL_ACCELERATED_VISUAL);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrRetainedBacking,
            SDL_GL_RETAINED_BACKING);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrContextMajorVersion,
            SDL_GL_CONTEXT_MAJOR_VERSION);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrContextMinorVersion,
            SDL_GL_CONTEXT_MINOR_VERSION);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrContextEgl,
            SDL_GL_CONTEXT_EGL);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrContextFlags,
            SDL_GL_CONTEXT_FLAGS);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrContextProfileMask,
            SDL_GL_CONTEXT_PROFILE_MASK);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrShareWithCurrentContext,
            SDL_GL_SHARE_WITH_CURRENT_CONTEXT);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrFramebufferSrgbCapable,
            SDL_GL_FRAMEBUFFER_SRGB_CAPABLE);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrContextReleaseBehavior,
            SDL_GL_CONTEXT_RELEASE_BEHAVIOR);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrContextResetNotification,
            SDL_GL_CONTEXT_RESET_NOTIFICATION);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrContextNoError,
            SDL_GL_CONTEXT_NO_ERROR);
    nar->enum_def(Nar_SDL_Video_GLAttr, Nar_SDL_Video_GLAttr__GLAttrFloatbuffers,
            SDL_GL_FLOATBUFFERS);

    nar->enum_def(Nar_SDL_Video_GLProfile, Nar_SDL_Video_GLProfile__GLProfileCore,
            SDL_GL_CONTEXT_PROFILE_CORE);
    nar->enum_def(Nar_SDL_Video_GLProfile, Nar_SDL_Video_GLProfile__GLProfileCompatibility,
            SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    nar->enum_def(Nar_SDL_Video_GLProfile, Nar_SDL_Video_GLProfile__GLProfileEs,
            SDL_GL_CONTEXT_PROFILE_ES);

    nar->enum_def(Nar_SDL_Video_GLContext, Nar_SDL_Video_GLContext__GLContextDebug,
            SDL_GL_CONTEXT_DEBUG_FLAG);
    nar->enum_def(Nar_SDL_Video_GLContext, Nar_SDL_Video_GLContext__GLContextForwardCompatible,
            SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    nar->enum_def(Nar_SDL_Video_GLContext, Nar_SDL_Video_GLContext__GLContextRobustAccess,
            SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG);
    nar->enum_def(Nar_SDL_Video_GLContext, Nar_SDL_Video_GLContext__GLContextResetIsolation,
            SDL_GL_CONTEXT_RESET_ISOLATION_FLAG);

    nar->enum_def(Nar_SDL_Video_GLContextReleaseBehavior,
            Nar_SDL_Video_GLContextReleaseBehavior__GLContextReleaseBehaviorNone,
            SDL_GL_CONTEXT_RELEASE_BEHAVIOR_NONE);
    nar->enum_def(Nar_SDL_Video_GLContextReleaseBehavior,
            Nar_SDL_Video_GLContextReleaseBehavior__GLContextReleaseBehaviorFlush,
            SDL_GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH);

    nar->enum_def(Nar_SDL_Video_GLContextResetNotification,
            Nar_SDL_Video_GLContextResetNotification__GLContextResetNotificationNoNotification,
            SDL_GL_CONTEXT_RESET_NO_NOTIFICATION);
    nar->enum_def(Nar_SDL_Video_GLContextResetNotification,
            Nar_SDL_Video_GLContextResetNotification__GLContextResetNotificationLoseContext,
            SDL_GL_CONTEXT_RESET_LOSE_CONTEXT);

    nar->enum_def(Nar_SDL_Video_HitTestResult, Nar_SDL_Video_HitTestResult__HitTestNormal,
            SDL_HITTEST_NORMAL);
    nar->enum_def(Nar_SDL_Video_HitTestResult, Nar_SDL_Video_HitTestResult__HitTestDraggable,
            SDL_HITTEST_DRAGGABLE);
    nar->enum_def(Nar_SDL_Video_HitTestResult, Nar_SDL_Video_HitTestResult__HitTestResizeTopLeft,
            SDL_HITTEST_RESIZE_TOPLEFT);
    nar->enum_def(Nar_SDL_Video_HitTestResult, Nar_SDL_Video_HitTestResult__HitTestResizeTop,
            SDL_HITTEST_RESIZE_TOP);
    nar->enum_def(Nar_SDL_Video_HitTestResult, Nar_SDL_Video_HitTestResult__HitTestResizeTopRight,
            SDL_HITTEST_RESIZE_TOPRIGHT);
    nar->enum_def(Nar_SDL_Video_HitTestResult, Nar_SDL_Video_HitTestResult__HitTestResizeRight,
            SDL_HITTEST_RESIZE_RIGHT);
    nar->enum_def(Nar_SDL_Video_HitTestResult,
            Nar_SDL_Video_HitTestResult__HitTestResizeBottomRight, SDL_HITTEST_RESIZE_BOTTOMRIGHT);
    nar->enum_def(Nar_SDL_Video_HitTestResult, Nar_SDL_Video_HitTestResult__HitTestResizeBottom,
            SDL_HITTEST_RESIZE_BOTTOM);
    nar->enum_def(Nar_SDL_Video_HitTestResult, Nar_SDL_Video_HitTestResult__HitTestResizeBottomLeft,
            SDL_HITTEST_RESIZE_BOTTOMLEFT);
    nar->enum_def(Nar_SDL_Video_HitTestResult, Nar_SDL_Video_HitTestResult__HitTestResizeLeft,
            SDL_HITTEST_RESIZE_LEFT);

    nar->enum_def(Nar_SDL_Video_FullscreenMode,
            Nar_SDL_Video_FullscreenMode__FullscreenModeWindowed, 0);
    nar->enum_def(Nar_SDL_Video_FullscreenMode,
            Nar_SDL_Video_FullscreenMode__FullscreenModeExclusive, SDL_WINDOW_FULLSCREEN);
    nar->enum_def(Nar_SDL_Video_FullscreenMode,
            Nar_SDL_Video_FullscreenMode__FullscreenModeBorderless, SDL_WINDOW_FULLSCREEN_DESKTOP);
}