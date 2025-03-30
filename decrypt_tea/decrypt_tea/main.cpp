#include <windows.h>
#include <iostream>
#include <cstring>

#include "tea_chars.h"

#include "sub_140001000.hpp"
#include "shellcode.hpp"

// 用于 传入要加密的数据和密钥， 接收加密后的数据 的缓冲区。
int password_data[] = { 0x00, 0x00, 0x41, 0x43, 0x45, 0x36 };
// 字符集。
const char word[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ1234567890!@+/";

int main()
{
    // 申请代码空间并执行动态修补。
    unsigned char* sub_140001000 = (unsigned char*)VirtualAlloc(0, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    unsigned char* shellcode = (unsigned char*)VirtualAlloc(0, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    memcpy(sub_140001000, sub_140001000_bytes, sizeof(sub_140001000_bytes));
    memcpy(shellcode, shellcode_bytes, sizeof(shellcode_bytes));

    unsigned char new_bytes[] = { 0x50, 0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xe0 };
    *(__int64*)(new_bytes + 3) = (__int64)shellcode;
    
    memcpy(sub_140001000 + 0x56, new_bytes, sizeof(new_bytes));

    *(__int64*)(shellcode + 0x2d + 2) = (__int64)sub_140001000 + 0x77;
    *(__int64*)(shellcode + 0x37 + 2) = (__int64)sub_140001000 + 0x34;

    printf("解密 tea_chars 得到的 base58 编码串:\n");
    // 循环暴力匹配密文。
    for (int n = 0; n < sizeof(tea_chars) / 8; n++) {
        char xorkey_a = 0;
        char xorkey_b = 0;

        // 根据字符位置切换异或密钥。
        switch (((n * 2) % 3)) {
        case 0:
            xorkey_a = 's';
            xorkey_b = 'x';
            break;
        case 1:
            xorkey_a = 'x';
            xorkey_b = 'x';
            break;
        case 2:
            xorkey_a = 'x';
            xorkey_b = 's';
            break;
        }

        bool found = false;
        int ci = 0, cj = 0;

        for (char i = 0; i < sizeof(word); i++) {
            for (char j = 0; j <= sizeof(word); j++) {
                password_data[0] = word[i] ^ xorkey_a;
                password_data[1] = word[j] ^ xorkey_b;

                // 调用 sub_140001000。
                ((__int64(*)(unsigned __int64 a1, unsigned __int64 a2))sub_140001000)((unsigned __int64)password_data, (unsigned __int64)password_data + 8);

                // 如果匹配，则记录匹配的数据并跳出循环。
                if (
                    password_data[0] == *((int*)tea_chars + n * 2) &&
                    password_data[1] == *((int*)tea_chars + n * 2 + 1)
                    ) {

                    found = true;
                    ci = i; cj = j;
                    break;
                }
            }
            if (found) break;
        }

        // 输出得到的字符。
        printf("%c%c", word[ci], word[cj]);
    }
    printf("\n");

    VirtualFree(sub_140001000, 0, MEM_RELEASE);
    VirtualFree(shellcode, 0, MEM_RELEASE);

    system("pause");

    return 0;
}