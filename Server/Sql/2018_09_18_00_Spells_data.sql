INSERT INTO Spells (Id, ManaCost) VALUES
(8, 3);

INSERT INTO SpellValues (Id, SpellEffectId, SpellAttributesMask, Target, EffectValue1, EffectValue2, EffectValue3, EffectValue4) VALUES
(8, 0/*SPELL_EFFECT_DIRECT_DAMAGE*/, 1 /*SPELL_ATTRIBUTE_TARGET_MELEE*/, 2/*TARGET_UNIT_CLEAVE_ENEMY*/, 4, 0, 0, 0);

INSERT INTO SpellsSpellValues (SpellId, SpellValueId) VALUES
(8, 8);

UPDATE Cards SET BaseMana = 4, SpellId = 8 WHERE Id = 1; -- Warrior
