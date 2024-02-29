import time
# import serial
import sqlite3
import os
import sys
import random
import subprocess
import paho.mqtt.client as mqtt
import json
import time

def thread1():

	def calcula_custo(CT,SL,RPM_time,RPM_max,PO):
		a = 127
		b = 94
		c = 33
		return (a*CT*SL)+(b*RPM_time*RPM_max)+(c*PO)

	def save_df(data):
		if "info_maquinas.db" not in os.listdir():
			print("\nERRO: base de dados não encontrada\n")
			sys.exit()

		with sqlite3.connect('info_maquinas.db') as conn:
			c = conn.cursor()
			try:
				##Seleciona variáveis para salvar
				#Status da máquina
				Emer = data['Status']['Emergency_Status']
				Control_mode = data["Status"]["Controller_Mode"]
				Auto_mode = data["Status"]["Automatic_Mode"]
				Exec_mode = data["Status"]["Execution_Mode"]

				#Programa
				program_name = data["Program"]["Name"]

				#Dados do spindle
				SpinLoad = data['Axis']['Spindle_Load']
				RPM_percent = data['Axis']['Spindle_Speed']/24000*100

				# #Dados dos eixos
				# pos_x = data['Axis']['AbsPos_X']
				# pos_y = data['Axis']['AbsPos_Y']
				# pos_z = data['Axis']['AbsPos_Z']

				# c.execute(
				# 	f"""
				# 		SELECT 
				# 			STATUS, id
				# 		FROM 
				# 			programs 
				# 		WHERE(
				# 			NAME = '{program_name}'
				# 			AND
				# 			STATUS = 0
				# 			)
				# 	"""
				# )

				# Prog = "Manual"
				# prog_id = 10
				# check_programs = False

				# results = c.fetchall()

				# print(results)

				# if results != []:
				# 	check_programs = True
				# 	prog_id = results[0][1]

				# print(program_name)
				# print(Exec_mode)

				# print(RPM_percent)

				# if program_name!="O0" and Exec_mode == "Active" and not check_programs:
				# 	Prog = program_name
				# 	c.execute(f"""
				# 	INSERT INTO programs
				# 	(NAME, STATUS)
				# 	VALUES
				# 	('{Prog}', 0)
				# 	""")

				# elif check_programs and Exec_mode == "Active":
				# 	Prog = program_name

				# elif Exec_mode == "Stopped" and check_programs:
				# 	print(1)
				# 	c.execute(f"""
				# 	UPDATE programs
			   	# 	SET 
			   	# 		STATUS = 1
			   	# 	WHERE
			   	# 		NAME = '{program_name}'
				# 	""")


				# print(Prog)

				c.execute(f"""
				INSERT INTO all_data
				(EMER, PROG_NAME, STATUS, CTRL_MODE, AUTO_MODE, SL, RPM_percent)
				VALUES
				('{Emer}', '{program_name}' , '{Exec_mode}' , '{Control_mode}' , '{Auto_mode}' , {SpinLoad}, {RPM_percent})
			  	""")
				conn.commit()
				print("Salvo na base \n")

			except sqlite3.Error as er:
				print("Não foi possível inserir dados na tabela. Erro: {0}".format(er))

	def on_connect(client, userdata, flags, rc):
		client.subscribe("iot/fanuc/HighSpeed")
		print("Conectado com o broker!")

	def on_message(client, userdata, msg):
		msg = json.loads(msg.payload.decode())
		save_df(msg)

	client = mqtt.Client()
	client.connect("localhost",1883,60)

	client.on_connect = on_connect
	client.on_message = on_message

	client.loop_forever()


if __name__ == '__main__':
    thread1()
