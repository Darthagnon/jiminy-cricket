// https://wiki.tox.chat/developers/client_examples/echo_bot
// g++ -Wall -g -o server_bot server_bot3.cpp -lsodium -I /usr/local/include/ -ltoxcore
//#define _GNU_SOURCE - already done


// getting disk space from the system
// https://www.systutorials.com/how-to-get-available-filesystem-space-on-linux-a-c-function-and-a-cpp-example/

// get the time that this program has been running

// 

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <iostream>
//#include <fstream>
#include <queue>

#include <fcntl.h>           /* Definition of AT_* constants */

#include <sodium/utils.h>
#include <tox/tox.h>

#include <dirent.h>

#include <errno.h>
#include <sys/statvfs.h>

#include <iterator> 
#include <map>


#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif //_LARGEFILE64_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>

#define EXIT(x) {printf ("%d\n", __LINE__); exit (0);}
#define DEBUG() //printf ("DBG> %s: %d\n", __FUNCTION__, __LINE__)

typedef struct DHT_node {
  const char *ip;
  uint16_t port;
  const char key_hex[TOX_PUBLIC_KEY_SIZE*2 + 1];
  unsigned char key_bin[TOX_PUBLIC_KEY_SIZE];
} DHT_node;

typedef struct file_map {
  char *sz_filename;
  uint64_t u64_length;
} file_map;

// https://appdividend.com/2019/07/05/c-map-tutorial-with-example-standard-template-library-stl/

class tfile
{
private:
  std::string mstr_fn;
  int mi_fd;
  off64_t moff_t_file_size;
  time_t mtime_t_last_access;

public:
  tfile ();
  ~tfile ();
  std::string get_name (void);
  void set_length (off64_t off_t_size);
  off64_t get_length (void);
  bool exists (const char *sz_name);
  bool open (const std::string str_name, int flags);
  ssize_t write (const void *buf, size_t len);
  ssize_t read (void *buf, size_t len);
  off64_t seek (off64_t offset);
};

tfile::tfile ()
{
  mstr_fn = "";
  mi_fd = -1;
  moff_t_file_size = 0;
  mtime_t_last_access = time (NULL);
}

tfile::~tfile ()
{
  if (mi_fd != -1) {
    close (mi_fd);
  }
}

std::string tfile::get_name (void)
{
  return mstr_fn;
}

void tfile::set_length (off64_t off_t_size)
{
  moff_t_file_size = off_t_size;
}

off64_t tfile::get_length (void)
{
  return moff_t_file_size;
}

bool tfile::exists (const char *sz_name)
{
  return (access(mstr_fn.c_str(), F_OK) != -1) ? true:false;
}

bool tfile::open (const std::string str_name, int flags)
{
  mtime_t_last_access = time (NULL);

  mi_fd = open (mstr_fn.c_str(), flags);

  return (mi_fd != -1) ? true:false;
}

ssize_t tfile::write (const void *buf, size_t len)
{
  ssize_t write_ret;
  size_t pos;

  for (pos = 0 ; pos < len ; ) {
    write_ret = ::write (mi_fd, (uint8_t*)buf+pos, len-pos);
    if (write_ret == -1) {
      // error
      pos = write_ret;
      break;
    }
    else {
      pos += write_ret;
    }
  }

  mtime_t_last_access = time (NULL);

  return pos;
}

ssize_t tfile::read (void *buf, size_t len)
{
  mtime_t_last_access = time (NULL);
  return ::read (mi_fd, buf, len);
}

off64_t tfile::seek (off64_t offset)
{
  return lseek64 (mi_fd, offset, SEEK_SET);
}


class tfriend
{
private:
  std::string mstr_address; // void tox_self_get_address(const Tox *tox, uint8_t *address);
  std::string mstr_name;
  std::map<int, tfile> mm_dl_files; // map fileno to file
  std::map<int, tfile> mm_ul_files;
  std::map<int, int> mm_ports; // in port -> destination port
  std::queue<std::string> mq_in_msg; // messages to send
  std::queue<std::string> mq_out_msg; // messages received to print

public:
  tfriend ();
  tfriend (int ifriend_number);
  ~tfriend ();

  std::string get_name ();
  void set_name (std::string str_name);
  void set_name (const char *sz_name, size_t len);
  // files

  // port forwarding

  // messages
  void rx_msg ();
  void tx_msg ();
};

tfriend::tfriend ()
{
  mm_dl_files.clear ();
  mm_ul_files.clear ();
  mstr_name = "";
}

tfriend::~tfriend ()
{
  mm_dl_files.clear ();
  mm_ul_files.clear ();
}

void tfriend::set_name (std::string str_name)
{
  mstr_name = str_name;
}

void tfriend::set_name (const char *sz_name, size_t len)
{
  mstr_name.resize (len);
  mstr_name.replace (0, len, sz_name);
}

std::string tfriend::get_name ()
{
  return mstr_name;
}

// http://unixwiz.net/techtips/gnu-c-attributes.html
int dbg_printf (const char *szFmt, ...) __attribute__ ((format (printf, 1, 2)));
int dbg_printf (const char *szFmt, ...)
{
  va_list argp;
  char *szString;
  int strLen;

  va_start (argp, szFmt);
  strLen = vasprintf (&szString, szFmt, argp);
  va_end (argp);

  if (strLen > 0)
  {
    fputs (szString, stdout);
  }
  free (szString);
  return strLen;
}

