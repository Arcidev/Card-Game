CREATE TABLE IF NOT EXISTS achievements (
    id INT PRIMARY KEY,
    parent_id INT references achievements (id)
);

CREATE TABLE IF NOT EXISTS achievement_criterias (
    id INT PRIMARY KEY,
    achievement_id INT NOT NULL references achievements (id),
    criteria_type SMALLINT NOT NULL,
    requirement INT NOT NULL
);

INSERT INTO achievements VALUES
(1, NULL),
(2, 1),
(3, 2),
(4, 3),
(5, 4);

INSERT INTO achievement_criterias VALUES
(1, 1, 0/*CRITERIA_TYPE_LOGIN*/, 1),
(2, 2, 0/*CRITERIA_TYPE_LOGIN*/, 5),
(3, 3, 0/*CRITERIA_TYPE_LOGIN*/, 10),
(4, 4, 0/*CRITERIA_TYPE_LOGIN*/, 50),
(5, 5, 0/*CRITERIA_TYPE_LOGIN*/, 100);

CREATE TABLE IF NOT EXISTS user_achievement_criterias (
    user_id INT NOT NULL references users (id),
    criteria_id INT NOT NULL references achievement_criterias (id),
    progress INT NOT NULL,
    PRIMARY KEY (user_id, criteria_id)
);
