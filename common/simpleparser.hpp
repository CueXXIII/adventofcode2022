#pragma once

#include <cctype>
#include <fstream>
#include <string>

class SimpleParser {
    std::ifstream localStream{};
    std::ifstream &in;

    // buffer will always contain at least 1 char, or eof==true
    std::string buffer;
    size_t pos;
    bool eof;

    void bufferSaturate() {
        while (!eof && pos >= buffer.size()) {
            eof = !std::getline(in, buffer);
            pos = 0;
        }
    }

    char bufferPeek() {
        if (eof) {
            return -1;
        }
        return buffer[pos];
    }

    void bufferNextChar() {
        if (!eof) {
            pos++;
            bufferSaturate();
        }
    }

    char bufferGet() {
        const auto c = bufferPeek();
        bufferNextChar();
        return c;
    }

  public:
    SimpleParser(std::ifstream &);
    SimpleParser(const char *);

    bool isEof() const;

    int64_t getInt64();
    std::string getToken();

    void skipWhitespace();
    bool skipChar(const char);
    bool skipToken(const std::string &);
};

SimpleParser::SimpleParser(std::ifstream &stream)
    : in(stream), buffer(""), pos(0), eof(false) {
    bufferSaturate();
}

SimpleParser::SimpleParser(const char *infile)
    : localStream(std::ifstream(infile)), in(localStream), buffer(""), pos(0), eof(false) {
    bufferSaturate();
}

int64_t SimpleParser::getInt64() {
    skipWhitespace();
    size_t processed;
    const int64_t value = std::stoll(buffer.substr(pos), &processed);
    pos += processed;
    bufferSaturate();
    return value;
}

std::string SimpleParser::getToken() {
    skipWhitespace();
    auto end = pos;
    while (end < buffer.size() && !std::isspace(buffer[end])) {
        ++end;
    }
    const auto token = buffer.substr(pos, end - pos);
    pos = end;
    bufferSaturate();
    return token;
}

bool SimpleParser::isEof() const { return eof; }

void SimpleParser::skipWhitespace() {
    while (!eof && std::isspace(buffer[pos])) {
        bufferNextChar();
    }
}

bool SimpleParser::skipChar(const char c) {
    if (!eof && buffer[pos] == c) {
        bufferNextChar();
        return true;
    }
    return false;
}

// If token follows in the input, skip over it and return true
bool SimpleParser::skipToken(const std::string &token) {
    skipWhitespace();
    if (buffer.find(token.data(), pos, token.size()) == pos) {
        pos += token.size();
        bufferSaturate();
        return true;
    }
    return false;
}
