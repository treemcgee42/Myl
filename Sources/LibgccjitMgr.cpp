
#include <cassert>
#include "LibgccjitMgr.h"
#include "Tracing/Tracing.h"

static struct TraceContext TC;

namespace Libgccjit {
  Ctx::Ctx() {
    m_raw_ctx = gcc_jit_context_acquire();
    assert(m_raw_ctx != NULL);
  }

  Ctx::~Ctx() {
    gcc_jit_context_release(m_raw_ctx);
  }

  gcc_jit_context*
  Ctx::raw() {
    return m_raw_ctx;
  }

  TypeMgr::TypeMgr(std::shared_ptr<Ctx> ctx)
    : m_ctx(ctx) {
    // t0(&TC, "");

    create_default_types();
  }

  gcc_jit_type*
  TypeMgr::get_type(const char* name) const {
    auto f = m_types.find(name);
    assert(f != m_types.end());
    return f->second;
  }

  void
  TypeMgr::create_default_types() {
    // t0(&TC, "");
    m_types["c_int"] = gcc_jit_context_get_type (m_ctx->raw(), GCC_JIT_TYPE_INT);
    m_types["c_void"] = gcc_jit_context_get_type (m_ctx->raw(), GCC_JIT_TYPE_VOID);
    m_types["c_const_char_ptr"] =
      gcc_jit_context_get_type (m_ctx->raw(), GCC_JIT_TYPE_CONST_CHAR_PTR);
  }

  Mgr::Mgr()
    : m_ctx(std::make_shared<Ctx>()),
      m_type_mgr(TypeMgr(m_ctx)) {
    init_tracing(&TC, stdout, "TypeMgr");
    // t0(&TC, "");
  }

  Mgr::~Mgr() {
    // t0(&TC, "");
    deinit_tracing(&TC);
  }
} // namespace Libgccjit
