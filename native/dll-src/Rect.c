#include "_package.h"
#include <Nar.Base.h>

void map_Point(nar_runtime_t rt, nar_cstring_t key, nar_object_t value, SDL_Point *point) {
    if (0 == strcmp(key, "x")) {
        point->x = (int) nar->to_int(rt, value);
    } else if (0 == strcmp(key, "y")) {
        point->y = (int) nar->to_int(rt, value);
    }
}

nar_object_t make_Point(nar_runtime_t rt, const SDL_Point *point) {
    return nar->make_record(rt, 2,
            (nar_cstring_t[]) {"x", "y"},
            (nar_object_t[]) {nar->make_int(rt, point->x), nar->make_int(rt, point->y)});
}

void map_FPoint(nar_runtime_t rt, nar_cstring_t key, nar_object_t value, SDL_FPoint *point) {
    if (0 == strcmp(key, "x")) {
        point->x = (float) nar->to_float(rt, value);
    } else if (0 == strcmp(key, "y")) {
        point->y = (float) nar->to_float(rt, value);
    }
}

nar_object_t make_FPoint(nar_runtime_t rt, const SDL_FPoint *point) {
    return nar->make_record(rt, 2,
            (nar_cstring_t[]) {"x", "y"},
            (nar_object_t[]) {nar->make_float(rt, point->x), nar->make_float(rt, point->y)});
}

void map_Rect(nar_runtime_t rt, nar_cstring_t key, nar_object_t value, SDL_Rect *rect) {
    if (0 == strcmp(key, "x")) {
        rect->x = (int) nar->to_int(rt, value);
    } else if (0 == strcmp(key, "y")) {
        rect->y = (int) nar->to_int(rt, value);
    } else if (0 == strcmp(key, "w")) {
        rect->w = (int) nar->to_int(rt, value);
    } else if (0 == strcmp(key, "h")) {
        rect->h = (int) nar->to_int(rt, value);
    }
}

nar_object_t make_Rect(nar_runtime_t rt, const SDL_Rect *rect) {
    return nar->make_record(rt, 4,
            (nar_cstring_t[]) {"x", "y", "w", "h"},
            (nar_object_t[]) {
                    nar->make_int(rt, rect->x),
                    nar->make_int(rt, rect->y),
                    nar->make_int(rt, rect->w),
                    nar->make_int(rt, rect->h)
            });
}

void map_FRect(nar_runtime_t rt, nar_cstring_t key, nar_object_t value, SDL_FRect *rect) {
    if (0 == strcmp(key, "x")) {
        rect->x = (float) nar->to_float(rt, value);
    } else if (0 == strcmp(key, "y")) {
        rect->y = (float) nar->to_float(rt, value);
    } else if (0 == strcmp(key, "w")) {
        rect->w = (float) nar->to_float(rt, value);
    } else if (0 == strcmp(key, "h")) {
        rect->h = (float) nar->to_float(rt, value);
    }
}

nar_object_t make_FRect(nar_runtime_t rt, const SDL_FRect *rect) {
    return nar->make_record(rt, 4,
            (nar_cstring_t[]) {"x", "y", "w", "h"},
            (nar_object_t[]) {
                    nar->make_float(rt, rect->x),
                    nar->make_float(rt, rect->y),
                    nar->make_float(rt, rect->w),
                    nar->make_float(rt, rect->h)
            });
}

nar_object_t /* Bool */ native__Rect_pointInRect(nar_runtime_t rt,
        nar_object_t p /* Point */,
        nar_object_t r /* Rect */) {
    SDL_Point point;
    nar->map_record(rt, p, &point, (nar_map_record_cb_fn_t) &map_Point);
    SDL_Rect rect;
    nar->map_record(rt, r, &rect, (nar_map_record_cb_fn_t) &map_Rect);
    return nar->make_bool(rt, SDL_PointInRect(&point, &rect));
}

nar_object_t /* Bool */ native__Rect_rectEmpty(nar_runtime_t rt,
        nar_object_t r /* Rect */) {
    SDL_Rect rect;
    nar->map_record(rt, r, &rect, (nar_map_record_cb_fn_t) &map_Rect);
    return nar->make_bool(rt, SDL_RectEmpty(&rect));
}

