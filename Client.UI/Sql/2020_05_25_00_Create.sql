DROP TABLE IF EXISTS Cards;
CREATE TABLE Cards
(
	Id INT NOT NULL PRIMARY KEY,
	Name TEXT NOT NULL,
	ImagePath TEXT NOT NULL
);

DROP TABLE IF EXISTS Spells;
CREATE TABLE Spells
(
	Id INT NOT NULL PRIMARY KEY,
	Name TEXT NOT NULL,
	Description TEXT NOT NULL
);

DROP TABLE IF EXISTS SpellAuras;
CREATE TABLE SpellAuras
(
	SpellId INT NOT NULL PRIMARY KEY,
	Text TEXT NOT NULL,
	ImagePath TEXT NOT NULL,
	
	FOREIGN KEY(SpellId) REFERENCES Spells(Id)
);

INSERT INTO "Cards" VALUES
(1,'Warrior','Warrior.png'),
(2,'Paladin','Paladin.png'),
(3,'Mage','Mage.png'),
(4,'Assassin','Assassin.png'),
(5,'Wizard','Wizard.png'),
(6,'Demon','Demon.png'),
(7,'Lich','Lich.png'),
(8, 'Priest', 'Priest.png'),
(9, 'Archer', 'Archer.png'),
(10, 'Werewolf', 'Werewolf.png'),
(11, 'Harpy', 'Harpy.png');

INSERT INTO Spells (Id, Name, Description) VALUES
(1, 'Lightning Bolt', 'Deals high amount of damage to an enemy'),
(2, 'Devil''s Curse', 'Curses the enemy and kills him periodically'),
(3, 'Strength of Light', 'Increases damage of friendly unit'),
(4, 'Purge', 'Draws life from an enemy or healing friendly unit'),
(5, 'Lich Slap', 'Slaps the target for moderate amount of damage'),
(6, 'Grace', 'Heals over time friendly unit or strikes enemy with high amount of damage'),
(7, 'Throw knives', 'Inflicts damage to an enemy and its nearest allies.'),
(8, 'Cleave', 'Inflicts melee damage to an enemy and its nearest allies.'),
(9, 'Bite flesh', 'Bites an enemy dealing melee damage while healing himself.'),
(10, 'Delude', 'Transforms self into target.');
