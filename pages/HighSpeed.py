import streamlit as st
import pandas as pd
import plotly.express as px
import time
import sys
import sqlite3
import os

from atualiza_db import thread1
import threading


def run_thread():
    t = threading.Thread(target=thread1, name=thread1)
    t.daemon = True
    t.start()
    return

run_thread()



#Função para pegar dados do banco de dados
def get_data():
    if "info_maquinas.db" not in os.listdir():
        print("\nERRO: base de dados não encontrada\n")
        sys.exit()

    with sqlite3.connect("info_maquinas.db") as conn:
        c = conn.cursor()

        c.execute("""
            SELECT EMER, PROG_NAME, STATUS, CTRL_MODE, AUTO_MODE, SL, RPM_percent, timestamp FROM all_data ORDER BY timestamp DESC LIMIT 100
        """)

        rows = c.fetchall()
        df = pd.DataFrame(columns=['EMER','PROG_NAME','STATUS','CTRL_MODE','AUTO_MODE','SL','RPM_percent','timestamp'], data=rows)
        return df
    

st.set_page_config(
    page_title="Dashboard",
    layout="wide"
)


st.title("Fanuc Robodrill HighSpeed")

placeholder = st.empty()

while(True):
    df = get_data()
    print(df.head(2))

    with placeholder.container():
        st.write("Posição do eixo")

        emer, prog, status, ctrl_mode, auto_mode = st.columns(5)
        emer.metric(
            label = "Emergência",
            value = str(df['EMER'].iloc[0])
        )
        prog.metric(
            label = "Programa",
            value = str(df['PROG_NAME'].iloc[0])
        )
        status.metric(
            label = "Status",
            value = str(df['STATUS'].iloc[0])
        )
        ctrl_mode.metric(
            label = "Modo de Controle",
            value = str(df['CTRL_MODE'].iloc[0])
        )
        auto_mode.metric(
            label = "Controle Automático",
            value = str(df['AUTO_MODE'].iloc[0])
        )
        # pos_x.metric(
        #     label = "X (mm)",
        #     value = int(df["Pos_X"].iloc[0])
        # )

        # pos_y.metric(
        #     label = "Y (mm)",
        #     value = int(df["Pos_Y"].iloc[0])
        # )

        # pos_z.metric(
        #     label = "Z (mm)",
        #     value = int(df["Pos_Z"].iloc[0])
        # )


        info1, info2, info3 = st.columns(3)

        info1.metric(
            label = "Spindle Load (%)",
            value = int(df["SL"].iloc[0])
        )

        info2.metric(
            label = "RPM_max (%)",
            value = int(df["RPM_percent"].iloc[0])
        )

        # info3.metric(
        #     label = "Custo Variavel",
        #     value = int(df["CV"].iloc[0])
        # )


        fig_1, fig_2= st.columns(2)

        with fig_1:
            st.markdown('### Histórico Spindle Load (%)')
            fig1 = px.line(df, x='timestamp', y='SL')
            fig1.update_layout(
                autosize=False,
                width=600,
                height=500
            )
            st.write(fig1)

        with fig_2:
            st.markdown('### Historico RPM (%)')
            fig2 = px.line(df, x='timestamp',y='RPM_percent')
            fig2.update_layout( 
                autosize=False,
                width=600,
                height=500
            )
            st.write(fig2)

        # with fig_3:
        #     st.markdown('### Historico Custo Variavel')
        #     fig3 = px.line(df, x='timestamp',y='CV')
        #     fig3.update_layout( 
        #         autosize=False,
        #         width=600,
        #         height=500
        #     )
        #     st.write(fig3)

        st.markdown('### Base Completa')
        st.dataframe(df)
    time.sleep(1)