nar_object_t /* Bool */ native__Rect_rectEquals(nar_runtime_t rt,
        nar_object_t a /* Rect */,
        nar_object_t b /* Rect */) {
    SDL_Rect rectA;
    nar->map_record(rt, a, &rectA, (nar_map_record_cb_fn_t) &map_Rect);
    SDL_Rect rectB;
    nar->map_record(rt, b, &rectB, (nar_map_record_cb_fn_t) &map_Rect);
    return nar->make_bool(rt, SDL_RectEquals(&rectA, &rectB));
}

nar_object_t /* Bool */ native__Rect_hasIntersection(nar_runtime_t rt,
        nar_object_t a /* Rect */,
        nar_object_t b /* Rect */) {
    SDL_Rect rectA;
    nar->map_record(rt, a, &rectA, (nar_map_record_cb_fn_t) &map_Rect);
    SDL_Rect rectB;
    nar->map_record(rt, b, &rectB, (nar_map_record_cb_fn_t) &map_Rect);
    return nar->make_bool(rt, SDL_HasIntersection(&rectA, &rectB));
}

nar_object_t /* Maybe[Rect] */ native__Rect_intersectRect(nar_runtime_t rt,
        nar_object_t a /* Rect */,
        nar_object_t b /* Rect */) {
    SDL_Rect rectA;
    nar->map_record(rt, a, &rectA, (nar_map_record_cb_fn_t) &map_Rect);
    SDL_Rect rectB;
    nar->map_record(rt, b, &rectB, (nar_map_record_cb_fn_t) &map_Rect);
    SDL_Rect result;
    if (SDL_IntersectRect(&rectA, &rectB, &result)) {
        nar_object_t fields[] = {
                nar->make_int(rt, result.x),
                nar->make_int(rt, result.y),
                nar->make_int(rt, result.w),
                nar->make_int(rt, result.h)
        };
        nar_object_t value = make_Rect(rt, &result);
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Just, 1, &value);
    } else {
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL);
    }
}

nar_object_t /* Rect */ native__Rect_unionRect(nar_runtime_t rt,
        nar_object_t a /* Rect */,
        nar_object_t b /* Rect */) {
    SDL_Rect rectA;
    nar->map_record(rt, a, &rectA, (nar_map_record_cb_fn_t) &map_Rect);
    SDL_Rect rectB;
    nar->map_record(rt, b, &rectB, (nar_map_record_cb_fn_t) &map_Rect);
    SDL_Rect result;
    SDL_UnionRect(&rectA, &rectB, &result);
    return make_Rect(rt, &result);
}

nar_object_t /* Rect */ native__Rect_enclosePoints(nar_runtime_t rt,
        nar_object_t points /* List[Point] */,
        nar_object_t clip /* Maybe[Rect] */) {
    nar_list_t list = nar->to_list(rt, points);
    SDL_Point *pointArray = nar->alloc(sizeof(SDL_Point) * list.size);
    for (nar_size_t i = 0; i < list.size; i++) {
        nar_object_t point = list.items[i];
        SDL_Point *pointPtr = pointArray + i;
        nar->map_record(rt, point, pointPtr, (nar_map_record_cb_fn_t) &map_Point);
    }
    SDL_Rect clipRect;
    nar->map_record(rt, clip, &clipRect, (nar_map_record_cb_fn_t) &map_Rect);
    SDL_Rect result;
    SDL_EnclosePoints(pointArray, (int) list.size, &clipRect, &result);
    nar->free(pointArray);
    return make_Rect(rt, &result);
}

nar_object_t /* Maybe[(Point, Point)] */ native__Rect_intersectRectAndLine(nar_runtime_t rt,
        nar_object_t rect /* Rect */,
        nar_object_t x1 /* Int */,
        nar_object_t y1 /* Int */,
        nar_object_t x2 /* Int */,
        nar_object_t y2 /* Int */ ) {
    SDL_Rect sdlRect;
    nar->map_record(rt, rect, &sdlRect, (nar_map_record_cb_fn_t) &map_Rect);
    int px1 = (int) nar->to_int(rt, x1);
    int py1 = (int) nar->to_int(rt, y1);
    int px2 = (int) nar->to_int(rt, x2);
    int py2 = (int) nar->to_int(rt, y2);
    if (nar->make_bool(rt, SDL_IntersectRectAndLine(&sdlRect, &px1, &py1, &px2, &py2))) {
        return nar->make_option(
                rt, Nar_Base_Maybe_Maybe__Just, 1, (nar_object_t[]) {
                        nar->make_tuple(
                                rt, 2, (nar_object_t[]) {
                                        nar->make_record(
                                                rt, 2, (nar_cstring_t[]) {"x", "y"},
                                                (nar_object_t[]) {
                                                        nar->make_int(rt, px1),
                                                        nar->make_int(rt, py1)
                                                }),
                                        nar->make_record(
                                                rt, 2, (nar_cstring_t[]) {"x", "y"},
                                                (nar_object_t[]) {
                                                        nar->make_int(rt, px2),
                                                        nar->make_int(rt, py2)
                                                })
                                })
                });
    } else {
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL);
    }
}

