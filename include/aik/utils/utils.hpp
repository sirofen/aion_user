#pragma once

// winapi
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>
#include <tlhelp32.h>
#include <ntstatus.h>

// C++ STL
#include <string>
#include <iostream>
#include <bitset>

#pragma comment(lib, "ntdll.lib")

typedef std::basic_string<unsigned char> ustring;

inline void _print_bytes_line(const std::string_view arr, const std::uint64_t base, std::uint32_t n_lm_l, const char delim) {
    for (std::size_t i = 0, arr_sz = arr.size(); i < arr_sz; i++) {
        bool last_iter = (i + 1) == arr_sz;
        if (i % n_lm_l == 0 || last_iter) {
            std::size_t iter_sz_part;
            if (last_iter) {
                iter_sz_part = i - arr_sz % n_lm_l + 1;
                printf_s("%02X%c", arr[i], delim);
                std::cout << std::string((n_lm_l - (arr_sz % n_lm_l)) * 3, ' ');
            } else {
                iter_sz_part = i - n_lm_l;
            }
            for (std::size_t j = 0; i != 0 && j < n_lm_l && j + iter_sz_part < arr_sz; j++) {
                const auto& _c = arr[j + iter_sz_part];
                printf_s(j == 0 ? "\t%c" : "%c", std::isgraph(_c) || _c == 0x20 ? _c : '.');
            }
            if (!last_iter) {
                printf_s("\n0x%llX:\t", base + i);
            }
        }
        if (!last_iter) {
            printf_s("%02X%c", arr[i], delim);
        }
    }
    printf_s("\n");
}

//https://stackoverflow.com/a/33447587/14073801
template<typename I>
std::string to_hex_string(I w, size_t hex_len = sizeof(I) << 1) {
    static const char* digits = "0123456789ABCDEF";
    std::string rc(hex_len, '0');
    for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
        rc[i] = digits[(w >> j) & 0x0f];
    return rc;
}
template<typename I>
std::wstring to_hex_wstring(I w, size_t hex_len = sizeof(I) << 1) {
    static const char* digits = "0123456789ABCDEF";
    std::wstring rc(hex_len, '0');
    for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
        rc[i] = digits[(w >> j) & 0x0f];
    return rc;
}

typedef enum _KEY_VALUE_INFORMATION_CLASS {
	KeyValueBasicInformation,
	KeyValueFullInformation,
	KeyValuePartialInformation,
	KeyValueFullInformationAlign64,
	KeyValuePartialInformationAlign64,
	KeyValueLayerInformation,
	MaxKeyValueInfoClass  // MaxKeyValueInfoClass should always be the last enum
} KEY_VALUE_INFORMATION_CLASS;

typedef struct _KEY_VALUE_FULL_INFORMATION {
	ULONG   TitleIndex;
	ULONG   Type;
	ULONG   DataOffset;
	ULONG   DataLength;
	ULONG   NameLength;
	WCHAR   Name[1];            // Variable size
//          Data[1];            // Variable size data not declared
} KEY_VALUE_FULL_INFORMATION, * PKEY_VALUE_FULL_INFORMATION;


#ifdef __cplusplus
extern "C++"
{
	char _RTL_CONSTANT_STRING_type_check(const char* s);
	char _RTL_CONSTANT_STRING_type_check(const WCHAR* s);
	// __typeof would be desirable here instead of sizeof.
	template <size_t N> class _RTL_CONSTANT_STRING_remove_const_template_class;
template <> class _RTL_CONSTANT_STRING_remove_const_template_class<sizeof(char)> { public: typedef  char T; };
template <> class _RTL_CONSTANT_STRING_remove_const_template_class<sizeof(WCHAR)> { public: typedef WCHAR T; };
#define _RTL_CONSTANT_STRING_remove_const_macro(s) \
    (const_cast<_RTL_CONSTANT_STRING_remove_const_template_class<sizeof((s)[0])>::T*>(s))
}
#else
char _RTL_CONSTANT_STRING_type_check(const void* s);
#define _RTL_CONSTANT_STRING_remove_const_macro(s) (s)
#endif
#define RTL_CONSTANT_STRING(s) \
{ \
    sizeof( s ) - sizeof( (s)[0] ), \
    sizeof( s ) / (sizeof(_RTL_CONSTANT_STRING_type_check(s))), \
    _RTL_CONSTANT_STRING_remove_const_macro(s) \
}

extern "C" {
	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwQueryValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			_Out_writes_bytes_to_opt_(Length, *ResultLength) PVOID KeyValueInformation,
			_In_ ULONG Length,
			_Out_ PULONG ResultLength
		);

	NTSYSAPI
        NTSTATUS
        NTAPI
        ZwDeleteValueKey(
                _In_ HANDLE KeyHandle,
                _In_ PUNICODE_STRING ValueName
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwClose(
			_In_ HANDLE Handle
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwOpenKey(
			_Out_ PHANDLE KeyHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_ POBJECT_ATTRIBUTES ObjectAttributes
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwQueryValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			_Out_writes_bytes_to_opt_(Length, *ResultLength) PVOID KeyValueInformation,
			_In_ ULONG Length,
			_Out_ PULONG ResultLength
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwSetValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_opt_ ULONG TitleIndex,
			_In_ ULONG Type,
			_In_reads_bytes_opt_(DataSize) PVOID Data,
			_In_ ULONG DataSize
		);

	NTSYSAPI NTSTATUS ZwCreateKey(
		PHANDLE            KeyHandle,
		ACCESS_MASK        DesiredAccess,
		POBJECT_ATTRIBUTES ObjectAttributes,
		ULONG              TitleIndex,
		PUNICODE_STRING    Class,
		ULONG              CreateOptions,
		PULONG             Disposition
	);
}

namespace utils {
inline NTSTATUS
write_registry(HKEY root_key, const std::string& sub_key, const std::string& val_name, PVOID val_addr, DWORD type, DWORD size) {
    return RegSetKeyValueA(root_key, sub_key.c_str(), val_name.c_str(), type, val_addr, size);
}

inline NTSTATUS
read_registry(HKEY root_key, const std::string& sub_key, const std::string& val_name, PVOID res, LPDWORD type, LPDWORD size) {
    return RegGetValueA(root_key, sub_key.c_str(), val_name.c_str(), RRF_RT_ANY, type, res, size);
}

inline DWORD get_process_pid(const std::wstring& process_name, bool ignore_case) {
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W process;

    process.dwSize = sizeof(process);
    if (!Process32FirstW(snapshot, &process)) {
        CloseHandle(snapshot);
        return 0;
    }

    if (ignore_case) {
        do {
            if (_wcsicmp(process.szExeFile, process_name.c_str()) == 0) {
                pid = process.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &process));
    } else {
        do {
            if (wcsstr(process.szExeFile, process_name.c_str()) == 0) {
                pid = process.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &process));
    }

    CloseHandle(snapshot);
    return pid;
}

std::uint64_t find_pattern(const char* buffer, std::uint64_t base, std::uint64_t size, const std::string_view& pattern, const std::bitset<256>& mask);
}
