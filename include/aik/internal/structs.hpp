#pragma once

namespace aik::internal {
using NTSTATUS  = long;
using HANDLE    = void*;
using PVOID     = void*;
using UINT      = unsigned int;
using DWORD     = unsigned long;
using PDWORD    = DWORD*;
using ULONGLONG = std::uint64_t;
using WCHAR     = wchar_t;
using UINT_PTR  = std::uint64_t;
using BOOL      = bool;

namespace constants {
inline constexpr auto kModulePagesAmount = 0x200;
inline constexpr auto kPagesArrSz        = 0xFFFF;
}

enum request_type : UINT {
    kWrite,
    kRead,
    kProtect,
    kAlloc,
    kFree,
    kPages,
    kModule,
    kMainbase,
    kThreadCall,
};

typedef struct _request_data {
    UINT type;
    PVOID args;
    NTSTATUS* status;
} request_data, *prequest_data;

typedef struct _request_write {
    DWORD pid;
    PVOID dest;
    PVOID src;
    DWORD size;
    BOOL physical_mode;
} request_write, *prequest_write;

typedef struct _request_read {
    DWORD pid;
    PVOID dest;
    PVOID src;
    ULONGLONG size;
    BOOL physical_mode;
} request_read, *prequest_read;

typedef struct _request_protect {
    DWORD pid;
    PVOID addr;
    DWORD size;
    PDWORD in_out_protection;
} request_protect, *prequest_protect;

typedef struct _request_alloc {
    DWORD pid;
    PVOID res_addr;
    DWORD size;
    DWORD protection;
} request_alloc, *prequest_alloc;

typedef struct _request_free {
    DWORD pid;
    PVOID addr;
} request_free, *prequest_free;

typedef struct _request_pages {
    DWORD pid;
    PVOID begin_addr;
    ULONGLONG size;
    page* pages;
} request_pages, *prequest_pages;

typedef struct _request_module {
    DWORD pid;
    WCHAR module[0xFF];
    UINT_PTR* res_addr;
    DWORD* res_size;
    bool list_pages;
    page* pages;
} request_module, *prequest_module;

typedef struct _request_mainbase {
    DWORD pid;
    UINT_PTR* res_addr;
} request_mainbase, *prequest_mainbase;
}// namespace aik_process::internal