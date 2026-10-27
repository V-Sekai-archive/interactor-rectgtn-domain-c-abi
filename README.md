# interactor-rectgtn-domain-c-abi

The RECTGTN **domain C-ABI** for the IEC 61131-3 function-block-diagram (FBD) /
gltf2-interactivity taskweft. This is the guest program that the **RISC-V
Linux guest** (godot-sandbox / libriscv) loads and runs: a pure-Lean
reference model compiled to a `.c` intermediate, exposed through the C-ABI
`rectgtn_api.h`.

## What it is

- A pure-Lean domain model (`lean/`) for RECTGTN — Relationship-Enabled
  Capability-Temporal Goal-Task-Network — with formal properties.
- A **C-ABI** (`api/rectgtn_api.h`) the domain is exposed through, built for
  the RISC-V guest.
- A **guest C shim** (`guest/`) — the `_start` entry point and the
  I/O glue that links the Lean `.c` intermediate into a stripped
  RISC-V64 Linux executable the sandbox loads.

## Two invariants

1. **Pascal strings.** Every string across the C-ABI is length + bytes, exactly
   like Godot's native `String` — never null-terminated (issue #1).
2. **Temporal is intrinsic.** Every plan is a temporal plan; the temporal
   block is a required member of `RectGTNPlan`, not a separate
   `temporal(plan)` recompute function (issue #2).

## Build (host)

```sh
lean  # Lean 4 toolchain (4.35+)
lake build
lake test            # native reference-model tests
```

## Build the guest ELF

```sh
riscv64-linux-gnu-gcc --version
riscv64-linux-gnu-g++ --version
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=guest/toolchain.cmake \
  -DCMAKE_BUILD_TYPE=MinSizeRel -DSTRIPPED=ON -S guest
cmake --build build
file build/rectgtn_domain.elf
```

The artifact is a stripped, position-independent **RISC-V64 Linux ELF** with a
`_start` entry, mirroring `godot-sandbox`'s `gdscript.elf` build.

## Subtree

This repo is designed to be pulled as a subtree by `interactor-taskweft`, which
deletes its own C implementation and uses this tree. The C-ABI is the contract;
the Lean reference model is authoritative and the guest links the Lean
`.c` intermediate into `rectgtn.elf`.

## License

MIT.
