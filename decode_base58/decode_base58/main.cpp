#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

// 从 ACEFirstRound.exe:sub_140001000 提取的 base58 字符集。
static const char* pszBase58 = "abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ1234567890!@+/";

std::vector<unsigned char> DecodeBase58Disassembly(const std::string& str) {
    // 首先反转输入字符串（因为编码时最后进行了反转）。
    std::string input = str;
    std::reverse(input.begin(), input.end());

    // 去掉末尾的'@'字符。
    if (!input.empty() && input.back() == '@') input.pop_back();

    // 创建Base58字符到索引的映射。
    int8_t b58map[256];
    memset(b58map, -1, sizeof(b58map));
    for (int i = 0; i < 58; i++)  b58map[(unsigned char)pszBase58[i]] = i;

    // 计算前导字符（编码结果的第一个字符）的数量。
    size_t zeroes = 0;
    for (size_t i = 0; i < input.size() && input[i] == pszBase58[0]; i++) zeroes++;

    // 为结果分配空间。
    std::vector<unsigned char> result((input.size() - zeroes) * 733 / 1000 + 1);
    size_t length = 0;

    // 处理每个输入字符。
    for (size_t i = zeroes; i < input.size(); i++) {
        // 如果字符不在字符集中，返回空结果。
        if (b58map[(unsigned char)input[i]] == -1) return std::vector<unsigned char>();

        int carry = b58map[(unsigned char)input[i]];

        // 应用基数转换算法。
        for (size_t j = 0; j < length; j++) {
            carry += (int)result[j] * 58;
            result[j] = carry & 0xff;
            carry >>= 8;
        }

        while (carry > 0) {
            result[length++] = carry & 0xff;
            carry >>= 8;
        }
    }

    // 去除前导零（处理大端序）。
    std::vector<unsigned char> final_result;
    final_result.assign(zeroes, 0);

    // 反转result并附加到final_result（因为我们现在是以小端序构建的）。
    for (int i = length - 1; i >= 0; i--) {
        final_result.push_back(result[i]);
    }

    return final_result;
}

std::string DecodeBase58DisassemblyToString(const std::string& str) {
    std::vector<unsigned char> decoded = DecodeBase58Disassembly(str);

    // 转换为字符串（去掉尾部的空字符）。
    if (decoded.empty()) return "";

    // 确保数据以null结尾。
    if (decoded.back() != 0) decoded.push_back(0);

    return std::string(reinterpret_cast<char*>(decoded.data()));
}

int main() {
    std::string Decoded = DecodeBase58DisassemblyToString("@PksUn39kYj763ggA1HLBUCaWSZv4vs4CwSevAnQEs");
    printf("以自定义字符集的 base58 解码 PksUn39kYj763ggA1HLBUCaWSZv4vs4CwSevAnQEs 得到的结果:\n%s\n", Decoded.c_str());

    system("pause");
    return 0;
}