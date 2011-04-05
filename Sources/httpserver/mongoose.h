// Copyright (c) 2004-2010 Sergey Lyubka
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#if defined(_WIN32)
#define _MY_DUMMY // Disable deprecation warning in VS2005
#else
#define _XOPEN_SOURCE 600 // For flockfile() on Linux
#define _LARGEFILE_SOURCE // Enable 64-bit file offsets
#endif

#ifndef _WIN32_WCE // Some ANSI #includes are not available on Windows CE
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#endif // !_WIN32_WCE

#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>

#if defined(_WIN32)  // Windows specific #includes and #defines
#define _WIN32_WINNT 0x0400 // To make it link in VS2005
#include <windows.h>
#include <winsock.h>

#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif

#ifndef _WIN32_WCE
#include <process.h>
#include <direct.h>
#include <io.h>
#else // _WIN32_WCE
#include <winsock2.h>
#define NO_CGI // WinCE has no pipes

typedef long off_t;
#define BUFSIZ  4096

#define errno   GetLastError()
#define strerror(x)  _ultoa(x, (char *) _alloca(sizeof(x) *3 ), 10)
#endif // _WIN32_WCE

#define MAKEUQUAD(lo, hi) ((uint64_t)(((uint32_t)(lo)) | \
      ((uint64_t)((uint32_t)(hi))) << 32))
#define RATE_DIFF 10000000 // 100 nsecs
#define EPOCH_DIFF MAKEUQUAD(0xd53e8000, 0x019db1de)
#define SYS2UNIX_TIME(lo, hi) \
  (time_t) ((MAKEUQUAD((lo), (hi)) - EPOCH_DIFF) / RATE_DIFF)

// Visual Studio 6 does not know __func__ or __FUNCTION__
// The rest of MS compilers use __FUNCTION__, not C99 __func__
// Also use _strtoui64 on modern M$ compilers
#if defined(_MSC_VER) && _MSC_VER < 1300
#define STRX(x) #x
#define STR(x) STRX(x)
#define __func__ "line " STR(__LINE__)
#define strtoull(x, y, z) strtoul(x, y, z)
#define strtoll(x, y, z) strtol(x, y, z)
#else
#define __func__  __FUNCTION__
#define strtoull(x, y, z) _strtoui64(x, y, z)
#define strtoll(x, y, z) _strtoi64(x, y, z)
#endif // _MSC_VER

#define ERRNO   GetLastError()
#define NO_SOCKLEN_T
#define SSL_LIB   "ssleay32.dll"
#define CRYPTO_LIB  "libeay32.dll"
#define DIRSEP '\\'
#define IS_DIRSEP_CHAR(c) ((c) == '/' || (c) == '\\')
#define O_NONBLOCK  0
#if !defined(EWOULDBLOCK)
#define EWOULDBLOCK  WSAEWOULDBLOCK
#endif // !EWOULDBLOCK
#define _POSIX_
#define INT64_FMT  "I64d"

#define WINCDECL __cdecl
#define SHUT_WR 1
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define sleep(x) Sleep((x) * 1000)

#define pipe(x) _pipe(x, BUFSIZ, _O_BINARY)
#define popen(x, y) _popen(x, y)
#define pclose(x) _pclose(x)
#define close(x) _close(x)
#define dlsym(x,y) GetProcAddress((HINSTANCE) (x), (y))
#define RTLD_LAZY  0
#define fseeko(x, y, z) fseek((x), (y), (z))
#define fdopen(x, y) _fdopen((x), (y))
#define write(x, y, z) _write((x), (y), (unsigned) z)
#define read(x, y, z) _read((x), (y), (unsigned) z)
#define flockfile(x) (void) 0
#define funlockfile(x) (void) 0

#if !defined(fileno)
#define fileno(x) _fileno(x)
#endif // !fileno MINGW #defines fileno

