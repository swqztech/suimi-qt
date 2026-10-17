#include "generate.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <random>
#include <string>

namespace {

std::mt19937& rng() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return gen;
}

const char* LOWER = "abcdefghijklmnopqrstuvwxyz";
const char* UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* DIGITS = "0123456789";
const char* SYMBOLS = "!@#$%^&*()-_=+[]{}|;:,.<>?";
const char* AMBIGUOUS = "0OoIl1";

std::string filter_ambiguous(const char* s, bool exclude) {
  if (!exclude) return s;
  std::string out;
  for (const char* p = s; *p; ++p) {
    if (!strchr(AMBIGUOUS, *p)) out.push_back(*p);
  }
  return out;
}

}  // namespace

std::string password_charset(const PasswordOptions& opts) {
  std::string pool;
  if (opts.lowercase) pool += filter_ambiguous(LOWER, opts.exclude_ambiguous);
  if (opts.uppercase) pool += filter_ambiguous(UPPER, opts.exclude_ambiguous);
  if (opts.digits) pool += filter_ambiguous(DIGITS, opts.exclude_ambiguous);
  if (opts.symbols) pool += SYMBOLS;
  return pool;
}

Strength password_strength(const PasswordOptions& opts) {
  const auto charset = password_charset(opts);
  if (charset.empty() || opts.length <= 0) {
    return {"—", 0.0, 0.0f};
  }
  const double bits = opts.length * (std::log2(static_cast<double>(charset.size())));
  Strength s;
  s.bits = bits;
  s.ratio = static_cast<float>(std::min(bits / 128.0, 1.0));
  if (bits < 40) s.label = "很弱";
  else if (bits < 60) s.label = "弱";
  else if (bits < 80) s.label = "中等";
  else if (bits < 100) s.label = "强";
  else s.label = "很强";
  return s;
}

std::string generate_password(const PasswordOptions& opts) {
  const auto charset = password_charset(opts);
  if (charset.empty() || opts.length <= 0) return {};
  std::uniform_int_distribution<size_t> dist(0, charset.size() - 1);
  std::string out;
  out.reserve(opts.length);
  for (int i = 0; i < opts.length; ++i) {
    out.push_back(charset[dist(rng())]);
  }
  return out;
}

std::string generate_passwords(const PasswordOptions& opts, int count) {
  std::string out;
  for (int i = 0; i < count; ++i) {
    if (i) out += '\n';
    out += generate_password(opts);
  }
  return out;
}

std::string generate_username(int length) {
  length = std::clamp(length, 4, 32);
  const std::string letters = "abcdefghijklmnopqrstuvwxyz";
  const std::string body = "abcdefghijklmnopqrstuvwxyz0123456789_";
  std::uniform_int_distribution<size_t> d_letter(0, letters.size() - 1);
  std::uniform_int_distribution<size_t> d_body(0, body.size() - 1);
  std::string s;
  s.push_back(letters[d_letter(rng())]);
  while (static_cast<int>(s.size()) < length) {
    s.push_back(body[d_body(rng())]);
  }
  return s;
}

static const char* NICK_ADJ[] = {
    "静默", "浅夏", "远山", "微光", "逐风", "清欢", "星落", "云深",
    "听雨", "拾穗", "半盏", "南巷", "北鸢", "青衫", "白茶", "晚钟"};
static const char* NICK_NOUN[] = {
    "旅人", "行者", "墨客", "观星者", "拾梦人", "渡舟", "栖鹤", "听潮",
    "煮茶", "折枝", "问月", "寻鹿", "枕书", "候风", "写序", "归人"};

std::string generate_nickname() {
  std::uniform_int_distribution<int> d_adj(0, 15);
  std::uniform_int_distribution<int> d_noun(0, 15);
  std::bernoulli_distribution d_suffix(0.35);
  const char* adj = NICK_ADJ[d_adj(rng())];
  const char* noun = NICK_NOUN[d_noun(rng())];
  if (d_suffix(rng())) {
    std::uniform_int_distribution<int> d_num(10, 99);
    return std::string(adj) + noun + std::to_string(d_num(rng()));
  }
  return std::string(adj) + noun;
}
