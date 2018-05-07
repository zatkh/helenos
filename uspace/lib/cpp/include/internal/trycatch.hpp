/*
 * Copyright (c) 2018 Jaroslav Jindrak
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBCPP_INTERNAL_TRYCATCH
#define LIBCPP_INTERNAL_TRYCATCH

// TODO: This header should be included in every libcpp header.

/**
 * For the time that exception support is not present
 * in HelenOS, we use mock macros in place of the keywords
 * try, throw and catch that allow us to atleast partially
 * mimic exception functionality (that is, without propagation
 * and stack unwinding).
 * The value of the macro below determines if the keyword
 * hiding macros get defined.
 */
#define LIBCPP_EXCEPTIONS_SUPPORTED 0

#if LIBCPP_EXCEPTIONS_SUPPORTED == 0

namespace std
{
    /**
     * In case the file where our macros get expanded
     * does not include cstdlib.
     */
    extern "C" void abort(void) __attribute__((noreturn));

    namespace aux
    {
        /**
         * Monitors the state of the program with
         * regards to exceptions.
         */
        extern bool exception_thrown;

        /**
         * These two variables can be used to suppress
         * the execution of eithe the try blocks
         * or the catch blocks.
         */
        inline constexpr bool try_blocks_allowed{true};
        inline constexpr bool catch_blocks_allowed{false};
    }
}

/**
 * These macros allow us to choose how the program
 * should behave when an exception is thrown
 * (LIBCPP_EXCEPTION_HANDLE_THROW) or caught
 * (LIBCPP_EXCEPTION_HANDLE_CATCH).
 * We also provide three handlers that either
 * hang the program (allowing us to read the
 * message), exit the program (allowing us to
 * redirect the message to some output file and
 * end) or ignore the throw (in which case the
 * state of program will be broken, but since
 * we output messages on both and catch, this option
 * might allow us to see which catch statement
 * catches the "thrown exception" (supposing
 * the program doesn't crash before reaching
 * that statement.
 */
#define LIBCPP_EXCEPTION_HANG         while (true);
#define LIBCPP_EXCEPTION_ABORT        ::std::abort();
#define LIBCPP_EXCEPTION_IGNORE       /* IGNORE */
#define LIBCPP_EXCEPTION_HANDLE_THROW LIBCPP_EXCEPTION_IGNORE
#define LIBCPP_EXCEPTION_HANDLE_CATCH LIBCPP_EXCEPTION_IGNORE

#define try if constexpr (::std::aux::try_blocks_allowed)

/**
 * Since we cannot use the thrown object in this macro,
 * we have to silence compiler warnings on it by
 * using (void)thrown-object.
 */
#define throw \
    do {\
        ::std::aux::exception_thrown = true; \
        printf("[EXCEPTION] Thrown at %s:%d\n", __FILE__, __LINE__); \
        LIBCPP_EXCEPTION_HANDLE_THROW \
    } while (false); (void)

#define catch(expr) \
    if (::std::aux::exception_thrown) \
    { \
        printf("[EXCEPTION] Caught < "#expr" > at %s:%d\n", __FILE__, __LINE__); \
        LIBCPP_EXCEPTION_HANDLE_CATCH \
    } \
    if constexpr (expr = {}; ::std::aux::catch_blocks_allowed)

/**
 * This macro can be used for testing the library. If
 * exception handling is not available, it uses the
 * internal bool variable and if it is, it uses a
 * universal catch clause in which it sets the passed
 * checking variable to true.
 */
#define LIBCPP_EXCEPTION_THROW_CHECK(variable) \
    variable = ::std::aux::exception_thrown

#else
#define LIBCPP_EXCEPTION_THROW_CHECK(variable) \
    catch (...) \
    { \
        variable = true; \
    }
#endif

#endif
