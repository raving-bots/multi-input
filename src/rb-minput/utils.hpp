// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <string>

#define RB_COPY_CTOR(T, state) \
    T(const T&) = state; \
    T& operator=(const T&) = state

#define RB_MOVE_CTOR(T, state) \
    T(T&&) = state; \
    T& operator=(T&&) = state

#define RB_COPYABLE(T) \
    RB_COPY_CTOR(T, default); \
    RB_MOVE_CTOR(T, default)

#define RB_NON_COPYABLE(T) \
    RB_COPY_CTOR(T, delete)

#define RB_MOVEABLE(T) \
    RB_COPY_CTOR(T, delete); \
    RB_MOVE_CTOR(T, default)

#define RB_NON_MOVEABLE(T) \
    RB_COPY_CTOR(T, delete); \
    RB_MOVE_CTOR(T, delete)

// this is is needed so that we will stringise the expanded
// macro, e.g. if you have #define FOO expanded, RB_STRINGIZE2(FOO)
// yields "FOO" but RB_STRINGIZE(FOO) yields "expanded"
#define RB_STRINGIZE2(x) #x
#define RB_STRINGIZE(x) RB_STRINGIZE2(x)

#ifdef _MSC_VER
#   define RB_FUNCTION_NAME __FUNCSIG__
#else
#   define RB_FUNCTION_NAME __PRETTY_FUNCTION__
#endif

// RB_TRACE_ENTER() needs to be called at the start of every function
// that uses tracing, otherwise RB_TRACE(...) will not compile
#ifdef RB_ENABLE_TRACING
#   define RB_TRACE_OBJECT _internal_tracer_
#   define RB_TRACE_ENTER() ::multi_input::debug::tracer RB_TRACE_OBJECT{RB_FUNCTION_NAME, __FILE__, RB_STRINGIZE(__LINE__)}
#   define RB_TRACE(msg) RB_TRACE_OBJECT.output(msg)
#else
#   define RB_TRACE_ENTER()
#   define RB_TRACE(msg)
#endif

namespace multi_input {
    namespace debug {
        struct tracer {
            RB_NON_MOVEABLE(tracer);

            tracer(const char *function, const char *file, const char *line)
                : m_function(function), m_file(file), m_line(line) {
                    enter();
                    output("entering");
            }

            ~tracer() {
                output("exiting");
                leave();
            }

            void output(const char*);
            void enter();
            void leave();
        private:
            const char* m_function;
            const char* m_file;
            const char* m_line;
        };
    }
}
