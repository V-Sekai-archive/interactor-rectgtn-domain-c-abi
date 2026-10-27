/* rectgtn_api.h — C surface for the RECTGTN HTN planner.

   Lives INSIDE the guest ELF (riscv64-linux, godot-sandbox / libriscv).
   The domain and all logic are pure Lean; on `lean build` they emit the
   .c intermediate this header declares. The host reads a plan the same way
   it reads the taskweft Elixir API: JSON in, JSON out.

   SPDX-License-Identifier: MIT
   Copyright (c) 2026 K. S. Ernest (iFire) Lee

   ------------------------------------------------------------------------

   API surface (mirrors Taskweft.MCP / Taskweft CLI):

     plan(domain)                      -> bare JSON array of steps
     replan(domain, plan, fail_step)   -> {fail_step, completed_steps,
                                          plan, temporal}
     validate(domain)                  -> {ok, ...}
     compose(domain, [overlays])       -> composed domain (JSON-LD)
     temporal(plan)                    -> temporal block (from action durations)
     rebac_check(graph, expr, subj, obj) -> bool (the capability guard)
     pointer_set(domain, "/key/path", value) -> domain
     pointer_get(domain, "/key/path")  -> variant
     version()                         -> "0.0.1"

   JSON string API (host-facing; null = empty / success)
   ------------------------------------------------------------------------
 */

#ifndef RECTGTN_API_H
#define RECTGTN_API_H

#if __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

/* ----------------------------------------------------------------------
 *  Version
 * ---------------------------------------------------------------------- */
const char *rectgtn_version(void);

/* ----------------------------------------------------------------------
 *  Domain lifecycle (JSON-LD in / out). A domain is one RECTGTN document
 *  (@type domain:Definition | domain:Problem | domain:MetaDomain, `name`
 *  required). A problem is a plain overlay: compose(domain, problem) is the
 *  whole "problem" the planner solves. Later documents win; a non-empty
 *  todo_list replaces. (Merge rules: docs/rectgtn.md "Composing several
 *  documents".)
 * ---------------------------------------------------------------------- */
/* Build a domain from one or more JSON-LD strings. `overlays[0..n-1]`
 *   compose over `base` left to right.  n may be 1 (just the base).
 *   Returns a NUL-terminated C string (caller must rectgtn_free it). */
char *rectgtn_domain_compose(const char **overlays, size_t n);

/* Validate one document (or an already-composed one). Returns a JSON object
 *   {"ok": true/false, "reason": "..."} or null on a hard parse crash. */
char *rectgtn_domain_validate(const char *domain);

/* ----------------------------------------------------------------------
 *  Planning
 * ---------------------------------------------------------------------- */
/* plan(domain) -> the bare JSON array of steps, e.g.
 *      [["a_fly","drone_1","city"], ["do_a"]]
 *   or null if no plan (a refusal: the domain has no alternative that
 *   satisfies every task — mirrors Taskweft.plan's no_plan). */
char *rectgtn_plan(const char *domain);

/* plan_envelope(domain) -> {"plan": [...], "temporal": {...}}.
 *   The full plan response as Taskweft returns it, with the temporal
 *   block (consistent, origin, total, steps[]) computed from action
 *   durations (ISO 8601; missing -> PT0S). */
char *rectgtn_plan_envelope(const char *domain);

/* replan(domain, plan_json, fail_step) ->
 *      {"fail_step": i, "completed_steps": k, "plan": [...],
 *       "temporal": {...}}
 *   Re-plan the domain, failing to execute at 0-based step `fail_step`
 *   (the first step that failed).  `fail_step == -1` replans the whole
 *   plan as complete (the soundness contract: replan with fail_step == -1
 *   reports the whole plan complete). */
char *rectgtn_replan(const char *domain, const char *plan_json,
                     long fail_step);

/* ----------------------------------------------------------------------
 *  Temporal
 * ---------------------------------------------------------------------- */
/* temporal(plan) -> {"consistent":bool,"origin":"PT0S","total":"PT45M",
 *      "steps":[{"action":"a","duration":"PT5M","start":"PT0S",
 *                "end":"PT5M"}, ...]}
 *   from the bare-step array (action duration from the composed domain;
 *   default PT0S). */
char *rectgtn_temporal(const char *plan_json);

/* ----------------------------------------------------------------------
 *  ReBAC  (capabilities/relationship graph, ADR 0004)
 * ---------------------------------------------------------------------- */
/* A relation expression is a JSON string of the form:
 *     {"rel": "OWNS", "pivot": {"rel":"IS_MEMBER_OF"}, ...}   (composited)
 *   or a bare name string "HAS_CAPABILITY".
 *   expr may be a string (bare name / expression object).  subject and
 *   object are strings.  Returns true if the relation holds along a chain.
 *   (The planner guards on this: {eval:{type:"rebac/check", rel: expr,
 *    subject:"{agent}", object:"fly"}}.) */
bool rectgtn_rebac_check(const char *graph_json,
                         const char *expr,
                         const char *subject,
                         const char *object);

/* Parse the domain's ReBAC graph and check one relation — convenience
 *   wrapper: reads capabilities.graph from the composed domain. */
bool rectgtn_domain_has_capability(const char *domain,
                                   const char *subject,
                                   const char *relation);

/* ----------------------------------------------------------------------
 *  Pointer state  (/var/key  = a JSON-LD pointer into `variables`)
 * ---------------------------------------------------------------------- */
/* Apply an eval/pointer/set step to the running state.  state is the
 *   current /var/ object (JSON object), step is the pointer/set JSON-LD
 *   step {"pointer/set": "/var/key", "value": <v>} or {"pointer/get": ...}.
 *   Returns the new state as a JSON string, or a JSON {"error":"..."}. */
char *rectgtn_ptr_set(const char *state, const char *step);

/* Pointer get: {"pointer"/value} into state. Returns JSON value or null. */
char *rectgtn_ptr_get(const char *state, const char *key_path);

/* ----------------------------------------------------------------------
 *  Memory
 * ---------------------------------------------------------------------- */
/* Free any string returned by the functions above.  The domain handles and
 *   C-string buffers are all rectgtn_free-able. */
void rectgtn_free(char *);

#if __cplusplus
}
#endif

#endif /* RECTGTN_API_H */
