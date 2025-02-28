#include <libgccjit.h>
#include <Tracing/Tracing.h>

#include "LibgccjitMgr.h"

static struct TraceContext TC;

int
main() {
  init_tracing(&TC, stdout, "Main");

  t0(&TC, "Hello, %s!", "world");

  Libgccjit::Mgr mgr{};

  gcc_jit_context* ctx = mgr.m_ctx->raw();
  gcc_jit_type* int_type = mgr.m_type_mgr.get_type("c_int");
  gcc_jit_function *func =
    gcc_jit_context_new_function (ctx, NULL,
                                  GCC_JIT_FUNCTION_EXPORTED,
                                  int_type,
                                  "foo",
                                  0, NULL,
                                  0);
  gcc_jit_block* block = gcc_jit_function_new_block (func, NULL);
  gcc_jit_rvalue* x1 = gcc_jit_context_new_rvalue_from_int(ctx, int_type, 2);
  gcc_jit_rvalue* x2 = gcc_jit_context_new_rvalue_from_int(ctx, int_type, 3);
  gcc_jit_rvalue* expr =
    gcc_jit_context_new_binary_op(ctx, NULL,
                                  GCC_JIT_BINARY_OP_MULT, int_type,
                                  x1, x2);
  gcc_jit_block_end_with_return(block, NULL, expr);

  gcc_jit_result* result = gcc_jit_context_compile(ctx);
  void* fn_ptr = gcc_jit_result_get_code(result, "foo");
  typedef int (*fn_type) (void);
  fn_type foo = (fn_type)fn_ptr;
  t0(&TC, "result: %d", foo());
  gcc_jit_result_release(result);

  deinit_tracing(&TC);
  return 0;
}