int xmain(int argc, char **argv)
{
  std::map<int, tfriend> map1; // map friend # to friend
  std::map<int, tfriend>::iterator cursor;

  map1[1].set_name ("11");
  map1[2].set_name ("22");
  map1[222].set_name ("33");
  map1[7283].set_name ("44");
  map1[183273].set_name ("55");
  map1[0].set_name ("66");

  for (int i = 0 ; i < 2 ; i++) {
    dbg_printf ("KEY\tELEMENT\n");
    for(cursor = map1.begin(); cursor!=map1.end(); cursor++){ 
      printf ("%d\t", cursor->first);
      printf ("%s\n", cursor->second.get_name().c_str());
    }
    printf ("\n\n\n");
    map1.erase (0);
  }

  return 0;
}


// this should be a map and a structure of file name / FD / perhaps bitmask / sha256sum
class Tox_cb
{
private:
  // run
  // └─tox
  //   └─$USER
  //     └─<tox_address>
  //       ├─name
  //       └─<friend_address>
  //         ├─name
  //         ├─status
  //         ├─ports
  //         │ ├─allowed
  //         │ ├─requested
  //         │ └─status
  //         ├─messages
  //         │ ├─in
  //         │ ├─out
  //         │ └─status
  //         ├─mbox
  //         │ ├─in
  //         │ ├─out
  //         │ └─status
  //         └─file
  //           ├─in
  //           │ ├─<file_number>
  //           │ │ ├─name
  //           │ │ └─status
  //           │ └─status
  //           └─out
  //             ├─<file_number>
  //             │ ├─name
  //             │ └─status
  //             └─status

  // Maybe this? - all these are either directories or pipes
  // run
  // └─tox
  //   └─$USER
  //     └─<tox_address>
  //       ├─name
  //       ├─status
  //       ├─control
  //       └─<friend_address>
  //         ├─name
  //         ├─status
  //         ├─ports
  //         │ ├─allowed
  //         │ ├─requested
  //         │ └─status
  //         ├─in
  //         │ ├─messages
  //         │ ├─mbox
  //         │ ├─status
  //         │ └─<file_number>
  //         │   ├─name
  //         │   └─status
  //         └─out
  //           ├─messages
  //           ├─mbox
  //           ├─status
  //           ├─control
  //           └─<file_number>
  //             ├─name
  //             └─status
  Tox *m_tox;
  std::string mstr_name;
  std::string mstr_address; // void tox_self_get_public_key(const Tox *tox, uint8_t *public_key);
  std::string mstr_tox_data_file;
  std::map<int, tfriend> m_friends; // friend #->tfriend

public:
  Tox_cb (std::string str_data_file);
  ~Tox_cb ();
  bool save_data_file ();
  bool get_friend (uint32_t num, tfriend &f);

public:
  // to deprecate
  FILE *fp_receive; // should be int FD, and a C++ map
  FILE *fp_send;    // should be int FD, and a C++ map

  // used for file listings
  uint32_t max_file_size;
  uint32_t file_entries;
  file_map *fm_array;
};

Tox_cb::Tox_cb (std::string str_data_file)
{
  int i_fd;
  struct Tox_Options options;

  tox_options_default(&options);

  mstr_tox_data_file = str_data_file;
  i_fd = open (mstr_tox_data_file.c_str(), O_RDONLY | O_LARGEFILE | O_TRUNC);

  if (i_fd != -1)
  {
    DEBUG();
    struct Tox_Options options;
    ssize_t pos;
    struct stat statbuf;

    if (lstat(mstr_tox_data_file.c_str(), &statbuf) == -1) {
      EXIT (1);
    }
    uint8_t *buf = (uint8_t *) alloca (statbuf.st_size);

    for (pos = 0 ; pos < statbuf.st_size ; ) {
      ssize_t ret = ::read (i_fd, buf+pos, statbuf.st_size - pos);
      if (ret != -1) {
        pos += ret;
      }
      else {
        EXIT (1);
      }
    }

    options.savedata_type = TOX_SAVEDATA_TYPE_TOX_SAVE;
    options.savedata_data = buf;
    options.savedata_length = pos;

    m_tox = tox_new (&options, NULL);
  }
  else {
    DEBUG();
    m_tox = tox_new (&options, NULL);
    save_data_file ();
  }

  // to deprecate
  fp_receive = NULL;
  fp_send = NULL;
  max_file_size = 0;
  file_entries = 0;
  fm_array = NULL;
}

Tox_cb::~Tox_cb ()
{
}

bool Tox_cb::save_data_file ()
{
  std::string str_tmp_file = mstr_tox_data_file + ".tmp";
  std::string str_bkp_file = mstr_tox_data_file + ".bkp";
  size_t len = tox_get_savedata_size(m_tox);
  uint8_t *data = (uint8_t *) alloca (len);
  int i_fd;
  bool b_ret = true;

  tox_get_savedata (m_tox, data);
  i_fd = open (str_tmp_file.c_str(), O_CREAT | O_WRONLY | O_LARGEFILE | O_TRUNC);

  DEBUG();
  if (i_fd != -1) {
    ssize_t write_ret;

    for (size_t pos = 0 ; pos < len ; ) {
      write_ret = ::write (i_fd, data+pos, len-pos);
      if (write_ret == -1) {
        // error
        b_ret = false;
        break;
      }
      else {
        pos += write_ret;
      }
    }
    ::close (i_fd);
    DEBUG();
  }
  else {
    // fail
    perror ("WTF");
    DEBUG();
    b_ret = false;
  }

  if (b_ret == true)
  {
    // backup old file
    if (rename(mstr_tox_data_file.c_str(), str_bkp_file.c_str()) == -1) {
      b_ret = false;
    }

    // rename to the original file
    if (rename(str_tmp_file.c_str(), mstr_tox_data_file.c_str()) == -1) {
      b_ret = false;
    }
    dbg_printf ("Saved %s\n", mstr_tox_data_file.c_str());
  }

  return true;
}

