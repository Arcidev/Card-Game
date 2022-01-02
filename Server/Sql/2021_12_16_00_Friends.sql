CREATE TABLE IF NOT EXISTS friend_requests (
    sender_id INT NOT NULL references users (id),
    receiver_id INT NOT NULL references users (id),
    PRIMARY KEY (sender_id, receiver_id)
);

CREATE TABLE IF NOT EXISTS friends (
    user_id INT NOT NULL references users (id),
    friend_id INT NOT NULL references users (id),
    PRIMARY KEY (user_id , friend_id)
);

CREATE TABLE IF NOT EXISTS blocked_users (
    user_id INT NOT NULL references users (id),
    blocked_id INT NOT NULL references users (id),
    PRIMARY KEY (user_id , blocked_id)
);