typedef HANDLE pthread_mutex_t;
typedef struct {HANDLE signal, broadcast;} pthread_cond_t;
typedef DWORD pthread_t;
#define pid_t HANDLE // MINGW typedefs pid_t to int. Using #define here.

struct timespec {
  long tv_nsec;
  long tv_sec;
};

static int pthread_mutex_lock(pthread_mutex_t *);
static int pthread_mutex_unlock(pthread_mutex_t *);
static FILE *mg_fopen(const char *path, const char *mode);

#if defined(HAVE_STDINT)
#include <stdint.h>
#else
typedef unsigned int  uint32_t;
typedef unsigned short  uint16_t;
typedef unsigned __int64 uint64_t;
typedef __int64   int64_t;
#define INT64_MAX  9223372036854775807
#endif // HAVE_STDINT

// POSIX dirent interface
struct dirent {
  char d_name[PATH_MAX];
};

typedef struct DIR {
  HANDLE   handle;
  WIN32_FIND_DATAW info;
  struct dirent  result;
} DIR;

#else    // UNIX  specific
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdint.h>
#include <inttypes.h>
#include <netdb.h>

#include <pwd.h>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>
#include <pthread.h>
#if defined(__MACH__)
#define SSL_LIB   "libssl.dylib"
#define CRYPTO_LIB  "libcrypto.dylib"
#else
#define SSL_LIB   "libssl.so"
#define CRYPTO_LIB  "libcrypto.so"
#endif
#define DIRSEP   '/'
#define IS_DIRSEP_CHAR(c) ((c) == '/')
#define O_BINARY  0
#define closesocket(a) close(a)
#define mg_fopen(x, y) fopen(x, y)
#define mg_mkdir(x, y) mkdir(x, y)
#define mg_remove(x) remove(x)
#define mg_rename(x, y) rename(x, y)
#define ERRNO errno
#define INVALID_SOCKET (-1)
#define INT64_FMT PRId64
typedef int SOCKET;
#define WINCDECL

#endif // End of Windows and UNIX specific includes

//#include "mongoose.h"

#define MONGOOSE_VERSION "2.11"
#define PASSWORDS_FILE_NAME ".htpasswd"
#define CGI_ENVIRONMENT_SIZE 4096
#define MAX_CGI_ENVIR_VARS 64
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#if defined(DEBUG)
#define DEBUG_TRACE(x) do { \
  flockfile(stdout); \
  printf("*** %lu.%p.%s.%d: ", \
         (unsigned long) time(NULL), (void *) pthread_self(), \
         __func__, __LINE__); \
  printf x; \
  putchar('\n'); \
  fflush(stdout); \
  funlockfile(stdout); \
} while (0)
#else
#define DEBUG_TRACE(x)
#endif // DEBUG

// Darwin prior to 7.0 and Win32 do not have socklen_t
#ifdef NO_SOCKLEN_T
typedef int socklen_t;
#endif // NO_SOCKLEN_T

typedef void * (*mg_thread_func_t)(void *);

static const char *http_500_error = "Internal Server Error";

// Snatched from OpenSSL includes. I put the prototypes here to be independent
// from the OpenSSL source installation. Having this, mongoose + SSL can be
// built on any system with binary SSL libraries installed.
typedef struct ssl_st SSL;
typedef struct ssl_method_st SSL_METHOD;
typedef struct ssl_ctx_st SSL_CTX;

#define SSL_ERROR_WANT_READ 2
#define SSL_ERROR_WANT_WRITE 3
#define SSL_FILETYPE_PEM 1
#define CRYPTO_LOCK  1