bool Tox_cb::get_friend (uint32_t num, tfriend &f)
{
  return false;
}

const char *savedata_filename = "savedata.tox";
const char *savedata_tmp_filename = "savedata.tox.tmp";


// http://unixwiz.net/techtips/gnu-c-attributes.html
int tox_printf (Tox *tox, uint32_t friend_number, TOX_MESSAGE_TYPE type, const char *szFmt, ...) __attribute__ ((format (printf, 4, 5)));
int tox_printf (Tox *tox, uint32_t friend_number, TOX_MESSAGE_TYPE type, const char *szFmt, ...)
{
  va_list argp;
  char *szString;
  int strLen;

  va_start (argp, szFmt);
  strLen = vasprintf (&szString, szFmt, argp);
  va_end (argp);

  if (strLen > 0)
  {
    tox_friend_send_message (tox, friend_number, type, (const uint8_t *)szString, strLen, NULL);
  }
  free (szString);
  return strLen;
}

uint64_t get_size_of_file (const char *szFile)
{
  struct stat buf;
  if (stat (szFile, &buf) == 0)
  {
    return (uint64_t)buf.st_size;
  }
  return 0;
}

void compile_file_string (Tox *tox, uint32_t friend_number, TOX_MESSAGE_TYPE type, Tox_cb *cb_ptr, uint32_t iter)
{
  const char *sz_fmt;
  switch (cb_ptr->max_file_size)
  {
  case 15:
    sz_fmt = "%15llu %s";
    break;
  case 12:
    sz_fmt = "%12llu %s";
    break;
  case  9:
    sz_fmt = "%9llu %s";
    break;
  case  6:
    sz_fmt = "%6llu %s";
    break;
  case  3:
    sz_fmt = "%3llu %s";
    break;
  default:
    dbg_printf ("Max file size = %d\n", cb_ptr->max_file_size);
    abort ();
  }

  uint8_t *response;
  int response_len = asprintf ((char **)&response, sz_fmt, cb_ptr->fm_array[iter].u64_length, cb_ptr->fm_array[iter].sz_filename);
  tox_friend_send_message (tox, friend_number, type, response, response_len, NULL);
  free (response);
}

void get_directory (Tox *tox, uint32_t friend_number, TOX_MESSAGE_TYPE type, Tox_cb *cb_ptr)
{
  DIR *d;
  char buf[1000];

  d = opendir (getcwd(buf, sizeof (buf)));
  dbg_printf ("directory from %s\n", buf);
  cb_ptr->file_entries = 0;
  cb_ptr->max_file_size = 3;

  if (d != NULL)
  {
    struct dirent *de;
    while ((de = readdir (d)) != NULL)
    {
      char *sz_name = de->d_name;
      uint64_t size;
      uint32_t len = 3;

      size = get_size_of_file ((const char *)sz_name);

      cb_ptr->fm_array = (file_map *)realloc (cb_ptr->fm_array, sizeof (cb_ptr->fm_array[0]) * (1+cb_ptr->file_entries));
      cb_ptr->fm_array[cb_ptr->file_entries].sz_filename = strdup (sz_name);
      cb_ptr->fm_array[cb_ptr->file_entries].u64_length = size;

      if (strcmp (sz_name, "..") == 0 || strcmp (sz_name, ".") == 0)
      {
        cb_ptr->file_entries--;
        //free (cb_ptr->fm_array[cb_ptr->file_entries].sz_filename);
      }
      else if (size >= 1000000000000000ULL)
      {
        len = 18;
      }
      else if (size >= 1000000000000ULL)
      {
        len = 15;
      }
      else if (size >= 1000000000ULL)
      {
        len = 12;
      }
      else if (size >= 1000000ULL)
      {
        len = 9;
      }
      else if (size >= 1000ULL)
      {
        len = 6;
      }

      if (len > cb_ptr->max_file_size)
      {
        cb_ptr->max_file_size = len;
      }
      
      //printf ("%llu\n", cb_ptr->fm_array[cb_ptr->file_entries].u64_length);
      //printf ("%p\n", cb_ptr->fm_array[cb_ptr->file_entries].sz_filename);
      //printf ("%s\n", cb_ptr->fm_array[cb_ptr->file_entries].sz_filename);

      cb_ptr->file_entries++;
    }
  }
  //printf ("max file size = %d\n", cb_ptr->max_file_size);

  for (uint32_t iter=0 ; iter < cb_ptr->file_entries ; iter++)
  {
    compile_file_string (tox, friend_number, type, cb_ptr, iter);
  }
  
  closedir (d);
}

Tox *create_tox()
{
  Tox *tox;

  struct Tox_Options options;

  tox_options_default(&options);

  FILE *f = fopen(savedata_filename, "rb");
  if (f) {
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t *savedata = (uint8_t *) malloc(fsize);

    fread(savedata, fsize, 1, f);
    fclose(f);

    options.savedata_type = TOX_SAVEDATA_TYPE_TOX_SAVE;
    options.savedata_data = savedata;
    options.savedata_length = fsize;

    tox = tox_new(&options, NULL);

    free(savedata);
  } else {
    tox = tox_new(&options, NULL);
  }

  return tox;
}

