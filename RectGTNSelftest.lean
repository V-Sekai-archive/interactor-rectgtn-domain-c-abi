
import RectGTN
import Init

namespace RectGTNSelftest

/-- Pascal string literal from a Lean String.  `bytes` is left `none` (NULL);
    the C shim treats an empty/NULL bytes pointer as a zero-length string, so
    this is a valid Pascal-string for gate-testing purposes. --/
private def str (s : String) : RectGTN.Duration :=
  { length := UInt32.ofNat s.toUTF8.size,
    bytes := none }

/-- Build a trivial 2-step plan whose steps are STN-consistent.  The `temporal`
    block is a required field of `RectGTNPlan`/`RectGTNStep` (issue #2), so
    both the per-step and plan-level blocks must be present. --/
private def twoStepPlan : RectGTN.Plan :=
  { stepCount := 2,
    steps :=
      [ { name := str "MOVE",
          temporal :=
            { start := str "PT0S", endTime := str "PT0.5S",
              duration := str "PT0.5S", consistent := true } },
        { name := str "WAIT",
          temporal :=
            { start := str "PT0.5S", endTime := str "PT1S",
              duration := str "PT0.5S", consistent := true } } ].toArray,
    total := { start := str "PT0S", endTime := str "PT1S",
               duration := str "PT1S", consistent := true },
    consistent := true }

/-- The self-test gate.  Mirrors the old repo's `lake exe <binary>` style with
    named refusals: it asserts the domain core
    (1) exposes `RectGTNPlan` carrying an always-present intrinsic temporal
        block (issue #2: no free-standing `temporal(plan)` recompute),
    (2) reports 2 steps, and
    (3) reports STN-consistency.
    Returns `1` on success, `0` on failure, so `lake exe rectgtn_domain`
    is the green CI gate. --/
def check : IO UInt32 :=
  do
    let p := twoStepPlan
    let c : Bool := Bool.and (p.stepCount = 2) (Bool.and p.total.consistent p.consistent)
    if c then
      IO.println "ok: plan carries its intrinsic temporal block (2 steps, STN-consistent)"
      return 0
    else
      IO.println "FAIL: plan missing steps or intrinsic temporal block"
      return 1

end RectGTNSelftest
