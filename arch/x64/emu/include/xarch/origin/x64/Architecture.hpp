#pragma once

#include <functional>
#include <xwm/core.hpp>

#include <iostream>

namespace xarch::origin {

class xarch_origin_x64;

using InstructionPtr = std::function<void(xarch_origin_x64 *)>;

using namespace xwm;

class xarch_origin_x64 : public xwm::Architecture {
protected:
  Map<u16, InstructionPtr> instruction_set;

  u32 exit_status = 0;
  bool interrupt = false;

public:
  xarch_origin_x64(Dispatcher &dispatcher) : xwm::Architecture(dispatcher) {}
  ~xarch_origin_x64() override {}
  u32 run() override {
    do {
      fetch();
      decode();
      execute();
    } while (!this->interrupt);

    return this->exit_status;
  }

  void fetch();
  void decode();
  void execute();
};

} // namespace xarch::origin
