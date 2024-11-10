# SunMonHW
## PV Monitoring System

En este repositorio se encuentran los codigos para 3 tipos de nodos de sensores y del cliente que recibe los datos para respaldarlo en una base de datos, con el fin de poder implementar una Wireless Sensor Network (WSN) capaz de monitorear niveles de temperatura, voltaje, corriente y detectar defectos de los paneles a traves del procesamiento digital de imagenes. Ademas se encuentran algunas instrucciones respecto la configuracion de la red para poder tener cada componente conectado entre si y en funcionamiento.

## Instrucciones:
Para los nodos de sensores se puede utilizar Arduino IDE para subir los codigos a los microcontroladores (ESP8266), asegurarse de cambiar las credenciales dentro de los codigos para que todo componenente se encuentre conectado a la misma red Wi-Fi, y tambien cambiar las IPs para conectarse al MQTT broker.

Para el MQTT broker, este puede ser online pero en nuestro proyecto fue implementado on premise, donde un computador en la misma red de Wi-Fi tenia MQTT mosquitto instalado y corriendo para su funcionamiento.

Finalmente, el MQTT cliente que cumple con el rol de subir los datos a nuestra base de datos (MongoDB) se implementa con node.js en el mismo computador donde se encuentra el MQTT Broker, para este hay que asegurarse de tener los paquetes MongoDB (+6.9.0) y MQTT (+5.10.1).

