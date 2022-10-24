GLOBAL _xchg

_xchg:
  mov rax, rsi
  xchg [rdi], eax
  ret