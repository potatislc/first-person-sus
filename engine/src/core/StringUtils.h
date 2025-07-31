#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace Engine {
    using size_type = std::string::size_type;

    static void removeLineComment(std::string& line, const std::string& commentChar) {
        if (const auto commentStart = line.find(commentChar); commentStart != std::string::npos) {
            line.resize(commentStart);
        }
    }

    inline void ltrim(std::string_view& s, const std::string& predicate = " \t\n\r\f\v;") {
        s.remove_prefix(std::min(s.find_first_not_of(predicate), s.size()));
    }

    inline std::vector<std::string_view>
    tokenize(std::string_view line, const std::string& predicate = " \t\n\r\f\v;") {
        std::vector<std::string_view> tokens;
        ltrim(line, predicate);

        while (!line.empty()) {
            if (const size_type tokenEnd = line.find_first_of(predicate); tokenEnd != std::string::npos) {
                tokens.emplace_back(line.substr(0, tokenEnd));
                line.remove_prefix(tokenEnd);
            } else {
                tokens.emplace_back(line);
                break;
            }

            ltrim(line, predicate);
        }

        return tokens;
    }
}
