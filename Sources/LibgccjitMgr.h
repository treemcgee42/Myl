#pragma once

#include <libgccjit.h>
#include <string>
#include <unordered_map>

namespace Libgccjit {
  class Ctx {
  public:
    Ctx();
    ~Ctx();

    gcc_jit_context* raw();

  private:
    gcc_jit_context* m_raw_ctx;
  };

  class TypeMgr {
  public:
    TypeMgr(std::shared_ptr<Ctx> ctx);

    gcc_jit_type* get_type(const char* name) const;

  private:
    void create_default_types();

    std::unordered_map<std::string, gcc_jit_type*> m_types;
    std::shared_ptr<Ctx> m_ctx;
  };

  class Mgr {
  public:
    Mgr();
    ~Mgr();

    std::shared_ptr<Ctx> m_ctx;
    TypeMgr m_type_mgr;
  };

}; // namespace Libgccjit
