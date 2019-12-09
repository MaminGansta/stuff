from lib.dbController import DataBase
import sqlite3
from tkinter import *
import tkinter as tk
import tkinter.ttk as ttk


connection = sqlite3.connect("data/data_base.db")
cursor = connection.cursor()

db = DataBase(connection)
db.create_tables()


# new window with table
class Table(tk.Frame):
    def __init__(self, parent=None, headings=tuple(), rows=tuple()):
        window = Toplevel(parent)
        super().__init__(window)

        table = ttk.Treeview(self, show="headings", selectmode="browse")
        table["columns"] = headings
        table["displaycolumns"] = headings

        for head in headings:
            table.heading(head, text=head, anchor=tk.CENTER)
            table.column(head, anchor=tk.CENTER)

        for row in rows:
            table.insert('', tk.END, values=row)

        scrolltable = tk.Scrollbar(self, command=table.yview)
        table.configure(yscrollcommand=scrolltable.set)
        scrolltable.pack(side=tk.RIGHT, fill=tk.Y)
        table.pack(expand=tk.YES, fill=tk.BOTH)

        window.transient(root)
        window.focus_set()


def window_add_comment():
    top = Toplevel(root)
    top.resizable(0, 0)
    top.title("добавление коментария")

    name_m = StringVar()
    l_name_m = Label(top, text="название", bg="#FFCC00")
    l_name_m.grid(row=2, column=0, ipadx=13)
    e_name_m = Entry(top, textvariable=name_m)
    e_name_m.grid(row=2, column=1, columnspan=2, padx=10, pady=10)

    author_m = StringVar()
    l_author_m = Label(top, text="автор", bg="#FFCC00")
    l_author_m.grid(row=3, column=0, ipadx=24)
    e_author_m = Entry(top, textvariable=author_m)
    e_author_m.grid(row=3, column=1, columnspan=2, padx=10)

    comment_m = StringVar()
    l_comment_m = Label(top, text="коментарий", bg="#FFCC00")
    l_comment_m.grid(row=4, column=0, ipadx=4)
    e_comment_m = Entry(top, textvariable=comment_m)
    e_comment_m.grid(row=4, column=1, columnspan=2, padx=10, pady=10)
    b_add_comment_m = Button(top, height=2, width=8, text="добавить",
                             command=lambda: db.add_comment(name_m.get().strip(),
                                                            author_m.get().strip(),
                                                            comment_m.get().strip()))

    b_add_comment_m.grid(row=5, column=2)

    top.transient(root)
    top.focus_set()
    top.wait_window()

def window_find_by_comment():
    top = Toplevel(root)
    top.resizable(0, 0)
    top.title("поиск по коментарию")

    comment_m = StringVar()
    l_comment_m = Label(top, text="коментарий", bg="#FFCC00")
    l_comment_m.grid(row=0, column=0, ipadx=4)
    e_comment_m = Entry(top, textvariable=comment_m)
    e_comment_m.grid(row=0, column=1, columnspan=2, padx=10, pady=10)
    b_add_comment_m = Button(top, height=2, width=8, text="искать",
                             command=lambda: find_by_comment(top, comment_m.get().strip()))

    b_add_comment_m.grid(row=5, column=2)

    top.transient(root)
    top.focus_set()
    top.wait_window()

def window_take_return_book():
    top = Toplevel(root)
    top.resizable(0, 0)
    top.title("взять/вернуть книгу")

    name_m = StringVar()
    l_name_m = Label(top, text="название", bg="#FFCC00")
    l_name_m.grid(row=0, column=0, ipadx=13)
    e_name_m = Entry(top, textvariable=name_m)
    e_name_m.grid(row=0, column=1, columnspan=2, padx=10, pady=10)

    author_m = StringVar()
    l_author_m = Label(top, text="год", bg="#FFCC00")
    l_author_m.grid(row=1, column=0, ipadx=32)
    e_author_m = Entry(top, textvariable=author_m)
    e_author_m.grid(row=1, column=1, columnspan=2, padx=10)

    b_take_book = Button(top, height=2, width=8, text="взять",
                         command=lambda: db.take_book(name_m.get().strip(), author_m.get().strip()))

    b_return_book = Button(top, height=2, width=8, text="вернуть",
                           command=lambda: db.return_book(name_m.get().strip(), author_m.get().strip()))

    b_take_book.grid(row=3, column=2)
    b_return_book.grid(row=3, column=1)

    top.transient(root)
    top.focus_set()
    top.wait_window()


def find():
    heading = ("",)
    res = ("",)
    if table.get() == "book":
        res = db.find_book(name.get().strip(), author.get().strip(), genre.get().strip())
        heading = ("книга", "произведение", "автор", "жанр", "год", "наличие")
    elif table.get() == "compos":
        res = db.find_compos(name.get().strip(), author.get().strip(), genre.get().strip())
        heading = ("произведение", "автор", "жанр", "комментарий")

    if output.get() == "text":
        res = tuple_array_to_s(res)
        output_screen(res)
    elif output.get() == "table":
        Table(root, headings=heading, rows=res).pack()