#if defined(NO_SSL_DL)
extern void SSL_free(SSL *);
extern int SSL_accept(SSL *);
extern int SSL_connect(SSL *);
extern int SSL_read(SSL *, void *, int);
extern int SSL_write(SSL *, const void *, int);
extern int SSL_get_error(const SSL *, int);
extern int SSL_set_fd(SSL *, int);
extern SSL *SSL_new(SSL_CTX *);
extern SSL_CTX *SSL_CTX_new(SSL_METHOD *);
extern SSL_METHOD *SSLv23_server_method(void);
extern int SSL_library_init(void);
extern void SSL_load_error_strings(void);
extern int SSL_CTX_use_PrivateKey_file(SSL_CTX *, const char *, int);
extern int SSL_CTX_use_certificate_file(SSL_CTX *, const char *, int);
extern int SSL_CTX_use_certificate_chain_file(SSL_CTX *, const char *);
extern void SSL_CTX_set_default_passwd_cb(SSL_CTX *, mg_callback_t);
extern void SSL_CTX_free(SSL_CTX *);
extern unsigned long ERR_get_error(void);
extern char *ERR_error_string(unsigned long, char *);
extern int CRYPTO_num_locks(void);
extern void CRYPTO_set_locking_callback(void (*)(int, int, const char *, int));
extern void CRYPTO_set_id_callback(unsigned long (*)(void));
#else
// Dynamically loaded SSL functionality
struct ssl_func {
  const char *name;   // SSL function name
  void  (*ptr)(void); // Function pointer
};

#define SSL_free (* (void (*)(SSL *)) ssl_sw[0].ptr)
#define SSL_accept (* (int (*)(SSL *)) ssl_sw[1].ptr)
#define SSL_connect (* (int (*)(SSL *)) ssl_sw[2].ptr)
#define SSL_read (* (int (*)(SSL *, void *, int)) ssl_sw[3].ptr)
#define SSL_write (* (int (*)(SSL *, const void *,int)) ssl_sw[4].ptr)
#define SSL_get_error (* (int (*)(SSL *, int)) ssl_sw[5])
#define SSL_set_fd (* (int (*)(SSL *, SOCKET)) ssl_sw[6].ptr)
#define SSL_new (* (SSL * (*)(SSL_CTX *)) ssl_sw[7].ptr)
#define SSL_CTX_new (* (SSL_CTX * (*)(SSL_METHOD *)) ssl_sw[8].ptr)
#define SSLv23_server_method (* (SSL_METHOD * (*)(void)) ssl_sw[9].ptr)
#define SSL_library_init (* (int (*)(void)) ssl_sw[10].ptr)
#define SSL_CTX_use_PrivateKey_file (* (int (*)(SSL_CTX *, \
        const char *, int)) ssl_sw[11].ptr)
#define SSL_CTX_use_certificate_file (* (int (*)(SSL_CTX *, \
        const char *, int)) ssl_sw[12].ptr)
#define SSL_CTX_set_default_passwd_cb \
  (* (void (*)(SSL_CTX *, mg_callback_t)) ssl_sw[13].ptr)
#define SSL_CTX_free (* (void (*)(SSL_CTX *)) ssl_sw[14].ptr)
#define SSL_load_error_strings (* (void (*)(void)) ssl_sw[15].ptr)
#define SSL_CTX_use_certificate_chain_file \
  (* (int (*)(SSL_CTX *, const char *)) ssl_sw[16].ptr)

#define CRYPTO_num_locks (* (int (*)(void)) crypto_sw[0].ptr)
#define CRYPTO_set_locking_callback \
  (* (void (*)(void (*)(int, int, const char *, int))) crypto_sw[1].ptr)
#define CRYPTO_set_id_callback \
  (* (void (*)(unsigned long (*)(void))) crypto_sw[2].ptr)
#define ERR_get_error (* (unsigned long (*)(void)) ssl_sw[3].ptr)
#define ERR_error_string (* (char * (*)(unsigned long, char *)) ssl_sw[4].ptr)