nar_object_t /* Bool */ native__Rect_pointInFRect(nar_runtime_t rt,
        nar_object_t p /* FPoint */,
        nar_object_t r /* FRect */) {
    SDL_FPoint point;
    nar->map_record(rt, p, &point, (nar_map_record_cb_fn_t) &map_FPoint);
    SDL_FRect rect;
    nar->map_record(rt, r, &rect, (nar_map_record_cb_fn_t) &map_FRect);
    return nar->make_bool(rt, SDL_PointInFRect(&point, &rect));
}

nar_object_t /* Bool */ native__Rect_fRectEmpty(nar_runtime_t rt,
        nar_object_t r /* FRect */) {
    SDL_FRect rect;
    nar->map_record(rt, r, &rect, (nar_map_record_cb_fn_t) &map_FRect);
    return nar->make_bool(rt, SDL_FRectEmpty(&rect));
}

nar_object_t /* Bool */ native__Rect_fRectEquals(nar_runtime_t rt,
        nar_object_t a /* FRect */,
        nar_object_t b /* FRect */) {
    SDL_FRect rectA;
    nar->map_record(rt, a, &rectA, (nar_map_record_cb_fn_t) &map_FRect);
    SDL_FRect rectB;
    nar->map_record(rt, b, &rectB, (nar_map_record_cb_fn_t) &map_FRect);
    return nar->make_bool(rt, SDL_FRectEquals(&rectA, &rectB));
}

nar_object_t /* Bool */ native__Rect_hasIntersectionF(nar_runtime_t rt,
        nar_object_t a /* FRect */,
        nar_object_t b /* FRect */) {
    SDL_FRect rectA;
    nar->map_record(rt, a, &rectA, (nar_map_record_cb_fn_t) &map_FRect);
    SDL_FRect rectB;
    nar->map_record(rt, b, &rectB, (nar_map_record_cb_fn_t) &map_FRect);
    return nar->make_bool(rt, SDL_HasIntersectionF(&rectA, &rectB));
}

nar_object_t /* Maybe[FRect] */ native__Rect_intersectFRect(nar_runtime_t rt,
        nar_object_t a /* FRect */,
        nar_object_t b /* FRect */) {
    SDL_FRect rectA;
    nar->map_record(rt, a, &rectA, (nar_map_record_cb_fn_t) &map_FRect);
    SDL_FRect rectB;
    nar->map_record(rt, b, &rectB, (nar_map_record_cb_fn_t) &map_FRect);
    SDL_FRect result;
    if (SDL_IntersectFRect(&rectA, &rectB, &result)) {
        nar_object_t value = make_FRect(rt, &result);
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Just, 1, &value);
    } else {
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL);
    }
}

nar_object_t /* FRect */ native__Rect_unionFRect(nar_runtime_t rt,
        nar_object_t a /* FRect */,
        nar_object_t b /* FRect */) {
    SDL_FRect rectA;
    nar->map_record(rt, a, &rectA, (nar_map_record_cb_fn_t) &map_FRect);
    SDL_FRect rectB;
    nar->map_record(rt, b, &rectB, (nar_map_record_cb_fn_t) &map_FRect);
    SDL_FRect result;
    SDL_UnionFRect(&rectA, &rectB, &result);
    return make_FRect(rt, &result);
}

