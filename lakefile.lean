import Lake
open Lake DSL

package «rectgtn-domain-c-abi» where

lean_lib RectGTN where
  roots := #[`RectGTN, `RectGTNSelftest]

@[default_target]
lean_exe rectgtn_domain where
  root := `Main
