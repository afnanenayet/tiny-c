/**
 * \file print_utils.h
 * \brief Useful tools to assist with printing
 *
 * A set of functions and macros that make printing a little more convenient.
 */

#pragma once

/**
 * \brief Print a line to stdout
 *
 * Given some string, this macro prints a line (with a trailing newling
 * characters "\n") to STDOUT. This function assumes that the input is a
 * string
 *
 * \param x (`char*`) A proper null-terminated string
 */
#define println(x)                                                             \
    if (x) {                                                                   \
        printf("%s\n", x);                                                     \
    }

/**
 * \brief Print a line to some file handler
 *
 * Given some string and a file, this macro prints a string to the file,
 * suffixed by a newline (`\n`) character.
 *
 * \param x (`FILE`) The file to print to
 * \param y (`char *`) The string to print. This _must_ be a string.
 */
#define fprintln(x, y)                                                         \
    if (x && y) {                                                              \
        fprintf(x, "%s\n", y);                                                 \
    }
