DROP TABLE IF EXISTS Achievements;
CREATE TABLE Achievements
(
	Id INT NOT NULL PRIMARY KEY,
	Name TEXT NOT NULL,
	Image TEXT NOT NULL
);

DROP TABLE IF EXISTS Criterias;
CREATE TABLE Criterias
(
	Id INT NOT NULL PRIMARY KEY,
	Description TEXT NOT NULL
);

INSERT INTO Achievements VALUES
(1, 'Welcome', 'Login1.png'),
(2, '5 Times the Charm', 'Login2.png'),
(3, 'Logged in 10 Times', 'Login3.png'),
(4, 'Seasoned Player', 'Login4.png'),
(5, 'It''s over 100', 'Login5.png');

INSERT INTO Criterias VALUES
(1, 'Log in 1 time'),
(2, 'Log in 5 times'),
(3, 'Log in 10 times'),
(4, 'Log in 50 times'),
(5, 'Log in 100 times');