void update_savedata_file(const Tox *tox)
{
  size_t size = tox_get_savedata_size(tox);
  uint8_t *savedata = (uint8_t *)malloc(size);
  tox_get_savedata(tox, savedata);

  FILE *f = fopen(savedata_tmp_filename, "wb");
  fwrite(savedata, size, 1, f);
  fclose(f);

  // renaming ensures we don't get a partial file, and lose contact
  // information
  rename(savedata_tmp_filename, savedata_filename);

  free(savedata);
}

void bootstrap(Tox *tox)
{
  DHT_node nodes[] =
    {
      {"178.62.250.138",             33445, "788236D34978D1D5BD822F0A5BEBD2C53C64CC31CD3149350EE27D4D9A2F9B6B", {0}},
      {"2a03:b0c0:2:d0::16:1",       33445, "788236D34978D1D5BD822F0A5BEBD2C53C64CC31CD3149350EE27D4D9A2F9B6B", {0}},
      {"tox.zodiaclabs.org",         33445, "A09162D68618E742FFBCA1C2C70385E6679604B2D80EA6E84AD0996A1AC8A074", {0}},
      {"163.172.136.118",            33445, "2C289F9F37C20D09DA83565588BF496FAB3764853FA38141817A72E3F18ACA0B", {0}},
      {"2001:bc8:4400:2100::1c:50f", 33445, "2C289F9F37C20D09DA83565588BF496FAB3764853FA38141817A72E3F18ACA0B", {0}},
      {"128.199.199.197",            33445, "B05C8869DBB4EDDD308F43C1A974A20A725A36EACCA123862FDE9945BF9D3E09", {0}},
      {"2400:6180:0:d0::17a:a001",   33445, "B05C8869DBB4EDDD308F43C1A974A20A725A36EACCA123862FDE9945BF9D3E09", {0}},
      {"node.tox.biribiri.org",      33445, "F404ABAA1C99A9D37D61AB54898F56793E1DEF8BD46B1038B9D822E8460FAB67", {0}}
    };

  for (size_t i = 0; i < sizeof(nodes)/sizeof(DHT_node); i ++) {
    sodium_hex2bin(nodes[i].key_bin, sizeof(nodes[i].key_bin),
                   nodes[i].key_hex, sizeof(nodes[i].key_hex)-1, NULL, NULL, NULL);
    tox_bootstrap(tox, nodes[i].ip, nodes[i].port, nodes[i].key_bin, NULL);
  }
}

void print_tox_id(Tox *tox)
{
  uint8_t tox_id_bin[TOX_ADDRESS_SIZE];
  tox_self_get_address(tox, tox_id_bin);

  char tox_id_hex[TOX_ADDRESS_SIZE*2 + 1];
  sodium_bin2hex(tox_id_hex, sizeof(tox_id_hex), tox_id_bin, sizeof(tox_id_bin));

  
  for (size_t i = 0; i < sizeof(tox_id_hex)-1; i ++) {
    tox_id_hex[i] = toupper(tox_id_hex[i]);
  }

  printf("Tox ID: %s\n", tox_id_hex);
}

void friend_request_cb(Tox *tox, const uint8_t *public_key, const uint8_t *message, size_t length,
                       void *user_data)
{
  tox_friend_add_norequest(tox, public_key, NULL);

  update_savedata_file(tox);
}

//void scramble_word (uint8_t *fragment, size_t start, size_t end)
//{
//  size_t iter;
//
//  if (end != 0 && end-start > 3) {
//    uint8_t tmp;
//
//    end--;
//    start++;
//
//    if (end - start == 2) {
//      tmp = fragment[start];
//      fragment[start] = fragment[start+1];
//      fragment[start+1] = tmp;
//    }
//    else {
//      // randomly scramble
//      for (iter = start ; iter < end ; iter++) {
//        size_t r = start + (rand () % (end - start));
//
//        fflush (stdout);
//        tmp = fragment[r];
//        fragment[r] = fragment[iter];
//        fragment[iter] = tmp;
//      }
//    }
//  }
//}
//
//void scramble (const uint8_t *message, uint8_t *new_message, size_t length)
//{
//  size_t iter;
//  size_t start = 0;
//
//  strncpy ((char *)new_message, (char *)message, length);
//  for (iter = 0 ; iter < length ; iter++) {
//    if (! isalpha(message[iter])) {
//      scramble_word (new_message, start, iter);
//      start = iter+1;
//    }
//  }
//
//  scramble_word (new_message, start, iter);
//}

char *last_online (const Tox *tox, uint32_t friend_number)
{
  TOX_ERR_FRIEND_GET_LAST_ONLINE error;
  uint64_t last;
  time_t t;

  last = tox_friend_get_last_online (tox, friend_number, &error);
  t = (time_t)last;

  return ctime (&t);
}

