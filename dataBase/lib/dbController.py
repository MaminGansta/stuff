class DataBase:

    def __init__(self, connection):
        self.connection = connection
        self.cursor = connection.cursor()

    def create_tables(self):
        # create book table
        self.cursor.execute(
            "CREATE TABLE IF NOT EXISTS books("
            "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "book_name TEXT,"
            "compos_id INTEGER,"
            "year INTEGER,"
            "town TEXT,"
            "illustr INTEGER,"
            "collection INTEGER,"
            "cost INTEGER,"
            "here TEXT,"
            "image_id INTEGER)")

        # create authors table
        self.cursor.execute(
            "CREATE TABLE IF NOT EXISTS authors("
            "author_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "author_name TEXT)")

        # create genre table
        self.cursor.execute(
            "CREATE TABLE IF NOT EXISTS genres("
            "genre_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "genre_name TEXT)")

        # create compositions table
        self.cursor.execute(
            "CREATE TABLE IF NOT EXISTS compositions("
            "compos_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "compos_name TEXT,"
            "genre_id INTEGER,"
            "author_id INTEGER,"
            "comment TEXT)")

        # create image table
        self.cursor.execute(
            "CREATE TABLE IF NOT EXISTS images("
            "image_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "image_name TEXT)")


    # str => "str" for sql query
    def wrap(self, string):
        return '"' + string + '"'


    # generate last part of query
    def query_generator(self, *args):
        if len(args) % 2 != 0:
            print("query_generator function fall")
            return None

        query_end = ""
        AND_flag = False
        for i in range(1, len(args), 2):
            if args[i] != self.wrap(""):
                if AND_flag:
                    query_end += " AND "
                    AND_flag = False
                query_end += args[i - 1]
                query_end += args[i]
                AND_flag = True

        return query_end


    # books
    def find_book(self, name, author, genre):

        if name == "" and author == "" and genre == "":
            where = ""
        else:
            where = "WHERE"

        query_end = self.query_generator("b.book_name == ", self.wrap(name),
                                         " a.author_name == ", self.wrap(author),
                                         " g.genre_name == ", self.wrap(genre))

        self.cursor.execute(f"SELECT b.book_name, c.compos_name, a.author_name, g.genre_name, b.year, b.cost, b.here "
                            f"FROM books as b NATURAL JOIN compositions as c "
                            f"NATURAL JOIN authors as a NATURAL JOIN genres as g {where} " + query_end)
        return self.cursor.fetchall()

    # composes by genre
    def find_compos(self, name, author, genre):
        if author == "" and genre == "" and name == "":
            where = ""
        else:
            where = "WHERE"

        query_end = self.query_generator(" a.author_name == ", self.wrap(author),
                                         " g.genre_name == ", self.wrap(genre),
                                         " c.compos_name == ", self.wrap(name))

        self.cursor.execute(f"SELECT c.compos_name, a.author_name, g.genre_name, c.comment "
                            f"FROM compositions as c NATURAL JOIN authors as a NATURAL JOIN genres as g "
                            f"{where} " + query_end)
        return self.cursor.fetchall()


    def countCopmos(self, author, genre):
        if author == "" and genre == "":
            where = ""
        else:
            where = "WHERE"

        query_end = self.query_generator(" a.author_name == ", self.wrap(author),
                                         " g.genre_name == ", self.wrap(genre))

        self.cursor.execute(f"SELECT COUNT(DISTINCT book_name) FROM books as b NATURAL JOIN compositions as c "
                            f"NATURAL JOIN authors as a NATURAL JOIN genres as g {where} " + query_end)

        out = self.cursor.fetchone()[0]
        return out

    # dublicates in books
    def duplicates(self):
        self.cursor.execute("SELECT COUNT(1) FROM books")
        all = self.cursor.fetchone()
        self.cursor.execute("SELECT COUNT(*) "
                            "from (select DISTINCT book_name, author_id FROM books NATURAL JOIN compositions)")
        res = all[0] - self.cursor.fetchone()[0]
        return res


    def get_author_id(self, name):
        self.cursor.execute(f"SELECT a.author_id FROM authors as a WHERE a.author_name = {self.wrap(name)}")
        return self.cursor.fetchone()[0]

    def add_comment(self, name, author, comment):
        author = self.get_author_id(author)
        self.cursor.execute(f"UPDATE compositions SET comment = {self.wrap(comment)} "
                            f"WHERE compos_name = {self.wrap(name)} AND author_id = {author}")
        #self.connection.commit()


    def find_compos_comment(self, comment):
        self.cursor.execute("SELECT c.compos_name, a.author_name, g.genre_name, c.comment "
                            "FROM compositions as c NATURAL JOIN authors as a NATURAL JOIN genres as g "
                            f"WHERE c.comment = {self.wrap(comment)}")
        return self.cursor.fetchall()

    def take_book(self, name, year):
        self.cursor.execute(f"UPDATE books SET here = {self.wrap('взята')} "
                            f"WHERE book_name = {self.wrap(name)} AND year = {year}")
        # self.connection.commit()


    def return_book(self, name, year):
        self.cursor.execute(f"UPDATE books SET here = {self.wrap('здесь')} "
                        f"WHERE book_name = {self.wrap(name)} AND year = {year}")
