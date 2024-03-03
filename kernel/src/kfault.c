#include "klib.h"
#include "cte.h"

//#define EXCEPTION_DEBUG // uncomment me if need

#ifdef EXCEPTION_DEBUG

struct dbgctx {
  uint32_t eip;
  uint32_t irq;
  uint32_t errcode;
  uint32_t cr2;
} dbgctx;

void dummy_debugger_kern(Context *ctx) {
  while (1) ; // set a breakpoint at me
}

void dummy_debugger_user(Context *ctx) {
  printf("Abort @ Exception #%d @ 0x%p, in user\n", dbgctx.irq, dbgctx.eip);
  panic("User Exception");
}

void prepare_dbgctx(Context *ctx) {
  dbgctx.eip = ctx->eip;
  dbgctx.irq = ctx->irq;
  dbgctx.errcode = ctx->errcode;
  dbgctx.cr2 = get_cr2();
}

void ret_to_kdbg(Context *ctx) {
  printf("Abort @ Exception #%d @ 0x%p, in kernel\n", dbgctx.irq, dbgctx.eip);
  printf("To get info about exception site, "
         "set a breakpoint at [kernel/src/kfault.c:16] dummy_debugger_kern\n");
  Context new_ctx = *ctx;
  new_ctx.eip = (uint32_t)dummy_debugger_kern;
  new_ctx.eflags &= ~FL_IF;
  new_ctx.esp = dbgctx.eip;
  new_ctx.ss = (uint32_t)ctx;
  irq_iret(&new_ctx);
}

void exception_debug_handler(Context *ctx) {
  prepare_dbgctx(ctx);
  if (ctx->cs & 3) {
    dummy_debugger_user(ctx);
  } else {
    ret_to_kdbg(ctx);
  }
  panic("Oops, you cannot go here!");
}

#else

void exception_debug_handler(Context *ctx) {}

#endif
