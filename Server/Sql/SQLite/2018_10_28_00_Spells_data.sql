INSERT INTO Spells (Id, ManaCost) VALUES
(10, 0);

INSERT INTO SpellValues (Id, SpellEffectId, SpellAttributesMask, Target, EffectValue1, EffectValue2, EffectValue3, EffectValue4) VALUES
(10, 1/*SPELL_EFFECT_APPLY_AURA*/, 0, 0/*TARGET_UNIT_TARGET_ENEMY*/, 3/*SPELL_AURA_EFFECT_MORPH*/, 8, 100/*100% mana of target card*/, 0),
(11, 1/*SPELL_EFFECT_APPLY_AURA*/, 2/*SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF*/, 1/*TARGET_UNIT_TARGET_FRIEND*/, 3/*SPELL_AURA_EFFECT_MORPH*/, 8, 100/*100% mana of target card*/, 0);

INSERT INTO SpellsSpellValues (SpellId, SpellValueId) VALUES
(10, 10),
(10, 11);

UPDATE Cards SET SpellId = 10 WHERE Id = 11; -- Harpy
