#pragma once

#include <stddef.h>
#include <stdint.h>

#define MAX_BUF_SIZE        128     /* Maximum size of CLI Rx buffer */

typedef enum
{
    CLI_OK,                 /* API execution successful.                */
    CLI_E_NULL_PTR,         /* Null pointer error.                      */
    CLI_E_IO,
    CLI_E_CMD_NOT_FOUND,    /* Command name not found in command table. */
    CLI_E_INVALID_ARGS,     /* Invalid function parameters/arguments.   */
    CLI_E_BUF_FULL          /* CLI buffer full.                         */
} cli_status_t;

/*!
 * @brief Function type declarations.
 */
typedef void (*cmd_func_ptr_t)(int argc, char **argv);
typedef void (*println_func_ptr_t)(char *string);

/*!
 * @brief Command structure, consisting of a name and function pointer.
 */
typedef struct {
    char *cmd;           /* Command name.                            */
    cmd_func_ptr_t func; /* Function pointer to associated function. */
} cmd_t;

/*!
 * @brief Command-line interface handle structure.
 */
typedef struct {
    println_func_ptr_t println; /* Function pointer to user defined println function.      */
    cmd_t *cmd_tbl;             /* Pointer to series of commands which are to be accepted. */
    size_t cmd_cnt;             /* Number of commands in cmd_tbl.                          */
    uint8_t looping;
    uint8_t loop_end;
    uint32_t loop_ms;
    uint32_t loop_snap;
    cmd_func_ptr_t loop_fn;
    uint8_t argc;
    char *argv[32];
} cli_t;