nar_object_t /* FRect */ native__Rect_encloseFPoints(nar_runtime_t rt,
        nar_object_t points /* List[FPoint] */,
        nar_object_t clip /* Maybe[FRect] */) {
    nar_list_t list = nar->to_list(rt, points);
    SDL_FPoint *pointArray = nar->alloc(sizeof(SDL_FPoint) * list.size);
    for (nar_size_t i = 0; i < list.size; i++) {
        nar_object_t point = list.items[i];
        SDL_FPoint *pointPtr = pointArray + i;
        nar->map_record(rt, point, pointPtr, (nar_map_record_cb_fn_t) &map_FPoint);
    }
    SDL_FRect clipRect;
    nar->map_record(rt, clip, &clipRect, (nar_map_record_cb_fn_t) &map_FRect);
    SDL_FRect result;
    SDL_EncloseFPoints(pointArray, (int) list.size, &clipRect, &result);
    nar->free(pointArray);
    make_FRect(rt, &result);
}

nar_object_t /* Maybe[(FPoint, FPoint)] */ native__Rect_intersectFRectAndLine(nar_runtime_t rt,
        nar_object_t rect /* FRect */,
        nar_object_t x1 /* Float */,
        nar_object_t y1 /* Float */,
        nar_object_t x2 /* Float */,
        nar_object_t y2 /* Float */ ) {
    SDL_FRect sdlRect;
    nar->map_record(rt, rect, &sdlRect, (nar_map_record_cb_fn_t) &map_FRect);
    float px1 = (float) nar->to_float(rt, x1);
    float py1 = (float) nar->to_float(rt, y1);
    float px2 = (float) nar->to_float(rt, x2);
    float py2 = (float) nar->to_float(rt, y2);
    if (nar->make_bool(rt, SDL_IntersectFRectAndLine(&sdlRect, &px1, &py1, &px2, &py2))) {
        return nar->make_option(
                rt, Nar_Base_Maybe_Maybe__Just, 1, (nar_object_t[]) {
                        nar->make_tuple(
                                rt, 2, (nar_object_t[]) {
                                        nar->make_record(
                                                rt, 2, (nar_cstring_t[]) {"x", "y"},
                                                (nar_object_t[]) {
                                                        nar->make_float(rt, px1),
                                                        nar->make_float(rt, py1)
                                                }),
                                        nar->make_record(
                                                rt, 2, (nar_cstring_t[]) {"x", "y"},
                                                (nar_object_t[]) {
                                                        nar->make_float(rt, px2),
                                                        nar->make_float(rt, py2)
                                                })
                                })
                });
    } else {
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL);
    }
}

void init_rect(nar_runtime_t rt) {
    nar->register_def(rt, "Nar.SDL.Rect", "pointInRect",
            &native__Rect_pointInRect, 2);
    nar->register_def(rt, "Nar.SDL.Rect", "rectEmpty",
            &native__Rect_rectEmpty, 1);
    nar->register_def(rt, "Nar.SDL.Rect", "rectEquals",
            &native__Rect_rectEquals, 2);
    nar->register_def(rt, "Nar.SDL.Rect", "hasIntersection",
            &native__Rect_hasIntersection, 2);
    nar->register_def(rt, "Nar.SDL.Rect", "intersectRect",
            &native__Rect_intersectRect, 2);
    nar->register_def(rt, "Nar.SDL.Rect", "unionRect",
            &native__Rect_unionRect, 2);
    nar->register_def(rt, "Nar.SDL.Rect", "enclosePoints",
            &native__Rect_enclosePoints, 2);
    nar->register_def(rt, "Nar.SDL.Rect", "intersectRectAndLine",
            &native__Rect_intersectRectAndLine, 5);
    nar->register_def(rt, "Nar.SDL.Rect", "pointInFRect",
            &native__Rect_pointInFRect, 2);
    nar->register_def(rt, "Nar.SDL.Rect", "fRectEmpty",
            &native__Rect_fRectEmpty, 1);
    nar->register_def(rt, "Nar.SDL.Rect", "fRectEquals",
            &native__Rect_fRectEquals, 2);
    nar->register_def(rt, "Nar.SDL.Rect", "hasIntersectionF",
            &native__Rect_hasIntersectionF, 2);
    nar->register_def(rt, "Nar.SDL.Rect", "intersectFRect",
            &native__Rect_intersectFRect, 2);
    nar->register_def(rt, "Nar.SDL.Rect", "unionFRect",
            &native__Rect_unionFRect, 2);
    nar->register_def(rt, "Nar.SDL.Rect", "encloseFPoints",
            &native__Rect_encloseFPoints, 2);
    nar->register_def(rt, "Nar.SDL.Rect", "intersectFRectAndLine",
            &native__Rect_intersectFRectAndLine, 5);
}
