DROP TABLE IF EXISTS SpellValues;
CREATE TABLE SpellValues
(
	Id INT NOT NULL PRIMARY KEY,
	SpellEffectId TINYINT NOT NULL,
	Target TINYINT NOT NULL,
	ManaCost TINYINT NOT NULL,
	EffectValue1 TINYINT NOT NULL,
	EffectValue2 TINYINT NOT NULL,
	EffectValue3 TINYINT NOT NULL,
	EffectValue4 TINYINT NOT NULL
);

DROP TABLE IF EXISTS Spells;
CREATE TABLE Spells
(
	Id INT NOT NULL,
	SpellValueId INT NOT NULL,
	FOREIGN KEY(SpellValueId) REFERENCES SpellValues(Id),
	PRIMARY KEY (Id, SpellValueId)
);
