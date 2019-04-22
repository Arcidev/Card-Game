UPDATE Cards SET BaseHp = BaseHp * 10 WHERE BaseHp >= 5;
UPDATE Cards SET BaseHp = BaseHp * 12 WHERE BaseHp = 4;
UPDATE Cards SET BaseHp = BaseHp * 13 WHERE BaseHp = 3;
UPDATE Cards SET BaseHp = BaseHp * 15 WHERE BaseHp < 3;
