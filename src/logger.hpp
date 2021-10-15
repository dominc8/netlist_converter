#ifndef LOGGER_HPP
#define LOGGER_HPP

#ifdef DEBUG

#include <stdio.h>

/********************ANSI COLORS**************************\
 *
 * Text attributes
 * 0	All attributes off
 * 1	Bold on
 * 4	Underscore (on monochrome display adapter only)
 * 5	Blink on
 * 7	Reverse video on
 * 8	Concealed on
 *
 * Foreground colors
 * 30	Black
 * 31	Red
 * 32	Green
 * 33	Yellow
 * 34	Blue
 * 35	Magenta
 * 36	Cyan
 * 37	White
 *
 * Background colors
 * 40	Black
 * 41	Red
 * 42	Green
 * 43	Yellow
 * 44	Blue
 * 45	Magenta
 * 46	Cyan
 * 47	White
 *
\*********************************************************/

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#define LOG_INFO(format, ...)   printf(GRN format "\n" RESET, ##__VA_ARGS__)
#define LOG_WARN(format, ...)   printf(YEL format "\n" RESET, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)  printf(RED format "\n" RESET, ##__VA_ARGS__)

#else /* DEBUG */

#define LOG_INFO(...) do {} while(0)
#define LOG_WARN(...) do {} while(0)
#define LOG_ERROR(...)  do {} while(0)

#endif /* DEBUG */

#endif /* LOGGER_HPP */

