#pragma once

#include <string>

enum class Mode { Password, Username, Nickname };

struct PasswordOptions {
  int length = 20;
  bool lowercase = true;
  bool uppercase = true;
  bool digits = true;
  bool symbols = true;
  bool exclude_ambiguous = true;
};

struct Strength {
  const char* label = "—";
  double bits = 0.0;
  float ratio = 0.0f;
};

std::string password_charset(const PasswordOptions& opts);
Strength password_strength(const PasswordOptions& opts);
std::string generate_password(const PasswordOptions& opts);
std::string generate_passwords(const PasswordOptions& opts, int count);
std::string generate_username(int length);
std::string generate_nickname();
