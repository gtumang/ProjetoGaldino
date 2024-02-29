import sqlite3
import pandas as pd

with sqlite3.connect('info_maquinas.db') as conn:
    c = conn.cursor()

    # c.execute("""
    #     SELECT * FROM programs
    #           """)
    
    # print(c.fetchall())

    c.execute("""
        SELECT * FROM all_data WHERE (PROG_NAME = 'fixa_tumang_sup_2.nc' AND STATUS = 'Active')
              """)
    
    resultado = c.fetchall()
    
    df = pd.DataFrame(resultado)

    print(df(df[6]!='0'))