CREATE TABLE IF NOT EXISTS expenses (
    id SERIAL PRIMARY KEY,
    amount DOUBLE PRECISION NOT NULL,
    category TEXT NOT NULL,
    description TEXT,
    date TIMESTAMP DEFAULT NOW()
);
