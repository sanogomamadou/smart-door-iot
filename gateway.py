import serial
import paho.mqtt.client as mqtt
import json
import time
from datetime import datetime

# Configuration série
serial_port = 'COM4'  # À remplacer avec votre port série (COMx ou /dev/tty...)
baud_rate = 9600
btSerial = serial.Serial(serial_port, baud_rate)
btSerial.timeout = 1

# Configuration MQTT
client_id = '83141808-1f46-4645-85c1-0a1fcdcdadd2'  # Même UUID que dans votre flux Node-RED
telemetry_topic = client_id + '/telemetry'
commands_topic = client_id + '/commands'
mqtt_broker = 'localhost'

# Initialisation MQTT
mqtt_client = mqtt.Client(client_id=client_id, protocol=mqtt.MQTTv311)
mqtt_client.connect(mqtt_broker)
mqtt_client.loop_start()



# Gestion des commandes
def handle_commands(client, userdata, message):
    try:
        command = json.loads(message.payload.decode())
        print("Commande reçue:", command)
        
        if command.get("command") == "open_door":
            btSerial.write(b"OPEN_DOOR\n")
            print("Commande: Ouvrir la porte")
            
        elif command.get("command") == "denied":
            btSerial.write(b"ACCESS_DENIED\n")
            print("Commande: Accès refusé")
            
        elif command.get("command") == "emergency":
            btSerial.write(b"EMERGENCY_MODE\n")
            print("Commande: Mode urgence activé")
            
        elif command.get("command") == "reset_emergency":
            btSerial.write(b"RESET_EMERGENCY\n")
            print("Commande: Réinitialisation mode urgence")
            
        elif command.get("command") == "update_keypad_password":
            new_password = command.get("new_password", "")
            if 4 <= len(new_password) <= 8 and new_password.isdigit():
                btSerial.write(f"UPDATE_PWD:{new_password}\n".encode('ascii'))
                print(f"Commande: Mise à jour mot de passe vers {new_password}")
            else:
                print("Erreur: Mot de passe invalide")
                
    except Exception as e:
        print("Erreur traitement commande:", str(e))

mqtt_client.subscribe(commands_topic)
mqtt_client.on_message = handle_commands

# Gestion de la télémétrie
def parse_telemetry(data):
    """Parse les données reçues de l'Arduino"""
    try:
        parts = data.split(',')
        if len(parts) >= 3:
            status = parts[0].strip()
            method = parts[1].strip()
            uid = parts[2].strip()
            
            telemetry = {
                'status': status.lower(),
                'method': method,
                'uid': uid,
                'timestamp': datetime.utcnow().isoformat() + 'Z'
            }
            
            # Gestion des cas spéciaux
            if status == "EMERGENCY":
                telemetry['status'] = "emergency"
            elif status == "GRANTED":
                telemetry['status'] = "granted"
            elif status == "DENIED":
                telemetry['status'] = "denied"
                
            return telemetry
    except Exception as e:
        print("Erreur parsing télémétrie:", str(e))
    return None

# Boucle principale
while True:
    try:
        # Lire les données série
        bt_data = btSerial.readline().decode('ascii').strip()
        
        if bt_data:
            print('Données reçues de l\'Arduino:', bt_data)
            telemetry = parse_telemetry(bt_data)
            
            if telemetry:
                print("Télémétrie parsée:", telemetry)
                mqtt_client.publish(telemetry_topic, json.dumps(telemetry), qos=2)
                
    except serial.SerialException as se:
        print("Erreur port série:", str(se))
        time.sleep(5)  # Attendre avant de réessayer
    except Exception as e:
        print("Erreur inattendue:", str(e))
        time.sleep(1)