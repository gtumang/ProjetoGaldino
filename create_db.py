import os
import sys
import sqlite3

if 'info_maquinas.db' in os.listdir():
    print("\nERRO: base de dados ja existe, apague manualmente para continuar\n")
    sys.exit()

with sqlite3.connect('info_maquinas.db') as conn:
    c = conn.cursor()

    c.execute("""
        CREATE TABLE IF NOT EXISTS all_data (
            id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
            EMER TEXT,
            PROG_NAME TEXT,
            STATUS TEXT,
            CTRL_MODE TEXT,
            AUTO_MODE TEXT,
            SL FLOAT NOT NULL,
            RPM_percent FLOAT NOT NULL,
            timestamp DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
            )
              """)

    # c.execute("""
    #     CREATE TABLE IF NOT EXISTS programs (
    #         id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    #         NAME TEXT,
    #         STATUS INTEGER,
    #         timestamp DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
    #     )
    #     """)
    
    # c.execute(f"""
    #         INSERT INTO programs
    #         (NAME, STATUS) VALUES (10, 0)
    #           """)

    c.execute("""
            INSERT INTO all_data 
            (EMER, PROG_NAME, STATUS, CTRL_MODE, AUTO_MODE, SL, RPM_percent)
            VALUES
            (' ',' ',' ',' ',' ', 0 , 0)
            """)


    conn.commit()