void friend_message_cb(Tox *tox, uint32_t friend_number, TOX_MESSAGE_TYPE type, const uint8_t *message,
                       size_t length, void *user_data)
{
  size_t friend_len;
  TOX_ERR_FRIEND_QUERY error;
  uint8_t *name;
  Tox_cb *cb_ptr = (Tox_cb *)user_data;
  char *nm = (char *)alloca (length +1);

  nm = strncpy (nm, (char *)message, length);
  nm[length] = '\0';
  friend_len = tox_friend_get_name_size (tox, friend_number, &error);

  name = (uint8_t *)alloca (friend_len+1);
  name[friend_len] = '\0';

  // length may not exceed TOX_MAX_MESSAGE_LENGTH - otherwise breakup
  // replaced with uint32_t tox_max_message_length(void);
  tox_friend_get_name(tox, friend_number, name, &error);
  dbg_printf ("friend length = %d\n", (int)friend_len);
  dbg_printf ("%s : %s\n", name, nm); fflush (stdout);

  if (strncasecmp (nm, "help", length) == 0) {
    dbg_printf ("help requested\n");
    tox_printf (tox, friend_number, type, "help:");
    tox_printf (tox, friend_number, type, " ls         - get a directory list");
    tox_printf (tox, friend_number, type, " get <file> - get a file");
    tox_printf (tox, friend_number, type, " rm <file>  - remove a file");
    tox_printf (tox, friend_number, type, " cd <dir>   - change directory");
    tox_printf (tox, friend_number, type, " uptime     - get system uptime");
    tox_printf (tox, friend_number, type, " users      - list of known users");
    tox_printf (tox, friend_number, type, " space      - get disk space");
    tox_printf (tox, friend_number, type, " reboot     - reboot box (if possible)");
    tox_printf (tox, friend_number, type, " msg [xxx]  - send the message [xxx] to all users that are connected");
    tox_printf (tox, friend_number, type, " toxid      - get tox id of this bot");
    tox_printf (tox, friend_number, type, " ");
    tox_printf (tox, friend_number, type, "You can place files here and delete them");
    tox_printf (tox, friend_number, type, " ");
    tox_printf (tox, friend_number, type, "I will also add:");
    tox_printf (tox, friend_number, type, " get <file> with wildcards");
    tox_printf (tox, friend_number, type, " rm <file/empty_dir> with wildcards");
    tox_printf (tox, friend_number, type, " mkdir <dir>");
    tox_printf (tox, friend_number, type, " mv <src> <dst>");
    tox_printf (tox, friend_number, type, " ");
    tox_printf (tox, friend_number, type, "But after I write this properly in C++ - dynamic memory is easier to deal with in C++ than C");
  }
  else if (strcasecmp (nm, "space") == 0) {
    struct statvfs stat;

    if (statvfs(".", &stat) != 0) {
      tox_printf (tox, friend_number, type, "error getting disk space");
    }
    else {
      uint64_t space = (uint64_t)stat.f_bavail * (uint64_t)stat.f_bsize;
      tox_printf (tox, friend_number, type, "available space %d MB (%d GB)", (int)(space/(1024*1024)), (int) (space/(1024*1024*1024)));
      //tox_printf (tox, friend_number, type, "f_bsize %lu", stat.f_bsize);
      //tox_printf (tox, friend_number, type, "f_frsize %lu", stat.f_frsize);
      //tox_printf (tox, friend_number, type, "f_blocks %lu", stat.f_blocks);
      //tox_printf (tox, friend_number, type, "f_bfree %lu", stat.f_bfree);
      //tox_printf (tox, friend_number, type, "f_bavail %lu", stat.f_bavail);
      //tox_printf (tox, friend_number, type, "f_files %lu", stat.f_files);
      //tox_printf (tox, friend_number, type, "f_ffree %lu", stat.f_ffree);
      //tox_printf (tox, friend_number, type, "f_favail %lu", stat.f_favail);
      //tox_printf (tox, friend_number, type, "f_fsid %lu", stat.f_fsid);
      //tox_printf (tox, friend_number, type, "f_flag %lu", stat.f_flag);
      //tox_printf (tox, friend_number, type, "f_namemax %lu", stat.f_namemax);
    }
  }
  else if (strcasecmp (nm, "users") == 0) {
    size_t frnd_count = tox_self_get_friend_list_size (tox);
    uint32_t *friend_list = (uint32_t *)alloca (sizeof (uint32_t) * frnd_count);
    uint32_t name_len = sizeof (uint8_t) * tox_max_name_length();
    uint8_t *name = (uint8_t *)alloca (name_len);
    uint32_t iter;

    tox_self_get_friend_list(tox, friend_list);
    for (iter = 0 ; iter < frnd_count ; iter++) {
      TOX_ERR_FRIEND_QUERY error;

      memset (name, '\0', name_len);
      if (tox_friend_get_name(tox, friend_list[iter], name, &error)) {
        name[tox_friend_get_name_size(tox, friend_list[iter], &error)] = '\0';
        tox_printf (tox, friend_number, type, "#%d: \"%s\" - last online %s", friend_list[iter], name, last_online (tox, friend_list[iter]));
      }
    }
  }
  else if (strcasecmp (nm, "ls") == 0) {
    dbg_printf ("dir requested\n");
    get_directory (tox, friend_number, type, cb_ptr);
  }
  else if (strcasecmp (nm, "uptime") == 0) {
    FILE *fp;
    char str[28];
    long uptime = 0;

    int days;
    int hours;
    int minutes;
    int seconds;
    
    if((fp = fopen("/proc/uptime", "r")) == NULL)
    {
      perror("supt"), exit(EXIT_FAILURE);
    }

    fgets (str, sizeof(str), fp);
    fclose (fp);
    
    uptime = strtol(str, NULL, 10);

    days = uptime / (24*60*60);
    uptime %= 24*60*60;
    
    hours = uptime / (60*60);
    uptime %= 60*60;

    minutes = uptime / 60;
    uptime %= 60;

    seconds = uptime;
    
    tox_printf (tox, friend_number, type, "System up for %d days, %d hours, %d minutes, %d seconds\n", days, hours, minutes, seconds);
    dbg_printf ("System up for %d days, %d hours, %d minutes, %d seconds\n", days, hours, minutes, seconds);
  }
  else if (strcasecmp (nm, "reboot") == 0)
  {
    tox_printf (tox, friend_number, type, "Going to attempt reboot..");
    system ("sudo reboot");
    sleep (1);
    tox_printf (tox, friend_number, type, "Reboot may have failed");
  }
  else if (strncasecmp (nm, "msg ", 4) == 0) {
    size_t frnd_count = tox_self_get_friend_list_size (tox);
    uint32_t *friend_list = (uint32_t *)alloca (sizeof (uint32_t) * frnd_count);
    uint32_t name_len = sizeof (uint8_t) * tox_max_name_length();
    uint8_t *name = (uint8_t *)alloca (name_len);
    uint8_t *self = (uint8_t *)alloca (name_len);
    uint32_t iter;
    char *broadcast = &nm[4];
    TOX_ERR_FRIEND_QUERY error;

    tox_friend_get_name(tox, friend_number, self, &error);
    self[tox_friend_get_name_size(tox, friend_number, &error)] = '\0';

    tox_self_get_friend_list(tox, friend_list);
    for (iter = 0 ; iter < frnd_count ; iter++) {
      memset (name, '\0', name_len);
      if (tox_friend_get_name(tox, friend_list[iter], name, &error)) {
        tox_printf (tox, friend_list[iter], type, "from \"%s\": %s", self, broadcast);
      }
    }
  }
  else if (strcasecmp (nm, "toxid") == 0) {
    uint8_t tox_id_bin[TOX_ADDRESS_SIZE];
    tox_self_get_address(tox, tox_id_bin);
    
    char tox_id_hex[TOX_ADDRESS_SIZE*2 + 1];
    sodium_bin2hex(tox_id_hex, sizeof(tox_id_hex), tox_id_bin, sizeof(tox_id_bin));

    for (size_t i = 0; i < sizeof(tox_id_hex)-1; i ++) {
      tox_id_hex[i] = toupper(tox_id_hex[i]);
    }

    tox_printf (tox, friend_number, type, "Tox ID: %s\n", tox_id_hex);
  }
  else if (strncasecmp (nm, "get ", 4) == 0) {
    TOX_ERR_FILE_SEND serror;
    char *sz_fp = &nm[3];

    while (*sz_fp != '\0' && *sz_fp == ' ') {
      sz_fp++;
    }

    dbg_printf ("File requested: \"%s\"\n", sz_fp);
    cb_ptr->fp_send = fopen (sz_fp, "r");
    if (cb_ptr->fp_send == NULL) {
      tox_printf (tox, friend_number, type, "unable to open: \"%s\": %s", sz_fp, strerror (errno));
    }
    else {
      // the NULL in this case, should be a hash of the file.
      tox_file_send (tox, friend_number, TOX_FILE_KIND_DATA, get_size_of_file (sz_fp), NULL, (uint8_t *)sz_fp, strlen (sz_fp), &serror);
    }
    dbg_printf ("tox file_send completed\n");
  }
  else if (strncasecmp (nm, "rm ", 3) == 0) {
    char *sz_fp = &nm[2];

    while (*sz_fp != '\0' && *sz_fp == ' ') {
      sz_fp++;
    }

    dbg_printf ("File delete requested: \"%s\"\n", sz_fp);
    if (unlink (sz_fp) != 0) {
      tox_printf (tox, friend_number, type, "unable to delete: \"%s\": %s", sz_fp, strerror (errno));
    }
    else {
      tox_printf (tox, friend_number, type, "deleted...");
    }
  }
  else if (strncasecmp (nm, "cd ", 3) == 0) {
    char *sz_fp = &nm[2];

    while (*sz_fp != '\0' && *sz_fp == ' ') {
      sz_fp++;
    }

    dbg_printf ("chdir requested: \"%s\"\n", sz_fp);
    if (chdir (sz_fp) != 0) {
      tox_printf (tox, friend_number, type, "cd failed: \"%s\": %s", sz_fp, strerror (errno));
    }
    else {
      char buf[1000];
      tox_printf (tox, friend_number, type, "new directory %s", getcwd(buf, sizeof (buf)));
    }
  }
  else
  {
    tox_printf (tox, friend_number, type, "Unknown cmd: \"%s\"", nm);
  }

  //tox_printf (tox, friend_number, type, "text received: \"%s\"", nm);
  //tox_friend_send_message(tox, friend_number, type, message, length, NULL);
}

