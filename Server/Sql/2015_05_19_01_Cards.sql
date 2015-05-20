DROP TABLE IF EXISTS Cards;

CREATE TABLE Cards 
(
	Id int not null primary key,
	Type tinyint not null,
	BaseHp tinyint not null,
	BaseDamage tinyint not null,
	BaseMana tinyint not null,
	BaseDefense tinyint not null,
	Cost tinyint not null,
	SpellId int,
	FOREIGN KEY (SpellId) REFERENCES Spells(Id)
);