// set_ssl_option() function updates this array.
// It loads SSL library dynamically and changes NULLs to the actual addresses
// of respective functions. The macros above (like SSL_connect()) are really
// just calling these functions indirectly via the pointer.
static struct ssl_func ssl_sw[] = {
  {"SSL_free",   NULL},
  {"SSL_accept",   NULL},
  {"SSL_connect",   NULL},
  {"SSL_read",   NULL},
  {"SSL_write",   NULL},
  {"SSL_get_error",  NULL},
  {"SSL_set_fd",   NULL},
  {"SSL_new",   NULL},
  {"SSL_CTX_new",   NULL},
  {"SSLv23_server_method", NULL},
  {"SSL_library_init",  NULL},
  {"SSL_CTX_use_PrivateKey_file", NULL},
  {"SSL_CTX_use_certificate_file",NULL},
  {"SSL_CTX_set_default_passwd_cb",NULL},
  {"SSL_CTX_free",  NULL},
  {"SSL_load_error_strings", NULL},
  {"SSL_CTX_use_certificate_chain_file", NULL},
  {NULL,    NULL}
};

// Similar array as ssl_sw. These functions could be located in different lib.
static struct ssl_func crypto_sw[] = {
  {"CRYPTO_num_locks",  NULL},
  {"CRYPTO_set_locking_callback", NULL},
  {"CRYPTO_set_id_callback", NULL},
  {"ERR_get_error",  NULL},
  {"ERR_error_string", NULL},
  {NULL,    NULL}
};
#endif // NO_SSL_DL

