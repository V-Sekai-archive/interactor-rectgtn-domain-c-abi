import Init

namespace RectGTN

/-- Pascal string: `length` (UTF-8 bytes) + `bytes`, exactly like Godot's
    native `String`.  Never null-terminated.  An empty string is
    `{ .length := 0, .bytes := None }` (the `None` encodes the NULL bytes
    pointer). --/
structure PascalString where
  length : UInt32
  bytes : Option (Array Byte)

/-- ISO-8601 duration, canonical `P[nY][nM][nD][W]T[nH][nM][nS]` with an
    optional sub-second fraction, carried as a Pascal string so the fraction
    is exact (no float round-trip). --/
abbrev Duration := PascalString

/-- The intrinsic temporal block of a plan or a single step.  The temporal
    block is computed once per plan and shipped with it -- there is no
    free-standing `temporal(plan)` recompute function (issue #2: "all
    RECTGTNs are temporal plans").  (issue #2: "all RECTGTNs are temporal
    plans") --/
structure Temporal where
  start : Duration      -- C-ABI field name is `start`
  endTime : Duration    -- C-ABI field name is `end` (renamed: `end` is a Lean keyword)
  duration : Duration   -- C-ABI field name is `duration`
  consistent : Bool     -- C-ABI field name is `consistent`

/-- One element of the step sequence.  A step is a temporal step: it carries
    its own `start`/`end`/`duration`/`consistent` block, derived from the
    plan's composed durations. --/
structure Step where
  name : Duration        -- C-ABI field name is `name`
  temporal : Temporal    -- C-ABI field name is `temporal`

/-- A temporal plan.  `total` is the plan-level block: origin, total
    duration, per-step offsets, STN consistency.  On a refusal, `stepCount`
    is 0 and `consistent` is false.  There is no separate temporal side
    channel -- the block is intrinsic and always present (issue #2). --/
structure Plan where
  stepCount : Nat        -- C-ABI field name is `step_count`
  steps : Array Step     -- C-ABI field name is `steps`
  total : Temporal       -- C-ABI field name is `total`
  consistent : Bool      -- C-ABI field name is `consistent`

/-- A composed domain, carried as one JSON-LD Pascal string. --/
abbrev Domain := Duration

end RectGTN