void self_connection_status_cb(Tox *tox, TOX_CONNECTION connection_status, void *user_data)
{
  switch (connection_status) {
  case TOX_CONNECTION_NONE:
    dbg_printf("Offline\n");
    break;
  case TOX_CONNECTION_TCP:
    dbg_printf("Online, using TCP\n");
    break;
  case TOX_CONNECTION_UDP:
    dbg_printf("Online, using UDP\n");
    break;
  }
}

void friend_typing_cb(Tox *tox, uint32_t friend_number, bool is_typing, void *user_data)
{
  size_t friend_len;
  TOX_ERR_FRIEND_QUERY error;
  uint8_t *name;

  friend_len = tox_friend_get_name_size (tox, friend_number, &error);
  name = (uint8_t *)alloca (friend_len+1);
  name[friend_len] = '\0';
  if (tox_friend_get_name(tox, friend_number, name, &error)) {
    dbg_printf ("(%d) %s %s typing\n", friend_number, name, is_typing? "is":"isn't");
  }
  
}

// * file * //
// file numbers are FRIEND specific - for some incredibly stupid reason to make this so much
// more damned complicated than it needs to be..
void file_recv_control_cb(Tox *tox, uint32_t friend_number, uint32_t file_number, TOX_FILE_CONTROL control,
                          void *user_data)
{
  DEBUG ();
  //bool ret;
  //TOX_ERR_FILE_CONTROL error;
  Tox_cb *cb_ptr = (Tox_cb *)user_data;

  switch (control) {
  case TOX_FILE_CONTROL_RESUME:
    dbg_printf ("Resuming?\n");
    break;
  case TOX_FILE_CONTROL_PAUSE:
    dbg_printf ("Moving from pause to resume: %d\n", file_number);
    //tox_file_control(tox, friend_number, file_number, TOX_FILE_CONTROL_RESUME, &error);
    break;
  case TOX_FILE_CONTROL_CANCEL:
    if (cb_ptr->fp_receive != NULL) {
      dbg_printf ("Closing on line %d\n", __LINE__);
      fclose (cb_ptr->fp_receive);
      cb_ptr->fp_receive = NULL;
    }
    dbg_printf ("Cancelled\n");
    break;
  }
}

