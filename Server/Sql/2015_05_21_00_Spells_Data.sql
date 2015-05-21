INSERT INTO SpellValues (Id, SpellEffectId, SpellAttributesMask, Target, EffectValue1, EffectValue2, EffectValue3, EffectValue4) VALUES
(1, 1/*SPELL_EFFECT_DIRECT_DAMAGE*/, 0, 0/*TARGET_UNIT_TARGET_ENEMY*/, 10, 0, 0, 0),
(2, 2/*SPELL_EFFECT_APPLY_AURA*/, 0, 0/*TARGET_UNIT_TARGET_ENEMY*/, 1/*SPELL_AURA_EFFECT_DAMAGE*/, 5, 1, 2),
(3, 2/*SPELL_EFFECT_APPLY_AURA*/, 0, 1/*TARGET_UNIT_TARGET_FRIEND*/, 2/*SPELL_AURA_EFFECT_MODIFY_STAT*/, 16, 2/*CARD_STAT_DAMAGE*/, 5),
(4, 3/*SPELL_EFFECT_HEAL*/, 0, 1/*TARGET_UNIT_TARGET_FRIEND*/, 5, 0, 0, 0);

INSERT INTO Spells (Id, ManaCost) VALUES
(1, 4), -- Lightning Bolt
(2, 2), -- Devil Curse
(3, 4), -- Strength of Light
(4, 4); -- Purge

INSERT INTO SpellsSpellValues (SpellId, SpellValueId) VALUES
(1, 1),
(2, 2),
(3, 3),
(4, 1),
(4, 4);

UPDATE Cards Set SpellId = 3 WHERE Id = 2; -- Paladin
UPDATE Cards SET SpellId = 1 WHERE Id = 3; -- Mage
UPDATE Cards SET SpellId = 4 WHERE Id = 5; -- Wizard
UPDATE Cards Set SpellId = 2 WHERE Id = 6; -- Demon
