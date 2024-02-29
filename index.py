import streamlit as st

st.set_page_config(
    page_title="Menu"
)

st.write('# Centro de Visualização')

st.sidebar.success("Selecione uma máquina")

st.markdown(
    """
    Aplicativo web para monitoramento das máquinas Fanuc CNC do TechLab
    """
)