//** file send **//
//uint32_t tox_file_send(Tox *tox, uint32_t friend_number, uint32_t kind, uint64_t file_size, const uint8_t *file_id,
//                       const uint8_t *filename, size_t filename_length, TOX_ERR_FILE_SEND *error)
//{
//  DEBUG ();
//}

//bool tox_file_send_chunk(Tox *tox, uint32_t friend_number, uint32_t file_number, uint64_t position, const uint8_t *data,
//                         size_t length, TOX_ERR_FILE_SEND_CHUNK *error)
//{
//  DEBUG ();
//}
//void tox_callback_file_chunk_request(Tox *tox, tox_file_chunk_request_cb *callback);

void file_chunk_request_cb(Tox *tox, uint32_t friend_number, uint32_t file_number, uint64_t position,
                               size_t length, void *user_data)
{
  Tox_cb *cb_ptr = (Tox_cb *)user_data;
  uint8_t *data = (uint8_t *)alloca (length);

  if (length == 0) {
    fclose (cb_ptr->fp_send);
    cb_ptr->fp_send = NULL;
  }
  else {
    fseek (cb_ptr->fp_send, position, SEEK_SET);

    // primitive, FILE is wrong to use with this.
    for (size_t i = 0 ; i < length ; i++) {
      data[i] = fgetc (cb_ptr->fp_send);
    }
    
    // this is for sending a file..
    TOX_ERR_FILE_SEND_CHUNK error;
    tox_file_send_chunk (tox, friend_number, file_number, position, data, length, &error);
    dbg_printf ("%d %d %d\n", file_number, (int)position, (int)length);
  }
  DEBUG ();
}

//** file recv **/
//void tox_callback_file_recv(Tox *tox, tox_file_recv_cb *callback);


void file_recv_cb(Tox *tox, uint32_t friend_number, uint32_t file_number, uint32_t kind, uint64_t file_size,
                  const uint8_t *filename, size_t filename_length, void *user_data)
{
  DEBUG ();

  dbg_printf ("File kind %d\n", kind);
  if (filename != NULL) {
    TOX_ERR_FILE_CONTROL error;
    Tox_cb *cb_ptr = (Tox_cb *)user_data;

    dbg_printf ("Getting request for %s, length %lud - accepting to /dev/null\n", filename, file_size);
    tox_file_control(tox, friend_number, file_number, TOX_FILE_CONTROL_RESUME, &error);

    cb_ptr->fp_receive = fopen ((char *)filename, "w"); // this is wrong, partially because filename isn't necessary null terminated
  }
}

void file_recv_chunk_cb(Tox *tox, uint32_t friend_number, uint32_t file_number, uint64_t position,
                        const uint8_t *data, size_t length, void *user_data)
{
  //DEBUG ();
  size_t iter;
  Tox_cb *cb_ptr = (Tox_cb *)user_data;

  if (cb_ptr->fp_receive != NULL) {
    fseek(cb_ptr->fp_receive, position, SEEK_SET);
    for (iter = 0 ; iter < length ; iter++) {
      fputc (data[iter], cb_ptr->fp_receive);
    }

    if (length == 0 && cb_ptr->fp_receive != NULL) {
      dbg_printf ("Closing on line %d\n", __LINE__);
      fclose (cb_ptr->fp_receive);
      cb_ptr->fp_receive = NULL; 
    }
  }
  else {
    dbg_printf ("file pointer is set to null\n");
  }
}

void friend_name_cb (Tox *tox, uint32_t friend_number, const uint8_t *name, size_t length, void *user_data)
{
  dbg_printf ("Friend %d is known as %*s\n", friend_number, (int)length, name);
}

void friend_status_message_cb(Tox *tox, uint32_t friend_number, const uint8_t *message, size_t length,
                              void *user_data)
{
  dbg_printf ("Status message %d is %*s\n", friend_number, (int)length, message);
}