static const char *month_names[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

// Unified socket address. For IPv6 support, add IPv6 address structure
// in the union u.
struct usa {
  socklen_t len;
  union {
    struct sockaddr sa;
    struct sockaddr_in sin;
  } u;
};

// Describes a string (chunk of memory).
struct vec {
  const char *ptr;
  size_t len;
};

// Structure used by mg_stat() function. Uses 64 bit file length.
struct mgstat {
  int is_directory;  // Directory marker
  int64_t size;      // File size
  time_t mtime;      // Modification time
};

// Describes listening socket, or socket which was accept()-ed by the master
// thread and queued for future handling by the worker thread.
struct socket {
  struct socket *next;  // Linkage
  SOCKET sock;          // Listening socket
  struct usa lsa;       // Local socket address
  struct usa rsa;       // Remote socket address
  int is_ssl;           // Is socket SSL-ed
  int is_proxy;
};

enum {
  CGI_EXTENSIONS, CGI_ENVIRONMENT, PUT_DELETE_PASSWORDS_FILE, CGI_INTERPRETER,
  PROTECT_URI, AUTHENTICATION_DOMAIN, SSI_EXTENSIONS, ACCESS_LOG_FILE,
  SSL_CHAIN_FILE, ENABLE_DIRECTORY_LISTING, ERROR_LOG_FILE,
  GLOBAL_PASSWORDS_FILE, INDEX_FILES,
  ENABLE_KEEP_ALIVE, ACCESS_CONTROL_LIST, MAX_REQUEST_SIZE,
  EXTRA_MIME_TYPES, LISTENING_PORTS,
  DOCUMENT_ROOT, SSL_CERTIFICATE, NUM_THREADS, RUN_AS_USER,
  NUM_OPTIONS
};

static const char *config_options[] = {
  "C", "cgi_extensions", ".cgi,.pl,.php",
  "E", "cgi_environment", NULL,
  "G", "put_delete_passwords_file", NULL,
  "I", "cgi_interpreter", NULL,
  "P", "protect_uri", NULL,
  "R", "authentication_domain", "mydomain.com",
  "S", "ssi_extensions", ".shtml,.shtm",
  "a", "access_log_file", NULL,
  "c", "ssl_chain_file", NULL,
  "d", "enable_directory_listing", "yes",
  "e", "error_log_file", NULL,
  "g", "global_passwords_file", NULL,
  "i", "index_files", "index.html,index.htm,index.cgi",
  "k", "enable_keep_alive", "no",
  "l", "access_control_list", NULL,
  "M", "max_request_size", "16384",
  "m", "extra_mime_types", NULL,
  "p", "listening_ports", "8080",
  "r", "document_root",  ".",
  "s", "ssl_certificate", NULL,
  "t", "num_threads", "10",
  "u", "run_as_user", NULL,
  NULL
};
#define ENTRIES_PER_CONFIG_OPTION 3

// This structure contains information about the HTTP request.
struct mg_request_info {
  char *request_method;  // "GET", "POST", etc
  char *uri;             // URL-decoded URI
  char *http_version;    // E.g. "1.0", "1.1"
  char *query_string;    // \0 - terminated
  char *remote_user;     // Authenticated user
  char *log_message;     // Mongoose error log message
  long remote_ip;        // Client's IP address
  int remote_port;       // Client's port
  int status_code;       // HTTP reply status code
  int is_ssl;            // 1 if SSL-ed, 0 if not
  int num_headers;       // Number of headers
  struct mg_header {
    char *name;          // HTTP header name
    char *value;         // HTTP header value
  } http_headers[64];    // Maximum 64 headers
};

// Various events on which user-defined function is called by Mongoose.
enum mg_event {
  MG_NEW_REQUEST,   // New HTTP request has arrived from the client
  MG_HTTP_ERROR,    // HTTP error must be returned to the client
  MG_EVENT_LOG,     // Mongoose logs an event, request_info.log_message
  MG_INIT_SSL,      // Mongoose initializes SSL. Instead of mg_connection *,
                    // SSL context is passed to the callback function.
};

// Prototype for the user-defined function. Mongoose calls this function
// on every event mentioned above.
//
// Parameters:
//   event: which event has been triggered.
//   conn: opaque connection handler. Could be used to read, write data to the
//         client, etc. See functions below that accept "mg_connection *".
//   request_info: Information about HTTP request.
//
// Return:
//   If handler returns non-NULL, that means that handler has processed the
//   request by sending appropriate HTTP reply to the client. Mongoose treats
//   the request as served.
//   If callback returns NULL, that means that callback has not processed
//   the request. Handler must not send any data to the client in this case.
//   Mongoose proceeds with request handling as if nothing happened.
typedef void * (*mg_callback_t)(enum mg_event event,
                                struct mg_connection *conn,
                                const struct mg_request_info *request_info);
  

struct mg_context {
  int stop_flag;                // Should we stop event loop
  SSL_CTX *ssl_ctx;             // SSL context
  char *config[NUM_OPTIONS];    // Mongoose configuration parameters
  mg_callback_t user_callback;  // User-defined callback function

  struct socket *listening_sockets;

  int num_threads;           // Number of threads
  pthread_mutex_t mutex;     // Protects (max|num)_threads
  pthread_cond_t  cond;      // Condvar for tracking workers terminations

  struct socket queue[20];   // Accepted sockets
  int sq_head;               // Head of the socket queue
  int sq_tail;               // Tail of the socket queue
  pthread_cond_t sq_full;    // Singaled when socket is produced
  pthread_cond_t sq_empty;   // Signaled when socket is consumed
};

struct mg_connection {
  struct mg_connection *peer; // Remote target in proxy mode
  struct mg_request_info request_info;
  struct mg_context *ctx;
  SSL *ssl;                   // SSL descriptor
  struct socket client;       // Connected client
  time_t birth_time;          // Time connection was accepted
  int64_t num_bytes_sent;     // Total bytes sent to client
  int64_t content_len;        // Content-Length header value
  int64_t consumed_content;   // How many bytes of content is already read
  char *buf;                  // Buffer for received data
  int buf_size;               // Buffer size
  int request_len;            // Size of the request + headers in a buffer
  int data_len;               // Total size of data in a buffer
};


// Start web server.
//
// Parameters:
//   callback: user defined event handling function or NULL.
//   options: NULL terminated list of option_name, option_value pairs that
//            specify Mongoose configuration parameters.
//
// Example:
//   const char *options[] = {
//     "document_root", "/var/www",
//     "listening_ports", "80,443s",
//     NULL
//   };
//   struct mg_context *ctx = mg_start(&my_func, options);
//
// Please refer to http://code.google.com/p/mongoose/wiki/MongooseManual
// for the list of valid option and their possible values.
//
// Return:
//   web server context, or NULL on error.
struct mg_context *mg_start(mg_callback_t callback, const char **options);


// Stop the web server.
//
// Must be called last, when an application wants to stop the web server and
// release all associated resources. This function blocks until all Mongoose
// threads are stopped. Context pointer becomes invalid.
void mg_stop(struct mg_context *);


// Get the value of particular configuration parameter.
// The value returned is read-only. Mongoose does not allow changing
// configuration at run time.
// If given parameter name is not valid, NULL is returned. For valid
// names, return value is guaranteed to be non-NULL. If parameter is not
// set, zero-length string is returned.
const char *mg_get_option(const struct mg_context *ctx, const char *name);


// Return array of strings that represent valid configuration options.
// For each option, a short name, long name, and default value is returned.
// Array is NULL terminated.
const char **mg_get_valid_option_names(void);


// Add, edit or delete the entry in the passwords file.
//
// This function allows an application to manipulate .htpasswd files on the
// fly by adding, deleting and changing user records. This is one of the
// several ways of implementing authentication on the server side. For another,
// cookie-based way please refer to the examples/chat.c in the source tree.
//
// If password is not NULL, entry is added (or modified if already exists).
// If password is NULL, entry is deleted.
//
// Return:
//   1 on success, 0 on error.
int mg_modify_passwords_file(struct mg_context *ctx, 
    const char *passwords_file_name, const char *user, const char *password);

// Send data to the client.
int mg_write(struct mg_connection *, const void *buf, size_t len);


// Send data to the browser using printf() semantics.
//
// Works exactly like mg_write(), but allows to do message formatting.
// Note that mg_printf() uses internal buffer of size IO_BUF_SIZE
// (8 Kb by default) as temporary message storage for formatting. Do not
// print data that is bigger than that, otherwise it will be truncated.
int mg_printf(struct mg_connection *, const char *fmt, ...);


// Read data from the remote end, return number of bytes read.
int mg_read(struct mg_connection *, void *buf, size_t len);


// Get the value of particular HTTP header.
//
// This is a helper function. It traverses request_info->http_headers array,
// and if the header is present in the array, returns its value. If it is
// not present, NULL is returned.
const char *mg_get_header(const struct mg_connection *, const char *name);


// Get a value of particular form variable.
//
// Parameters:
//   data: pointer to form-uri-encoded buffer. This could be either POST data,
//         or request_info.query_string.
//   data_len: length of the encoded data.
//   var_name: variable name to decode from the buffer
//   buf: destination buffer for the decoded variable
//   buf_len: length of the destination buffer
//
// Return:
//   On success, length of the decoded variable.
//   On error, -1 (variable not found, or destination buffer is too small).
//
// Destination buffer is guaranteed to be '\0' - terminated. In case of
// failure, dst[0] == '\0'.
int mg_get_var(const char *data, size_t data_len,
    const char *var_name, char *buf, size_t buf_len);

// Fetch value of certain cookie variable into the destination buffer.
//
// Destination buffer is guaranteed to be '\0' - terminated. In case of
// failure, dst[0] == '\0'. Note that RFC allows many occurrences of the same
// parameter. This function returns only first occurrence.
//
// Return:
//   On success, value length.
//   On error, -1 (either "Cookie:" header is not present at all, or the
//   requested parameter is not found, or destination buffer is too small
//   to hold the value).
int mg_get_cookie(const struct mg_connection *,
    const char *cookie_name, char *buf, size_t buf_len);


// Return Mongoose version.
const char *mg_version(void);


// MD5 hash given strings.
// Buffer 'buf' must be 33 bytes long. Varargs is a NULL terminated list of
// asciiz strings. When function returns, buf will contain human-readable
// MD5 hash. Example:
//   char buf[33];
//   mg_md5(buf, "aa", "bb", NULL);
void mg_md5(char *buf, ...);

void free_context(struct mg_context *ctx);


#ifdef __cplusplus
}
#endif // __cplusplus
;
