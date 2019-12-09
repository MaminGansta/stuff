# coding=utf-8
import random as r


for i in range(10):
    print(f"insert into authors(author_name) values('автор{i}');")


for i in range(10):
    print(f"insert into genres(genre_name) values('жанр{i}');")

for i in range(50):
    print(f"insert into compositions(compos_name, author_id, genre_id) values('произведение{i}', {r.randint(1,10)}, {r.randint(1,10)});")

for i in range(100):
    print(f"insert into books(book_name, compos_id, year, town, illustr, collection, cost, here)"
          f" values('книга{i}', {r.randint(1, 50)}, {r.randint(1900, 2019)}, 'город{i}', {r.randint(0,1)}, {r.randint(0,1)},"
          f" {r.randint(500,2000)}, 'здесь');")
