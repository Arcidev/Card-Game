INSERT INTO Spells (Id, ManaCost) VALUES
(7, 3);

INSERT INTO SpellValues (Id, SpellEffectId, SpellAttributesMask, Target, EffectValue1, EffectValue2, EffectValue3, EffectValue4) VALUES
(7, 0/*SPELL_EFFECT_DIRECT_DAMAGE*/, 0, 2/*TARGET_UNIT_CLEAVE_ENEMY*/, 3, 0, 0, 0);

INSERT INTO SpellsSpellValues (SpellId, SpellValueId) VALUES
(7, 7);

UPDATE Cards SET BaseMana = 5, SpellId = 7 WHERE Id = 4; -- Assassin
