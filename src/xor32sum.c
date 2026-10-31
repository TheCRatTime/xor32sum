/*
 * Copyright (C) 2026 TheCRatTime
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <bits/flags.h>
#include <version.h>
#include <xorshift32.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
# define LOG(call) fputs("xor32sum: debug: ", stdout); printf##call
#else
# define LOG(call)
#endif

#define Error(err)      fputs("xor32sum: ", stderr); fputs(err, stderr)
#define ErrPutChar(msg) fputc(msg, stderr)
#define Info(msg)       fputs(msg, stdout)
#define PutChar(msg)    putchar(msg)

static char** ArgvSkip(char** argv, char* str) {
  while (*argv != str && *argv) {
    argv++;
  }

  if (*argv) {
    argv++;
  }

  return argv;
}

static char* GetFirstNonFlag(char** argv) {
  char* saved = NULL;
  while (*argv) {
    if (argv[0][0] != '-') {
      saved = *argv;
      break;
    }

    argv++;
  }

  return saved;
}

static X32S_u8 ParseFlag(char* str) {
  if (strcmp(str, "--help") == 0 ||
      strcmp(str, "-h") == 0 ||
      strcmp(str, "-?") == 0)  {
    return FLAG_HELP;
  }

  if (strcmp(str, "--verify") == 0 ||
      strcmp(str, "--check") == 0) {
    return FLAG_VERIFY;
  }
  
  if (strcmp(str, "--version") == 0 ||
      strcmp(str, "-V") == 0)  {
    return FLAG_VERSION;
  }

  Error("invalid flag: '");
  fputs(str, stderr);
  fputs("'\n", stderr);

  return 0x00;
}

static X32S_u8 ParseArgv(char** argv) {
  X32S_u8 settings = 0x00;
  argv++;
  
  while (*argv) {
    if (argv[0][0] != '-') {
      argv++;
      continue;
    }

    settings |= ParseFlag(*argv);

    argv++;
  }

  return settings;
}

static void Version(void) {
  Info("xor32sum v"XOR32SUM_VERSION" - "XOR32SUM_DESC"\n\
Copyright (C) 2026 TheCRatTime\n");
  LOG(("Debug build\n"));
}

static void Help(void) {
  Info("\
NAME\n\
    xor32sum - calculate hash sum of FILE.\n\
\n\
SYNOPSIS\n\
    xor32sum FILE ...\n\
    xor32sum --verify|--check SIGNATURE_FILE FILE\n\
    xor32sum --version|-V\n\
    xor32sum --help|-h|-?\n\
\n\
DESCRIPTION\n\
    xor32sum is a non crypto hash checker, based on\n\
    Xorshift32 algorithm.\n\
\n\
OPTIONS\n\
    --verify, --check\n\
        Check FILE by hash in SIGNATURE_FILE.\n\
    \n\
    --version, -V\n\
        Show xor32sum version.\n\
    \n\
    --help, -h, -?\n\
        Show this help and exit.\n\
    \n\
\n\
RETURN VALUE\n\
    Return value is:\n\
\n");
  Info("\
    0\n\
         Zero errors, signature correct.\n\
\n\
    1\n\
        Signature not correct.\n\
\n\
    2\n\
        Can't open file / allocate memory.\n\
\n\
You can see manual for more details\n");
}

static char* OpenAndReadFile(const char* filename, long* len) {
  FILE* file = NULL;
  char* mem = NULL;
  size_t read = 0;
  long file_size = 0;

  LOG(("opening %s\n", filename));
  file = fopen(filename, "rb");
  if (!file) {
    Error("can't open '");
    fputs(filename, stderr);
    fputs("'\n", stderr);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (file_size <= 0) {
    Error("invalid file size\n");
    fclose(file);
    return NULL;
  }

  mem = (char*)malloc((size_t)file_size+1);
  if (!mem) {
    Error("can't allocate memory\n");
    fclose(file);
    return NULL;
  }

  LOG(("allocated %ld bytes\n", file_size+1));

  read = fread(mem, 1, (size_t)file_size, file);
  if (read != (size_t)file_size) {
    Error("bad read from file\n");
    free(mem);
    fclose(file);
    return NULL;
  }

  mem[file_size] = '\0';
  *len = file_size;

  fclose(file);
  
  return mem;
}

static const char HexLookup[16] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

static void PrintByteInHex(const char byte) {
  PutChar(HexLookup[byte & 0x0F]);
  PutChar(HexLookup[(byte & 0xF0) >> 4]);
}

static void ToByteInHex(const char byte, char* out) {
  out[0] = HexLookup[byte & 0x0F];
  out[1] = HexLookup[(byte & 0xF0) >> 4];
}

static void PrintHex(X32S_u32 num) {
  PrintByteInHex((num >> 24) & 0xFF);
  PrintByteInHex((num >> 16) & 0xFF);
  PrintByteInHex((num >> 8) & 0xFF);
  PrintByteInHex(num & 0xFF);
}

static void ToHex(X32S_u32 num, char* out) {
  ToByteInHex((num >> 24) & 0xFF, out);
  ToByteInHex((num >> 16) & 0xFF, out+2);
  ToByteInHex((num >> 8) & 0xFF, out+4);
  ToByteInHex(num & 0xFF, out+6);
  out[8] = '\0';
}

static int DefaultHandle(char** argv) {
  X32S_u32 hash = 0;
  char* name = NULL;
  char* mem = NULL;

  argv++;

  name = GetFirstNonFlag(argv);
  if (!name) {
    Error("expected filename\n");
    return 1;
  }

  argv = ArgvSkip(argv, name);

  while (name != NULL) {
    long len = 0;
    mem = OpenAndReadFile(name, &len);
    if (!mem) {
      Error("error ");
      fputs(name, stderr);
      ErrPutChar('\n');
      name = GetFirstNonFlag(argv);
      argv = ArgvSkip(argv, name);
      continue;
    }

    hash = GetXor32Sum(mem, (size_t)len);
    free(mem);

    PrintHex(hash);
    PutChar(' ');
    Info(name);
    PutChar('\n');

    LOG(("successfully calculated sum for %s\n", name));

    name = GetFirstNonFlag(argv);
    argv = ArgvSkip(argv, name);
  }

  return 0;
}

static int VerifyHandle(int argc, char** argv) {
  char hash[9] = {0};
  char* signature = NULL;
  char* name = NULL;
  long signature_len = 0;
  long name_len = 0;
  char* signature_name = NULL;
  char* mem = NULL;
  int code = 0;
  
  if (argc < 4) {
    Error("expected 4 arguments, got ");
    fprintf(stderr, "%d\n", argc);
    return 1;
  }

  argv++;

  signature_name = GetFirstNonFlag(argv);
  if (!signature_name) {
    Error("expected sign-file name\n");
    return 1;
  }

  argv = ArgvSkip(argv, signature_name);
  
  name = GetFirstNonFlag(argv);
  if (!name) {
    Error("expected file name\n");
    return 1;
  }

  mem = OpenAndReadFile(name, &name_len);
  if (!mem) {
    return 2;
  }

  signature = OpenAndReadFile(signature_name, &signature_len);
  if (!signature) {
    return 2;
  }

  ToHex(GetXor32Sum(mem, (size_t)name_len), hash);
  if (strncmp(hash, signature, 8)) {
    Info("FAILED\n");
    code = 1;
  } else {
    Info("OK\n");
  }

  LOG(("verify exiting with code %u\n", code));
  
  return code;
}

int main(int argc, char** argv) {
  X32S_u8 settings = 0x00;
  
  if (argc < 2) {
    Help();
    return 1;
  }

  settings = ParseArgv(argv);

  if (settings & FLAG_HELP) {
    Help();
    return 0;
  } else if (settings & FLAG_VERSION) {
    Version();
    return 0;
  } else if (settings & FLAG_VERIFY) {
    return VerifyHandle(argc, argv);
  } else {
    return DefaultHandle(argv);
  }

  /* return 0; */
} 
