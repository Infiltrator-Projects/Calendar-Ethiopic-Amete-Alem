# Engineering Philosophy

Calendar-Ethiopic-Amete-Alem adopts the [Infiltrator Engineering Philosophy](https://github.com/Infiltrator-Projects/Libraries/blob/main/docs/PHILOSOPHY.md) as its governing standard. The following requirements apply to this independent calendar provider; they describe acceptance criteria, not a claim that every capability is already qualified.

## Ownership and fidelity

This provider owns the Ethiopic Amete Alem calendar's model and conversion rules. Common owns the shared temporal ABI and discovery contract; consuming applications own presentation. Do not duplicate the calendar's algorithms in consumers or substitute a superficially similar calendar. Document the supported epoch, chronology, era and leap rules, day boundary, historical or regional variants, and the provenance and limits of any tables or approximations. Distinguish model dates from historical observations.

## Correctness and qualification

Specify coordinate units, valid ranges, overflow behaviour, and the conditions under which conversion is reversible. Test known independent reference dates, boundary transitions, invalid inputs and round-trip properties within the documented domain. Refuse unsupported or ambiguous inputs explicitly rather than silently selecting another model. Keep ABI compatibility, dependencies and release identities explicit; qualify the exact distributed artifacts.

## Craft and documentation

Preserve postgraduate-level reasoning in source comments and technical documents: invariants, derivations, ownership, trade-offs and failure semantics. Apply the family's [visual design authority](https://github.com/Infiltrator-Projects/InfiltratorOS/blob/main/docs/PHILOSOPHY.md) to any supplied presentation, without allowing aesthetics to change chronological meaning. A passing build alone is not evidence of historical accuracy.
