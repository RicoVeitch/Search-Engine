CREATE TABLE history (
  query varchar(1024),
  q_count INT,
  PRIMARY KEY (query)
);
INSERT INTO history VALUES ('test query', 1);