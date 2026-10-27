import RectGTNSelftest

/-- The domain entrypoint. `main` returns the self-test exit code:
    `1` on a green gate, `0` on a failure.  This is the Lean 4.35 entrypoint
    type for `lean_exe` (`IO UInt32`), matching the old repo's
    `lake exe taskweft_fbd_compiler` style. --/
def main : IO UInt32 :=
  RectGTNSelftest.check