void friend_status_cb(Tox *tox, uint32_t friend_number, TOX_USER_STATUS status, void *user_data)
{
  dbg_printf ("Status %d is %d\n", friend_number, status);
  // TOX_USER_STATUS_NONE,
  // TOX_USER_STATUS_AWAY,
  // TOX_USER_STATUS_BUSY,
}

void friend_connection_status_cb(Tox *tox, uint32_t friend_number, TOX_CONNECTION connection_status,
                                     void *user_data)
{
  dbg_printf ("Connection status %d is %d\n", friend_number, connection_status);
  //TOX_CONNECTION_NONE,
  //TOX_CONNECTION_TCP,
  //TOX_CONNECTION_UDP,
}

void friend_read_receipt_cb(Tox *tox, uint32_t friend_number, uint32_t message_id, void *user_data)
{
  dbg_printf ("Read receipt for %d message id %d\n", friend_number, message_id);
}

void conference_invite_cb(Tox *tox, uint32_t friend_number, TOX_CONFERENCE_TYPE type, const uint8_t *cookie,
                          size_t length, void *user_data)
{
  DEBUG();
  //TOX_CONFERENCE_TYPE_TEXT,
  //TOX_CONFERENCE_TYPE_AV,

  uint32_t conference_number;
  conference_number =tox_conference_join(tox, friend_number, cookie, length, NULL);
  //TOX_ERR_CONFERENCE_JOIN *error);

  tox_conference_send_message(tox, conference_number, TOX_MESSAGE_TYPE_NORMAL, (const uint8_t*)"message",
                              strlen ("message"), NULL);
  //TOX_ERR_CONFERENCE_SEND_MESSAGE *error);

}

void conference_connected_cb(Tox *tox, uint32_t conference_number, void *user_data)
{
  DEBUG();
}

void conference_message_cb(Tox *tox, uint32_t conference_number, uint32_t peer_number,
                           TOX_MESSAGE_TYPE type, const uint8_t *message, size_t length, void *user_data)
{
  DEBUG();
}

void conference_title_cb(Tox *tox, uint32_t conference_number, uint32_t peer_number, const uint8_t *title,
                         size_t length, void *user_data)
{
  DEBUG();
}

void conference_peer_name_cb(Tox *tox, uint32_t conference_number, uint32_t peer_number,
                             const uint8_t *name, size_t length, void *user_data)
{
  DEBUG();
}

void conference_peer_list_changed_cb(Tox *tox, uint32_t conference_number, void *user_data)
{
  DEBUG();
}

int main(int argc, char **argv)
{
  Tox *tox;
  const uint8_t *status_message = (uint8_t *) "Google sucks";
  uint8_t *name = (uint8_t *) "unknown bot";
  FILE *fp;

  fp = fopen ("/etc/hostname", "r");
  if (fp != NULL) {
    char tmp[200];
    char *ptr;
    fgets (tmp, sizeof (tmp), fp);
    fclose (fp);
    for (ptr = tmp ; *ptr != '\0' && *ptr != '\n'; ptr++) {
      // do nothing
    }
    *ptr = '\0';
    asprintf ((char **)&name, "%s - bot", tmp);
  }

  if (argc == 2) {
    name = (uint8_t *)argv[1];
  }

  tox = create_tox();
  tox_self_set_name(tox, name, strlen((char *)name), NULL);
  tox_self_set_status_message(tox, status_message, strlen((char *)status_message), NULL);

  bootstrap(tox);

  print_tox_id(tox);

  tox_callback_friend_request(tox, friend_request_cb);
  tox_callback_friend_message(tox, friend_message_cb);
  tox_callback_self_connection_status(tox, self_connection_status_cb);
  tox_callback_friend_typing(tox, friend_typing_cb);

  tox_callback_file_recv_control(tox, file_recv_control_cb);
  tox_callback_file_recv(tox, file_recv_cb);
  tox_callback_file_recv_chunk(tox, file_recv_chunk_cb);

  tox_callback_file_chunk_request(tox, file_chunk_request_cb);

  tox_callback_friend_name(tox, friend_name_cb); // this doesn't seem to properly trigger

  tox_callback_friend_status_message(tox, friend_status_message_cb);
  tox_callback_friend_status(tox, friend_status_cb);
  tox_callback_friend_connection_status(tox, friend_connection_status_cb);
  tox_callback_friend_read_receipt(tox, friend_read_receipt_cb);

  tox_callback_conference_invite(tox, conference_invite_cb);
  tox_callback_conference_connected(tox, conference_connected_cb);
  tox_callback_conference_message(tox, conference_message_cb);
  tox_callback_conference_title(tox, conference_title_cb);
  tox_callback_conference_peer_name(tox, conference_peer_name_cb);
  tox_callback_conference_peer_list_changed(tox, conference_peer_list_changed_cb);

  /*

  tox_callback_friend_lossy_packet(Tox *tox, tox_friend_lossy_packet_cb *callback);
  tox_callback_friend_lossless_packet(Tox *tox, tox_friend_lossless_packet_cb *callback);
  */


  update_savedata_file(tox);

  Tox_cb cb("fakesavedata");
  
  for ( ;; ) {
    uint32_t tox_sleep_interval;

    tox_iterate(tox, &cb);
    tox_sleep_interval = tox_iteration_interval(tox);
    if (tox_sleep_interval != 50) {
      dbg_printf ("sleep interval = %d\n", tox_sleep_interval);
    }
    usleep(tox_sleep_interval * 1000);
  }

  tox_kill(tox);

  return 0;
}