def count():
    res = db.countCopmos(author.get().strip(), genre.get().strip())
    output_screen(f"количество книг {res}")


def duplicates():
    res = db.duplicates()
    output_screen(f"дубликаты книг {res}")

def find_by_comment(top, comment):
    res = db.find_compos_comment(comment)
    if output.get() == "text":
        res = tuple_array_to_s(res)
        output_screen(res)
    elif output.get() == "table":
        Table(root, headings=("произведение", "автор", "жанр", "комментарий"), rows=res).pack()



def output_screen(str):
    output_text.delete(1.0, END)
    output_text.insert(1.0, str)

def tuple_array_to_s(out):
    res = ""
    for row in out:
        for column in row:
            s = str(column)
            #res += " " + (s + " " * (12 - len(s))) if len(s) < 12 else s[0:12]
            res += " " + s
        res += "\n"
    return res



# init window class
root = Tk()
root.resizable(0, 0)
root.title("библиотека")

# out text field ------------------------------------------
canvas = Canvas(root, height=600, width=800, bg="white")
canvas.pack(side="left")

menu = tk.Frame(canvas, bg="#FFAA00")
menu.place(relwidth=0.4, relheight=1, relx=0.6)


frame = tk.Frame(canvas, bg="#FFFFFF")
frame.place(relwidth=0.6, relheight=1)


# output
output_text = Text(frame, width=64, height=50)
output_text.pack(side="left")

# vertical scroll bar
scrollbar = Scrollbar(frame, command=output_text.yview)
scrollbar.pack(side='right', fill="y")
output_text['yscrollcommand'] = scrollbar.set
# ---------------------------------------------------------


# table radio
l_table = Label(menu, text="таблица", font="area 12", bg="#FFCC00")
l_table.grid(row=0, column=0, pady=10, ipadx=18)
table = StringVar()
r_table1 = Radiobutton(menu, text="книги", variable=table, value="book", bg="#FFAA00")
r_table2 = Radiobutton(menu, text="произведения", variable=table, value="compos", bg="#FFAA00")
r_table1.grid(row=0, column=1, padx=10)
r_table2.grid(row=0, column=2)

# output radio
l_output = Label(menu, text="вывод", font="area 12", bg="#FFCC00")
l_output.grid(row=1, column=0, pady=20, ipadx=24)
output = StringVar()
r_out1 = Radiobutton(menu, text="текст", variable=output, value="text", bg="#FFAA00")
r_out2 = Radiobutton(menu, text="таблица", variable=output, value="table", bg="#FFAA00")
r_out1.grid(row=1, column=1, padx=10)
r_out2.grid(row=1, column=2)

# name
name = StringVar()
l_name = Label(menu, text="название", bg="#FFCC00")
l_name.grid(row=2, column=0, ipadx=13)
e_name = Entry(menu, textvariable=name)
e_name.grid(row=2, column=1, columnspan=2, padx=10)

# author
author = StringVar()
l_author = Label(menu, text="автор", bg="#FFCC00")
l_author.grid(row=3, column=0, ipadx=24)
e_author = Entry(menu, textvariable=author)
e_author.grid(row=3, column=1, columnspan=2, padx=10, pady=20)

# genre
genre = StringVar()
l_genre = Label(menu, text="жанр", bg="#FFCC00")
l_genre.grid(row=4, column=0, ipadx=25)
e_genre = Entry(menu, textvariable=genre)
e_genre.grid(row=4, column=1, columnspan=2, padx=10)

# button find
b_find = Button(menu, text="искать", width=6, height=2, bg="black", command=find)
b_find.grid(row=5, column=2, pady=20)

# count and duplicates
l_count = Label(menu, text="количество и дубликаты", bg="#FFCC00")
l_count.grid(row=6, column=0, columnspan=3, pady=10)

b_count = Button(menu, text="количество", width=9, height=2, command=count)
b_count.grid(row=7, column=0, columnspan=2)

b_duplicates = Button(menu, text="дубликаты", width=9, heigh=2, command=duplicates)
b_duplicates.grid(row=7, column=1, columnspan=2)

# composition comments
l_comment = Label(menu, text="коментарии для произведений", bg="#FFCC00")
l_comment.grid(row=8, column=0, columnspan=3, pady=20)

b_add_comment = Button(menu, width=14, heigh=2, text="добавить \nкомментарий", command=window_add_comment)
b_add_comment.grid(row=9, column=0, columnspan=2)

b_find_comment = Button(menu, width=14, heigh=2, text="найти \nпо комментарию", command=window_find_by_comment)
b_find_comment.grid(row=9, column=2, columnspan=2)

# take and return book
l_take_book = Label(menu, text="вять/вернуть книгу", bg="#FFCC00")
l_take_book.grid(row=10, column=0, columnspan=3, pady=20)

b_take_return_book = Button(menu, text="взять/вернуть", width=14, height=2,  command=window_take_return_book)
b_take_return_book.grid(row=11, column=0, columnspan=3)


root.mainloop()

