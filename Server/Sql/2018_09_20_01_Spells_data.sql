INSERT INTO Spells (Id, ManaCost) VALUES
(9, 3);

INSERT INTO SpellValues (Id, SpellEffectId, SpellAttributesMask, Target, EffectValue1, EffectValue2, EffectValue3, EffectValue4) VALUES
(9, 3/*SPELL_EFFECT_DRAIN*/, 1/*SPELL_ATTRIBUTE_TARGET_MELEE*/ | 4 /*SPELL_ATTRIBUTE_APPLY_DEFENSE*/, 0/*TARGET_UNIT_TARGET_ENEMY*/, 6, 3, 0, 0);

INSERT INTO SpellsSpellValues (SpellId, SpellValueId) VALUES
(9, 9);

UPDATE Cards SET BaseMana = 5, SpellId = 9 WHERE Id = 10; -- Werewolf
