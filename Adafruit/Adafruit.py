# Código de Proyecto 1
# Universidad del Valle de Guatemala
# IE3027 - Electrónica Digital 2
# Pablo Moreno, Fernando Arribas y Josue Salazar

from Adafruit_IO import Client, RequestError, Feed
import serial
import time

envio = 184     #dato que viene desde la pág
nulo = 0        #valor nulo
nulo = str(nulo)#char nulo
temporal1 = 0
temporal2 = 0
val1=0

#ubicación de mi plataforma
ADAFRUIT_IO_KEY = "aio_djai86Ee1ftf42YPh3KEy4tXnuiF"  #contraseña
ADAFRUIT_IO_USERNAME = "mor19034"                    #usuario
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

#Defino nombre de comunicación
puerto = serial.Serial("COM3",9600) 
puerto.timeout = 3 #tiempo de espera para recibir datos
time.sleep(1) #para que establezca conexion

print('Puerto activado con exito \n')
#----------------Temperatura y humedad valor---------------------------------

while True:
    with puerto:   #utilice el COM serial
        #nombre_feed = cliente.feed('llave_de_mi_feed')
        temperatura_feed = aio.feeds('sensor-de-temperatura')
        humedad_feed = aio.feeds('sensor-de-humedad')
        calidad_aire_feed = aio.feeds('calidad-de-aire')

        #variable = cliente.receive(mifeed_feed.key) ; Esto para leer valor
        temperatura_data = aio.receive(temperatura_feed.key)
        humedad_data = aio.receive(humedad_feed.key)
        calidad_aire_data = aio.receive(calidad_aire_feed.key)
        
        #***************************************hay que cambiar toodo lo de abajo para probar si funciona*********************
        valor = int(temperatura_data.value)
        puerto.write(b's') #mando codigo de caracter
        val = puerto.readline(3).decode('ascii') #recibo 2 bytes
        val = int(val)
        
        valorh = int(humedad_data.value)
        puerto.write(b'd') #mando codigo de caracter
        valh = puerto.readline(3).decode('ascii') #recibo 2 bytes
        valh = int(valh) 

        if(valor != temporal1 or val != temporal2):
            print(f'valor de temperatura: {temperatura_data.value}')
            print(f'valor de humedad: {humedad_data.value} \n')

            if (valor < 10 and valor > -1):
                puerto.write(nulo.encode('ascii')) #envio un cero
                puerto.write(nulo.encode('ascii'))
                
            elif (valor <100 and valor > 9):
                puerto.write(nulo.encode('ascii')) #envio un cero
            
            #envio = str(int(uart_data.value))
            #puerto.write(envio.encode('ascii'))
            #print("Escrito en pic: {} \n".format(envio))
            #print("Escrito en AdaFruit: ")
            #print(val)
            
            #cliente.send_data(mifeed.feed.key, valor a enviar) ; Esto para mandar dato y leer
            aio.send_data(humedad_feed.key, valh)
            aio.send_data(temperatura_feed.key, val)
            print('----------------------------------------------------')
            temporal1 = int(temperatura_data.value)
            temporal2 = int(humedad_data.value)

            
##    valor = int(calidad_aire_data.value) #chequo si detener la simulación
##        if valor == 1:
##            break
##        time.sleep(0.1)
##print('Mala calidad de aire')
