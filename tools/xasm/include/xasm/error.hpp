#pragma once

#include <string>
#include <expected>
#include <format>

#define fmt(...) std::format(__VA_ARGS__)

namespace xasm {

  struct FatalError {
    std::string message;
    FatalError(const char* msg) : message(msg) {}
    FatalError(std::string msg) : message(msg) {}
  };

  template<typename T>
  T unwrap_or_fatal(std::expected<T, xasm::FatalError>&& e) {
    if (!e) {
      std::cerr << std::format("[\033[31mERROR\033[0m] ") << e.error().message << "\n";
      std::exit(1);
    }
    return *e;
  }

  
}

#define Err(...) return std::unexpected{FatalError(fmt(__VA_ARGS__))};
