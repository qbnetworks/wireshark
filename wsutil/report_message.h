/** @file
 * Declarations of routines for code that can run in GUI and command-line
 * environments to use to report errors and warnings to the user (e.g.,
 * I/O errors, or problems with preference settings) if the message should
 * be shown as a GUI error in a GUI environment.
 *
 * The application using libwsutil will register message-reporting
 * routines, and the routines declared here will call the registered
 * routines.  That way, these routines can be called by code that
 * doesn't itself know whether to pop up a dialog or print something
 * to the standard error.
 *
 * XXX - Should the capture file (_cfile_) routines be moved to libwiretap?
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __REPORT_MESSAGE_H__
#define __REPORT_MESSAGE_H__

#include <wireshark.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 *  Initialize the report message routines
 */
struct report_message_routines {
	void (*vreport_failure)(const char *, va_list);
	void (*vreport_warning)(const char *, va_list);
	void (*report_open_failure)(const char *, int, bool);
	void (*report_read_failure)(const char *, int);
	void (*report_write_failure)(const char *, int);
	void (*report_rename_failure)(const char *, const char *, int);
	void (*report_cfile_open_failure)(const char *, int, char *);
	void (*report_cfile_dump_open_failure)(const char *, int, char *, int);
	void (*report_cfile_read_failure)(const char *, int, char *);
	void (*report_cfile_write_failure)(const char *, const char *,
	    int, char *, uint64_t, int);
	void (*report_cfile_close_failure)(const char *, int, char *);
};

WS_DLL_PUBLIC void init_report_message(const char *friendly_program_name,
    const struct report_message_routines *routines);

/*
 * Report a general error.
 */
WS_DLL_PUBLIC void report_failure(const char *msg_format, ...) G_GNUC_PRINTF(1, 2);

/*
 * Report a general warning.
 */
WS_DLL_PUBLIC void report_warning(const char *msg_format, ...) G_GNUC_PRINTF(1, 2);

/*
 * Report an error when trying to open a file.
 * "err" is assumed to be an error code from Wiretap; positive values are
 * UNIX-style errnos, so this can be used for open failures not from
 * Wiretap as long as the failure code is just an errno.
 */
WS_DLL_PUBLIC void report_open_failure(const char *filename, int err,
    bool for_writing);

/*
 * Report an error when trying to read a file.
 * "err" is assumed to be a UNIX-style errno.
 */
WS_DLL_PUBLIC void report_read_failure(const char *filename, int err);

/*
 * Report an error when trying to write a file.
 * "err" is assumed to be a UNIX-style errno.
 */
WS_DLL_PUBLIC void report_write_failure(const char *filename, int err);

/*
 * Report an error when trying to rename a file.
 * "err" is assumed to be a UNIX-style errno.
 */
WS_DLL_PUBLIC void report_rename_failure(const char *old_filename,
    const char *new_filename, int err);

/*
 * Report an error from opening a capture file for reading.
 */
WS_DLL_PUBLIC void report_cfile_open_failure(const char *filename,
    int err, char *err_info);

/*
 * Report an error from opening a capture file for writing.
 */
WS_DLL_PUBLIC void report_cfile_dump_open_failure(const char *filename,
    int err, char *err_info, int file_type_subtype);

/*
 * Report an error from attempting to read from a capture file.
 */
WS_DLL_PUBLIC void report_cfile_read_failure(const char *filename,
    int err, char *err_info);

/*
 * Report an error from attempting to write to a capture file.
 */
WS_DLL_PUBLIC void report_cfile_write_failure(const char *in_filename,
    const char *out_filename, int err, char *err_info, uint64_t framenum,
    int file_type_subtype);

/*
 * Report an error from closing a capture file open for writing.
 */
WS_DLL_PUBLIC void report_cfile_close_failure(const char *filename,
    int err, char *err_info);

/*
 * Return the "friendly" program name.
 */
WS_DLL_PUBLIC const char *get_friendly_program_name(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __REPORT_MESSAGE_H__ */
