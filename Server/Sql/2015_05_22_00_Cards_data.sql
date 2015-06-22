INSERT INTO Spells (Id, ManaCost) VALUES
(6, 4);

INSERT INTO SpellValues (Id, SpellEffectId, SpellAttributesMask, Target, EffectValue1, EffectValue2, EffectValue3, EffectValue4) VALUES
(6, 1/*SPELL_EFFECT_APPLY_AURA*/, 0, 1/*TARGET_UNIT_TARGET_FRIEND*/, 2/*SPELL_AURA_EFFECT_HEAL*/, 5, 1, 2);

INSERT INTO SpellsSpellValues (SpellId, SpellValueId) VALUES
(6, 6),
(6, 1);

INSERT INTO Cards (Id, Type, BaseHp, BaseDamage, BaseMana, BaseDefense, Price, SpellId) VALUES
(8, 2, 40, 2, 10, 3, 0, 6),
(9, 2, 50, 10, 0, 4, 0, NULL),
(10, 1, 70, 6, 0, 7, 0, NULL),
(11, 1, 40, 10, 0, 6, 0, NULL);
